#include "MyConstants.h"

const double Consts::DEFAULT_SETTING_DEGREE_PER_PIXEL = 0.0177; // @1680 x 1050px at 90cm (48 x 30cm)
const double Consts::DEFAULT_SETTING_POSTHOC_MERGE_CONSECUTIVE_VAL = 0.25;
const double Consts::DEFAULT_SETTING_INTERP_MAXIMUM_DISPLACEMENT = 0.25;

const QString Consts::SETTING_PROJECT = "PROJECT";
const QString Consts::SETTING_HZ = "HZ";
const QString Consts::SETTING_EXP_WIDTH = "EXP_WIDTH";
const QString Consts::SETTING_EXP_HEIGHT = "EXP_HEIGHT";
const QString Consts::SETTING_DEGREE_PER_PIX = "DEGREE_PER_PIX";
const QString Consts::SETTING_SECS_FRAGMENT = "SECS_FRAGMENT";
const QString Consts::SETTINGS_CONFIGURATION_CHANGED_DATE =    "SETTINGS_CONFIGURATION_CHANGED_DATE";
const QString Consts::SETTING_INTERP_LATENCY=                  "SETTING_INTERP_LATENCY";
const QString Consts::SETTING_VELOCITY_THRESHOLD=       "SETTING_INTERP_VELOCITY_THRESHOLD";
const QString Consts::SETTING_INTERP_MAXIMUM_DISPLACEMENT=     "SETTING_INTERP_MAXIMUM_DISPLACEMENT";
const QString Consts::SETTING_POSTHOC_MERGE_CONSECUTIVE_VAL=   "SETTING_POSTHOC_MERGE_CONSECUTIVE_VAL";
const QString Consts::SETTING_POSTHOC_MERGE_CONSECUTIVE_FLAG=  "SETTING_POSTHOC_MERGE_CONSECUTIVE_FLAG";
const QString Consts::SETTING_POSTHOC_LIMIT_RMS_VAL=           "SETTING_POSTHOC_LIMIT_RMS_VAL";
const QString Consts::SETTING_POSTHOC_LIMIT_RMS_FLAG=          "SETTING_POSTHOC_LIMIT_RMS_FLAG";
const QString Consts::SETTING_POSTHOC_MIN_DURATION_VAL=        "SETTING_POSTHOC_MIN_DURATION_VAL";
const QString Consts::SETTING_POSTHOC_MIN_DURATION_FLAG=       "SETTING_POSTHOC_MIN_DURATION_FLAG";


const QString Consts::PSETTING_ESTIMATED_FIX_DATE=  "PSETTING_ESTIMATED_FIX_DATE";
const QString Consts::PSETTING_INTERPOLATED_DATE=   "PSETTING_INTERPOLATED_DATE";
const QString Consts::PSETTING_SMOOTHED_DATE=       "PSETTING_SMOOTHED_DATE";
const QString Consts::PSETTING_NAME=                "PSETTING_NAME";
const QString Consts::PSETTING_ROUGH_PATH=          "PSETTING_ROUGH_PATH";
const QString Consts::PSETTING_SEGMENTS_PATH=       "PSETTING_SEGMENTS_PATH";
const QString Consts::PSETTING_NOTES=               "PSETTING_NOTES";
const QString Consts::PSETTING_CONFIGURATION=       "PSETTING_CONFIGURATION";
const QString Consts::PSETTING_LAST_SAVED_DATE=     "PSETTING_LAST_SAVED_DATE";
const QString Consts::PSETTING_CONFIGURATION_CHANGE_DATE= "PSETTING_CONFIGURATION_CHANGE_DATE";


const QString Consts::SETTING_SMOOTHING_SIGMA_S =              "SETTING_SMOOTHING_SIGMA_S";
const QString Consts::SETTING_SMOOTHING_SIGMA_R =              "SETTING_SMOOTHING_SIGMA_R";
const QString Consts::SETTING_SMOOTHING_USE_OTHER_EYE=         "SETTING_SMOOTH_USE_OTHER_EYE";

const QString Consts::SETTING_CONFIGURATION =                   "SETTING_CONFIGURATION";
const QString Consts::SETTING_CONFIGURATION_ID=                 "SETTING_CONFIGURATION_ID";
const QString Consts::SETTING_CONFIGURATION_NAME=               "SETTING_CONFIGURATION_NAME";
const QString Consts::SETTING_PARTICIPANT_DIRECTORY=            "SETTING_PARTICIPANT_DIRECTORY";
const QString Consts::SETTING_NUMBER_CONFIGURATIONS=            "SETTING_NUMBER_CONFIGURATIONS";
const QString Consts::SETTING_NUMBER_PARTICIPANTS=              "SETTING_NUMBER_PARTICIPANTS";
const QString Consts::SETTING_LAST_SAVED=                       "SETTING_LAST_SAVED";

