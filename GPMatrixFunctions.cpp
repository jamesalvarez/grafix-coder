#include "GPMatrixFunctions.h"
#define NO_XML
#include "TRUNCATED_KERNEL_BF/include/fast_lbf.h"

using namespace GPMatrixFunctions;

namespace GPMatrixFunctions {
    void smoothSegment(mat &cutM, mat &smoothedM, bool copy_eyes);
    mat smoothSegment(mat &cutM, bool copy_eyes, int expWidth, int expHeight);
    void fast_LBF(Array_2D<double> &image_X, double sigma_s, double Xsigma_r, bool b, Array_2D<double> *filtered_X);
    double calculateRMSRaw(mat &preparedRoughM, int expWidth, int expHeight, double degPerPixel);
    void calculateVelocity(mat &smoothM, GrafixSettingsLoader settingsLoader);
}


/***************************
 *      SMOOTHING
 ***************************/

void GPMatrixFunctions::smoothRoughMatrixFBF(const mat &RoughM, const QString path, const GrafixConfiguration &configuration, mat *SmoothM, GPMatrixProgressBar *gpProgressBar) {
    gpProgressBar->beginProcessing("Smoothing Data...", 50);
    smoothRoughMatrixFBF(RoughM, path, configuration, SmoothM);
    gpProgressBar->endProcessing();
}

void GPMatrixFunctions::smoothRoughMatrixFBF(const mat &RoughM, const QString path, const GrafixConfiguration &configuration, mat *SmoothM) {
    if(RoughM.is_empty()) {
        return;
    }

    GrafixSettingsLoader settings(path, configuration);

    bool copy_eyes = settings.LoadSetting(Consts::SETTING_SMOOTHING_USE_OTHER_EYE).toBool();
    int expWidth = settings.LoadSetting(Consts::SETTING_EXP_WIDTH).toInt();
    int expHeight = settings.LoadSetting(Consts::SETTING_EXP_HEIGHT).toInt();
    double sigma_r = settings.LoadSetting(Consts::SETTING_SMOOTHING_SIGMA_R).toDouble();
    double sigma_s = settings.LoadSetting(Consts::SETTING_SMOOTHING_SIGMA_S).toDouble();
    double Xsigma_r = sigma_r / expWidth;
    double Ysigma_r = sigma_r / expHeight;

    // prepare the smooth matrix
    SmoothM->zeros(RoughM.n_rows, 10);

    // create copy of matrix to copy eyes etc
    mat RoughMCopy = RoughM;

    typedef Array_2D<double> image_type;

    uword validSegmentStartIndex = 0;
    uword validSegmentEndIndex = 0;
    bool inValidSegment = false;

    //mark missing data, and get segments inbetween to smotth
    for (uword i = 0; i < RoughM.n_rows; ++i) {
        bool leftXMissing = (RoughM(i, 2) < 0 || RoughM(i, 2) > 1);
        bool leftYMissing = (RoughM(i, 3) < 0 || RoughM(i, 3) > 1);
        bool rightXMissing = (RoughM(i, 4) < 0 || RoughM(i, 4) > 1);
        bool rightYMissing = (RoughM(i, 5) < 0 || RoughM(i, 5) > 1);

        bool leftMissing = leftXMissing || leftYMissing;
        bool rightMissing = rightXMissing || rightYMissing;

        bool missing = copy_eyes ? (leftMissing && rightMissing) : (leftMissing || rightMissing);



        //copy eyes if necessary
        if (copy_eyes && !missing) {
            if (leftMissing && !rightMissing) {
                RoughMCopy(i, 2) = RoughM(i, 4);
                RoughMCopy(i, 3) = RoughM(i, 5);
            } else if (rightMissing && !leftMissing) {
                RoughMCopy(i, 4) = RoughM(i, 2);
                RoughMCopy(i, 5) = RoughM(i, 3);
            }
        }

        bool lastRow = i == RoughM.n_rows - 1;

        if (inValidSegment) {

            //if at the last row and it is valid, then mark it so!
            if (lastRow && !missing) {
                validSegmentEndIndex  = i;
            }

            if (missing || lastRow) {

                //found a segment - so lets copy it and smooth it
                mat validSegment = RoughMCopy.rows(validSegmentStartIndex, validSegmentEndIndex);

                //no need to smooth just one data point, so copy it
                if (validSegment.n_rows == 1) {
                    SmoothM->at(validSegmentEndIndex, 2) = expWidth * ((RoughMCopy(validSegmentEndIndex, 2) + RoughMCopy(validSegmentEndIndex, 4)) / 2);
                    SmoothM->at(validSegmentEndIndex, 3) = expHeight * ((RoughMCopy(validSegmentEndIndex, 3) + RoughMCopy(validSegmentEndIndex, 5)) / 2);
                } else {
                    // do fbf smoothing on segment
                    image_type image_X(validSegment.n_rows, 1);
                    image_type image_Y(validSegment.n_rows, 1);

                    image_type filtered_X(validSegment.n_rows, 1);
                    image_type filtered_Y(validSegment.n_rows, 1);

                    //copy to image_type - there should be no missing data.
                    for (uword j = 0; j < validSegment.n_rows; ++j) {
                        uword dataIndex = j + validSegmentStartIndex;
                        image_X(j, 0) = (RoughMCopy(dataIndex, 2) + RoughMCopy(dataIndex, 4)) / 2;
                        image_Y(j, 0) = (RoughMCopy(dataIndex, 3) + RoughMCopy(dataIndex, 5)) / 2;
                    }

                    //filter the X and Y data
                    GPMatrixFunctions::fast_LBF(image_X, sigma_s, Xsigma_r, false, &filtered_X);
                    GPMatrixFunctions::fast_LBF(image_Y, sigma_s, Ysigma_r, false, &filtered_Y);

                    //copy to the smoothed matrix
                    for (uword j = 0; j < validSegment.n_rows; ++j) {
                        uword dataIndex = j + validSegmentStartIndex;
                        SmoothM->at(dataIndex, 2) = filtered_X.at(j, 0) * expWidth;
                        SmoothM->at(dataIndex, 3) = filtered_Y.at(j, 0) * expHeight;
                    }
                }

                inValidSegment = false;
            } else {
                validSegmentEndIndex = i;

            }
        } else {
            if (!missing) {
                validSegmentStartIndex = i;
                validSegmentEndIndex = i;
                inValidSegment = true;
            }
        }

        // copy timestamp
        SmoothM->at(i, 0) = RoughM.at(i, 0);

        // set missing data code
        if (missing) {
            SmoothM->at(i, 2) = -1;
            SmoothM->at(i, 3) = -1;
        }
    }

    calculateVelocity(*SmoothM, settings);
}

