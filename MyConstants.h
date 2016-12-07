#ifndef MYCONSTANTS_H
#define MYCONSTANTS_H

#include <QPair>
#include <QString>
#include <QList>
#include <QVariant>

// Indexes for correct columns in fixation matrix
#define FIXCOL_START 0
#define FIXCOL_END 1
#define FIXCOL_DURATION 2
#define FIXCOL_AVERAGEX 3
#define FIXCOL_AVERAGEY 4
#define FIXCOL_RMS 5
#define FIXCOL_SMOOTH_PURSUIT 6
#define FIXCOL_PUPIL 7

// Indexes for correct columns in saccade matrix
#define SACCOL_START 0
#define SACCOL_END 1
#define SACCOL_DURATION 2
#define SACCOL_DISTANCE 3
#define SACCOL_VEL_AVG 4
#define SACCOL_VEL_PEAK 5

// Indexes for correct columns for segments matric
#define SEGCOL_ID 0
#define SEGCOL_START 1
#define SEGCOL_END 2


typedef struct config {
    QString first;
    int second;
    bool operator==(config a) {
        return (a.first == this->first && a.second == this->second);
    }
} GrafixConfiguration;

namespace Consts {

    enum BATCH_TASK {
        TASK_SMOOTH,
        TASK_INTERPOLATE,
        TASK_ESTIMATE_FIXATIONS,
        TASK_EXPORT
    };

    enum MATRIX_TYPE {
        MATRIX_ROUGH = 0,
        MATRIX_SMOOTH = 1,
        MATRIX_FIXALL = 2,
        MATRIX_AUTOFIXALL = 3,
        MATRIX_SEGMENTS = 4
    };

    enum EXPORT_TYPE {
        EXPORT_TIMESTAMPED = 0,
        EXPORT_FIXATIONS = 1,
        EXPORT_SACCADES = 2
    };


    const GrafixConfiguration ACTIVE_CONFIGURATION();
    const QList<QString> LIST_CONFIGURATION_SETTINGS();
    const QVariant DefaultSetting(QString setting);

    const int DEFAULT_SETTING_HZ = 120;
    const int DEFAULT_SETTING_SECS_FRAGMENT = 15;

    const int DEFAULT_SETTING_EXP_WIDTH = 1024;  // Size of the experimental screen
    const int DEFAULT_SETTING_EXP_HEIGHT = 768;

    const int BOX_WIDTH = 850;
    const int BOX_HEIGHT = 200;

    // Distances for the regions
    const int FROM_X = 20;
    const int TO_X = 920;
    const int FROM_Y = 20;
    const int INCREMENT_Y = 220;

    const int AC_NOACTION = 0;
    const int AC_CREATE = 1;
    const int AC_DELETE = 2;
    const int AC_MERGE = 3;
    const int AC_SMOOTH_PURSUIT = 4;
    const int AC_RESET_TO_AUTO = 5;
    const int AC_DRAG_ENDS = 6;

    const int SMOOTHP_NO = 0;
    const int SMOOTHP_YES = 1;

    const int FIX_ON = 1;
    const int FIX_OFF = 0;

    const int MAX_MAXIMIZATION_INDEX = 5;

    // Smoothing algorithm
    // Bilateral filtering algorithm based on Ed Vul (Frank, Vul, & Johnson,2009; based on Durand & Dorsey, 2002).
    // THis algorithm eliminates Jitter in fixations while preserving saccades.
    const int SMOOTH_SCALES = 3;
    const int SMOOTH_ISCALES = 30;
    const int SMOOTH_MAXITS = 2060;

    // FIXATION DURATION DETECTION
    const int DEFAULT_SETTING_INTERP_LATENCY = 60;
    const int DEFAULT_SETTING_INTERP_VELOCITY_THRESHOLD = 10;
    const int DEFAULT_SETTING_POSTHOC_MIN_DURATION_VAL = 50;
    const int DEFAULT_SETTING_POSTHOC_LIMIT_RMS_VAL = 20;
    const int DEFAULT_SETTING_FIX_PREVIOUS_VELOCITY = 50;
    const int INVALID_SAMPLES = 3;
    const double DEFAULT_SETTING_DEGREE_PER_PIXEL = 0.0177; // @1680 x 1050px at 90cm (48 x 30cm)
    const double DEFAULT_SETTING_POSTHOC_MERGE_CONSECUTIVE_VAL = 0.25;
    const double DEFAULT_SETTING_INTERP_MAXIMUM_DISPLACEMENT = 0.25;

