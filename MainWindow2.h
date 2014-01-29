#ifndef MAINWINDOW2_H
#define MAINWINDOW2_H

#include "MyConstants.h"
#include "GPFixationOperations.h"
#include "GrafixProject.h"

#include "DialogParticipantEditNotes.h"
#include "DialogGrafixError.h"
#include "DialogTestSmoothParameters.h"
#include "DialogConfig.h"
#include "DialogVisualization.h"
#include "DialogVisualizationSegments.h"
#include "DialogSmoothXY.h"
#include "DialogStatistics.h"
#include "DialogHeatMap.h"
#include "DialogExport.h"
#include "DialogOpenProject.h"
#include "DialogInterpolation.h"
#include "DialogEstimateFixations.h"
#include "DialogSaveNewConfiguration.h"

#include <QProgressDialog>
#include <QMainWindow>
#include <QtGui>
#include <QtWidgets/QWidget>
#include <QMutex>
#include <QtConcurrent/QtConcurrent>
#include <QtWidgets/QLayout>
#include <qregion.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qtimer.h>

#include <stdlib.h>
#include <math.h>
#include <fstream>
#include <ios>
#include <iostream>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <iostream>
#include <cstdlib>

#include "armadillo"

namespace Ui {
class MainWindow2;
}

class MainWindow2 : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow2(QWidget *parent = 0);
    ~MainWindow2();

private:

    //actual matrices
    mat roughM;   // [time ms, 0 , left x , left y, right x, right y]
    mat smoothM;  // [time ms, interpolation(0 or 1), x, y]
    mat fixAllM;
    mat experimentalSegmentsM;
    mat autoFixAllM; // Automatic detection of fixation durations

    //pointers to above matrices
    mat* p_roughM;
    mat* p_smoothM;
    mat* p_fixAllM;
    mat* p_experimentalSegmentsM;
    mat* p_autoFixAllM;

    uword _hz;
    uword _secsFragment;
    uword _maxPupilDilation;
    uword _displayStartIndex;
    uword _displayStopIndex;
    double _displayIncrement;
    int _previousFragment;
    int _currentFragment;
    int _nFragments;
    double _degPerPixel;
    int _expWidth;
    int _expHeight;
    Ui::MainWindow2 *ui;
    QMutex _files_mutex;
    QProgressDialog _files_progress;
    QFuture<void> _files_future;
    bool _files_stop_flag;
    int _files_next_position;
    int _currentAction; // Create/ delete/ merge
    int _fixInAction;  // If there is a fixation creation/ merge/ delete happenning it's on.
    int _fixStartPos;
    uword _selectedFixationRow;
    bool _selectedFixationEnd;
    GrafixProject _project;
    GrafixParticipant* p_active_participant;
    GrafixConfiguration _last_selected_configuration;
    int _index_active_participant;
    QSize _maximised_size; //stores window size
    QLabel* p_over_display_label;
    QLabel* p_lMagnification;
    QPushButton* p_bZoomin;
    QPushButton* p_bZoomout;
    int _previous_mouse_posX;
    QRgb _previous_colour;
    bool _configuration_changed;
    bool _hold_changes;
    double _velocity_view_zoom;
    //changing participant / display / editing
    void fncLoadSettings(GrafixConfiguration configuration);
    void fncInitializeProject();
    void fncSetActiveParticipant(int position);
    void fncSetActiveParticipantThread(int position);
    void fncWaitForLoad();
    void fncResetDisplayParams();//Resets the display parameters i.e. when switching participant
    void fncSetDisplayParams();//Sets the display parameters i.e. when switching fragment
    void fncManipulateFix(int from, int to);
    void fncSettingsChanged();
    void fncSetToolTips();
    void fncChangeSlider(QString setting, QSlider* sl, QLineEdit* le);
    void fncChangeLineEdit(QString setting, QLineEdit* le, QSlider* sl);
    void fncPressActionButton(int action, QPushButton* button, QString message);


    //saving loading files
    bool fncReadAllFiles(GrafixParticipant* participant);
    void fncSaveAllFiles(GrafixParticipant *participant);

    //painting
    void paintAll();
    void paintRoughData();
    void paintSmoothData();
    void paintFixations();
    void paintAutomaticFixations();
    void paintVelocity();
    void paintPupilDilation();
    void paintCurrentFixation(int from, int to);
    void paintCurrentFixationOnList(int from, int to);
    void paintExperimentalSegments();
    void paintLabels();
    void paintClear();

protected:
    void  resizeEvent(QResizeEvent* event);
    bool  eventFilter(QObject *obj, QEvent *event);

public slots:
    void fncChange_sInterpolation();
    void fncChange_sDisplacement();
    void fncChange_sDisplacInterpolate();
    void fncChange_sMinFixation() ;
    void fncChange_sVelocity() ;
    void fncChange_sVelocityVariance() ;
    void fncChange_sSigmaS();
    void fncChange_sSigmaR();

    void fncChange_tInterpolation();
    void fncChange_tDisplacement();
    void fncChange_tDisplacInterpolate();
    void fncChange_tMinFixation() ;
    void fncChange_tVelocity() ;
    void fncChange_tVelocityVariance() ;
    void fncChange_tSigmaS();
    void fncChange_tSigmaR();
    void fncChange_tParticipantNumber();
    void fncChange_tSegmentNumber();

    void fncPress_cbDisplacement();
    void fncPress_cbMinFixation() ;
    void fncPress_cbVelocityVariance();

    void fncPress_subMenuDialogConfig();
    void fncPress_subMenuProjectOpen();
    void fncPress_subMenuVisualizeCurrentFragment();
    void fncPress_subMenuVisualizeSegments();
    void fncPress_subMenuExport();
    void fncPress_subMenuCalculateSmooth();
    void fncPress_subMenuRecalculateFixations();
    void fncPress_subMenuShowStatistics();
    void fncPress_subMenuHeatMap();
    void fncPress_subMenuShowErrorLog();
    void fncPress_subMenuDetectFixations();
    void fncPress_subMenuManualEditing();
    void fncPress_subMenuAdjustParameters();
    void fncPress_subMenuChangeConfiguration();
    void fncPress_subMenuClose();
    void fncPress_subMenuReloadConfiguration();

    void fncPress_bNext();
    void fncPress_bPrevious();
    void fncPress_bExecuteManual();
    void fncPress_bParticipantLeft();
    void fncPress_bParticipantRight();
    void fncPress_bCreate();
    void fncPress_bDelete();
    void fncPress_bMerge();
    void fncPress_bExecuteManual(int from, int to);
    void fncPress_bSmoothPursuit();
    void fncPress_bResetFixations();
    void fncPress_bResizeFixation();
    void fncPress_bNotes();
    void fncPress_bAcceptEstimation();
    void fncPress_bInterpolate();
    void fncPress_bSmooth();
    void fncPress_bEstimateFixations();
    void fncPress_bZoomin();
    void fncPress_bZoomout();


};

#endif // MAINWINDOW2_H