// Smoothing algorithm
// Bilateral filtering algorithm based on Ed Vul (Frank, Vul, & Johnson,2009; based on Durand & Dorsey, 2002).
// THis algorithm eliminates Jitter in fixations while preserving saccades.
void GPMatrixFunctions::smoothSegment(mat &cutM, mat &smoothedM, bool copy_eyes) {
    const int n_rows = cutM.n_rows + 1;
    smoothedM = zeros(n_rows, 4); // [miliseconds, 0, x, y]

    mat allX =  zeros(n_rows, 1);
    mat allY =  zeros(n_rows, 1);

    // When there are no values for one eye, copy the values from the other eye.
    // Calculate X and Y initial averages. Exclude when the eyes were not detected
    for (uword i = 0; i < cutM.n_rows; ++i) { // Eyes were not detected
        if (cutM.at(i, 2) == -1 && cutM.at(i, 4) == -1) {
            allX.at(i, 0) = 0;
            allY.at(i, 0) = 0;
        } else if (copy_eyes && cutM.at(i, 2) != -1 && cutM.at(i, 4) == -1 ) { // Left eyes were detected but not right
            allX.at(i, 0) = cutM.at(i, 2);
            allY.at(i, 0) = cutM.at(i, 3);
        } else if(copy_eyes && cutM.at(i, 2) == -1 && cutM.at(i, 4) != -1) { // Right eyes were detected, not left
            allX.at(i, 0) = cutM.at(i, 4);
            allY.at(i, 0) = cutM.at(i, 5);
        } else if(!copy_eyes && ((cutM.at(i, 2) != -1 && cutM.at(i, 4) == -1) || (cutM.at(i, 4) != -1 && cutM.at(i, 2) == -1) )) {
            allX.at(i, 0) = 0;
            allY.at(i, 0) = 0;
        } else {  // Both eyes were detected
            allX.at(i, 0) = (cutM.at(i, 2) + cutM.at(i, 4)) / 2; // x
            allY.at(i, 0) = (cutM.at(i, 3) + cutM.at(i, 5)) / 2; // y
        }
    }

    mat x1 = zeros(n_rows, n_rows);
    mat y1 = zeros(n_rows, n_rows);
    mat iw = zeros(n_rows, n_rows);
    mat iw2 = zeros(n_rows, n_rows);
    mat t1 = zeros(n_rows, n_rows);
    mat t2 = zeros(n_rows, n_rows);
    mat dw = zeros(n_rows, n_rows);
    mat tw = zeros(n_rows, n_rows);

    //old1:
    // [x1 x2] = meshgrid (allX , allX);
    //for (uword i = 0; i < n_rows; ++i){
    //     x1(span::all, i).fill(allX(i,0));  // Vertical columns with the data
    //     x2(i, span::all).fill(allX(i,0));  // Horizontal rows with the data.
    //}

    //new1:
    // [x1 x2] = meshgrid (allX , allX);
    for (uword i = 0; i < (uword)n_rows; ++i) {
        x1(span::all, i).fill(allX(i, 0)); // Vertical columns with the data
        y1(span::all, i).fill(allY(i, 0));
        t1(span::all, i).fill(i);  // Vertical columns with the data
        //t2(i, span::all).fill(i);  // Horizontal rows with the data.
    }

    //x2 = x1.t();

    // iw = exp(-(x1-x2).^2./(2.*iscales.^2));
    iw = exp(-square(x1 - x1.t()) / (2 * Consts::SMOOTH_ISCALES * Consts::SMOOTH_ISCALES));
    iw2 = exp(-square(y1 - y1.t()) / (2 * Consts::SMOOTH_ISCALES * Consts::SMOOTH_ISCALES));

    // [t1 t2] = meshgrid((1:length(allxds)), (1:length(allxds)));
    dw = exp(-square(t1 - t1.t()) / (2 * Consts::SMOOTH_SCALES * Consts::SMOOTH_SCALES));


    // tw = iwa.*iwa2.*iw .* iw2 .* dw;
    tw =  (iw % iw2) % dw;

    // Get only the ones that are not NaN
    uvec goodX =  arma::find(allX != datum::nan);
    uvec goodY =  arma::find(allY != datum::nan);

    mat smx = zeros(1, n_rows);
    smx.fill(datum::nan);

    mat smy = zeros(1, n_rows);
    smy.fill(datum::nan);

    smx.elem(goodX) = sum(((mat)(tw(goodX, goodX) % repmat(allX.elem(goodX), 1, ((mat)allX.elem(goodX)).n_rows))).t(), 1) / sum( tw(goodX, goodX) , 1);
    smy.elem(goodY) = sum(((mat)(tw(goodY, goodY) % repmat(allY.elem(goodY), 1, ((mat)allY.elem(goodY)).n_rows))).t(), 1) / sum( tw(goodY, goodY) , 1);

    smoothedM.col(2) = smx.t();
    smoothedM.col(3) = smy.t();
}