    const int DEFAULT_SETTING_SMOOTHING_SIGMA_S = 30;
    const int DEFAULT_SETTING_SMOOTHING_SIGMA_R = 40;
    const bool DEFAULT_SETTING_SMOOTHING_USE_OTHER_EYE = true;

    const bool DEFAULT_SETTING_EXPORT_LEFT_X_ROUGH = false;
    const bool DEFAULT_SETTING_EXPORT_LEFT_Y_ROUGH = false;
    const bool DEFAULT_SETTING_EXPORT_RIGHT_X_ROUGH = false;
    const bool DEFAULT_SETTING_EXPORT_RIGHT_Y_ROUGH = false;
    const bool DEFAULT_SETTING_EXPORT_LEFT_PUPIL = false;
    const bool DEFAULT_SETTING_EXPORT_RIGHT_PUPIL = false;
    const bool DEFAULT_SETTING_EXPORT_X_SMOOTH = false;
    const bool DEFAULT_SETTING_EXPORT_Y_SMOOTH = false;
    const bool DEFAULT_SETTING_EXPORT_FIXATION_NUMBER = false;
    const bool DEFAULT_SETTING_EXPORT_FIXATION_DURATION = false;
    const bool DEFAULT_SETTING_EXPORT_FIXATION_X_AVERAGE = false;
    const bool DEFAULT_SETTING_EXPORT_FIXATION_Y_AVERAGE = false;
    const bool DEFAULT_SETTING_EXPORT_FIXATION_DISTANCE = false;
    const bool DEFAULT_SETTING_EXPORT_FIXATION_IS_SMOOTH_PURSUIT = false;
    const bool DEFAULT_SETTING_EXPORT_SACCADE_NUMBER = false;
    const bool DEFAULT_SETTING_EXPORT_SACCADE_DURATION = false;
    const bool DEFAULT_SETTING_EXPORT_SACCADE_DISTANCE = false;
    const bool DEFAULT_SETTING_EXPORT_SACCADE_VELOCITY_AVERAGE = false;
    const bool DEFAULT_SETTING_EXPORT_SACCADE_VELOCITY_PEAK = false;
    const int DEFAULT_SETTING_EXPORT_TYPE = 0;

