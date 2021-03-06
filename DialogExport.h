#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QtWidgets/QMainWindow>
#include <QtGui>
#include <QtWidgets/QWidget>
#include <QtWidgets/QDialog>

#include <sstream>
#include <string>

#include <iostream>
#include "armadillo"
#include "GrafixProject.h"
#include "DialogGrafixError.h"
#include "DialogBatchProcess.h"
#include <iomanip>
using namespace arma;
using namespace std;

namespace Ui {
class DialogExport;
}

class DialogExport : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogExport(QWidget *parent = 0);
    ~DialogExport();

    void loadData(GrafixParticipant *particpant, mat roughM,mat smoothM, mat fixAllM);

    QString getSettingForCheckBox(QCheckBox* checkBox);

private:
    Ui::DialogExport *ui;

    GrafixParticipant *_participant;
    mat roughM;
    mat smoothM;
    mat fixAllM;
    mat experimentalSegmentsM;

    QVector<QCheckBox*> _checkboxes;

public slots:
    void fncPress_bExport();
    void fncPress_bExportAll();
    void fncPress_bCancel();
    void fncPress_bSelectAll();
    Consts::EXPORT_TYPE fncGetCurrentExportTypeFromRadioButtons();
    void fncChangeExportType();
};

#endif // EXPORTDIALOG_H
