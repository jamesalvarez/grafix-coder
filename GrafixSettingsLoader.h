#ifndef GRAFIXSETTINGSLOADER_H
#define GRAFIXSETTINGSLOADER_H

#include "GrafixProject.h"
#include "MyConstants.h"

#include <QSettings>

class GrafixProject;

//This is used to load settings from a particular path and configuration in
//a more efficient way - also prevents accidentally accessing different settings etc
//note QSettings is re-entrant so no need to store an object to it.

class GrafixSettingsLoader
{
public:
    GrafixSettingsLoader(const GrafixProject &project);
    GrafixSettingsLoader(const GrafixProject &project, const GrafixConfiguration configuration);
    GrafixSettingsLoader(const QString path);
    GrafixSettingsLoader(const QString path, const GrafixConfiguration configuration);

    QVariant LoadSetting(QString setting);
    int LoadSliderSetting(QString setting);
    void SetSetting(const QString setting, const QVariant value);

private:
    void Init(const QString path, const GrafixConfiguration configuration);
    GrafixConfiguration _configuration;
    QString _path;
};

#endif // GRAFIXSETTINGSLOADER_H