    //Settings Tokens
    const QString SETTING_PROJECT = "PROJECT";
    const QString SETTING_HZ = "HZ";
    const QString SETTING_EXP_WIDTH = "EXP_WIDTH";
    const QString SETTING_EXP_HEIGHT = "EXP_HEIGHT";
    const QString SETTING_DEGREE_PER_PIX = "DEGREE_PER_PIX";
    const QString SETTING_SECS_FRAGMENT = "SECS_FRAGMENT";
    const QString SETTINGS_CONFIGURATION_CHANGED_DATE =    "SETTINGS_CONFIGURATION_CHANGED_DATE";
    const QString SETTING_INTERP_LATENCY =                  "SETTING_INTERP_LATENCY";
    const QString SETTING_VELOCITY_THRESHOLD =       "SETTING_INTERP_VELOCITY_THRESHOLD";
    const QString SETTING_INTERP_MAXIMUM_DISPLACEMENT =     "SETTING_INTERP_MAXIMUM_DISPLACEMENT";
    const QString SETTING_POSTHOC_MERGE_CONSECUTIVE_VAL =   "SETTING_POSTHOC_MERGE_CONSECUTIVE_VAL";
    const QString SETTING_POSTHOC_MERGE_CONSECUTIVE_FLAG =  "SETTING_POSTHOC_MERGE_CONSECUTIVE_FLAG";
    const QString SETTING_POSTHOC_LIMIT_RMS_VAL =           "SETTING_POSTHOC_LIMIT_RMS_VAL";
    const QString SETTING_POSTHOC_LIMIT_RMS_FLAG =          "SETTING_POSTHOC_LIMIT_RMS_FLAG";
    const QString SETTING_POSTHOC_MIN_DURATION_VAL =        "SETTING_POSTHOC_MIN_DURATION_VAL";
    const QString SETTING_POSTHOC_MIN_DURATION_FLAG =       "SETTING_POSTHOC_MIN_DURATION_FLAG";
    const QString SETTING_EXPORT_LEFT_X_ROUGH = "SETTING_EXPORT_LEFT_X_ROUGH";
    const QString SETTING_EXPORT_LEFT_Y_ROUGH = "SETTING_EXPORT_LEFT_Y_ROUGH";
    const QString SETTING_EXPORT_RIGHT_X_ROUGH = "SETTING_EXPORT_RIGHT_X_ROUGH";
    const QString SETTING_EXPORT_RIGHT_Y_ROUGH = "SETTING_EXPORT_RIGHT_Y_ROUGH";
    const QString SETTING_EXPORT_LEFT_PUPIL = "SETTING_EXPORT_LEFT_PUPIL";
    const QString SETTING_EXPORT_RIGHT_PUPIL = "SETTING_EXPORT_RIGHT_PUPIL";
    const QString SETTING_EXPORT_X_SMOOTH = "SETTING_EXPORT_X_SMOOTH";
    const QString SETTING_EXPORT_Y_SMOOTH = "SETTING_EXPORT_Y_SMOOTH";
    const QString SETTING_EXPORT_VELOCITY = "SETTING_EXPORT_VELOCITY";
    const QString SETTING_EXPORT_FIXATION_NUMBER = "SETTING_EXPORT_FIXATION_NUMBER";
    const QString SETTING_EXPORT_FIXATION_DURATION = "SETTING_EXPORT_FIXATION_DURATION";
    const QString SETTING_EXPORT_FIXATION_X_AVERAGE = "SETTING_EXPORT_FIXATION_X_AVERAGE";
    const QString SETTING_EXPORT_FIXATION_Y_AVERAGE = "SETTING_EXPORT_FIXATION_Y_AVERAGE";
    const QString SETTING_EXPORT_FIXATION_RMS = "SETTING_EXPORT_FIXATION_DISTANCE";
    const QString SETTING_EXPORT_FIXATION_IS_SMOOTH_PURSUIT = "SETTING_EXPORT_FIXATION_IS_SMOOTH_PURSUIT";
    const QString SETTING_EXPORT_SACCADE_NUMBER = "SETTING_EXPORT_SACCADE_NUMBER";
    const QString SETTING_EXPORT_SACCADE_DURATION = "SETTING_EXPORT_SACCADE_DURATION";
    const QString SETTING_EXPORT_SACCADE_DISTANCE = "SETTING_EXPORT_SACCADE_DISTANCE";
    const QString SETTING_EXPORT_SACCADE_VELOCITY_AVERAGE = "SETTING_EXPORT_SACCADE_VELOCITY_AVERAGE";
    const QString SETTING_EXPORT_SACCADE_VELOCITY_PEAK = "SETTING_EXPORT_SACCADE_VELOCITY_PEAK";
    const QString SETTING_EXPORT_TYPE = "SETTING_EXPORT_TYPE";

    // Participant settings
    const QString PSETTING_ESTIMATED_FIX_DATE =  "PSETTING_ESTIMATED_FIX_DATE";
    const QString PSETTING_INTERPOLATED_DATE =   "PSETTING_INTERPOLATED_DATE";
    const QString PSETTING_SMOOTHED_DATE =       "PSETTING_SMOOTHED_DATE";
    const QString PSETTING_NAME =                "PSETTING_NAME";
    const QString PSETTING_ROUGH_PATH =          "PSETTING_ROUGH_PATH";
    const QString PSETTING_SEGMENTS_PATH =       "PSETTING_SEGMENTS_PATH";
    const QString PSETTING_NOTES =               "PSETTING_NOTES";
    const QString PSETTING_CONFIGURATION =       "PSETTING_CONFIGURATION";
    const QString PSETTING_LAST_SAVED_DATE =     "PSETTING_LAST_SAVED_DATE";
    const QString PSETTING_CONFIGURATION_CHANGE_DATE = "PSETTING_CONFIGURATION_CHANGE_DATE";


    const QString SETTING_SMOOTHING_SIGMA_S =              "SETTING_SMOOTHING_SIGMA_S";
    const QString SETTING_SMOOTHING_SIGMA_R =              "SETTING_SMOOTHING_SIGMA_R";
    const QString SETTING_SMOOTHING_USE_OTHER_EYE =         "SETTING_SMOOTH_USE_OTHER_EYE";

    const QString SETTING_CONFIGURATION_ID =                 "SETTING_CONFIGURATION_ID";
    const QString SETTING_CONFIGURATION_NAME =               "SETTING_CONFIGURATION_NAME";
    const QString SETTING_PARTICIPANT_DIRECTORY =            "SETTING_PARTICIPANT_DIRECTORY";
    const QString SETTING_NUMBER_CONFIGURATIONS =            "SETTING_NUMBER_CONFIGURATIONS";
    const QString SETTING_NUMBER_PARTICIPANTS =              "SETTING_NUMBER_PARTICIPANTS";
    const QString SETTING_LAST_SAVED =                       "SETTING_LAST_SAVED";

