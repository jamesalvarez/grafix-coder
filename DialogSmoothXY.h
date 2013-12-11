#ifndef DialogSmoothXY_H
#define DialogSmoothXY_H

#include <QtWidgets/QMainWindow>
#include <QtGui>
#include <QtWidgets/QWidget>
#include <QtWidgets/QDialog>
#include <QWaitCondition>

#include "armadillo"
#include "GrafixProject.h"
#include "GPMatrixFunctions.h"
#include "MyConstants.h"
#include <QProgressDialog>
#include "DialogBatchProcess.h"

using namespace arma;


namespace Ui {
class DialogSmoothXY;
}

class DialogSmoothXY : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogSmoothXY(QWidget *parent = 0);
    ~DialogSmoothXY();

    void loadData(GrafixParticipant &participant, mat const &roughM, mat &smoothM);
    
private:
    Ui::DialogSmoothXY* ui;
    GrafixParticipant*  _participant;
    GrafixProject*      _project;
    GrafixConfiguration _configuration;
    mat const*          p_roughM;
    mat*                p_smoothM;

public slots:
    void fncPress_bAccept();
    void fncPress_bClose();
    void fncPress_bBatch();
    void fncPress_cbUseBothEyes();
};

#endif // DialogSmoothXY_H