mat GPMatrixFunctions::smoothSegment(mat &cutM, bool copy_eyes, int expWidth, int expHeight) {
    mat data = zeros(cutM.n_rows + 1, 4); // [miliseconds, 0, x, y]

    // When there are no values for one eye, copy the values from the other eye.
    // Calculate X and Y initial averages. Exclude when the eyes were not detected
    for (uword i = 0; i < cutM.n_rows; ++i) { // Eyes were not detected
        if (cutM(i, 2) == -1 && cutM(i, 4) == -1) {
            data(i, 2) = 0;
            data(i, 3) = 0;
        } else if (copy_eyes && cutM(i, 2) != -1 && cutM(i, 4) == -1 ) { // Left eyes were detected but not right
            data(i, 2) = cutM(i, 2) * expWidth;
            data(i, 3) = cutM(i, 3) * expHeight;
        } else if(copy_eyes && cutM(i, 2) == -1 && cutM(i, 4) != -1) { // Right eyes were detected, not left
            data(i, 2) = cutM(i, 4) * expWidth;
            data(i, 3) = cutM(i, 5) * expHeight;
        } else if(!copy_eyes && ((cutM(i, 2) != -1 && cutM(i, 4) == -1) || (cutM(i, 4) != -1 && cutM(i, 2) == -1) )) {
            data(i, 2) = 0;
            data(i, 3) = 0;
        } else {  // Both eyes were detected
            data(i, 2) = (cutM(i, 2) + cutM(i, 4)) / 2 * expWidth; // x
            data(i, 3) = (cutM(i, 3) + cutM(i, 5)) / 2  * expHeight; // y
        }
    }

    mat allX =  data(span::all, 2);
    mat allY =  data(span::all, 3);

    mat x1 = zeros(allX.n_rows, allX.n_rows);
    mat x2 = zeros(allX.n_rows, allX.n_rows);
    mat iw = zeros(allX.n_rows, allX.n_rows);
    mat iw2 = zeros(allX.n_rows, allX.n_rows);
    mat t1 = zeros(allX.n_rows, allX.n_rows);
    mat t2 = zeros(allX.n_rows, allX.n_rows);
    mat dw = zeros(allX.n_rows, allX.n_rows);
    mat tw = zeros(allX.n_rows, allX.n_rows);

    // [x1 x2] = meshgrid (allX , allX);
    for (uword i = 0; i < allX.n_rows; ++i) {
        x1(span::all, i).fill(allX(i, 0)); // Vertical columns with the data
        x2(i, span::all).fill(allX(i, 0)); // Horizontal rows with the data.
    }

    // iw = exp(-(x1-x2).^2./(2.*iscales.^2));
    iw = exp(-square(x1 - x2) / (2 * Consts::SMOOTH_ISCALES * Consts::SMOOTH_ISCALES));

    for (uword i = 0; i < allY.n_rows; ++i) {
        x1(span::all, i).fill(allY(i, 0)); // Vertical columns with the data
        x2(i, span::all).fill(allY(i, 0)); // Horizontal rows with the data.
    }
    iw2 = exp(-square(x1 - x2) / (2 * Consts::SMOOTH_ISCALES * Consts::SMOOTH_ISCALES));

    // [t1 t2] = meshgrid((1:length(allxds)), (1:length(allxds)));
    for (uword i = 0; i < allX.n_rows; ++i) {
        t1(span::all, i).fill(i);  // Vertical columns with the data
        t2(i, span::all).fill(i);  // Horizontal rows with the data.
    }
    dw = exp(-square(t1 - t2) / (2 * Consts::SMOOTH_SCALES * Consts::SMOOTH_SCALES));


    // tw = iwa.*iwa2.*iw .* iw2 .* dw;
    tw =  (iw % iw2) % dw;

    // Get only the ones that are not NaN
    uvec goodX =  arma::find(allX != datum::nan);
    uvec goodY =  arma::find(allY != datum::nan);

    mat smx = zeros(1, allX.n_rows);
    smx.fill(datum::nan);

    mat smy = zeros(1, allY.n_rows);
    smy.fill(datum::nan);

    smx.elem(goodX) = sum(((mat)(tw(goodX, goodX) % repmat(allX.elem(goodX), 1, ((mat)allX.elem(goodX)).n_rows))).t(), 1) / sum( tw(goodX, goodX) , 1);
    smy.elem(goodY) = sum(((mat)(tw(goodY, goodY) % repmat(allY.elem(goodY), 1, ((mat)allY.elem(goodY)).n_rows))).t(), 1) / sum( tw(goodY, goodY) , 1);

    data.col(2) = smx.t();
    data.col(3) = smy.t();

    return data;

}

