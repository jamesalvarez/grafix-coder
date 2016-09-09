#ifndef GPMATRIXFUNCTIONS_H
#define GPMATRIXFUNCTIONS_H

#include "MyConstants.h"
#include "GrafixProject.h"
#include "GPGeneralStaticLibrary.h"
#include "GrafixSettingsLoader.h"


#include <QtGui>
#include <QtWidgets/QWidget>
#include <QtWidgets/QDialog>
#include <QProgressDialog>

#include "armadillo"

#define NO_XML
#include "TRUNCATED_KERNEL_BF/include/fast_lbf.h"
#include "Trilateral/Trilateral2003.h"

using namespace arma;

class GrafixParticipant;
class GrafixProject;
class GrafixSettingsLoader;


class GPMatrixProgressBar
{

public:
    GPMatrixProgressBar(QWidget* parent);
    ~GPMatrixProgressBar();

    bool             _use_progress_dialog;
    bool             _batch_processing;
    QProgressDialog*  _progress_dialog;
    QWidget*         _parent_widget;
    int              _current_process_progress;
    int              _current_process_end;
    int              _batch_number_tasks;
    int              _batch_current_task;

    void beginBatch(int number_batch_files);
    void beginProcessing(QString process_name, int process_length);
    void updateProgressDialog(int progress, QString label);
    void endProcessing();
    void endBatch();
    bool wasCanceled();
};

class GPMatrixFunctions
{
private:
    static bool             _want_to_close;
public:
    GPMatrixFunctions();

    static void interpolateData(mat &SmoothM, GrafixSettingsLoader settingsLoader, GPMatrixProgressBar &gpProgressBar);
    static void prepareRoughMatrix(mat &preparedRoughM, const mat &RoughM, bool copy_eyes);
    static void excludeMissingDataRoughMatrix(mat &cutRoughM, const mat &RoughM, bool copy_eyes);
    static void smoothRoughMatrix(const mat &RoughM, mat &SmoothM, QString settingsPath, GPMatrixProgressBar &gpProgressBar);
    static void smoothRoughMatrixFBF(const mat &RoughM, const QString path, const GrafixConfiguration &configuration, mat *SmoothM, GPMatrixProgressBar *gpProgressBar);
    static void smoothRoughMatrixFBF(const mat &RoughM, const QString path, const GrafixConfiguration &configuration, mat *SmoothM);

    static void fast_LBF(Array_2D<double> &image_X, double sigma_s, double Xsigma_r, bool b, Array_2D<double> *filtered_X);

    //These are a bit too slow at the moment but could be rewritten
    static void smoothRoughMatrixTrilateral(const mat &RoughM, GrafixSettingsLoader &settingsLoader, mat *SmoothM, GPMatrixProgressBar *gpProgressBar);
    static void smoothRoughMatrixTrilateral(const mat &RoughM, GrafixSettingsLoader &settingsLoader, mat *SmoothM);

    static void estimateFixations(mat &RoughM, mat &SmoothM, mat &AutoFixAll, GrafixSettingsLoader &settingsLoader, GPMatrixProgressBar &gpProgressBar);
    static void estimateFixations(mat &RoughM, mat &SmoothM, mat &AutoFixAll, GrafixSettingsLoader &settingsLoader);

    //file operations
    static bool saveFile(mat &matrix, QString fileName, QString headerString = "");
    static bool readFile(mat &matrix, QString fileName);
    static bool saveFileSafe(mat &matrix, QString fileName, QString headerString = "");
    static bool readFileSafe(mat &matrix, QString fileName);
    static int  fncGetMatrixColsFromFile(QString fullpath);
    static bool exportFile(mat &roughM, mat &smoothM, mat &fixAllM, QString filename, GrafixSettingsLoader &settingsLoader);

    //FicDurOperations
    static double fncCalculateEuclideanDistanceSmooth(mat *p_aux);
    static double fncCalculateRMSRough(mat &p_aux, int expWidth, int expHeight, double degree_per_pixel, bool copy_eyes);
    static double calculateRMSRaw(mat &preparedRoughM, int expWidth, int expHeight, double degPerPixel);
    static void fncCalculateVelocity(mat &smoothM, GrafixSettingsLoader settingsLoader);
    static void fncCalculateFixations(mat &fixAllM, mat &roughM , mat &smoothM, GrafixSettingsLoader settingsLoader);

    static void fncCalculateFixation(const mat &roughM, int startIndex, int endIndex, bool copy_eyes, int expWidth, int expHeight, double degPerPixel, mat &outFixation);

    static void debugPrintMatrix(mat &matrix);
    static void fncCalculateSaccades(mat &saccadesM, mat &fixAllM, mat &smoothM, GrafixSettingsLoader settingsLoader);

    static void fncRemoveUndetectedValuesRough(mat *p_a);
    static void fncReturnFixationinSegments(mat *p_fixAllM, mat *p_segmentsM);
    static void fncRemoveMinFixations(mat *p_fixAllM, mat *p_smoothM, double minDur);
    static void fncMergeDisplacementThreshold(mat &roughM,  mat &smoothM, mat &fixAllM, GrafixSettingsLoader settingsLoader);
    static void fncRemoveHighVarianceFixations(mat *p_smoothM, mat *p_fixAllM, double variance);

private:
    //Used by smoothRoughMatrix to smooth a section of raw matrix
    static void smoothSegment(mat &cutM, mat &smoothedM, bool copy_eyes);
    static mat smoothSegment(mat &cutM, bool copy_eyes, int expWidth, int expHeight);
};



#endif // GPMATRIXFUNCTIONS_H
