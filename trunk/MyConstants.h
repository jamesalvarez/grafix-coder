#ifndef MYCONSTANTS_H
#define MYCONSTANTS_H

#include <QPair>
#include <QString>
#include <QList>

typedef struct config { QString first;
                 int second;
                 bool operator==(config a)
                 {
                     return (a.first == this->first && a.second == this->second);
                 }
                      } GrafixConfiguration;

class Consts
{
public:
    Consts();

    // HZ
    static const int HZ_50 = 50; // Eye tracking Hz
    static const int HZ_60 = 60; // Eye tracking Hz
    static const int HZ_120 = 120; // Eye tracking Hz
    static const int HZ_300 = 300; // Eye tracking Hz
    static const int HZ_1000 = 1000;

    static const int DEFAULT_SETTING_HZ = 120;

    // FRAGMENT DURATIONS
    static const int FRAGMENT_DURATION_10 = 10; // seconds
    static const int FRAGMENT_DURATION_11 = 11; // seconds
    static const int FRAGMENT_DURATION_12 = 12; // seconds
    static const int FRAGMENT_DURATION_13 = 13; // seconds
    static const int FRAGMENT_DURATION_14 = 14; // seconds
    static const int FRAGMENT_DURATION_15 = 15; // seconds
    static const int FRAGMENT_DURATION_16 = 16; // seconds
    static const int FRAGMENT_DURATION_17 = 17; // seconds
    static const int FRAGMENT_DURATION_18 = 18; // seconds
    static const int FRAGMENT_DURATION_19 = 19; // seconds
    static const int FRAGMENT_DURATION_20 = 20; // seconds
    static const int FRAGMENT_DURATION_21 = 21; // seconds
    static const int FRAGMENT_DURATION_22 = 22; // seconds
    static const int FRAGMENT_DURATION_23 = 23; // seconds
    static const int FRAGMENT_DURATION_24 = 24; // seconds
    static const int FRAGMENT_DURATION_25 = 25; // seconds

    static const int DEFAULT_SETTING_SECS_FRAGMENT = 15;

    static const int DEFAULT_SETTING_EXP_WIDTH = 1024;  // Size of the experimental screen
    static const int DEFAULT_SETTING_EXP_HEIGHT = 768;

    static const int BOX_WIDTH = 850;
    static const int BOX_HEIGHT = 200;

    // Distances for the regions
    static const int FROM_X = 20;
    static const int TO_X = 920;
    static const int FROM_Y = 20;
    static const int INCREMENT_Y = 220;

    static const int AC_NOACTION = 0;
    static const int AC_CREATE = 1;
    static const int AC_DELETE = 2;
    static const int AC_MERGE = 3;
    static const int AC_SMOOTH_PURSUIT = 4;

    static const int SMOOTHP_NO = 0;
    static const int SMOOTHP_YES = 1;

    static const int FIX_ON = 1;
    static const int FIX_OFF = 0;

    static const int MAX_MAXIMIZATION_INDEX = 5;

    // Smoothing algorithm
    // Bilateral filtering algorithm based on Ed Vul (Frank, Vul, & Johnson,2009; based on Durand & Dorsey, 2002).
    // THis algorithm eliminates Jitter in fixations while preserving saccades.
    static const int SMOOTH_SCALES = 3;
    static const int SMOOTH_ISCALES = 30;
    static const int SMOOTH_MAXITS = 2060;

    // FIXATION DURATION DETECTION
    static const int DEFAULT_SETTING_INTERP_LATENCY = 60;
    static const int DEFAULT_SETTING_INTERP_VELOCITY_THRESHOLD = 10;
    static const int DEFAULT_SETTING_POSTHOC_MIN_DURATION_VAL = 50;
    static const int DEFAULT_SETTING_POSTHOC_LIMIT_RMS_VAL = 20;
    static const int DEFAULT_SETTING_FIX_PREVIOUS_VELOCITY = 50;
    static const int INVALID_SAMPLES = 3;
    static const double DEFAULT_SETTING_DEGREE_PER_PIXEL;// = 0.0177; // @1680 x 1050px at 90cm (48 x 30cm)
    static const double DEFAULT_SETTING_POSTHOC_MERGE_CONSECUTIVE_VAL;// = 0.25;
    static const double DEFAULT_SETTING_INTERP_MAXIMUM_DISPLACEMENT;// = 0.25;

    static const int DEFAULT_SETTING_SMOOTHING_SIGMA_S = 30;
    static const int DEFAULT_SETTING_SMOOTHING_SIGMA_R = 40;
    static const bool DEFAULT_SETTING_SMOOTHING_USE_OTHER_EYE = true;

    //Settings Tokens
    static const QString SETTING_PROJECT;
    static const QString SETTING_HZ;
    static const QString SETTING_EXP_WIDTH;
    static const QString SETTING_EXP_HEIGHT;
    static const QString SETTING_DEGREE_PER_PIX;
    static const QString SETTING_SECS_FRAGMENT;
    static const QString SETTING_INTERP_LATENCY;
    static const QString SETTING_INTERP_VELOCITY_THRESHOLD;
    static const QString SETTING_INTERP_MAXIMUM_DISPLACEMENT;
    static const QString SETTING_POSTHOC_MERGE_CONSECUTIVE_VAL;
    static const QString SETTING_POSTHOC_MERGE_CONSECUTIVE_FLAG;
    static const QString SETTING_POSTHOC_LIMIT_RMS_VAL;
    static const QString SETTING_POSTHOC_LIMIT_RMS_FLAG;
    static const QString SETTING_POSTHOC_MIN_DURATION_VAL;
    static const QString SETTING_POSTHOC_MIN_DURATION_FLAG;
    static const QString SETTING_SMOOTHING_SIGMA_S;
    static const QString SETTING_SMOOTHING_SIGMA_R;
    static const QString SETTING_SMOOTHING_USE_OTHER_EYE;
    static const QString SETTINGS_CONFIGURATION_CHANGED_DATE;
    static const QString SETTING_CONFIGURATION;
    static const QString SETTING_CONFIGURATION_ID;
    static const QString SETTING_CONFIGURATION_NAME;
    static const QString SETTING_PARTICIPANT_DIRECTORY;
    static const QString SETTING_NUMBER_CONFIGURATIONS;
    static const QString SETTING_NUMBER_PARTICIPANTS;
    static const QString SETTING_LAST_SAVED;

    static const GrafixConfiguration ACTIVE_CONFIGURATION();
    static const QList<QString> LIST_CONFIGURATION_SETTINGS();

    static const QString PSETTING_NAME;
    static const QString PSETTING_ROUGH_PATH;
    static const QString PSETTING_SEGMENTS_PATH;
    static const QString PSETTING_NOTES;
    static const QString PSETTING_CONFIGURATION;
    static const QString PSETTING_LAST_SAVED_DATE;
    static const QString PSETTING_CONFIGURATION_CHANGE_DATE;
    static const QString PSETTING_ESTIMATED_FIX_DATE;
    static const QString PSETTING_INTERPOLATED_DATE;
    static const QString PSETTING_SMOOTHED_DATE;

    enum BATCH_TASK { TASK_SMOOTH, TASK_INTERPOLATE, TASK_ESTIMATE_FIXATIONS };
    enum MATRIX_TYPE
    {
        MATRIX_ROUGH = 0,
        MATRIX_SMOOTH = 1,
        MATRIX_FIXALL = 2,
        MATRIX_AUTOFIXALL = 3,
        MATRIX_SEGMENTS = 4
    };


};


#endif