void GPMatrixFunctions::fast_LBF(Array_2D<double>& image_X, double sigma_s, double Xsigma_r, bool b, Array_2D<double>* filtered_X) {
    typedef Array_2D<double> image_type;

    try {
        Image_filter::fast_LBF(image_X, image_X, sigma_s, Xsigma_r, b, filtered_X, filtered_X);
    } catch(const std::runtime_error& re) {
        // speciffic handling for runtime_error
        DialogGrafixError::LogNewError(0, "Runtime error: " + QString(re.what()));;
    } catch(const std::exception& ex) {
        // speciffic handling for all exceptions extending std::exception, except
        // std::runtime_error which is handled explicitly
        DialogGrafixError::LogNewError(0, "Error occurred: " + QString(ex.what()) +
                                       " with array size: " + QString::number(image_X.x_size()) +
                                       " splitting file and retrying...");
        //split file, process and patch with seam (that is double sigma s length)
        int mid_point = image_X.x_size() / 2;

        int x1_size = mid_point;
        int x2_size = image_X.x_size() - mid_point;
        int seam_size = sigma_s * 2;

        image_type image_X_1(x1_size, 1);
        image_type image_X_2(x2_size, 1);
        image_type image_X_seam(seam_size, 1);

        image_type filt_X_1(x1_size, 1);
        image_type filt_X_2(x2_size, 1);
        image_type filt_X_seam(seam_size, 1);

        for (int i = 0; i < x1_size; ++i) {
            image_X_1(i, 0) = image_X(i, 0);
        }
        for (int i = 0; i < x2_size; ++i) {
            image_X_2(i, 0) = image_X(mid_point + i, 0);
        }
        for (int i = 0; i < seam_size; ++i) {
            image_X_seam(i, 0) = image_X(mid_point - (seam_size / 2), 0);
        }

        fast_LBF(image_X_1, sigma_s, Xsigma_r, false, &filt_X_1);
        fast_LBF(image_X_2, sigma_s, Xsigma_r, false, &filt_X_2);
        fast_LBF(image_X_seam, sigma_s, Xsigma_r, false, &filt_X_seam);

        //now to put back together again
        for (int i = 0; i < x1_size; ++i) {
            (*filtered_X)(i, 0) = filt_X_1(i, 0);
        }
        for (int i = 0; i < x2_size; ++i) {
            (*filtered_X)(mid_point + i, 0) = filt_X_2(i, 0);
        }
        for (int i = 0; i < seam_size; ++i) {
            (*filtered_X)(mid_point - (seam_size / 2), 0) = filt_X_seam(i, 0);
        }


    } catch(...) {
        DialogGrafixError::LogNewError(0, "Unknown Error: Parameters - too small?");
    }
}

/***************************
 *      INTERPOLATING
 ***************************/

void GPMatrixFunctions::interpolateData(mat &SmoothM, GrafixSettingsLoader settingsLoader, GPMatrixProgressBar &gpProgressBar) {
    // Here we interpolate the smoothed data and create an extra column
    // Smooth = [time,0,x,y,velocity,saccadeFlag(0,1), interpolationFlag]

    qDebug() << "Interpolating...";

    if (SmoothM.is_empty())
        return;

    gpProgressBar.beginProcessing("Interpolating Data", 100);


    double hz                   = settingsLoader.LoadSetting(Consts::SETTING_HZ).toDouble();
    double interpolationLatency = settingsLoader.LoadSetting(Consts::SETTING_INTERP_LATENCY).toDouble();
    double maxDisplacementInterpolation = settingsLoader.LoadSetting(Consts::SETTING_INTERP_MAXIMUM_DISPLACEMENT).toDouble();
    int    maxSamplesInterpolation            = interpolationLatency * hz / 1000;
    double degreesPerPixel   = settingsLoader.LoadSetting(Consts::SETTING_DEGREE_PER_PIX).toDouble();

    qDebug() << "Samples threshold: " << maxSamplesInterpolation << " Displacement: " << maxDisplacementInterpolation;

    // Reset previous interpolation results
    if (SmoothM.n_cols > 7) {
        for(uword i = 0; i < SmoothM.n_rows; ++i) {
            if (SmoothM(i, 6) == 1) {
                SmoothM(i, 2) = -1;
                SmoothM(i, 3) = -1;
            }
        }
    }


    // Remove previous interpolation data and create the new columns
    mat aux = zeros(SmoothM.n_rows, 11);
    aux.cols(0, 3) = SmoothM.cols(0, 3);
    SmoothM = aux;

    // Iterate through the samples, detecting missing points.
    for(uword i = 0; i < SmoothM.n_rows; ++i) {

        // Sample is not missing
        if ( SmoothM(i, 2) >= 0 && SmoothM(i, 3) >= 0) continue;



        // Skip if first sample is missing
        if (i == 0) continue;

        // Find the previous non-missing data
        int indexPrevData = -1;
        for (int j = i - 1; j > 0 ; --j) {
            if ( SmoothM(j, 2) >= 0 && SmoothM(j, 3) >= 0) {
                indexPrevData = j;
                break;
            }
        }



        // If no previous data is detected, then cannot interpolate
        if (indexPrevData == -1) continue;

        // Find the point where the data is back:
        int indexDataBack = -1;
        for (uword j = i; j < SmoothM.n_rows; ++j) {
            if (SmoothM(j, 2) > 0 && SmoothM(j, 3) > 0) {
                indexDataBack = j ;
                break;
            }
        }

        // If the data doesn't come back then cannot interpolate
        if (indexDataBack == -1) break;

        int gapLength = indexDataBack - indexPrevData;

        // Skip main search to this point
        i = indexDataBack;

        if (gapLength > maxSamplesInterpolation) {
            continue; // Cannot interpolate as over samples threshold
        }

        //Check the displacement distance
        double xDiff = SmoothM(indexDataBack, 2) - SmoothM(indexPrevData, 2);
        double yDiff = SmoothM(indexDataBack, 3) - SmoothM(indexPrevData, 3);
        double displacement = degreesPerPixel * sqrt((xDiff * xDiff) + (yDiff * yDiff));

        if (displacement > maxDisplacementInterpolation) {
            continue;  // Distance is too great - no interpolation
        }

        // INTERPOLATE

        // Jumps in signal for each sample to interpolate linearly
        double xInterval = xDiff / gapLength;
        double yInterval = yDiff / gapLength;

        for(int j = indexPrevData + 1; j < indexDataBack; j++) {
            SmoothM(j, 2) = SmoothM(j - 1, 2) + xInterval;
            SmoothM(j, 3) = SmoothM(j - 1, 3) + yInterval;
            SmoothM(j, 6) = 1;
        }


    }

    // Recalculate velocity
    calculateVelocity(SmoothM, settingsLoader);
    gpProgressBar.endProcessing();
}

/***************************
 *      FIXATIONS
 ***************************/

