#include "GrafixSettingsLoader.h"

GrafixSettingsLoader::GrafixSettingsLoader(const GrafixProject &project)
{
    GrafixSettingsLoader(project, Consts::ACTIVE_CONFIGURATION());
}


GrafixSettingsLoader::GrafixSettingsLoader(const GrafixProject &project, const GrafixConfiguration configuration)
{
    GrafixSettingsLoader(project.GetProjectSettingsPath(), configuration);
}

GrafixSettingsLoader::GrafixSettingsLoader(QString path, const GrafixConfiguration configuration)
{
    this->_path = path;
    this->_configuration = configuration;

}

GrafixSettingsLoader::GrafixSettingsLoader(const QString path)
{
    GrafixSettingsLoader(path, Consts::ACTIVE_CONFIGURATION());
}

QVariant GrafixSettingsLoader::LoadSetting(QString setting)
{
    QSettings settings(_path, QSettings::IniFormat);
    QString key = setting + "_" + QString::number(this->_configuration.second);
    QVariant value = settings.value(key);
    if (value.isNull())
    {
        return Consts::DefaultSetting(setting);
    }
    return value;
}

void GrafixSettingsLoader::SetSetting(const QString setting, const QVariant value)
{
    QSettings settings(_path, QSettings::IniFormat);
    settings.setValue(setting, value);
}
