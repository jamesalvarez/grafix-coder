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

    void loadData(GrafixParticipant *particpant, mat roughM,mat smoothM, mat fixAllM, mat experimentalSegmentsM);
    
private:
    Ui::DialogExport *ui;

    GrafixParticipant *_participant;
    mat roughM;
    mat smoothM;
    mat fixAllM;
    mat experimentalSegmentsM;

public slots:
     void fncPress_bExport();
     void fncPress_bCancel();

};

#endif // EXPORTDIALOG_H