void GPMatrixFunctions::estimateFixations(mat &RoughM, mat &SmoothM, mat &AutoFixAllM, GrafixSettingsLoader &settingsLoader, GPMatrixProgressBar &gpProgressBar) {
    gpProgressBar.beginProcessing("Estimating Fixations", 100);
    estimateFixations(RoughM, SmoothM, AutoFixAllM, settingsLoader);
    gpProgressBar.endProcessing();
}

/*
 * Main function called by UI to estimate fixations automatically (applying post-hoc corrections).
 */
void GPMatrixFunctions::estimateFixations(mat &RoughM, mat &SmoothM, mat &AutoFixAllM, GrafixSettingsLoader &settingsLoader) {
    if (SmoothM.is_empty()) return;// If the data is not smoothed we don't allow to estimate fixations.

    qDebug() << "Calculating velocity...";
    GPMatrixFunctions::calculateVelocity(SmoothM, settingsLoader);

    // Calculate Fixations mat *p_fixAllM, mat *p_roughM, mat *p_smoothM
    qDebug() << "Calculating fixations...";
    GPMatrixFunctions::calculateFixations(AutoFixAllM, RoughM, SmoothM, settingsLoader);

    //we cannot work with less than one fixation
    if(AutoFixAllM.n_rows < 1) return;


    // **** POST-HOC VALIDATION **** (The order is important!)

    // Add columns for the flags in the smooth data if needed
    if (SmoothM.n_cols < 10) {
        mat aux = zeros(SmoothM.n_rows, 10);
        aux.cols(0, 3) = SmoothM.cols(0, 3);
        SmoothM = aux;
    } else {
        SmoothM.cols(7, 9).fill(0); // Restart
    }

    //TODO Check if fixations found or next part crasheds

    bool cb_displacement = settingsLoader.LoadSetting(Consts::SETTING_POSTHOC_MERGE_CONSECUTIVE_FLAG).toBool();
    bool cb_velocityVariance = settingsLoader.LoadSetting(Consts::SETTING_POSTHOC_LIMIT_RMS_FLAG).toBool();
    bool cb_minFixation = settingsLoader.LoadSetting(Consts::SETTING_POSTHOC_MIN_DURATION_FLAG).toBool();
    double sliderVelocityVariance = settingsLoader.LoadSetting(Consts::SETTING_POSTHOC_LIMIT_RMS_VAL).toDouble();
    double sliderMinFixation = settingsLoader.LoadSetting(Consts::SETTING_POSTHOC_MIN_DURATION_VAL).toDouble();

    // Merge all fixations with a displacement lower than the displacement threshold
    if (cb_displacement) {
        qDebug() << "Merging...";
        GPMatrixFunctions::posthocMergeDisplacementThreshold(RoughM, SmoothM, AutoFixAllM,  settingsLoader);
    }


    // Remove all fixations below the minimun variance
    if (cb_velocityVariance) {
        qDebug() << "Removing high variance...";
        GPMatrixFunctions::posthocRemoveHighVarianceFixations( &SmoothM,  &AutoFixAllM, sliderVelocityVariance);
    }


    // Remove all the fixations below the minimun fixation
    if (cb_minFixation) {
        qDebug() << "Removing minimum length...";
        GPMatrixFunctions::posthocRemoveMinFixations(&AutoFixAllM, &SmoothM, sliderMinFixation);
    }


    //debugPrintMatrix(AutoFixAllM);
}

void GPMatrixFunctions::calculateFixations(mat &fixAllM, mat &roughM, mat &smoothM, GrafixSettingsLoader settingsLoader) {



    bool copy_eyes = settingsLoader.LoadSetting(Consts::SETTING_SMOOTHING_USE_OTHER_EYE).toBool();
    int expWidth            = settingsLoader.LoadSetting(Consts::SETTING_EXP_WIDTH).toInt();
    int expHeight           = settingsLoader.LoadSetting(Consts::SETTING_EXP_HEIGHT).toInt();
    double degreePerPixel   = settingsLoader.LoadSetting(Consts::SETTING_DEGREE_PER_PIX).toDouble();

    //clear fixall matrix
    fixAllM.reset();

    int indexStartFix = -1;
    for(uword i = 0; i < smoothM.n_rows; i ++) {

        bool inFixation = smoothM.at(i, 5) == 0 && smoothM.at(i, 2) > -1 && smoothM.at(i, 3) > -1;

        if (inFixation) { //the velocity between this and previous sample was below threshold
            if (indexStartFix == -1) { //the previous sample was the beginning of the fixation
                indexStartFix = i - 1;
            }
        } else { //the velocity between this and the previous sample was above threshold
            if (indexStartFix != -1) { //the previous sample was the end of the fixation

                int indexEndFix = i - 1;

                mat newRow;
                calculateFixation(roughM,
                                     indexStartFix,
                                     indexEndFix,
                                     copy_eyes,
                                     expWidth,
                                     expHeight,
                                     degreePerPixel,
                                     newRow);

                fixAllM = (fixAllM.n_rows == 0) ? newRow : join_cols(fixAllM, newRow);

                indexStartFix = -1;

            }
        }
    }

    qDebug() << "Finished calculating fixations";
    //debugPrintMatrix(fixAllM);
}

void GPMatrixFunctions::calculateFixation(const mat &roughM, int startIndex, int endIndex, bool copy_eyes, int expWidth, int expHeight, double degPerPixel, mat &outFixation) {

    outFixation.reset();

    mat roughFixationM = roughM.rows(startIndex, endIndex);

    mat preparedRoughM;
    excludeMissingDataRoughMatrix(preparedRoughM, roughFixationM, copy_eyes);

    bool validData = preparedRoughM.n_cols > 2;

    //debugPrintMatrix(preparedRoughM);

    //duration is in ms calculated from timestamps
    double dur = ((roughM.at(endIndex, 0) - roughM.at(startIndex, 0)));
    double averageX = validData ? mean(preparedRoughM.col(1)) : -1;
    double averageY = validData ? mean(preparedRoughM.col(2)) : -1;
    double variance = validData ? calculateRMSRaw(preparedRoughM, expWidth, expHeight, degPerPixel) : -1;
    double pupilDilation = (roughM.n_cols >= 8) ? (mean(roughFixationM.col(6)) + mean(roughFixationM.col(7))) / 2 : 0;

    outFixation << startIndex << endIndex << dur << averageX << averageY << variance << 0 << pupilDilation <<  endr ;
}