    const QString HELP_CREATE_MODE = "Create mode: Drag across display to create fixations.";
    const QString HELP_MERGE_MODE = "Merge mode: Drag across existing fixations to merge.";
    const QString HELP_PURSUIT_MODE = "Smooth pursuit mode: Drag across a fixation to mark as smooth pursuit";
    const QString HELP_DELETE_MODE = "Delete mode: Drag across a fixation to delete it.";
    const QString HELP_STATUS_READY = "Status: Ready.";

    const QString HELP_RESET_MODE = "Reset mode: Drag to rest manual fixation edits to the fixations that were automatically detected"
                                    " (if present).";

    const QString HELP_DRAG_MODE = "Adjust mode: Drag the edge of a fixation to lengthen or shorten it.";

    const QString HELP_MANUAL_EXEC = "Enter in manual positions (in samples), and press execute to carryout the operation"
                                     " selected in the edit mode above.";

    const QString HELP_POSITION = "Position (in samples) within the file.";

    const QString HELP_SETTING_SIGMA_S = "Sigma S in the bilateral filter - the width (in samples) of the guassian window in"
                                         " which to carry out smoothing.  Larger values result in smoother data, at the loss of"
                                         " high frequency information.";

    const QString HELP_SETTING_SIGMA_R = "Sigma R in the bilateral filter - the width (in pixels) of the guassian window in"
                                         " which nearby movements are counted in the smoothing algorithm.  The higher the value,"
                                         " the more smoothing applied to individual fixations, but too high values will result"
                                         " in blending of fixations which are near to each other.";

    const QString HELP_SETTING_INTERP_TIME = "The maximum length of time to let the interpolation algorithm fill in missing data.";

    const QString HELP_SETTING_INTERP_DISP = "With a gap in the signal, the interpolation algorithm will fill in missing data,"
            " unless the displacement of the fixations at the start and and of the gap is greater"
            " than this threshold.";

    const QString HELP_SETTING_VEL = "The threshold in velocity at which a saccade is registered.";

    const QString HELP_SETTING_MERGE = "Setting this will cause fixations which have a similar location to be merged if the"
                                       " difference between the two locations is below the threshold.";

    const QString HELP_SETTING_VARIANCE = "Setting this will cause fixations with a variance greater than the threshold to be"
                                          " removed.";

    const QString HELP_SETTING_MIN_DUR = "Setting this will cause fixations with a duration shorter than the threhold to be"
                                         " removed.";


    const QString HELP_BUTTON_FILTER = "Filter rough data using bilateral filter.";

    const QString HELP_BUTTON_INTERP = "Interpolate missing data: This relies on the velocity threshold parameter to mark"
                                       " saccades (the beginning and end of fixations) and the two interpolation thresholds."
                                       " The aim is to interpolate missing data which occurs within and only within a single"
                                       " fixation.";

    const QString HELP_BUTTON_EST = "Produces automatically detected fixations:  This relies on the velocity threshold"
                                    " parameter, as well as the three post-hoc options.  Fixations will be created over"
                                    " periods in which velocity does not exceed the velocity threshold.  Subsequent"
                                    " processing is applied to merge similar fixations and remove noisy or short"
                                    " fixations.";


    const int MAXSLIDER_SETTING_INTERP_LATENCY = 18000;
    const int MAXSLIDER_SETTING_INTERP_VELOCITY_THRESHOLD = 10000;
    const int MAXSLIDER_SETTING_INTERP_MAXIMUM_DISPLACEMENT = 15000;
    const int MAXSLIDER_SETTING_POSTHOC_MERGE_CONSECUTIVE_VAL = 6000;
    const int MAXSLIDER_SETTING_POSTHOC_LIMIT_RMS_VAL = 6000;
    const int MAXSLIDER_SETTING_POSTHOC_MIN_DURATION_VAL = 20000;
    const int MAXSLIDER_SETTING_SMOOTHING_SIGMA_S = 5000;
    const int MAXSLIDER_SETTING_SMOOTHING_SIGMA_R = 10000;

    const int MINSLIDER_SETTING_SMOOTHING_SIGMA_R = 100;
    const int MINSLIDER_SETTING_SMOOTHING_SIGMA_S = 50;


    int GetSliderValue(QString setting, double value);
    double GetValueFromSlider(QString setting, int slider_value);

};


#endif

