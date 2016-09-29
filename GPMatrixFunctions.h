#ifndef GPMATRIXFUNCTIONS_H
#define GPMATRIXFUNCTIONS_H

#include "MyConstants.h"
#include "GrafixProject.h"
#include "GPGeneralStaticLibrary.h"
#include "GrafixSettingsLoader.h"
#include "GPMatrixProgressBar.h"
#include "GPMatrixFiles.h"

#include "armadillo"



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
    void calculateFixations(mat &fixAllM, mat &roughM , mat &smoothM, GrafixSettingsLoader settingsLoader);
    void calculateFixation(const mat &roughM, int startIndex, int endIndex, bool copy_eyes, int expWidth, int expHeight, double degPerPixel, mat &outFixation);


    //Saccades
    void calculateSaccades(mat &saccadesM, const mat &fixAllM, const mat &smoothM, double degreesPerPixel);


    //Post-hoc
    void posthocRemoveMinFixations(mat *p_fixAllM, mat *p_smoothM, double minDur);
    void posthocMergeDisplacementThreshold(mat &roughM,  mat &smoothM, mat &fixAllM, GrafixSettingsLoader settingsLoader);
    void posthocRemoveHighVarianceFixations(mat *p_smoothM, mat *p_fixAllM, double variance);


    // Utilities
    void prepareRoughMatrix(mat &preparedRoughM, const mat &RoughM, bool copy_eyes);
    void excludeMissingDataRoughMatrix(mat &cutRoughM, const mat &RoughM, bool copy_eyes);
    void returnFixationinSegments(mat *p_fixAllM, mat *p_segmentsM);
    void debugPrintMatrix(mat &matrix);

}



#endif // GPMATRIXFUNCTIONS_H