uvec GPMatrixFunctions::getFixationsInBetween(uword startIndex, uword endIndex, const mat &fixAllM) {

    uvec fixationIndexes;

    for (uword i = 0; i < fixAllM.n_rows; i++) {
        // Fixations that start anywhere in this segment
        bool fixationStartsInSegment = (fixAllM(i, FIXCOL_START) >= startIndex && fixAllM(i, FIXCOL_START) <=  endIndex );

        // Fixations that start before the segment begins, and end in this one.
        bool fixationStartsBeforeSegment = (fixAllM(i, FIXCOL_START) <= startIndex && fixAllM(i, FIXCOL_END) >= endIndex );

        bool displayFixation = fixationStartsInSegment || fixationStartsBeforeSegment;

        if (displayFixation) {
            fixationIndexes << i;
        }
    }

    return fixationIndexes;
}

/***************************
 *      SACCADES
 ***************************/

void GPMatrixFunctions::calculateSaccades(mat &saccadesM, const mat &fixAllM, const mat &smoothM, double degreesPerPixel) {

    //clear saccades matrix
    saccadesM.reset();

    if (fixAllM.n_rows < 2) return;

    for (uword indexFixation = 1; indexFixation < fixAllM.n_rows ; indexFixation++) {
        double startOfSaccade = fixAllM.at(indexFixation - 1, FIXCOL_END); //end of previous fixation
        double endOfSaccade = fixAllM.at(indexFixation, FIXCOL_START); // beginning of next fixation

        if (startOfSaccade >= endOfSaccade) {
            continue;
        }

        double duration = ((smoothM.at(endOfSaccade, 0) - smoothM.at(startOfSaccade, 0)));

        //for amplitude take the position at the end of the fixation, and start of next
        double xDiff = smoothM.at(startOfSaccade, 2) - smoothM.at(endOfSaccade, 2);
        double yDiff = smoothM.at(startOfSaccade, 3) - smoothM.at(endOfSaccade, 3);

        double amplitudePixels = sqrt((xDiff * xDiff) + (yDiff * yDiff)) * degreesPerPixel;

        //calculate average and peak velocity

        double averageVelocity = 0;
        int nVelocities = 0;

        double peakVelocity = -1;

        for (uword j = startOfSaccade; j < endOfSaccade; j++) {

            double sampleVelocity = smoothM(j + 1, 4);
            //check not missing
            if (sampleVelocity > -1) {
                nVelocities++;
                peakVelocity = qMax(peakVelocity, sampleVelocity);
                averageVelocity += sampleVelocity;
            }
        }

        if (nVelocities > 0) {
            averageVelocity = averageVelocity / nVelocities;
        } else {
            averageVelocity = -1;
        }

        mat row;
        row << startOfSaccade << endOfSaccade << duration << amplitudePixels << averageVelocity << peakVelocity << endr;

        if (saccadesM.n_rows == 0) {
            saccadesM = row;
        } else {
            saccadesM = join_cols(saccadesM, row);
        }
    }
}

/***************************
 *      POST-HOCS
 ***************************/

void GPMatrixFunctions::posthocRemoveMinFixations(mat *p_fixAllM, mat *p_smoothM, double minDur) {

    if (p_fixAllM->is_empty()) return; //cannot remove no fixations lol
    p_smoothM->col(9).fill(0); // Restart

    // Find all fixations we will delete
    uvec fixIndex =  arma::find(p_fixAllM->col(FIXCOL_DURATION) < minDur);
    mat minFix = p_fixAllM->rows(fixIndex);

    for (uword i = 0; i < minFix.n_rows; ++i) { // Modify the flag for the fixations we delete
        p_smoothM->operator()(span(minFix(i, FIXCOL_START), minFix(i, FIXCOL_END)), span(9, 9) ).fill(1);
    }

    // Delete min fixations
    fixIndex =  arma::find(p_fixAllM->col(FIXCOL_DURATION) > minDur);

    if (!fixIndex.empty()) {
        (*p_fixAllM) = p_fixAllM->rows(fixIndex);
    } else {
        (*p_fixAllM).reset();
    }

}

/*
 * This merges fixations that are within 50ms of eachother, and have a displacement of less than the user setting
 */
