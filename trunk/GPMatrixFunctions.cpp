#include "GPMatrixFunctions.h"

bool            GPMatrixFunctions::_want_to_close              = false;


GPMatrixFunctions::GPMatrixFunctions()
{
}

GPMatrixProgressBar::GPMatrixProgressBar(QWidget* parent)
{
    _progress_dialog = new QProgressDialog(parent);
    (*_progress_dialog).setMinimumDuration(0);

    _use_progress_dialog = true;
    _batch_processing = false;
}

GPMatrixProgressBar::~GPMatrixProgressBar()
{
    delete _progress_dialog;
}

void GPMatrixProgressBar::beginBatch(int number_tasks)
{
    //TODO check number of tasks is feasible?
    _batch_number_tasks = number_tasks;
    _batch_current_task = 0;
    _batch_processing = true;
    ((QWidget*)((*_progress_dialog).parent()))->setCursor(Qt::WaitCursor);

    if (_use_progress_dialog)
    {
       (*_progress_dialog).setWindowModality(Qt::WindowModal);
        (*_progress_dialog).setAutoClose(false);
        (*_progress_dialog).setAutoReset(false);
        (*_progress_dialog).setCancelButtonText(("Cancel"));
        (*_progress_dialog).setRange(0, number_tasks*100);
        (*_progress_dialog).setValue(0);
        (*_progress_dialog).setWindowTitle(QString("Processing task %1 of %2").arg(0).arg(number_tasks));
        (*_progress_dialog).setLabelText("");
        (*_progress_dialog).show();
    }
    qApp->processEvents();
}


void GPMatrixProgressBar::beginProcessing(QString process_name, int process_length)
{
    if (!_batch_processing)
    {
        ((QWidget*)((*_progress_dialog).parent()))->setCursor(Qt::WaitCursor);

        if (_use_progress_dialog)
        {
           (*_progress_dialog).setWindowModality(Qt::WindowModal);
            (*_progress_dialog).setAutoClose(false);
            (*_progress_dialog).setAutoReset(false);
            (*_progress_dialog).setCancelButtonText(("Cancel"));
            (*_progress_dialog).setRange(0, process_length);
            (*_progress_dialog).setValue(0);
            (*_progress_dialog).setWindowTitle("Processing Data");
            (*_progress_dialog).setLabelText(process_name);
            (*_progress_dialog).show();
        }
        qApp->processEvents();
    }
    else
    {
        //beginning new task in batch processing
        _batch_current_task++;
        _current_process_end = process_length;
        _current_process_progress = 0;
        (*_progress_dialog).setValue(100*_batch_current_task);
        (*_progress_dialog).setWindowTitle(QString("Processing participant %1 of %2").arg(_batch_current_task).arg(_batch_number_tasks));
        (*_progress_dialog).setLabelText(process_name);
        int height = (*_progress_dialog).height();
        (*_progress_dialog).resize(300,height);
        (*_progress_dialog).show();

        qApp->processEvents();
    }
}