const GrafixConfiguration Consts::ACTIVE_CONFIGURATION()
{
    GrafixConfiguration gc;
    gc.first = "Active";
    gc.second = 0;
    return gc;
}

const QList<QString> Consts::LIST_CONFIGURATION_SETTINGS()
{
    QList<QString> ret;
    ret.append(Consts::SETTING_HZ);
    ret.append(Consts::SETTING_EXP_WIDTH);
    ret.append(Consts::SETTING_EXP_HEIGHT);
    ret.append(Consts::SETTING_SECS_FRAGMENT);
    ret.append(Consts::SETTING_DEGREE_PER_PIX);
    ret.append(Consts::SETTING_INTERP_LATENCY);
    ret.append(Consts::SETTING_POSTHOC_MERGE_CONSECUTIVE_VAL);
    ret.append(Consts::SETTING_INTERP_MAXIMUM_DISPLACEMENT);
    ret.append(Consts::SETTING_POSTHOC_MIN_DURATION_VAL);
    ret.append(Consts::SETTING_VELOCITY_THRESHOLD);
    ret.append(Consts::SETTING_POSTHOC_LIMIT_RMS_VAL);
    ret.append(Consts::SETTING_SMOOTHING_USE_OTHER_EYE);
    ret.append(Consts::SETTING_SMOOTHING_SIGMA_R);
    ret.append(Consts::SETTING_SMOOTHING_SIGMA_S);
    ret.append(Consts::SETTING_POSTHOC_LIMIT_RMS_FLAG);
    ret.append(Consts::SETTING_POSTHOC_MERGE_CONSECUTIVE_FLAG);
    ret.append(Consts::SETTING_POSTHOC_MIN_DURATION_FLAG);
    return ret;
}

const QString Consts::HELP_CREATE_MODE = "Create mode: Drag across display to create fixations.";
const QString Consts::HELP_MERGE_MODE = "Merge mode: Drag across existing fixations to merge.";
const QString Consts::HELP_PURSUIT_MODE = "Smooth pursuit mode: Drag across a fixation to mark as smooth pursuit";
const QString Consts::HELP_DELETE_MODE = "Delete mode: Drag across a fixation to delete it.";
const QString Consts::HELP_STATUS_READY = "Status: Ready.";

const QString Consts::HELP_RESET_MODE = "Reset mode: Drag to rest manual fixation edits to the fixations that were automatically detected"
                                        " (if present).";

const QString Consts::HELP_DRAG_MODE = "Adjust mode: Drag the edge of a fixation to lengthen or shorten it.";

const QString Consts::HELP_MANUAL_EXEC = "Enter in manual positions (in samples), and press execute to carryout the operation"
                                            " selected in the edit mode above.";

const QString Consts::HELP_POSITION = "Position (in samples) within the file.";

const QString Consts::HELP_SETTING_SIGMA_S = "Sigma S in the bilateral filter - the width (in samples) of the guassian window in"
" which to carry out smoothing.  Larger values result in smoother data, at the loss of"
" high frequency information.";

const QString Consts::HELP_SETTING_SIGMA_R = "Sigma R in the bilateral filter - the width (in pixels) of the guassian window in"
" which nearby movements are counted in the smoothing algorithm.  The higher the value,"
" the more smoothing applied to individual fixations, but too high values will result"
" in blending of fixations which are near to each other.";

const QString Consts::HELP_SETTING_INTERP_TIME = "The maximum length of time to let the interpolation algorithm fill in missing data.";

const QString Consts::HELP_SETTING_INTERP_DISP = "With a gap in the signal, the interpolation algorithm will fill in missing data,"
" unless the displacement of the fixations at the start and and of the gap is greater"
" than this threshold.";

const QString Consts::HELP_SETTING_VEL = "The threshold in velocity at which a saccade is registered.";

const QString Consts::HELP_SETTING_MERGE = "Setting this will cause fixations which have a similar location to be merged if the"
" difference between the two locations is below the threshold.";

const QString Consts::HELP_SETTING_VARIANCE = "Setting this will cause fixations with a variance greater than the threshold to be"
" removed.";

const QString Consts::HELP_SETTING_MIN_DUR = "Setting this will cause fixations with a duration shorter than the threhold to be"
" removed.";


const QString Consts::HELP_BUTTON_FILTER = "Filter rough data using bilateral filter.";

const QString Consts::HELP_BUTTON_INTERP = "Interpolate missing data: This relies on the velocity threshold parameter to mark"
" saccades (the beginning and end of fixations) and the two interpolation thresholds."
" The aim is to interpolate missing data which occurs within and only within a single"
" fixation.";

