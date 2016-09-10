#ifndef GPMATRIXFUNCTIONS_H
#define GPMATRIXFUNCTIONS_H

#include "MyConstants.h"
#include "GrafixProject.h"
#include "GPGeneralStaticLibrary.h"
#include "GrafixSettingsLoader.h"
#include "GPMatrixProgressBar.h"
#include "GPMatrixFiles.h"

#include "armadillo"

#define NO_XML
#include "TRUNCATED_KERNEL_BF/include/fast_lbf.h"

using namespace arma;

class GrafixParticipant;
class GrafixProject;
class GrafixSettingsLoader;


namespace GPMatrixFunctions {

    //Smoothing
    void smoothRoughMatrixFBF(const mat &RoughM, const QString path, const GrafixConfiguration &configuration, mat *SmoothM, GPMatrixProgressBar *gpProgressBar);
    void smoothRoughMatrixFBF(const mat &RoughM, const QString path, const GrafixConfiguration &configuration, mat *SmoothM);


    //Interpolating
    void interpolateData(mat &SmoothM, GrafixSettingsLoader settingsLoader, GPMatrixProgressBar &gpProgressBar);


    //Fixations
    void estimateFixations(mat &RoughM, mat &SmoothM, mat &AutoFixAll, GrafixSettingsLoader &settingsLoader, GPMatrixProgressBar &gpProgressBar);
    void estimateFixations(mat &RoughM, mat &SmoothM, mat &AutoFixAll, GrafixSettingsLoader &settingsLoader);
    void fncCalculateFixations(mat &fixAllM, mat &roughM , mat &smoothM, GrafixSettingsLoader settingsLoader);
    void fncCalculateFixation(const mat &roughM, int startIndex, int endIndex, bool copy_eyes, int expWidth, int expHeight, double degPerPixel, mat &outFixation);


    //Saccades
    void fncCalculateSaccades(mat &saccadesM, mat &fixAllM, mat &smoothM, GrafixSettingsLoader settingsLoader);


    //Post-hoc
    void fncRemoveMinFixations(mat *p_fixAllM, mat *p_smoothM, double minDur);
    void fncMergeDisplacementThreshold(mat &roughM,  mat &smoothM, mat &fixAllM, GrafixSettingsLoader settingsLoader);
    void fncRemoveHighVarianceFixations(mat *p_smoothM, mat *p_fixAllM, double variance);


    // Utilities
    void prepareRoughMatrix(mat &preparedRoughM, const mat &RoughM, bool copy_eyes);
    void excludeMissingDataRoughMatrix(mat &cutRoughM, const mat &RoughM, bool copy_eyes);
    void debugPrintMatrix(mat &matrix);
    void fncRemoveUndetectedValuesRough(mat *p_a);
    void fncReturnFixationinSegments(mat *p_fixAllM, mat *p_segmentsM);


    // Calculations
    double fncCalculateEuclideanDistanceSmooth(mat *p_aux);
    double fncCalculateRMSRough(mat &p_aux, int expWidth, int expHeight, double degree_per_pixel, bool copy_eyes);
    double calculateRMSRaw(mat &preparedRoughM, int expWidth, int expHeight, double degPerPixel);
    void fncCalculateVelocity(mat &smoothM, GrafixSettingsLoader settingsLoader);

}



#endif // GPMATRIXFUNCTIONS_H