void GPMatrixProgressBar::updateProgressDialog(int progress, QString label)
{
    if (!_batch_processing)
    {
        (*_progress_dialog).setValue(progress);
        (*_progress_dialog).setLabelText(label);
        (*_progress_dialog).show();
        qApp->processEvents();
    }
    else
    {
        _current_process_progress = progress;
        int batch = 100*(_batch_current_task-1);
        int current = 100*_current_process_progress / _current_process_end; //out of 100
        (*_progress_dialog).setValue(batch + current);
        (*_progress_dialog).setLabelText(label);
        (*_progress_dialog).show();
        qApp->processEvents();
    }
}



 void GPMatrixProgressBar::endProcessing()
{
     if(!_batch_processing)
     {
        ((QWidget*)((*_progress_dialog).parent()))->setCursor(Qt::ArrowCursor);
         (*_progress_dialog).setValue((*_progress_dialog).maximum());
     }
}

 void GPMatrixProgressBar::endBatch()
 {
     ((QWidget*)((*_progress_dialog).parent()))->setCursor(Qt::ArrowCursor);
     (*_progress_dialog).setValue((*_progress_dialog).maximum());
     _batch_processing = false;
 }

 bool GPMatrixProgressBar::wasCanceled()
 {
     return (*_progress_dialog).wasCanceled();
 }

 void GPMatrixFunctions::estimateFixations(mat &RoughM, mat &SmoothM, mat &AutoFixAllM, QString settingsPath, GPMatrixProgressBar &gpProgressBar)
 {
      gpProgressBar.beginProcessing("Estimating Fixations",100);
      estimateFixations(RoughM,SmoothM,AutoFixAllM,settingsPath);
      gpProgressBar.endProcessing();
 }

 void GPMatrixFunctions::estimateFixations(mat &RoughM, mat &SmoothM, mat &AutoFixAllM, QString settingsPath)
 {
    if (SmoothM.is_empty())
    {
         return;// If the data is not smoothed we don't allow to estimate fixations.
    }


    //TODO check if velocity is calculated.
    GPMatrixFunctions::fncCalculateVelocity(&SmoothM, settingsPath);
    //GPMatrixFunctions::saveFile(SmoothM, _participant->GetPath(GrafixParticipant::SMOOTH));


    // Calculate Fixations mat *p_fixAllM, mat *p_roughM, mat *p_smoothM
    GPMatrixFunctions::fncCalculateFixations(&AutoFixAllM, &RoughM, &SmoothM, settingsPath);

    //we cannot work with less than one fixation
    if(AutoFixAllM.n_rows < 1) return;

    //GPMatrixFunctions::saveFile(AutoFixAllM, _participant->GetPath(GrafixParticipant::AUTOFIXALL));
    // **** POST-HOC VALIDATION **** (The order is important!)

    // Add columns for the flags in the smooth data if needed
    if (SmoothM.n_cols == 4){
     mat aux = zeros(SmoothM.n_rows, 10);
     aux.cols(0,3) = SmoothM.cols(0,3);
     SmoothM = aux;
    }else{
     SmoothM.cols(7,9).fill(0); // Restart
    }

    //TODO Check if fixations found or next part crasheds

    QSettings settings(settingsPath, QSettings::IniFormat);
    bool cb_displacement = settings.value(Consts::SETTING_POSTHOC_MERGE_CONSECUTIVE_FLAG).toBool();
    bool cb_velocityVariance = settings.value(Consts::SETTING_POSTHOC_LIMIT_RMS_FLAG).toBool();
    bool cb_minFixation = settings.value(Consts::SETTING_POSTHOC_MIN_DURATION_FLAG).toBool();
    double sliderVelocityVariance = settings.value(Consts::SETTING_POSTHOC_LIMIT_RMS_VAL).toDouble();
    double sliderMinFixation = settings.value(Consts::SETTING_POSTHOC_MIN_DURATION_VAL).toDouble();

    // Merge all fixations with a displacement lower than the displacement threshold
    if (cb_displacement)
        GPMatrixFunctions::fncMergeDisplacementThreshold(&RoughM, &SmoothM, &AutoFixAllM,  settingsPath);


    // Remove all fixations below the minimun variance
    if (cb_velocityVariance)
        GPMatrixFunctions::fncRemoveHighVarianceFixations( &SmoothM,  &AutoFixAllM, sliderVelocityVariance);


    // Remove all the fixations below the minimun fixation
    if (cb_minFixation)
        GPMatrixFunctions::fncRemoveMinFixations(&AutoFixAllM, &SmoothM, sliderMinFixation);


 }

 void GPMatrixFunctions::smoothRoughMatrix(const mat &RoughM, mat &SmoothM, QString settingsPath, GPMatrixProgressBar &gpProgressBar)
{
     QSettings settings(settingsPath, QSettings::IniFormat);

     if(!settings.contains(Consts::SETTING_SMOOTHING_USE_OTHER_EYE))
     {
         //TODO: Handle errors with user
         return;
     }
     bool copy_eyes = settings.value(Consts::SETTING_SMOOTHING_USE_OTHER_EYE).toBool();
     int expWidth = settings.value(Consts::SETTING_EXP_WIDTH).toInt();
     int expHeight = settings.value(Consts::SETTING_EXP_HEIGHT).toInt();

    // We cut the big file and smooth it by segments, as otherwise it is too big to process and the computer goes crazy.
    int numSegments = (int)(RoughM.n_rows / Consts::SMOOTH_MAXITS) + 1;

    gpProgressBar.beginProcessing("Smoothing Data...",numSegments);

    mat cutM;
    mat smoothedM;

    SmoothM.zeros(RoughM.n_rows, 4);

    uword startIndex, stopIndex;

    for (int j= 1; j <= numSegments; ++j)
    {
        gpProgressBar.updateProgressDialog(j, QString("Smoothing segment number %1 of %2...").arg(j).arg(numSegments));

        if (_want_to_close || gpProgressBar.wasCanceled())
        {
            SmoothM.clear();
            gpProgressBar.endProcessing();
            return;
        }

        startIndex = (j-1) * Consts::SMOOTH_MAXITS ;
        stopIndex =  (j) * Consts::SMOOTH_MAXITS ;
        if (stopIndex > RoughM.n_rows)
            stopIndex = RoughM.n_rows-1;
        cutM =  RoughM.submat(startIndex, 0, stopIndex -1 , RoughM.n_cols -1 );

        //smoothSegment(cutM,smoothedM, copy_eyes);
        SmoothM.rows(startIndex,stopIndex) = smoothSegment(cutM, copy_eyes, expWidth, expHeight);
        SmoothM.rows(startIndex,stopIndex).col(0) = RoughM.submat(startIndex, 0, stopIndex  , 0 );
    }

    // Now, create the new rows for the flags:
    mat newCol = zeros(SmoothM.n_rows,6); // velocity, isSaccadeFlag, INterpolationFlag ,Displacement,  velocity variance,    min fixations
    SmoothM = join_rows(SmoothM,newCol);

    /*for (uword rowi = 0; rowi < SmoothM.n_rows; rowi++ )
    {
        double x = SmoothM.at(rowi,2);
        double y = SmoothM.at(rowi,3);
        if (x == 0)
            SmoothM.at(rowi,2) = -1;
        else if (x == datum::nan)
            SmoothM.at(rowi,2) = -1;
        else
            SmoothM.at(rowi,2) = x / expWidth;

        if (y == 0)
            SmoothM.at(rowi,3) = -1;
        else if (x == datum::nan)
            SmoothM.at(rowi,3) = -1;
        else
            SmoothM.at(rowi,3) = y / expHeight;
    }*/
}

 void GPMatrixFunctions::smoothRoughMatrixFBF(const mat &RoughM, const QString path, const GrafixConfiguration &configuration, mat *SmoothM, GPMatrixProgressBar *gpProgressBar)
 {
     gpProgressBar->beginProcessing("Smoothing Data...",50);
     smoothRoughMatrixFBF(RoughM, path, configuration, SmoothM);
     gpProgressBar->endProcessing();
 }

 void GPMatrixFunctions::smoothRoughMatrixFBF(const mat &RoughM, const QString path, const GrafixConfiguration &configuration, mat *SmoothM)
 {
      if(RoughM.is_empty())
      {
          //TODO: Handle errors with user
          return;
      }

      GrafixSettingsLoader loader(path, configuration);


      bool copy_eyes = loader.LoadSetting(Consts::SETTING_SMOOTHING_USE_OTHER_EYE).toBool();


      int expWidth = loader.LoadSetting(Consts::SETTING_EXP_WIDTH).toInt();
      int expHeight = loader.LoadSetting(Consts::SETTING_EXP_HEIGHT).toInt();
      double sigma_r = loader.LoadSetting(Consts::SETTING_SMOOTHING_SIGMA_R).toDouble();
      double sigma_s = loader.LoadSetting(Consts::SETTING_SMOOTHING_SIGMA_S).toDouble();

      double Xsigma_r = sigma_r / expWidth;
      double Ysigma_r = sigma_r / expHeight;
     SmoothM->zeros(RoughM.n_rows, 4);

     typedef Array_2D<double> image_type;

     image_type image_X(RoughM.n_rows,1);
     image_type image_Y(RoughM.n_rows,1);

     image_type filtered_X(RoughM.n_rows,1);
     image_type filtered_Y(RoughM.n_rows,1);

     //copy eyes if necessary
     for (uword i = 0; i < RoughM.n_rows; ++i)
     {
         if (RoughM(i,2) == -1 && RoughM(i,4) == -1){
             image_X(i,0) = 0;
             image_Y(i,0) = 0;
         }else if (copy_eyes && RoughM(i,2)!= -1 && RoughM(i,4) == -1 ) { // Left eyes were detected but not right
             image_X(i,0) = RoughM(i,2);
             image_Y(i,0) = RoughM(i,3);
         }else if(copy_eyes && RoughM(i,2) == -1 && RoughM(i,4) != -1){  // Right eyes were detected, not left
             image_X(i,0) = RoughM(i,4);
             image_Y(i,0) = RoughM(i,5);
         }else if(!copy_eyes && ((RoughM(i,2)!= -1 && RoughM(i,4) == -1) || (RoughM(i,4)!= -1 && RoughM(i,2) == -1) )){
             image_X(i,0) = 0;
             image_Y(i,0) = 0;
         }else{   // Both eyes were detected
             image_X(i,0) = (RoughM(i,2) + RoughM(i,4))/2;  // x
             image_Y(i,0) = (RoughM(i,3) + RoughM(i,5))/2;  // y
         }
     }

     Image_filter::fast_LBF(image_X,image_X,sigma_s,Xsigma_r,false,&filtered_X,&filtered_X);
     Image_filter::fast_LBF(image_Y,image_Y,sigma_s,Ysigma_r,false,&filtered_Y,&filtered_Y);

     SmoothM->zeros(RoughM.n_rows,10);
     for (uword i = 0; i < RoughM.n_rows; ++i)
     {
         SmoothM->at(i,0) = RoughM.at(i,0);
         SmoothM->at(i,2) = filtered_X.at(i,0) * expWidth;
         SmoothM->at(i,3) = filtered_Y.at(i,0) * expHeight;
     }
 }

 void GPMatrixFunctions::interpolateData(mat &SmoothM, QString settingsPath, GPMatrixProgressBar &gpProgressBar)
 { // Here we interpolate the smoothed data and create an extra column
     // Smooth = [time,0,x,y,velocity,saccadeFlag(0,1), interpolationFlag]

     if (SmoothM.is_empty())
         return; //TODO Error

     gpProgressBar.beginProcessing("Interpolating Data", 100);
     //can use different settingsPaths so long as they have the required settings!
     QSettings settings(settingsPath, QSettings::IniFormat);

     if(!settings.contains(Consts::SETTING_INTERP_LATENCY) |
        !settings.contains(Consts::SETTING_HZ) |
        !settings.contains(Consts::SETTING_INTERP_MAXIMUM_DISPLACEMENT) |
        !settings.contains(Consts::SETTING_INTERP_VELOCITY_THRESHOLD) |
        !settings.contains(Consts::SETTING_DEGREE_PER_PIX) |
        !settings.contains(Consts::SETTING_EXP_WIDTH))
     {
         //TODO: Handle errors with user
         return;
     }

     //double degreePerPixel       = settings.value(MyConstants::SETTING_DEGREE_PER_PIX).toDouble();
     double hz                   = settings.value(Consts::SETTING_HZ).toDouble();
     double interpolationLatency = settings.value(Consts::SETTING_INTERP_LATENCY).toDouble();
     double displacInterpolation = settings.value(Consts::SETTING_INTERP_MAXIMUM_DISPLACEMENT).toDouble();
     //double velocity             = settings.value(MyConstants::SETTING_INTERP_VELOCITY_THRESHOLD).toDouble();
     //int    invalidSamples       = MyConstants::INVALID_SAMPLES; // 3 maximun invalid samples
     int    gapLenght            = interpolationLatency * hz / 1000;
     //int    expWidth             = settings.value(MyConstants::SETTING_EXP_WIDTH).toInt();

     int indexPrevSacc, indexNextSacc, indexDataBack;
     double euclideanDisPrev, euclideanDisNext = -1, xAvg, yAvg;

     mat aux2, aux3;
     uvec fixIndex;

     if (SmoothM.n_rows < 1)
     {
         //TODO Error
         return;
     }
     // Remove previous interpolation and create the new coulmns
     mat aux = zeros(SmoothM.n_rows, 11);
     aux.cols(0,3) = SmoothM.cols(0,3);
     SmoothM = aux;

      // 1- Calculate provisional velocity for each point and flag the points avobe threshold
     //GPMatrixFunctions::fncCalculateVelocity(&SmoothM,invalidSamples,expWidth,velocity, degreePerPixel, hz);
    GPMatrixFunctions::fncCalculateVelocity(&SmoothM, settingsPath);
     // 2- Run the whole smooth file.
     for(uword i = 0; i < SmoothM.n_rows; ++i)
     {
         if ( SmoothM(i,2) < 1 && SmoothM(i,3) < 1){ // If there is a missing point,

             // Go to the previous saccade and calculate the average X and Y.
             indexPrevSacc = euclideanDisPrev = xAvg = yAvg = -1;
             for (int j = i; j > 0 ; --j){
                 if (SmoothM(j,5) == 1){
                     indexPrevSacc = j;
                     break;
                 }
             }

             if (indexPrevSacc != -1 && indexPrevSacc < (int)i-1){  // If there is a previous saccade we proceed
                 aux2 =  SmoothM.submat(indexPrevSacc, 2, i-1 , 3 ); // cut the data for the previous fixation to calculate euclidean distance

                 // If the euclidean distance of the previous fixation is similar to the one for the next fixation, it may be the same fixation.

                 fixIndex =  arma::find(aux2.col(0) > 0); // Remove the -1 values for doing this calculation!!
                 //TODO: possible check we have found some... also if fail then error setting col 5?
                 aux3 = aux2.rows(fixIndex);
                 xAvg = mean(aux3.col(0)); // x
                 yAvg = mean(aux3.col(1)); // y
                 euclideanDisPrev = GPMatrixFunctions::fncCalculateEuclideanDistanceSmooth(&aux3);

                 // Find the point where de data is back:
                 indexDataBack = -1;
                 for (uword j = i; j < SmoothM.n_rows; ++j){
                     if (SmoothM(j,2) > 0 && SmoothM(j,3) > 0){
                         indexDataBack = j ;
                         break;
                     }
                 }

                 if (indexDataBack != -1){ // if the data comes back at some point

                     // find the next saccade. Start counting from  indexDataBack+2 , as the first point after missing data can be identified as saccade by mistake.
                     indexNextSacc = -1;
                     for (uword j = indexDataBack+2 ; j < SmoothM.n_rows; ++j){
                         if (SmoothM(j,5) == 1 ){ // Saccade!
                             indexNextSacc = j;
                             break;
                         }
                     }

                     // if there is a next saccade:
                     if (indexNextSacc != -1 && indexDataBack + 2 < indexNextSacc){
                         aux2 =  SmoothM.submat(indexDataBack+2, 2, indexNextSacc , 3 ); // cut the data for the next fixation to calculate euclidean distance
                         fixIndex =  arma::find(aux2.col(0) > 0); // Remove the -1 values for doing this calculation!!
                         //todo: here again check it found any
                         aux3 = aux2.rows(fixIndex);
                         euclideanDisNext = GPMatrixFunctions::fncCalculateEuclideanDistanceSmooth(&aux3);
                     }

                     // If the eucluclidean distance is similar (less than "displacInterpolation") at both ends and the gap is smaller than the latency, interpolate!
                     if (euclideanDisNext != -1 && sqrt((euclideanDisNext - euclideanDisPrev)* (euclideanDisNext - euclideanDisPrev)) <= displacInterpolation && indexDataBack - (int)i < gapLenght){ // INTERPOLATE!!!
                     //    cout << "**** Interpolate from  :" << i  << " too:" << indexDataBack << endl;

                         SmoothM.rows(i,indexDataBack).col(2).fill(xAvg);
                         SmoothM.rows(i,indexDataBack).col(3).fill(yAvg);
                         SmoothM.rows(i,indexDataBack).col(6).fill(1);  //Interpolation index!

                         GPMatrixFunctions::fncCalculateVelocity(&SmoothM,settingsPath); // Update.

                     }

                     i = indexDataBack;
                 }

             }
         }

         gpProgressBar.endProcessing();
     }

 }

 // Smoothing algorithm
 // Bilateral filtering algorithm based on Ed Vul (Frank, Vul, & Johnson,2009; based on Durand & Dorsey, 2002).
 // THis algorithm eliminates Jitter in fixations while preserving saccades.
 void GPMatrixFunctions::smoothSegment(mat &cutM, mat &smoothedM, bool copy_eyes){
    const int n_rows = cutM.n_rows+1;
    smoothedM = zeros(n_rows, 4); // [miliseconds, 0, x, y]

    mat allX =  zeros(n_rows,1);
    mat allY =  zeros(n_rows,1);

    // When there are no values for one eye, copy the values from the other eye.
    // Calculate X and Y initial averages. Exclude when the eyes were not detected
    for (uword i = 0; i < cutM.n_rows; ++i){  // Eyes were not detected
        if (cutM.at(i,2) == -1 && cutM.at(i,4) == -1){
            allX.at(i,0) = 0;
            allY.at(i,0) = 0;
        }else if (copy_eyes && cutM.at(i,2)!= -1 && cutM.at(i,4) == -1 ) { // Left eyes were detected but not right
            allX.at(i,0) = cutM.at(i,2);
            allY.at(i,0) = cutM.at(i,3);
        }else if(copy_eyes && cutM.at(i,2) == -1 && cutM.at(i,4) != -1){  // Right eyes were detected, not left
            allX.at(i,0) = cutM.at(i,4);
            allY.at(i,0) = cutM.at(i,5);
        }else if(!copy_eyes && ((cutM.at(i,2)!= -1 && cutM.at(i,4) == -1) || (cutM.at(i,4)!= -1 && cutM.at(i,2) == -1) )){
            allX.at(i,0) = 0;
            allY.at(i,0) = 0;
        }else{   // Both eyes were detected
            allX.at(i,0) = (cutM.at(i,2) + cutM.at(i,4))/2;  // x
            allY.at(i,0) = (cutM.at(i,3) + cutM.at(i,5))/2;  // y
        }
    }

    mat x1 = zeros(n_rows, n_rows);
    mat y1 = zeros(n_rows, n_rows);
    mat iw = zeros(n_rows, n_rows);
    mat iw2 = zeros(n_rows,n_rows);
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
    for (uword i = 0; i < (uword)n_rows; ++i){
         x1(span::all, i).fill(allX(i,0));  // Vertical columns with the data
         y1(span::all, i).fill(allY(i,0));
         t1(span::all, i).fill(i);  // Vertical columns with the data
         //t2(i, span::all).fill(i);  // Horizontal rows with the data.
    }

    //x2 = x1.t();

    // iw = exp(-(x1-x2).^2./(2.*iscales.^2));
    iw = exp(-square(x1-x1.t()) / (2 * Consts::SMOOTH_ISCALES * Consts::SMOOTH_ISCALES));
    iw2 = exp(-square(y1-y1.t()) / (2 * Consts::SMOOTH_ISCALES * Consts::SMOOTH_ISCALES));

    // [t1 t2] = meshgrid((1:length(allxds)), (1:length(allxds)));
    dw = exp(-square(t1-t1.t()) / (2 * Consts::SMOOTH_SCALES * Consts::SMOOTH_SCALES));


    // tw = iwa.*iwa2.*iw .* iw2 .* dw;
    tw =  (iw % iw2) % dw;

    // Get only the ones that are not NaN
    uvec goodX =  arma::find(allX != datum::nan);
    uvec goodY =  arma::find(allY != datum::nan);

    mat smx = zeros(1,n_rows);
    smx.fill(datum::nan);

    mat smy = zeros(1,n_rows);
    smy.fill(datum::nan);

    smx.elem(goodX) = sum(((mat)(tw(goodX,goodX) % repmat(allX.elem(goodX),1, ((mat)allX.elem(goodX)).n_rows))).t(), 1) / sum( tw(goodX,goodX) , 1);
    smy.elem(goodY) = sum(((mat)(tw(goodY,goodY) % repmat(allY.elem(goodY),1, ((mat)allY.elem(goodY)).n_rows))).t(), 1) / sum( tw(goodY,goodY) , 1);

    smoothedM.col(2) = smx.t();
    smoothedM.col(3) = smy.t();
}

 mat GPMatrixFunctions::smoothSegment(mat &cutM, bool copy_eyes, int expWidth, int expHeight)
 {
     mat data = zeros(cutM.n_rows + 1, 4); // [miliseconds, 0, x, y]

     // When there are no values for one eye, copy the values from the other eye.
     // Calculate X and Y initial averages. Exclude when the eyes were not detected
     for (uword i = 0; i < cutM.n_rows; ++i){  // Eyes were not detected
         if (cutM(i,2) == -1 && cutM(i,4) == -1){
             data(i,2) = 0;
             data(i,3) = 0;
         }else if (copy_eyes && cutM(i,2)!= -1 && cutM(i,4) == -1 ) { // Left eyes were detected but not right
             data(i,2) = cutM(i,2) * expWidth;
             data(i,3) = cutM(i,3) * expHeight;
         }else if(copy_eyes && cutM(i,2) == -1 && cutM(i,4) != -1){  // Right eyes were detected, not left
             data(i,2) = cutM(i,4) * expWidth;
             data(i,3) = cutM(i,5) * expHeight;
         }else if(!copy_eyes && ((cutM(i,2)!= -1 && cutM(i,4) == -1) || (cutM(i,4)!= -1 && cutM(i,2) == -1) )){
             data(i,2) = 0;
             data(i,3) = 0;
         }else{   // Both eyes were detected
             data(i,2) = (cutM(i,2) + cutM(i,4))/2 * expWidth;  // x
             data(i,3) = (cutM(i,3) + cutM(i,5))/2  * expHeight;  // y
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
     for (uword i = 0; i < allX.n_rows; ++i){
          x1(span::all, i).fill(allX(i,0));  // Vertical columns with the data
          x2(i, span::all).fill(allX(i,0));  // Horizontal rows with the data.
     }

     // iw = exp(-(x1-x2).^2./(2.*iscales.^2));
     iw = exp(-square(x1-x2) / (2 * Consts::SMOOTH_ISCALES * Consts::SMOOTH_ISCALES));

     for (uword i = 0; i < allY.n_rows; ++i){
          x1(span::all, i).fill(allY(i,0));  // Vertical columns with the data
          x2(i, span::all).fill(allY(i,0));  // Horizontal rows with the data.
     }
     iw2 = exp(-square(x1-x2) / (2 * Consts::SMOOTH_ISCALES * Consts::SMOOTH_ISCALES));

     // [t1 t2] = meshgrid((1:length(allxds)), (1:length(allxds)));
     for (uword i = 0; i < allX.n_rows; ++i){
          t1(span::all, i).fill(i);  // Vertical columns with the data
          t2(i, span::all).fill(i);  // Horizontal rows with the data.
     }
     dw = exp(-square(t1-t2) / (2 * Consts::SMOOTH_SCALES * Consts::SMOOTH_SCALES));


     // tw = iwa.*iwa2.*iw .* iw2 .* dw;
     tw =  (iw % iw2) % dw;

     // Get only the ones that are not NaN
     uvec goodX =  arma::find(allX != datum::nan);
     uvec goodY =  arma::find(allY != datum::nan);

     mat smx = zeros(1,allX.n_rows);
     smx.fill(datum::nan);

     mat smy = zeros(1,allY.n_rows);
     smy.fill(datum::nan);

     smx.elem(goodX) = sum(((mat)(tw(goodX,goodX) % repmat(allX.elem(goodX),1, ((mat)allX.elem(goodX)).n_rows))).t(), 1) / sum( tw(goodX,goodX) , 1);
     smy.elem(goodY) = sum(((mat)(tw(goodY,goodY) % repmat(allY.elem(goodY),1, ((mat)allY.elem(goodY)).n_rows))).t(), 1) / sum( tw(goodY,goodY) , 1);

     data.col(2) = smx.t();
     data.col(3) = smy.t();

     return data;

 }

 /*/ Root mean square (RMS)
 double GPMatrixFunctions::fncCalculateRMSRough(mat *p_a, int expWidth, int expHeight, double degree_per_pixel)
 { // a = [ leftx, lefty, rightx, righty]

     if (p_a->n_rows == 0 )
         return 0;

     // Calculate mean euclidean distance.
     mat b = zeros(p_a->n_rows);
     for (uword j = 0; j < p_a->n_rows-1 ; ++j){
         // Here, we are calculating the RMS
        // b(j) =( ((((a(j,0) + a(j,2))/2 * expWidth) - xAvg ) * (((a(j,0) + a(j,2))/2 * expWidth) - xAvg ) + (((a(j,1) + a(j,3))/2 * expHeight) - yAvg ) * (((a(j,1) + a(j,3))/2 * expHeight) - yAvg ))/2);
         b(j) = pow(expWidth * degree_per_pixel * ((((p_a->at(j,0) + p_a->at(j,2))/2)) - ((((p_a->at(j+1,0) + p_a->at(j+1,2))/2)))),2); // X
         b(j) += pow(expHeight * degree_per_pixel * ((((p_a->at(j,1) + p_a->at(j,3))/2)) - ((((p_a->at(j+1,1) + p_a->at(j+1,3))/2)))),2); // Y
         b(j) = b(j) / 2;
     }

     return sqrt(mean(mean(b)));
 }*/

 // Root mean square (RMS)
 double GPMatrixFunctions::fncCalculateRMSRough(mat *p_a, int expWidth, int expHeight, double degPerPixel){ // a = [ leftx, lefty, rightx, righty]

     if (p_a->n_rows == 0 )
         return 0;

     // Calculate mean euclidean distance.
     mat b = zeros(p_a->n_rows);
     for (uword j = 0; j < (*p_a).n_rows-1 ; ++j){
         // Here, we are calculating the RMS
        // b(j) =( ((((a(j,0) + a(j,2))/2 * expWidth) - xAvg ) * (((a(j,0) + a(j,2))/2 * expWidth) - xAvg ) + (((a(j,1) + a(j,3))/2 * expHeight) - yAvg ) * (((a(j,1) + a(j,3))/2 * expHeight) - yAvg ))/2);
         b(j) = (((((*p_a)(j,0) + (*p_a)(j,2))/2 * expWidth) * degPerPixel) - (((((*p_a)(j+1,0) + (*p_a)(j+1,2))/2 * expWidth) * degPerPixel))) * (((((*p_a)(j,0) + (*p_a)(j,2))/2 * expWidth) * degPerPixel) - (((((*p_a)(j+1,0) + (*p_a)(j+1,2))/2 * expWidth) * degPerPixel)))    ; // X
         b(j) = b(j)  + (((((*p_a)(j,1) + (*p_a)(j,3))/2 * expHeight) * degPerPixel) - (((((*p_a)(j+1,1) + (*p_a)(j+1,3))/2 * expHeight) * degPerPixel))) * (((((*p_a)(j,1) + (*p_a)(j,3))/2 * expHeight) * degPerPixel) - (((((*p_a)(j+1,1) + (*p_a)(j+1,3))/2 * expHeight) * degPerPixel))); // Y
         b(j) = b(j) / 2;
     }

     return sqrt(mean(mean(b)));
 }

 double GPMatrixFunctions::fncCalculateEuclideanDistanceSmooth(mat *p_a){

     double xAvg = mean(p_a->col(0)); // x
     double yAvg = mean(p_a->col(1)); // y

     // Calculate mean euclidean distance.
     mat b = zeros(p_a->n_rows);
     for (uword j = 0; j < p_a->n_rows ; ++j){
         b(j) =sqrt((((p_a->at(j,0) - xAvg ) * (p_a->at(j,0) - xAvg )) + ( (p_a->at(j,1) - yAvg ) * (p_a->at(j,1) - yAvg )))/2);
     }
     return (mean(mean(b)));
 }

 void GPMatrixFunctions::fncRemoveUndetectedValuesRough(mat *p_a){ // Removes the rows where any of the eyes were detected
     // Remove the rows where there was no valid data.
     // When there are no values for one eye, copy the values from the other eye.
     // Calculate X and Y initial averages. Exclude when the eyes were not detected
     for (uword i = 0; i < p_a->n_rows; ++i){  // Eyes were not detected
         if (p_a->at(i,0)!= -1 && p_a->at(i,2) == -1 ) { // Left eyes were detected but not right
             p_a->at(i,2) = p_a->at(i,0);
             p_a->at(i,3) = p_a->at(i,1);
         }else if(p_a->at(i,0) == -1 && p_a->at(i,2) != -1){  // Right eyes were detected, not left
             p_a->at(i,0) = p_a->at(i,2);
             p_a->at(i,1) = p_a->at(i,3);
         }
     }

     // Only use the points where eyes were detected:
     uvec fixIndex =  arma::find(p_a->col(0) != -1);
     if (!fixIndex.is_empty()){
         (*p_a) = p_a->rows(fixIndex);
     }

     if (p_a->n_rows == 0)
         (*p_a) = zeros(1,4);
 }


 void GPMatrixFunctions::fncCalculateVelocity(mat *p_smoothM, QString settingsPath)
 {
     QSettings settings(settingsPath, QSettings::IniFormat);

     if(!settings.contains(Consts::SETTING_EXP_WIDTH) |
        !settings.contains(Consts::SETTING_EXP_HEIGHT) |
        !settings.contains(Consts::SETTING_DEGREE_PER_PIX) |
        !settings.contains(Consts::SETTING_HZ) |
        !settings.contains(Consts::SETTING_INTERP_VELOCITY_THRESHOLD))
     {
         //TODO: Handle error
         return;
     }

     int invalidSamples      = Consts::INVALID_SAMPLES;
     int expWidth            = settings.value(Consts::SETTING_EXP_WIDTH).toInt();
     //int expHeight           = settings.value(MyConstants::SETTING_EXP_HEIGHT).toInt();
     double velocity         = settings.value(Consts::SETTING_INTERP_VELOCITY_THRESHOLD).toDouble();
     double degreePerPixel   = settings.value(Consts::SETTING_DEGREE_PER_PIX).toDouble();
     int hz                  = settings.value(Consts::SETTING_HZ).toInt();


     mat aux;
     double minC, maxC, amp, vel;

     if (p_smoothM->n_cols < 5){
         mat aux = zeros(p_smoothM->n_rows, 11);
         aux.cols(0,3) = p_smoothM->cols(0,3);
         (*p_smoothM) = aux;
     }

     for (uword i = invalidSamples; i < p_smoothM->n_rows; ++i){
         // a) Check if any of the coordinates for this or the previous samples where invalid
         aux = p_smoothM->submat(i- invalidSamples, 2, i  , 3);
         minC = aux.min();   // We calculate the min and max of both x and y together. If one is not correct, the othet either.
         maxC = aux.max();

         if (minC > 1 && maxC < expWidth)
         { // b)The values are correct
             /*
             double xDist = expWidth * (p_smoothM->at(i-1,2) - p_smoothM->at(i,2));
             double yDist = expHeight * (p_smoothM->at(i-1,3) - p_smoothM->at(i,3));

             amp = sqrt(pow(xDist, 2) + pow (yDist, 2) );*/
             amp = sqrt(((((*p_smoothM)(i-1,2) - (*p_smoothM)(i,2)) * ((*p_smoothM)(i-1,2) - (*p_smoothM)(i,2))) + (((*p_smoothM)(i-1,3) - (*p_smoothM)(i,3))*((*p_smoothM)(i-1,3) - (*p_smoothM)(i,3))) ) /2);

             vel = (amp *hz) * degreePerPixel ;
             p_smoothM->at(i,4) = vel;
             if (vel >= velocity ){ // If the velocity if higher than threshold
                 p_smoothM->at(i,5) = 1; // is saccade
             }else{
                 p_smoothM->at(i,5) = 0;
             }
         }

     }
 }

 void GPMatrixFunctions::fncCalculateFixations(mat *p_fixAllM, mat *p_roughM, mat *p_smoothM, QString settingsPath)
 {
     QSettings settings(settingsPath, QSettings::IniFormat);

     if(!settings.contains(Consts::SETTING_EXP_WIDTH) |
        !settings.contains(Consts::SETTING_EXP_HEIGHT) |
        !settings.contains(Consts::SETTING_DEGREE_PER_PIX) |
        !settings.contains(Consts::SETTING_HZ) |
        !settings.contains(Consts::SETTING_INTERP_VELOCITY_THRESHOLD))
     {
         //TODO: Handle error
         return;
     }

     //int invalidSamples      = MyConstants::INVALID_SAMPLES;
     int expWidth            = settings.value(Consts::SETTING_EXP_WIDTH).toInt();
     int expHeight           = settings.value(Consts::SETTING_EXP_HEIGHT).toInt();
     //double velocity         = settings.value(MyConstants::SETTING_INTERP_VELOCITY_THRESHOLD).toDouble();
     double degreePerPixel   = settings.value(Consts::SETTING_DEGREE_PER_PIX).toDouble();
     //int hz                  = settings.value(MyConstants::SETTING_HZ).toInt();

     //clear fixall matrix
     (*p_fixAllM).reset();

     int indexStartFix = -1;
     for(uword i = 0; i < p_smoothM->n_rows-1; i ++){
         if (p_smoothM->at(i,5) == 1 ){ // Saccade. Velocity over threshold
             if (indexStartFix == -1 && p_smoothM->at(i + 1,5) == 0){ // The next point is the start of a fixation
                 indexStartFix = i;
             }else if (indexStartFix != -1 ){ // End of the fixation. Create fixation!

                 double dur = ((p_roughM->at(i,0) - p_roughM->at(0,0))/ 1000 ) - ((p_roughM->at(indexStartFix,0) - p_roughM->at(0,0))/ 1000 );

                 // Only use the points where eyes were detected:
                 mat roughCutM;
                 if (p_roughM->n_cols == 8){
                     roughCutM= p_roughM->submat(indexStartFix,2,i,7);
                     fncRemoveUndetectedValuesRough(&roughCutM);
                 }else{
                     roughCutM= p_roughM->submat(indexStartFix,2,i,5);
                     fncRemoveUndetectedValuesRough(&roughCutM);
                 }

                 double averageX = mean((roughCutM.col(0) + roughCutM.col(2))/2);
                 double averageY = mean((roughCutM.col(1) + roughCutM.col(3))/2);
                 double variance = fncCalculateRMSRough(&roughCutM,expWidth,expHeight,degreePerPixel);
                 double pupilDilation = 0;
                 if (roughCutM.n_cols == 6){
                     pupilDilation = (mean(roughCutM.col(4)) + mean(roughCutM.col(5)))/2;
                 }

                 mat row;
                 row << indexStartFix << i << dur << averageX << averageY << variance << 0 << pupilDilation <<  endr ;

                 if (p_fixAllM->n_rows == 0){
                     (*p_fixAllM) = row;
                 }else {
                     (*p_fixAllM) = join_cols((*p_fixAllM), row);
                 }

                 i = i - 1; // Otherwise it doesn't capture some fixations that as very close together
                 indexStartFix = -1;
             }
         }else if(indexStartFix != -1  && p_smoothM->at(i,2) < 1 && p_smoothM->at(i,3) < 1 ){ // Remove fake fixations: When the data disapears, the saccade flag does nt get activated and it creates fake fixations.
             indexStartFix = -1;
         }
     }
 }

 void GPMatrixFunctions::fncReturnFixationinSegments(mat *p_fixAllM, mat *p_segmentsM){
     mat fixations;

     if (!p_segmentsM->is_empty()){  // If there are experimental segments
         int fixIndex = 0;

         // order the segments
         uvec indices = sort_index(p_segmentsM->cols(1,1));

         mat a = p_segmentsM->rows(indices);

         for (uword i = 0; i < a.n_rows; ++i){  // Find fixations for each fragment and copy them in a matrix
             int start = a(i,1);
             int end = a(i,2);
             // Go through the whole fixations matrix
             for (uword j = fixIndex; j < p_fixAllM->n_rows; ++j){
                 if ((p_fixAllM->at(j,0) >= start && p_fixAllM->at(j,0) <= end )|| (p_fixAllM->at(j,0) < end && p_fixAllM->at(j,1) > end)){  // fixations that start  and end in the segment of start in the segment and end in the next
                     fixations = join_cols(fixations, p_fixAllM->row(j));
                 }
             }

         }
     }
     if (!fixations.is_empty())
         (*p_fixAllM) = fixations;//POSSIBLE ERROR?
 }

 void GPMatrixFunctions::fncRemoveMinFixations(mat *p_fixAllM, mat *p_smoothM, double minDur){
     p_smoothM->col(9).fill(0); // Restart

     // Find all fixations we will delete
     uvec fixIndex =  arma::find(p_fixAllM->col(2) < (minDur)/1000);
     mat minFix = p_fixAllM->rows(fixIndex);

     for (uword i = 0; i < minFix.n_rows; ++i){  // Modify the flag for the fixations we delete
         p_smoothM->operator()(span(minFix(i,0),minFix(i,1)),span(9,9) ).fill(1);
     }

     // Delete min fixations
     fixIndex =  arma::find(p_fixAllM->col(2) > (minDur)/1000);
     (*p_fixAllM) = p_fixAllM->rows(fixIndex);

 }

 void GPMatrixFunctions::fncMergeDisplacementThreshold(mat *p_roughM, mat *p_smoothM, mat *p_fixAllM, QString settingsPath){
     if (p_fixAllM->n_rows < 3) return;
     QSettings settings(settingsPath, QSettings::IniFormat);

     if(!settings.contains(Consts::SETTING_EXP_WIDTH) |
        !settings.contains(Consts::SETTING_EXP_HEIGHT) |
        !settings.contains(Consts::SETTING_DEGREE_PER_PIX) |
        !settings.contains(Consts::SETTING_HZ) |
        !settings.contains(Consts::SETTING_INTERP_VELOCITY_THRESHOLD))
     {
         //TODO: Handle error
         return;
     }

     //int invalidSamples      = MyConstants::INVALID_SAMPLES;
     int expWidth            = settings.value(Consts::SETTING_EXP_WIDTH).toInt();
     int expHeight           = settings.value(Consts::SETTING_EXP_HEIGHT).toInt();
     //double velocity         = settings.value(MyConstants::SETTING_INTERP_VELOCITY_THRESHOLD).toDouble();
     double degreePerPixel   = settings.value(Consts::SETTING_DEGREE_PER_PIX).toDouble();
     int hz                  = settings.value(Consts::SETTING_HZ).toInt();
     double displacement     = settings.value(Consts::SETTING_POSTHOC_MERGE_CONSECUTIVE_VAL).toDouble();

     double dur, averageX, averageY, variance, pupilDilation, xDist1, yDist1,xDist2, yDist2;

     int gapLenght = 50 * hz / 1000; // to merge, the distance between one fixation and the next is less than 50 ms

     mat aux, roughCutM, f1,f2;

     uvec fixIndex;


     p_smoothM->col(7).fill(0); // Restart

     for (uword i = 0; i < p_fixAllM->n_rows-2; ++ i){

        // Recalculate euclidean distances for both fixations, using the smoothed data.
         aux =  p_smoothM->submat(p_fixAllM->at(i,0), 2, p_fixAllM->at(i,1) , 3 ); // cut the data for the next fixation to calculate euclidean distance
         fixIndex =  arma::find(aux.col(0) > 1); // Remove the -1 values for doing this calculation!!
         aux = aux.rows(fixIndex);
         if (!aux.is_empty()){
             xDist1 = mean(aux.col(0));
             yDist1 = mean(aux.col(1));
         }else{
             xDist1 = yDist1 = 1000;
         }

         aux =  p_smoothM->submat(p_fixAllM->at(i+1,0), 2, p_fixAllM->at(i+1,1) , 3 ); // cut the data for the next fixation to calculate euclidean distance
         fixIndex =  arma::find(aux.col(0) > 1); // Remove the -1 values for doing this calculation!!
         aux = aux.rows(fixIndex);
         if (!aux.is_empty()){
             xDist2 = mean(aux.col(0));
             yDist2 = mean(aux.col(1));
         }else{
             xDist2 = yDist2 = 0;
         }
         // Merge if: The displacement from fixation 1 and fixation 2 is less than "displacement" AND
         // if the distance between fixation 1 and fixation 2 is less than 50 ms .
         if (sqrt((xDist1 -  xDist2)* (xDist1 - xDist2)) * degreePerPixel <= displacement && sqrt((yDist1 -  yDist2)* (yDist1 - yDist2)) * degreePerPixel <= displacement  // if the euclidean distance between the two fixations is lower than the threshold, we merge!!
             && p_fixAllM->at(i+1,0) - p_fixAllM->at(i,1) <= gapLenght){
             // INdicate the flag
             p_smoothM->operator()(span(p_fixAllM->at(i,0),p_fixAllM->at(i+1,1)),span(7, 7) ).fill(1);

             // MERGE!
             dur = ((p_roughM->at(p_fixAllM->at(i+1,1),0) - p_roughM->at(0,0))/ 1000 ) - ((p_roughM->at(p_fixAllM->at(i,0),0) - p_roughM->at(0,0))/ 1000 );

             // Only use the points where eyes were detected:
             if (p_roughM->n_cols == 8){  // Depens on if we have pupil dilation data or not!
                 roughCutM= p_roughM->submat(p_fixAllM->at(i,0),2,p_fixAllM->at(i+1,1),7);
                 fncRemoveUndetectedValuesRough(&roughCutM);
             }else{
                 roughCutM= p_roughM->submat(p_fixAllM->at(i,0),2,p_fixAllM->at(i+1,1),5);
                 fncRemoveUndetectedValuesRough(&roughCutM);
             }

             averageX = mean((roughCutM.col(0) + roughCutM.col(2))/2);
             averageY = mean((roughCutM.col(1) + roughCutM.col(3))/2);
             variance = fncCalculateRMSRough(&roughCutM, expWidth,expHeight,degreePerPixel);
             pupilDilation = 0;
             if (roughCutM.n_cols == 6){
                 pupilDilation = (mean(roughCutM.col(4)) + mean(roughCutM.col(5)))/2;
             }

             p_fixAllM->at(i,1) = p_fixAllM->at(i+1,1);
             p_fixAllM->at(i,2) = dur;
             p_fixAllM->at(i,3) = averageX;
             p_fixAllM->at(i,4) = averageY;
             p_fixAllM->at(i,5) = variance;
             p_fixAllM->at(i,6) = 0;
             p_fixAllM->at(i,7) = pupilDilation;

             f1 = p_fixAllM->operator()( span(0, i), span(0, p_fixAllM->n_cols-1) );
             if (i + 1 < p_fixAllM->n_rows-1){
                 f2 = p_fixAllM->operator()(span(i+2,p_fixAllM->n_rows-1),span(0, p_fixAllM->n_cols-1) );
                 (*p_fixAllM) = join_cols(f1,f2);
                  i = i-1;
             }else{
                 (*p_fixAllM) = f1;
             }

         }
     }
 }

 void GPMatrixFunctions::fncRemoveHighVarianceFixations(mat *p_smoothM, mat *p_fixAllM, double variance){

     if (p_fixAllM->n_rows<1) return;
     p_smoothM->col(8).fill(0); // Restart

     for (uword i = 0; i < p_fixAllM->n_rows-1; ++i){
         if (p_fixAllM->at(i,5) > variance ){ // Remove fixation
             p_smoothM->operator()(span(p_fixAllM->at(i,0),p_fixAllM->at(i,1)),span(8,8) ).fill(1); // Indicate it
         }
     }

     uvec fixIndex =  arma::find(p_fixAllM->col(5) < variance);
     if (!fixIndex.empty()){
         (*p_fixAllM) = p_fixAllM->rows(fixIndex);
     }

 }

 bool GPMatrixFunctions::saveFileSafe(mat &matrix, QString fileName)
 {
     try
     {
         return saveFile(matrix,fileName);
     }
     catch(...)
     {
         return false;
     }
 }

 /*bool GPMatrixFunctions::saveFile(mat &matrix, QString fileName)
 {
     bool result;
     std::string filename = fileName.toStdString();
     try
     {
         result = matrix.save(filename, csv_ascii);
         if (!result) throw std::runtime_error("Error saving file: " + filename);
     }
     catch(...)
     {
         throw std::runtime_error("Error saving file: " + filename);
     }

     return result;
 }*/

bool GPMatrixFunctions::saveFile(mat &matrix, QString fileName)
{
    std::string filename = fileName.toStdString();
    std::ofstream stream(filename.c_str(),std::ios::binary);
    uword nRows = matrix.n_rows;
    uword nCols = matrix.n_cols;
    if (!stream) return false;
    for (uword row = 0; row < nRows; ++row)
    {
        for (uword col = 0; col < (nCols-1); ++col)
        {
            stream << matrix.at(row,col);
            stream << ',';
        }
        stream << matrix.at(row,nCols-1);
        stream << endl;
    }
    stream.close();
    return true;
}
bool GPMatrixFunctions::readFileSafe(mat &matrix, QString fileName)
{
    try
    {
        return readFile(matrix,fileName);
    }
    catch(...)
    {
        return false;
    }
}

bool GPMatrixFunctions::readFile(mat &matrix, QString fileName)
{
     std::string filename = fileName.toStdString();

     double* squarematrix;
     unsigned char* buffer_;
     unsigned int buffer_size_;

     std::ifstream stream(filename.c_str(),std::ios::binary);

     if (stream) {

         stream.seekg (0,std::ios::end);
         buffer_size_ = static_cast<std::size_t>(stream.tellg());
         stream.seekg (0,std::ios::beg);

         if (0 != buffer_size_) {
             try {buffer_ = new unsigned char[buffer_size_];}
             catch (...){throw (std::runtime_error("Out of memory when loading: " + filename)); return false;}
             try {squarematrix = new double[buffer_size_/2];} //should have enough room
             catch (...){delete[] buffer_;throw (std::runtime_error("Out of memory when loading: " + filename)); return false;}
             unsigned int row = 0;
             unsigned int items = 0;

             try {
             stream.read(reinterpret_cast<char*>(buffer_),static_cast<std::streamsize>(buffer_size_));
             qDebug() << QString::fromStdString(filename) + " -Loaded";

             bool found_a_number = false;
             //parse
             for (unsigned int pos = 0; pos<buffer_size_; ++pos) {
                 if (_want_to_close) return false;
                 switch (buffer_[pos]) {
                 case ',':
                     found_a_number=false;
                     break;
                 case '\n':
                     found_a_number=false;
                     ++row;
                     break;
                 case '0': case '1':case '2': case '3': case '4':
                 case '5': case '6': case '7': case '8': case '9':
                 case '-': case '+':
                     if (!found_a_number) {  //start of a string
                         unsigned int move_on=0;
                         squarematrix[items++] = GPGeneralStaticLibrary::string_to_double_fast(buffer_+pos,move_on);
                         pos += move_on;
                         found_a_number=true;
                     }
                     break;
                 default:  //whitespace
                     break;
                 }
             }

             unsigned int n_cols = items / row;
             matrix = mat(squarematrix,n_cols,row).t();
                 } catch (...) {
                     delete[] buffer_;
                     delete[] squarematrix;
                     buffer_ = 0;
                     squarematrix = 0;
                     qDebug() << "Error loading file";
                     throw (std::runtime_error("Error parsing CSV: " + filename));
                     return false;
                 }
             delete[] buffer_;
             delete[] squarematrix;
             buffer_ = 0;
             squarematrix = 0;
         }
         stream.close();
         return true;
     }
     throw (std::runtime_error("Error loading: " + filename));
     return false;
}

int GPMatrixFunctions::fncGetMatrixColsFromFile(QString fullpath)
{
    std::string filename = fullpath.toStdString();

    unsigned char* buffer_;
    unsigned int buffer_size_;
    int n_cols = 0;

    std::ifstream stream(filename.c_str(),std::ios::binary);

    if (stream) {

        stream.seekg (0,std::ios::end);
        buffer_size_ = static_cast<std::size_t>(stream.tellg());
        stream.seekg (0,std::ios::beg);

        if (0 != buffer_size_) {
            try {buffer_ = new unsigned char[buffer_size_];}
            catch (...){return 0;}

            try {
            stream.read(reinterpret_cast<char*>(buffer_),static_cast<std::streamsize>(buffer_size_));
            qDebug() << QString::fromStdString(filename) + " -Loaded";

            bool found_a_number = false;
            bool found_a_row = false;
            unsigned int pos = 0;
            //parse
            while (!found_a_row && pos<buffer_size_)
            {
                switch (buffer_[pos])
                {
                case ',':
                    n_cols++;
                    found_a_number=false;
                    break;
                case '\n':
                    n_cols++;
                    found_a_row = true;
                    found_a_number=false;
                    break;
                case '0': case '1':case '2': case '3': case '4':
                case '5': case '6': case '7': case '8': case '9':
                case '-': case '+':
                    if (!found_a_number) {  //start of a string
                        unsigned int move_on=0;
                        GPGeneralStaticLibrary::string_to_double_fast(buffer_+pos,move_on);
                        pos += move_on;
                        found_a_number=true;
                    }
                    break;
                default:  //whitespace
                    break;
                }
                ++pos;
            }

            } catch (...) {
                delete[] buffer_;
                buffer_ = 0;
                qDebug() << "Error loading file";
                return 0;
            }
            delete[] buffer_;
            buffer_ = 0;
        }
        stream.close();
        return n_cols;
    }
    return 0;
}





