#ifndef DialogInterpolation_H
#define DialogInterpolation_H

#include <QMainWindow>
#include <QtGui>
#include <QWidget>
#include <QDialog>

#include "GPMatrixFunctions.h"
#include "MyConstants.h"

#include "armadillo"
#include "GrafixProject.h"
#include "DialogBatchProcess.h"

using namespace arma;
using namespace std;

namespace Ui {
class DialogInterpolation;
}

class DialogInterpolation : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogInterpolation(QWidget *parent = 0);
    ~DialogInterpolation();

    void loadData(GrafixParticipant* participant, mat &SmoothM);
    

private:
    Ui::DialogInterpolation*    ui;
    mat*                        p_smoothM;
    GrafixParticipant*          _participant;
    GrafixProject*              _project;

    void fncInterpolateData();


public slots:
    void fncPress_bAccept();
    void fncPress_bBatch();

};

#endif // DialogInterpolation_H

