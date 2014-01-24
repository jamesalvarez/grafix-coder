#ifndef GRAFIXSETTINGSLOADER_H
#define GRAFIXSETTINGSLOADER_H

#include "GrafixProject.h"
#include "MyConstants.h"

#include <QSettings>

class GrafixProject;

class GrafixSettingsLoader
{
public:
    GrafixSettingsLoader(const GrafixProject &project);
    GrafixSettingsLoader(const GrafixProject &project, const GrafixConfiguration configuration);
    GrafixSettingsLoader(const GrafixProject &project, const QString path);
    GrafixSettingsLoader(const QString path);
    GrafixSettingsLoader(const QString path, const GrafixConfiguration configuration);
    QVariant LoadSetting(QString setting);

private:
    GrafixProject *p_project;
    GrafixConfiguration _configuration;
    QSettings _qsettings;
};

#endif // GRAFIXSETTINGSLOADER_H