const QString Consts::HELP_BUTTON_EST = "Produces automatically detected fixations:  This relies on the velocity threshold"
" parameter, as well as the three post-hoc options.  Fixations will be created over"
" periods in which velocity does not exceed the velocity threshold.  Subsequent"
" processing is applied to merge similar fixations and remove noisy or short"
" fixations.";

const QVariant Consts::DefaultSetting(QString setting)
{
    if (setting == Consts::SETTING_HZ)
        return QVariant(Consts::DEFAULT_SETTING_HZ);
    else if (setting == Consts::SETTING_EXP_WIDTH)
        return QVariant(Consts::DEFAULT_SETTING_EXP_WIDTH);
    else if (setting == Consts::SETTING_EXP_HEIGHT)
        return QVariant(Consts::DEFAULT_SETTING_EXP_HEIGHT);
    else if (setting == Consts::SETTING_SECS_FRAGMENT)
        return QVariant(Consts::DEFAULT_SETTING_SECS_FRAGMENT);
    else if (setting == Consts::SETTING_DEGREE_PER_PIX)
        return QVariant(Consts::DEFAULT_SETTING_DEGREE_PER_PIXEL);
    else if (setting == Consts::SETTING_INTERP_LATENCY)
        return QVariant(Consts::DEFAULT_SETTING_INTERP_LATENCY);
    else if (setting == Consts::SETTING_POSTHOC_MERGE_CONSECUTIVE_VAL)
        return QVariant(Consts::DEFAULT_SETTING_POSTHOC_MERGE_CONSECUTIVE_VAL);
    else if (setting == Consts::SETTING_INTERP_MAXIMUM_DISPLACEMENT)
        return QVariant(Consts::DEFAULT_SETTING_INTERP_MAXIMUM_DISPLACEMENT);
    else if (setting == Consts::SETTING_POSTHOC_MIN_DURATION_VAL)
        return QVariant(Consts::DEFAULT_SETTING_POSTHOC_MIN_DURATION_VAL);
    else if (setting == Consts::SETTING_VELOCITY_THRESHOLD)
        return QVariant(Consts::DEFAULT_SETTING_INTERP_VELOCITY_THRESHOLD);
    else if (setting == Consts::SETTING_POSTHOC_LIMIT_RMS_VAL)
        return QVariant(Consts::DEFAULT_SETTING_POSTHOC_LIMIT_RMS_VAL);
    else if (setting == Consts::SETTING_SMOOTHING_USE_OTHER_EYE)
        return QVariant(Consts::DEFAULT_SETTING_SMOOTHING_USE_OTHER_EYE);
    else if (setting == Consts::SETTING_SMOOTHING_SIGMA_R)
        return QVariant(Consts::DEFAULT_SETTING_SMOOTHING_SIGMA_R);
    else if (setting == Consts::SETTING_SMOOTHING_SIGMA_S)
        return QVariant(Consts::DEFAULT_SETTING_SMOOTHING_SIGMA_S);
    else if (setting == Consts::SETTING_CONFIGURATION)
        return QVariant(false);
    else
        return 0;
}

int Consts::GetSliderValue(QString setting, double value)
{
    if (setting == Consts::SETTING_SMOOTHING_SIGMA_S ||
        setting == Consts::SETTING_SMOOTHING_SIGMA_R ||
        setting == Consts::SETTING_INTERP_LATENCY ||
        setting == Consts::SETTING_POSTHOC_MIN_DURATION_VAL ||
        setting == Consts::SETTING_VELOCITY_THRESHOLD)
        return (int)(value * 100);

    if (setting == Consts::SETTING_POSTHOC_LIMIT_RMS_VAL ||
        setting == Consts::SETTING_POSTHOC_MERGE_CONSECUTIVE_VAL ||
        setting == Consts::SETTING_INTERP_MAXIMUM_DISPLACEMENT)
        return (int)(value * 100 * 100);
    return 0;
}

double Consts::GetValueFromSlider(QString setting, int slider_value)
{
    if (setting == Consts::SETTING_SMOOTHING_SIGMA_S ||
        setting == Consts::SETTING_SMOOTHING_SIGMA_R ||
        setting == Consts::SETTING_INTERP_LATENCY ||
        setting == Consts::SETTING_POSTHOC_MIN_DURATION_VAL ||
        setting == Consts::SETTING_VELOCITY_THRESHOLD)
        return (double) slider_value / 100;

    if (setting == Consts::SETTING_POSTHOC_LIMIT_RMS_VAL ||
        setting == Consts::SETTING_POSTHOC_MERGE_CONSECUTIVE_VAL ||
        setting == Consts::SETTING_INTERP_MAXIMUM_DISPLACEMENT)
        return (double)(slider_value) / 100 / 100;
    return 0;
}

Consts::Consts(void)
{
}

