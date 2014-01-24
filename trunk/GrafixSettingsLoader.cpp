#include "GrafixSettingsLoader.h"

GrafixSettingsLoader::GrafixSettingsLoader(const GrafixProject &project)
{
    GrafixSettingsLoader(project, Consts::ACTIVE_CONFIGURATION());
}

GrafixSettingsLoader::GrafixSettingsLoader(const GrafixProject &project, const QString path)
{
    GrafixSettingsLoader(project,project.GetProjectSettingsPath());
}

GrafixSettingsLoader::GrafixSettingsLoader(const GrafixProject &project, const GrafixConfiguration configuration)
{
    GrafixSettingsLoader(project.GetProjectSettingsPath(), configuration);
}

GrafixSettingsLoader::GrafixSettingsLoader(QString path, const GrafixConfiguration configuration)
{
    this->_configuration = configuration;
    this->_qsettings = QSettings(path, QSettings::IniFormat);
}

GrafixSettingsLoader::GrafixSettingsLoader(const QString path)
{
    GrafixSettingsLoader(path, Consts::ACTIVE_CONFIGURATION());
}



QVariant GrafixSettingsLoader::LoadSetting(QString setting)
{
    QVariant value = _qsettings.value(setting + "_" + QString::number(configuration.second));
    if (value.isNull())
    {
        return Consts::DefaultSetting(setting);
    }
    return value;
}
