#ifndef DIALOGTESTSMOOTHPARAMETERS_H
#define DIALOGTESTSMOOTHPARAMETERS_H

#include "GPMatrixFunctions.h"
#include "GrafixProject.h"

#include "DialogPlot.h"

#include <QDialog>

#include <math.h>

#include "armadillo"

namespace Ui {
class DialogTestSmoothParameters;
}

class DialogTestSmoothParameters : public QDialog
{
    Q_OBJECT

public:
    explicit DialogTestSmoothParameters(QWidget *parent = 0);
    ~DialogTestSmoothParameters();

    void loadData(const mat &RoughM, uword displayStartIndex, uword displayStopIndex, const GrafixParticipant &p);

private:
    GrafixProject *p_project;
    const GrafixParticipant *p_participant;
    GrafixConfiguration _configuration;
    Ui::DialogTestSmoothParameters *ui;
    bool _data_loaded;
    const mat *p_original_roughM;
    mat _roughM;
    mat _smoothM;
    mat _fixAllM;
    QString _temp_settings;
    int _expHeight;
    int _expWidth;
    int _hz;
    int _secsFragmentHz;
    double _degreePerPixel;
    double _displayIncrement;
    uword _displayStartIndex;
    uword _displayStopIndex;

    //Sets a project.ini setting
    void fncSetTempSetting(QString setting, QVariant value);

    //Gets submatrix based on stop and start index
    void fncGetSubmat(uword displayStartIndex, uword displayStopIndex);

    //smooths and computes fixations on screen
    void fncSmoothData();

    //repaints on resize
    void resizeEvent(QResizeEvent *e);

    void paintRoughData();
    void paintSmoothData();
    void paintFixations();
    void paintVelocity();

public slots:
    void fncChange_sSmooth_S();
    void fncChange_sSmooth_R();
    void fncChange_sInterpolation();
    void fncChange_sDisplacement();
    void fncChange_sDisplacInterpolate();
    void fncChange_sMinFixation() ;
    void fncChange_sVelocity() ;
    void fncChange_sVelocityVariance() ;
    void fncPress_cbDisplacement();
    void fncPress_cbMinFixation() ;
    void fncPress_cbVelocityVariance();
    void fncUpdateLabels();
    void fncPress_bPlotRMS();
    void fncPress_bPlotDur();
    void fncPress_bPlotVelocities();
    void fncPress_bLeft();
    void fncPress_bRight();
    void apply();


};


#endif // DIALOGTESTSMOOTHPARAMETERS_H