void GPMatrixFunctions::posthocMergeDisplacementThreshold(mat &roughM, mat &smoothM, mat &fixAllM, GrafixSettingsLoader settingsLoader) {
    if (fixAllM.n_rows < 3 || smoothM.n_cols < 7) return;

    //int invalidSamples      = MyConstants::INVALID_SAMPLES;
    int expWidth            = settingsLoader.LoadSetting(Consts::SETTING_EXP_WIDTH).toInt();
    int expHeight           = settingsLoader.LoadSetting(Consts::SETTING_EXP_HEIGHT).toInt();
    bool copy_eyes = settingsLoader.LoadSetting(Consts::SETTING_SMOOTHING_USE_OTHER_EYE).toBool();
    double degreePerPixel   = settingsLoader.LoadSetting(Consts::SETTING_DEGREE_PER_PIX).toDouble();
    int hz                  = settingsLoader.LoadSetting(Consts::SETTING_HZ).toInt();
    double maximumDisplacement     = settingsLoader.LoadSetting(Consts::SETTING_POSTHOC_MERGE_CONSECUTIVE_VAL).toDouble();
    int maximumDelay = 50 * hz / 1000; // to merge, the distance between one fixation and the next is less than 50 ms


    double xMultiplier = expWidth * degreePerPixel;
    double yMultiplier = expHeight * degreePerPixel;


    qDebug() << "start merging";
    smoothM.col(7).fill(0); // Reset flags that mark merging

    for (uword iCurrentFixation = 0; iCurrentFixation < fixAllM.n_rows - 1; ++iCurrentFixation) {

        uword iNextFixation = iCurrentFixation + 1;
        double x1Degs = fixAllM.at(iCurrentFixation, FIXCOL_AVERAGEX) * xMultiplier;
        double y1Degs = fixAllM.at(iCurrentFixation, FIXCOL_AVERAGEY) * yMultiplier;

        double x2Degs = fixAllM.at(iNextFixation, FIXCOL_AVERAGEX) * xMultiplier;
        double y2Degs = fixAllM.at(iNextFixation, FIXCOL_AVERAGEY) * yMultiplier;

        double xDiff = x1Degs - x2Degs;
        double yDiff = y1Degs - y2Degs;

        double distance = sqrt((xDiff * xDiff) + (yDiff * yDiff));
        double delay = fixAllM.at(iNextFixation, FIXCOL_START) - fixAllM.at(iCurrentFixation, FIXCOL_END);

        // Merge if: The displacement from fixation 1 and fixation 2 is less than "displacement" AND
        // if the distance between fixation 1 and fixation 2 is less than 50 ms .
        if (distance <= maximumDisplacement && delay <= maximumDelay) {

            int startIndex = fixAllM.at(iCurrentFixation, FIXCOL_START);
            int endIndex = fixAllM.at(iNextFixation, FIXCOL_END);

            // Flag on the smooth matrix
            smoothM(span(startIndex, endIndex), span(7, 7) ).fill(1);

            // MERGE!
            mat mergedFixation;
            calculateFixation(roughM, startIndex, endIndex, copy_eyes, expWidth, expHeight, degreePerPixel, mergedFixation);

            fixAllM.row(iCurrentFixation) = mergedFixation;
            fixAllM.shed_row(iNextFixation);

            //see if this fixation can merge with next one
            iCurrentFixation = iCurrentFixation - 1;

        }
    }

    qDebug() << "Merged.";
}

void GPMatrixFunctions::posthocRemoveHighVarianceFixations(mat *p_smoothM, mat *p_fixAllM, double variance) {

    if (p_fixAllM->n_rows < 1) return;
    p_smoothM->col(8).fill(0); // Restart

    for (uword i = 0; i < p_fixAllM->n_rows - 1; ++i) {
        if (p_fixAllM->at(i, FIXCOL_RMS) > variance) {
            // Remove fixation
            p_smoothM->operator()(span(p_fixAllM->at(i, FIXCOL_START), p_fixAllM->at(i, FIXCOL_END)), span(8, 8) ).fill(1); // Indicate it
        }
    }

    uvec fixIndex =  arma::find(p_fixAllM->col(FIXCOL_RMS) < variance);
    if (!fixIndex.empty()) {
        (*p_fixAllM) = p_fixAllM->rows(fixIndex);
    } else {
        (*p_fixAllM).reset();
    }

}

/***************************
 *      CALCULATIONS
 ***************************/



/*
 * Calculate RMS, but rough matrix must have all missing data removed. Returns -1 if cannot calculate.
 * Use excludeMissingDataRoughMatrix to prepare.
 */
double GPMatrixFunctions::calculateRMSRaw(mat &preparedRoughM, int expWidth, int expHeight, double degPerPixel) {

    if (preparedRoughM.n_rows < 2 )
        return -1;
    // Calculate mean euclidean distance.
    mat squaredDistances = zeros(preparedRoughM.n_rows - 1);

    double x1, y1, x2, y2, xDiff, yDiff;

    double xMultiplier = expWidth * degPerPixel;
    double yMultiplier = expHeight * degPerPixel;

    x1 = preparedRoughM.at(0, 1) * xMultiplier;
    y1 = preparedRoughM.at(0, 2) * yMultiplier;



    for (uword j = 1; j < preparedRoughM.n_rows; ++j) {
        x2 = preparedRoughM.at(j, 1) * xMultiplier;
        y2 = preparedRoughM.at(j, 2) * yMultiplier;

        xDiff = x1 - x2;
        yDiff = y1 - y2;

        double distanceSquared = ((xDiff * xDiff) + (yDiff * yDiff)); //squared and rooted cancel eachother out;
        squaredDistances(j - 1) = distanceSquared;

        x1 = x2;
        y1 = y2;
    }

    double rms = sqrt(mean(mean(squaredDistances)));

    //qDebug() << " rms " << rms;
    return rms;
}


/*
 *                                   Calculate Velocity
 * Takes 0 - 3 indexed columns of smooth matrix and returns 11 column matrix calculates velocity (col 4)
 * and whether it is a saccade (col 5). -1 s are put if any missing data either side.
 */
