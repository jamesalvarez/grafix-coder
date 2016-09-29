#include "MyConstants.h"


//const QString Consts::SETTING_PROJECT = "PROJECT";


const GrafixConfiguration Consts::ACTIVE_CONFIGURATION() {
    GrafixConfiguration gc;
    gc.first = "Active";
    gc.second = 0;
    return gc;
}

const QList<QString> Consts::LIST_CONFIGURATION_SETTINGS() {
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
    ret.append(Consts::SETTING_EXPORT_LEFT_X_ROUGH);
    ret.append(Consts::SETTING_EXPORT_LEFT_Y_ROUGH);
    ret.append(Consts::SETTING_EXPORT_RIGHT_X_ROUGH);
    ret.append(Consts::SETTING_EXPORT_RIGHT_Y_ROUGH);
    ret.append(Consts::SETTING_EXPORT_LEFT_PUPIL);
    ret.append(Consts::SETTING_EXPORT_RIGHT_PUPIL);
    ret.append(Consts::SETTING_EXPORT_X_SMOOTH);
    ret.append(Consts::SETTING_EXPORT_Y_SMOOTH);
    ret.append(Consts::SETTING_EXPORT_FIXATION_NUMBER);
    ret.append(Consts::SETTING_EXPORT_FIXATION_DURATION);
    ret.append(Consts::SETTING_EXPORT_FIXATION_X_AVERAGE);
    ret.append(Consts::SETTING_EXPORT_FIXATION_Y_AVERAGE);
    ret.append(Consts::SETTING_EXPORT_FIXATION_RMS);
    ret.append(Consts::SETTING_EXPORT_FIXATION_IS_SMOOTH_PURSUIT);
    ret.append(Consts::SETTING_EXPORT_SACCADE_NUMBER);
    ret.append(Consts::SETTING_EXPORT_SACCADE_DURATION);
    ret.append(Consts::SETTING_EXPORT_SACCADE_DISTANCE);
    ret.append(Consts::SETTING_EXPORT_SACCADE_VELOCITY_AVERAGE);
    ret.append(Consts::SETTING_EXPORT_SACCADE_VELOCITY_PEAK);
    return ret;
}



const QVariant Consts::DefaultSetting(QString setting) {
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
    else if (setting == Consts::SETTING_EXPORT_LEFT_X_ROUGH)
        return QVariant(Consts::DEFAULT_SETTING_EXPORT_LEFT_X_ROUGH);
    else if (setting == Consts::SETTING_EXPORT_LEFT_Y_ROUGH)
        return QVariant(Consts::DEFAULT_SETTING_EXPORT_LEFT_Y_ROUGH);
    else if (setting == Consts::SETTING_EXPORT_RIGHT_X_ROUGH)
        return QVariant(Consts::DEFAULT_SETTING_EXPORT_RIGHT_X_ROUGH);
    else if (setting == Consts::SETTING_EXPORT_RIGHT_Y_ROUGH)
        return QVariant(Consts::DEFAULT_SETTING_EXPORT_RIGHT_Y_ROUGH);
    else if (setting == Consts::SETTING_EXPORT_LEFT_PUPIL)
        return QVariant(Consts::DEFAULT_SETTING_EXPORT_LEFT_PUPIL);
    else if (setting == Consts::SETTING_EXPORT_RIGHT_PUPIL)
        return QVariant(Consts::DEFAULT_SETTING_EXPORT_RIGHT_PUPIL);
    else if (setting == Consts::SETTING_EXPORT_X_SMOOTH)
        return QVariant(Consts::DEFAULT_SETTING_EXPORT_X_SMOOTH);
    else if (setting == Consts::SETTING_EXPORT_Y_SMOOTH)
        return QVariant(Consts::DEFAULT_SETTING_EXPORT_Y_SMOOTH);
    else if (setting == Consts::SETTING_EXPORT_FIXATION_NUMBER)
        return QVariant(Consts::DEFAULT_SETTING_EXPORT_FIXATION_NUMBER);
    else if (setting == Consts::SETTING_EXPORT_FIXATION_DURATION)
        return QVariant(Consts::DEFAULT_SETTING_EXPORT_FIXATION_DURATION);
    else if (setting == Consts::SETTING_EXPORT_FIXATION_X_AVERAGE)
        return QVariant(Consts::DEFAULT_SETTING_EXPORT_FIXATION_X_AVERAGE);
    else if (setting == Consts::SETTING_EXPORT_FIXATION_Y_AVERAGE)
        return QVariant(Consts::DEFAULT_SETTING_EXPORT_FIXATION_Y_AVERAGE);
    else if (setting == Consts::SETTING_EXPORT_FIXATION_RMS)
        return QVariant(Consts::DEFAULT_SETTING_EXPORT_FIXATION_DISTANCE);
    else if (setting == Consts::SETTING_EXPORT_FIXATION_IS_SMOOTH_PURSUIT)
        return QVariant(Consts::DEFAULT_SETTING_EXPORT_FIXATION_IS_SMOOTH_PURSUIT);
    else if (setting == Consts::SETTING_EXPORT_SACCADE_NUMBER)
        return QVariant(Consts::DEFAULT_SETTING_EXPORT_SACCADE_NUMBER);
    else if (setting == Consts::SETTING_EXPORT_SACCADE_DURATION)
        return QVariant(Consts::DEFAULT_SETTING_EXPORT_SACCADE_DURATION);
    else if (setting == Consts::SETTING_EXPORT_SACCADE_DISTANCE)
        return QVariant(Consts::DEFAULT_SETTING_EXPORT_SACCADE_DISTANCE);
    else if (setting == Consts::SETTING_EXPORT_SACCADE_VELOCITY_AVERAGE)
        return QVariant(Consts::DEFAULT_SETTING_EXPORT_SACCADE_VELOCITY_AVERAGE);
    else if (setting == Consts::SETTING_EXPORT_SACCADE_VELOCITY_PEAK)
        return QVariant(Consts::DEFAULT_SETTING_EXPORT_SACCADE_VELOCITY_PEAK);
    else if (setting == Consts::SETTING_EXPORT_TYPE)
        return QVariant(Consts::DEFAULT_SETTING_EXPORT_TYPE);
    return 0;
}

int Consts::GetSliderValue(QString setting, double value) {
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

double Consts::GetValueFromSlider(QString setting, int slider_value) {
    if (setting == Consts::SETTING_SMOOTHING_SIGMA_S ||
            setting == Consts::SETTING_SMOOTHING_SIGMA_R ||
            setting == Consts::SETTING_INTERP_LATENCY ||
            setting == Consts::SETTING_POSTHOC_MIN_DURATION_VAL ||
            setting == Consts::SETTING_VELOCITY_THRESHOLD)
        return (double) slider_value / 100;

    if (setting == Consts::SETTING_POSTHOC_LIMIT_RMS_VAL ||
            setting == Consts::SETTING_POSTHOC_MERGE_CONSECUTIVE_VAL ||
            setting == Consts::SETTING_INTERP_MAXIMUM_DISPLACEMENT)
        return (double)(slider_value) / 10000;
    return 0;
}

