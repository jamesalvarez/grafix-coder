#ifndef GPMATRIXFILES_H
#define GPMATRIXFILES_H

#include "armadillo"
#include "GrafixSettingsLoader.h"

using namespace arma;

class GrafixSettingsLoader;

namespace GPMatrixFiles {

    bool readFileSafe(mat &matrix, QString fileName);
    int  getFileColumnCount(QString fullpath);
    bool saveFileSafe(mat &matrix, QString fileName, QString headerString = "");

    bool exportFile(mat &roughM, mat &smoothM, mat &fixAllM, QString filename, GrafixSettingsLoader &settingsLoader);
    bool makeExportTimestampedData(mat &roughM, mat &smoothM, mat &fixAllM, QString filename, GrafixSettingsLoader &settingsLoader);
    bool makeExportSaccades( mat &smoothM, mat &fixAllM, QString filename, GrafixSettingsLoader &settingsLoader);
    bool makeExportFixations(mat &fixAllM, QString filename, GrafixSettingsLoader &settingsLoader);

}
#endif // GPMATRIXFILES_H