void GPMatrixFunctions::calculateVelocity(mat &smoothM, GrafixSettingsLoader settingsLoader) {
    if (smoothM.n_cols < 4) return;

    double velocityThreshold = settingsLoader.LoadSetting(Consts::SETTING_VELOCITY_THRESHOLD).toDouble();
    double degreesPerPixel   = settingsLoader.LoadSetting(Consts::SETTING_DEGREE_PER_PIX).toDouble();


    // If SmoothM has less than 5 columns, create an 10 column version and copy over the first 4 columns.
    if (smoothM.n_cols < 5) {
        mat aux = zeros(smoothM.n_rows, 10);
        aux.cols(0, 3) = smoothM.cols(0, 3);
        smoothM = aux;
    }

    // Iterate through each data point, add velocity at row 4, and whether it is saccade at row 5

    double x_1back = smoothM(0, 2);
    double y_1back = smoothM(0, 3);
    double time_1back = smoothM(0, 0);

    // Set firs row to be saccade
    smoothM.at(0, 4) = -1;
    smoothM.at(0, 5) = 0;

    for (uword i = 1; i < smoothM.n_rows; ++i) {

        double x_cur = smoothM(i, 2);
        double y_cur = smoothM(i, 3);
        double time_cur = smoothM(i, 0);

        if (x_1back > -1 && x_cur > -1 && y_1back > -1 && y_cur > -1) {

            // Calculate amplitude and velocity
            double xDist = x_1back - x_cur;
            double yDist = y_1back - y_cur;
            double amplitude = sqrt(((xDist * xDist) + (yDist * yDist))) * degreesPerPixel;
            double time = time_cur - time_1back; //time in ms
            double velocity = (1000 * amplitude) / time;

            // Velocity
            smoothM.at(i, 4) = velocity;
            smoothM.at(i, 5) = (velocity >= velocityThreshold ) ? 1 : 0;
        } else {
            smoothM.at(i, 4) = -1;
            smoothM.at(i, 5) = -1;
        }

        x_1back = x_cur;
        y_1back = y_cur;
        time_1back = time_cur;
    }

    qDebug() << "Finished calculating velocity";
}

/***************************
 *      UTILITIES
 ***************************/

/*
 * Marks all out of range and one eye missing data as missing + combines eyes
 */
void GPMatrixFunctions::prepareRoughMatrix(mat &preparedRoughM, const mat &RoughM, bool copy_eyes) {

    //copy time stamp
    preparedRoughM = zeros(RoughM.n_rows, 3);
    preparedRoughM.col(0) = RoughM.col(0);

    if(RoughM.is_empty()) {
        return;
    }

    for (uword i = 0; i < RoughM.n_rows; ++i) {
        bool leftXMissing = (RoughM(i, 2) < 0 || RoughM(i, 2) > 1);
        bool leftYMissing = (RoughM(i, 3) < 0 || RoughM(i, 3) > 1);
        bool rightXMissing = (RoughM(i, 4) < 0 || RoughM(i, 4) > 1);
        bool rightYMissing = (RoughM(i, 5) < 0 || RoughM(i, 5) > 1);

        bool leftMissing = leftXMissing || leftYMissing;
        bool rightMissing = rightXMissing || rightYMissing;

        bool missing = copy_eyes ? (leftMissing && rightMissing) : (leftMissing || rightMissing);

        // make everything missing if it is
        if (missing) {
            preparedRoughM(i, 1) = -1;
            preparedRoughM(i, 2) = -1;
        } else {
            preparedRoughM(i, 1) = (RoughM(i, 2) + RoughM(i, 4)) / 2;
            preparedRoughM(i, 2) = (RoughM(i, 3) + RoughM(i, 5)) / 2;

            //copy eyes if necessary
            if (copy_eyes) {
                if (leftMissing && !rightMissing) {
                    preparedRoughM(i, 1) = RoughM(i, 4);
                    preparedRoughM(i, 2) = RoughM(i, 5);
                } else if (rightMissing && !leftMissing) {
                    preparedRoughM(i, 1) = RoughM(i, 2);
                    preparedRoughM(i, 2) = RoughM(i, 3);
                }
            }
        }
    }
}

/*
 * Returns only non missing rows of rough data.  (Also prepares the matrix via prepareRoughMatrix)
 */
void GPMatrixFunctions::excludeMissingDataRoughMatrix(mat &cutRoughM, const mat &RoughM, bool copy_eyes) {

    cutRoughM.reset();

    mat preparedRoughMatrix;

    prepareRoughMatrix(preparedRoughMatrix, RoughM, copy_eyes);

    for (uword i = 0; i < RoughM.n_rows; ++i) {
        bool missing = (preparedRoughMatrix(i, 1) == -1 || preparedRoughMatrix(i, 2) == -1);
        if (!missing) {
            cutRoughM = join_cols(cutRoughM, preparedRoughMatrix.row(i));
        }
    }

}


void GPMatrixFunctions::debugPrintMatrix(mat &matrix) {
    qDebug() << "Matrix with cols: " << matrix.n_cols << " and rows: " << matrix.n_rows;
    if (matrix.n_rows > 0) {
        for(uword j = 0; j < matrix.n_rows; ++j) {
            QString rowPrint = "";
            for(uword k = 0; k < matrix.n_cols; ++k) {
                rowPrint += " " + QString::number(matrix.at(j, k));
            }
            qDebug() << rowPrint;
        }
    }
}



void GPMatrixFunctions::returnFixationinSegments(mat *p_fixAllM, mat *p_segmentsM) {
    mat fixations;

    if (!p_segmentsM->is_empty()) { // If there are experimental segments
        int fixIndex = 0;

        // order the segments
        uvec indices = sort_index(p_segmentsM->cols(1, 1));

        mat a = p_segmentsM->rows(indices);

        for (uword i = 0; i < a.n_rows; ++i) { // Find fixations for each fragment and copy them in a matrix
            int start = a(i, 1);
            int end = a(i, 2);
            // Go through the whole fixations matrix
            for (uword j = fixIndex; j < p_fixAllM->n_rows; ++j) {
                if ((p_fixAllM->at(j, FIXCOL_START) >= start && p_fixAllM->at(j, FIXCOL_START) <= end ) || (p_fixAllM->at(j, FIXCOL_START) < end && p_fixAllM->at(j, FIXCOL_END) > end)) { // fixations that start  and end in the segment of start in the segment and end in the next
                    fixations = join_cols(fixations, p_fixAllM->row(j));
                }
            }

        }
    }
    if (!fixations.is_empty())
        (*p_fixAllM) = fixations;//POSSIBLE ERROR?
}













