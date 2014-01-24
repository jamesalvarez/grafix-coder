
#include "MainWindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //this->setWindowFlags(this->windowFlags() | Qt::WindowMinMaxButtonsHint);
    //default initialise parameters
    this->_index_active_participant = 0;
    this->p_active_participant = NULL;
    this->_displayStartIndex = 0;
    this->_displayStopIndex = 0;
    this->_currentFragment = 1;
    this->_degPerPixel = Consts::DEFAULT_SETTING_DEGREE_PER_PIXEL;
    this->_displayIncrement = 0;
    this->_expHeight = Consts::DEFAULT_SETTING_EXP_HEIGHT;
    this->_expWidth = Consts::DEFAULT_SETTING_EXP_WIDTH;
    this->_files_next_position = 0;
    this->_files_stop_flag = false;
    this->_fixStartPos = 0;
    this->_fixStartPos = 0;
    this->_hz = Consts::DEFAULT_SETTING_HZ;
    this->_nFragments = 0;
    this->_maxPupilDilation = 0;
    this->_previousFragment = 0;
    this->_secsFragment = Consts::DEFAULT_SETTING_SECS_FRAGMENT;
    this->_currentAction = Consts::AC_NOACTION; // create/ merge/ delete fixation
    this->_fixInAction = Consts::FIX_OFF;   // If we are painting a fixation or not
    this->_configuration_changed = true;

    // Menu events
    connect( ui->subMenuActionProject_config, SIGNAL( triggered() ), this, SLOT( fncPress_subMenuDialogConfig() ) );
    connect( ui->subMenuActionOpen_project, SIGNAL( triggered() ), this, SLOT( fncPress_subMenuProjectOpen() ) );
    connect( ui->subMenuVisualizeCurrentFragment, SIGNAL( triggered() ), this, SLOT( fncPress_subMenuVisualizeCurrentFragment() ) );
    connect( ui->subMenuVisualizeSegmenrt, SIGNAL( triggered() ), this, SLOT( fncPress_subMenuVisualizeSegments() ) );
    connect( ui->subMenuExport, SIGNAL( triggered() ), this, SLOT( fncPress_subMenuExport()));
    connect( ui->subMenuCalculateSmooth, SIGNAL( triggered() ), this, SLOT( fncPress_subMenuCalculateSmooth()));
    connect( ui->subMenuRecalculateFixations, SIGNAL( triggered() ), this, SLOT( fncPress_subMenuRecalculateFixations())); // It recalculates the duration, variance,
    connect( ui->subMenuShowStatistics, SIGNAL( triggered() ), this, SLOT( fncPress_subMenuShowStatistics()));
    connect( ui->subMenuHeatMap, SIGNAL( triggered() ), this, SLOT( fncPress_subMenuHeatMap()));
    connect( ui->subMenuShowErrorLog, SIGNAL(triggered()), this, SLOT(fncPress_subMenuShowErrorLog()));
    connect( ui->subMenuDetectFixations, SIGNAL(triggered()), this,SLOT(fncPress_subMenuDetectFixations()));
    connect( ui->subMenuManualFixationEditing, SIGNAL(triggered()), this, SLOT(fncPress_subMenuManualEditing()));
    connect( ui->subMenuAdjustParameters, SIGNAL( triggered()), this, SLOT(fncPress_subMenuAdjustParameters()));
    connect( ui->subMenuChangeConfiguration, SIGNAL(triggered()), this, SLOT(fncPress_subMenuChangeConfiguration()));
    connect( ui->subMenuClose, SIGNAL( triggered()), this, SLOT( fncPress_subMenuClose()));

    // Main Buttons
    connect( ui->bNext, SIGNAL( clicked() ), this, SLOT( fncPress_bNext() ) );
    connect( ui->bPrevious, SIGNAL( clicked() ), this, SLOT( fncPress_bPrevious() ) );
    connect( ui->bCreate, SIGNAL( clicked()), this, SLOT(fncPress_bCreate()));
    connect( ui->bDelete, SIGNAL( clicked()), this, SLOT( fncPress_bDelete() ) );
    connect( ui->bMerge, SIGNAL( clicked()), this, SLOT( fncPress_bMerge() ) );
    connect( ui->bSmoothPursuit, SIGNAL( clicked()), this, SLOT( fncPress_bSmoothPursuit() ) );
    connect( ui->bExecManual, SIGNAL( clicked()), this, SLOT( fncPress_bExecuteManual() ) );
    connect( ui->lineEditParticipant, SIGNAL(returnPressed()), this, SLOT ( fncChange_tParticipantNumber() ));
    connect( ui->pushButtonParticipantLeft, SIGNAL( clicked() ), this, SLOT( fncPress_bParticipantLeft() ) );
    connect( ui->pushButtonParticipantRight, SIGNAL( clicked() ), this, SLOT( fncPress_bParticipantRight() ) );
    connect( ui->bNotes,SIGNAL(clicked()), this, SLOT (fncPress_bNotes()));

    // Automatic fixation buttons:
    connect( ui->b_acceptEstimation, SIGNAL( clicked() ), this, SLOT( fncPress_bAcceptEstimation() ) );
    connect( ui->b_interpolate, SIGNAL( clicked() ), this, SLOT( fncPress_bInterpolate() ) );
    connect( ui->b_smooth, SIGNAL( clicked() ), this, SLOT( fncPress_bSmooth() ) );
    connect( ui->b_estimateFixations, SIGNAL( clicked() ), this, SLOT( fncPress_bEstimateFixations() ));
    connect( ui->sliderInterpolation, SIGNAL( valueChanged(int)), this, SLOT( fncChange_sInterpolation() ) );
    connect( ui->sliderDisplacInterpolate, SIGNAL( valueChanged(int)), this, SLOT( fncChange_sDisplacInterpolate() ) );
    connect( ui->sliderDisplacement, SIGNAL( valueChanged(int)), this, SLOT( fncChange_sDisplacement() ) );
    connect( ui->sliderMinFixation, SIGNAL( valueChanged(int)), this, SLOT( fncChange_sMinFixation() ) );
    connect( ui->sliderVelocity, SIGNAL( valueChanged(int)), this, SLOT( fncChange_sVelocity() ) );
    connect( ui->sliderVelocityVariance, SIGNAL( valueChanged(int)), this, SLOT( fncChange_sVelocityVariance() ) );
    connect( ui->sliderSigmaR, SIGNAL( valueChanged(int)), this, SLOT( fncChange_sSigmaR()));
    connect( ui->sliderSigmaS, SIGNAL( valueChanged(int)), this, SLOT( fncChange_sSigmaS()));

    connect( ui->leInterpolation, SIGNAL(editingFinished()), this, SLOT( fncChange_tInterpolation() ) );
    connect( ui->leDisplacInterpolate, SIGNAL( editingFinished()), this, SLOT( fncChange_tDisplacInterpolate() ) );
    connect( ui->leDisplacement, SIGNAL( editingFinished()), this, SLOT( fncChange_tDisplacement() ) );
    connect( ui->leMinFixation, SIGNAL( editingFinished()), this, SLOT( fncChange_tMinFixation() ) );
    connect( ui->leVelocity, SIGNAL( editingFinished()), this, SLOT( fncChange_tVelocity() ) );
    connect( ui->leVelocityVariance, SIGNAL( editingFinished()), this, SLOT( fncChange_tVelocityVariance() ) );
    connect( ui->leSigmaR, SIGNAL( editingFinished()), this, SLOT( fncChange_tSigmaR()));
    connect( ui->leSigmaS, SIGNAL( editingFinished()), this, SLOT( fncChange_tSigmaS()));

    connect( ui->cb_displacement, SIGNAL( clicked()), this, SLOT( fncPress_cbDisplacement() ) );
    connect( ui->cb_minFixation, SIGNAL( clicked()), this, SLOT( fncPress_cbMinFixation() ) );
    connect( ui->cb_velocityVariance, SIGNAL( clicked()), this, SLOT( fncPress_cbVelocityVariance() ) );


    p_roughM = &roughM;   // [time ms, 0 , left x , left y, right x, right y]
    p_smoothM = &smoothM;  // [time ms, interpolation(0 or 1), x, y]
    p_fixAllM = &fixAllM;
    p_experimentalSegmentsM = &experimentalSegmentsM;
    p_autoFixAllM = &autoFixAllM; // Automatic detection of fixation durations

    //set slider maxes etc
    ui->sliderDisplacInterpolate->setMinimum(0);
    ui->sliderDisplacInterpolate->setMaximum(60);
    ui->sliderDisplacement->setMinimum(0);
    ui->sliderDisplacement->setMaximum(60);
    ui->sliderVelocityVariance->setMinimum(0);
    ui->sliderVelocityVariance->setMaximum(60);
    ui->sliderVelocity->setMinimum(0);
    ui->sliderVelocity->setMaximum(50);
    ui->sliderInterpolation->setMaximum(180);
    ui->sliderInterpolation->setMinimum(0);
    ui->sliderMinFixation->setMinimum(0);
    ui->sliderMinFixation->setMaximum(200);

    //start new label
    this->p_over_display_label = new QLabel(this);
    this->p_over_display_label->setAttribute(Qt::WA_TransparentForMouseEvents);
    this->p_over_display_label->raise();

    //ui->lPanelMissingData->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->frameData->setAttribute(Qt::WA_TransparentForMouseEvents);
    this->setAttribute(Qt::WA_TransparentForMouseEvents);
    // Allow the main widget to track the mouse
    //ui->centralWidget->setAttribute(Qt::WA_TransparentForMouseEvents);
    this->setMouseTracking(true);

    ui->stackedWidget->setCurrentWidget(ui->page_manual);

    qApp->installEventFilter(this);

    fncInitializeProject();

    //show maximised and remember size, for some reason it sometimes expands a little TODO: fix this
    QDesktopWidget *desktop = QApplication::desktop();
    int h = desktop->availableGeometry().height() - 100;
    int w = desktop->availableGeometry().width();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (!obj->isWindowType()) return false;

    int posX = -1;

    if (event->type() == QEvent::MouseMove ||
        event->type() == QEvent::MouseButtonPress ||
        event->type() == QEvent::MouseButtonRelease)
    {
        // When we move the mouse and we are executing an action, we paint the fixation!
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        QPoint mouse_pos = this->p_over_display_label->mapFromGlobal(mouseEvent->pos());

        posX = mouse_pos.x() <= 0 ? 0 : mouse_pos.x();
//        if (mouse_pos.x() <= 0 )
//        {
//            posX = 0;
//        }
//        else
//        {
//            posX = mouse_pos.x();
//        }
    }

    if (event->type() == QEvent::MouseMove)
    {
        paintExperimentalSegments();

        // If the cursor is in the central area we draw a line
        if (posX >= 0 && posX <=ui->lPanelFixations->width() )
        {
            QPixmap pixmap = *this->p_over_display_label->pixmap();
            QPainter painter(&pixmap);
            QPen myPen(Qt::green, 1, Qt::SolidLine);
            painter.drawLine(posX,0,posX,pixmap.height());
            this->p_over_display_label->setPixmap(pixmap);
            painter.end();
        }

        // Convert the posX value to the real one in the file:
        int file_posX = _displayStartIndex + (posX * _displayIncrement);

        if (_fixInAction == Consts::FIX_OFF)
        {
            // Paint the  FRom/to labels
            ui->lFrom->clear();
            ui->lFrom->setText(QString::number(file_posX));
        }
        else
        {
            // If we are manipulating a fixation
            paintFixations();
            if (_fixStartPos >= file_posX)
            {
                // If "to" is smaller than "from", do nothing
                file_posX = 0;
            }
            else
            {
                // "from" is bigger than "to". Paint rectangle
               paintCurrentFixation(_fixStartPos, file_posX);
               paintCurrentFixationOnList(_fixStartPos, file_posX);
            }

            ui->lTo->clear();                   // Change "lTo" label
            ui->lTo->setText(QString::number(file_posX));
        }
    }

    if (event->type() == QEvent::MouseButtonRelease)
    {
         _fixInAction = Consts::FIX_OFF;
         int fixStopPos = _displayStartIndex + (posX * _displayIncrement);
         ui->lTo->setText("0");
         // function for create/ merge/ delete depending in the action.
         fncManipulateFix(_fixStartPos, fixStopPos);
         _fixStartPos = -1;
    }

    if (event->type() == QEvent::MouseButtonPress)
    {
       _fixInAction = Consts::FIX_ON;
       // store start value
       _fixStartPos = _displayStartIndex + (posX * _displayIncrement);
       statusBar()->showMessage(QString("Mouse clicked on ") + obj->objectName());
    }
    return false;
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
   //QMainWindow::resizeEvent(event);
   int w = ui->lPanelMissingData->width();
   int h = ui->lPanelVelocity->y() + ui->lPanelVelocity->height() - ui->lPanelMissingData->y();

   QPoint new_pos = ui->lPanelMissingData->mapToGlobal(QPoint(0,0));
   this->p_over_display_label->setGeometry(new_pos.x(),
                                           new_pos.y(),
                                           w,
                                           h);

   this->p_over_display_label->raise();
   statusBar()->showMessage("Resized");
   paintAll();
}


MainWindow::~MainWindow()
{
    _project.SaveSettings();
    delete this->p_over_display_label;
    delete ui;
}


void MainWindow::fncInitializeProject()
{
    //loads settings (info of previous project
    QSettings settings("options.ini", QSettings::IniFormat);

    // If there is not a project opened (in the settings file), we open the openProject dialog.
    while (settings.value(Consts::SETTING_PROJECT).isNull() ||
            !(_project.LoadProjectSettings(settings.value(Consts::SETTING_PROJECT).toString())))
    {
        DialogOpenProject w(&_project);
        w.setWindowTitle(tr("Select project or create new one"));
        w.exec();

        //saves directory of this project
        settings.setValue(Consts::SETTING_PROJECT,_project.GetFullDirectory());
    }

    //set current participant
    if (_project.numParticipants()>0)
    {
        qDebug() << "Participants out:" << _project.numParticipants() << "\n";
        fncSetActiveParticipant(0);//TODO: Settings remember last participant
    }

    fncLoadSettings(Consts::ACTIVE_CONFIGURATION());
}

void MainWindow::fncWaitForLoad()
{
    QMessageBox msgBox;
    QString text = "Please wait.";
    this->setCursor(Qt::WaitCursor);
    QApplication::processEvents();
    msgBox.setText(text);
    msgBox.setStandardButtons(0);
    msgBox.show();
    qApp->processEvents();
    while(!_files_future.isFinished() && this->_index_active_participant != this->_files_next_position)
    {
        QApplication::processEvents();
    }
    msgBox.close();
    this->setCursor(Qt::ArrowCursor);
}

void MainWindow::fncSetActiveParticipant(int position)
{
    //check position is available
    if (!_project.HasParticipant(position)) return;

    this->_files_next_position = position;
    ui->statusBar->showMessage("Status: Closing participant...");
    _files_stop_flag = true; //sets flag telling current file loading process to end
    while(!_files_future.isFinished() && position == _files_next_position)
    {
        qApp->processEvents();
    }

    //check if target position is still this one before setting future
    if (position == _files_next_position)
    {
        ui->statusBar->showMessage("Status: Loading...");
        _files_future = QtConcurrent::run(this, &MainWindow::fncSetActiveParticipantThread, position);

        while(!_files_future.isFinished())
        {
            QApplication::processEvents();
        }

        if(this->_index_active_participant == position)
        {
            fncResetDisplayParams();
            paintAll();
        }
    }
}

void MainWindow::fncSetActiveParticipantThread(int position)
{
    //remember: do not call gui things in non gui thread (some bits dont like it)
    _files_stop_flag = false;
    this->roughM.reset();
    this->smoothM.reset();
    this->fixAllM.reset();
    this->experimentalSegmentsM.reset();
    this->autoFixAllM.reset();

    if (fncReadAllFiles(this->_project.GetParticipant(position)))
    {
        this->p_active_participant = this->_project.GetParticipant(position);
        this->_index_active_participant = position;
    }
}

void MainWindow::fncLoadSettings(GrafixConfiguration configuration)
{
    // Read the Configuration info from settings.

    _expHeight = _project.GetProjectSetting(Consts::SETTING_EXP_HEIGHT, configuration).toInt();
    _expWidth = _project.GetProjectSetting(Consts::SETTING_EXP_WIDTH, configuration).toInt();
    _hz = _project.GetProjectSetting(Consts::SETTING_HZ, configuration).toInt();
    _secsFragment = _project.GetProjectSetting(Consts::SETTING_SECS_FRAGMENT, configuration).toInt();
    _degPerPixel = _project.GetProjectSetting(Consts::SETTING_DEGREE_PER_PIX, configuration).toDouble();

    ui->sliderSigmaS->setValue(
                _project.GetProjectSetting(
                        Consts::SETTING_SMOOTHING_SIGMA_S, configuration).toInt());

    ui->sliderSigmaR->setValue(
                _project.GetProjectSetting(
                        Consts::SETTING_SMOOTHING_SIGMA_R, configuration).toInt());


    ui->sliderInterpolation->setValue(
                _project.GetProjectSetting(
                        Consts::SETTING_INTERP_LATENCY, configuration).toInt());

    ui->sliderMinFixation->setValue(
                _project.GetProjectSetting(
                        Consts::SETTING_POSTHOC_MIN_DURATION_VAL, configuration).toInt());

    ui->sliderVelocity->setValue(
                _project.GetProjectSetting(
                        Consts::SETTING_INTERP_VELOCITY_THRESHOLD, configuration).toInt());

    ui->sliderVelocityVariance->setValue(
                (int)(_project.GetProjectSetting(
                        Consts::SETTING_POSTHOC_LIMIT_RMS_VAL, configuration).toDouble() * 100));

    ui->sliderDisplacement->setValue(
                (int)(_project.GetProjectSetting(
                          Consts::SETTING_POSTHOC_MERGE_CONSECUTIVE_VAL, configuration).toDouble() * 100));

    ui->sliderDisplacInterpolate->setValue(
                (int)(_project.GetProjectSetting(
                          Consts::SETTING_INTERP_MAXIMUM_DISPLACEMENT, configuration).toDouble() * 100));

    ui->cb_displacement->setChecked(_project.GetProjectSetting(
                                        Consts::SETTING_POSTHOC_MERGE_CONSECUTIVE_FLAG, configuration).toBool());
    ui->cb_minFixation->setChecked(_project.GetProjectSetting(
                                        Consts::SETTING_POSTHOC_MIN_DURATION_FLAG, configuration).toBool());
    ui->cb_velocityVariance->setChecked(_project.GetProjectSetting(
                                        Consts::SETTING_POSTHOC_LIMIT_RMS_FLAG, configuration).toBool());
}

void MainWindow::fncResetDisplayParams()
{
    _currentFragment = 1;
    _previousFragment = 0;

    fncSetDisplayParams();
}

void MainWindow::fncSetDisplayParams()
{
    // Calculate fragments
    _nFragments = floor((double)(roughM.n_rows / ( _secsFragment * _hz))) + 1;

    if (_nFragments == 1)
        _displayStartIndex = 0;
    else
        _displayStartIndex = ((_currentFragment-1) * _secsFragment * _hz);

    _displayStopIndex =  _displayStartIndex + (_secsFragment * _hz);

    if (_displayStopIndex > roughM.n_rows)
        _displayStopIndex = roughM.n_rows;

    // Calculate the increment in the frame from one point to the next. (important if resizing)
    _displayIncrement = (double) _secsFragment * _hz / ui->frameData->width();
}



// function for create/ merge/ delete depending in the selected action.
void MainWindow::fncManipulateFix(int from, int to)
{
    if (from < to){ // If the end of the fixation is bigger than the start.
        switch (_currentAction){
            case Consts::AC_CREATE:
               fixAllM = GPFixationOperations::fncCreateFixation(fixAllM, roughM,   _hz,  _secsFragment,  _currentFragment, from, to, _expWidth, _expHeight, _degPerPixel);
               break;
            case Consts::AC_DELETE:
                fixAllM = GPFixationOperations::fncDeleteFixations(fixAllM, _hz,  _secsFragment,  _currentFragment, from,  to);
                break;
            case Consts::AC_MERGE:
                fixAllM = GPFixationOperations::fncMergeFixations(fixAllM, roughM, _hz,  _secsFragment,  _currentFragment, from,  to, _expWidth, _expHeight, _degPerPixel);
                break;
            case Consts::AC_SMOOTH_PURSUIT:
                fixAllM = GPFixationOperations::fncSmoothPursuitFixation(fixAllM, _hz,  _secsFragment,  _currentFragment, from,  to);
                break;
        }

        // Save file
        if (!fixAllM.is_empty() && p_active_participant != NULL)
            GPMatrixFunctions::saveFile(fixAllM, p_active_participant->GetMatrixPath(Consts::MATRIX_FIXALL));

        // Paint fixations again
        paintFixations();
    }
}


/**
 *          READ/WRITE FILES METHODS
 */

bool MainWindow::fncReadAllFiles(GrafixParticipant* participant)
{
    if (_files_stop_flag) return false;
    GPMatrixFunctions::readFileSafe(roughM,participant->GetMatrixPath(Consts::MATRIX_ROUGH));
    if (_files_stop_flag) return false;
    GPMatrixFunctions::readFileSafe(smoothM, participant->GetMatrixPath(Consts::MATRIX_SMOOTH));
    if (_files_stop_flag) return false;
    GPMatrixFunctions::readFileSafe(fixAllM, participant->GetMatrixPath(Consts::MATRIX_FIXALL));
    if (_files_stop_flag) return false;
    GPMatrixFunctions::readFileSafe(autoFixAllM,participant->GetMatrixPath(Consts::MATRIX_AUTOFIXALL));
    if (_files_stop_flag) return false;
    GPMatrixFunctions::readFileSafe(experimentalSegmentsM,participant->GetMatrixPath(Consts::MATRIX_SEGMENTS));
    if (_files_stop_flag) return false;

    // If fixAllM has only 6 columns, we add another one for the smooth pursuit.
    if (fixAllM.n_cols == 6){
        mat a = zeros<mat>(fixAllM.n_rows,1);
        fixAllM = join_rows(fixAllM, a);
    }else if (fixAllM.n_cols == 10){  // Pre previous version of the application included more info.
        fixAllM = join_rows(fixAllM.cols(0,5),fixAllM.col(9));
        fixAllM = join_rows(fixAllM, zeros(fixAllM.n_rows,1)); // Pupil dilation!
    }

    return true;
}


void MainWindow::fncSaveAllFiles(GrafixParticipant *participant)
{
    if (!roughM.is_empty()) GPMatrixFunctions::saveFile(roughM,participant->GetMatrixPath(Consts::MATRIX_ROUGH));
    if (!smoothM.is_empty()) GPMatrixFunctions::saveFile(smoothM, participant->GetMatrixPath(Consts::MATRIX_SMOOTH));
    if (!fixAllM.is_empty()) GPMatrixFunctions::saveFile(fixAllM, participant->GetMatrixPath(Consts::MATRIX_FIXALL));
    if (!autoFixAllM.is_empty()) GPMatrixFunctions::saveFile(autoFixAllM, participant->GetMatrixPath(Consts::MATRIX_AUTOFIXALL));
    if (!experimentalSegmentsM.is_empty()) GPMatrixFunctions::saveFile(experimentalSegmentsM, participant->GetMatrixPath(Consts::MATRIX_SEGMENTS));
}


void MainWindow::paintAll()
{
    paintClear();
    fncSetDisplayParams();

    if (p_active_participant == NULL)
        return;

    paintRoughData();
    paintSmoothData();
    paintFixations();
    paintExperimentalSegments();
    paintVelocity();
    paintPupilDilation();
    paintLabels();

    ui->statusBar->showMessage("Status: Ready.");
}


void MainWindow::paintExperimentalSegments(){
    // The method paints grey boxes in the areas that don't need to be coded
    //TODO: Expand to all other boxes too

    QPixmap pixmap2(this->p_over_display_label->width(), this->p_over_display_label->height());
    pixmap2.fill(Qt::transparent);
    //pixmap2.fill(Qt::white);
    if (!experimentalSegmentsM.is_empty())
    {
        QPainter painter(&pixmap2);

        painter.setBrush(QBrush(QColor(192, 192, 192, 127)));

        // Order the segments depending on time
        uvec indices = sort_index(experimentalSegmentsM.cols(1,1));
        mat expSegAux = experimentalSegmentsM.rows(indices);

        // find the segments that may be involved in the current fragment
        uvec fixIndex =  arma::find(expSegAux.col(2) >= _displayStartIndex);
        mat startSAux = expSegAux.rows(fixIndex);
        fixIndex =  arma::find(startSAux.col(1) <= _displayStopIndex);

        if (fixIndex.is_empty()){ // If it's empty that means that there is not segment in this fragment, hence, all is grey
            painter.drawRect(QRect( 0 ,0,pixmap2.width(), pixmap2.height()));
        }else{

            startSAux =  startSAux.rows(fixIndex); // in this matrix we have all the segments that start in the current fragment
            fixIndex = arma::find(expSegAux.col(0) == startSAux(0,0)); // We find the first segment that starts in the current fragment

            bool isNextSegment = false;
            uword segmentNumber = fixIndex(0,0);
            while (!isNextSegment && segmentNumber < expSegAux.n_rows){
                if (fixIndex(0,0) <= 0 || expSegAux(fixIndex(0,0)-1,2) <= _displayStartIndex){ // this is the very first segment or it is the first segment of the fragment
                    painter.drawRect(QRect( 0 ,0,(expSegAux(0,1)- _displayStartIndex) * (1/_displayIncrement ), pixmap2.height()));

                }
                if (segmentNumber > 0){
                    if (expSegAux(segmentNumber,1) >=_displayStopIndex){
                        painter.drawRect(QRect( (expSegAux(segmentNumber-1,2)- _displayStartIndex) * (1/_displayIncrement ) ,0,pixmap2.width(), pixmap2.height()));
                    }else{
                        int x1 = (expSegAux(segmentNumber-1,2)- _displayStartIndex) * (1/_displayIncrement );
                        int x2 = ((expSegAux(segmentNumber,1)- _displayStartIndex) * (1/_displayIncrement )) - x1;

                        painter.drawRect(QRect( x1 ,0,x2, pixmap2.height()));
                    }
                }
                if (expSegAux(segmentNumber,2) >=_displayStopIndex){
                    isNextSegment = true;
                }
                segmentNumber = segmentNumber + 1;
            }

        }
        painter.end();
    }
    this->p_over_display_label->setPixmap(pixmap2);

}

void MainWindow::paintRoughData()
{
    if (roughM.is_empty()) return;
    QPixmap pixmapMissingData(ui->lPanelMissingData->width()-2,ui->lPanelMissingData->height()-2);
    pixmapMissingData.fill(Qt::transparent);
    QPixmap pixmap(ui->lPanelRough->width()-2,ui->lPanelRough->height()-2);
    pixmap.fill(Qt::white);
    QPainter painter(&pixmap);
    QPainter painterMissingData(&pixmapMissingData);
    QPen myPen(Qt::red, 1.2, Qt::SolidLine);

   // also so el la capacidad del array es menor que stop index
    for (uword i = _displayStartIndex; i < _displayStopIndex; ++i) {
        myPen.setColor(QColor(255, 0, 0, 127));

        if (roughM(i,2 ) != -1){
            painter.setPen(myPen);
            painter.drawPoint( (i- _displayStartIndex )*(1/_displayIncrement ), roughM(i,2 ) * ui->lPanelRough->height()); // XL
        }else{ // Missing data
            painterMissingData.setPen(myPen);
            painterMissingData.drawPoint( (i- _displayStartIndex+1)*(1/_displayIncrement ), 10); // XL
        }

        myPen.setColor(QColor(0, 0, 128, 127));
        if (roughM(i,3 ) != -1){
            painter.setPen(myPen);
            painter.drawPoint( (i- _displayStartIndex)*(1/_displayIncrement ), roughM(i,3 ) * ui->lPanelRough->height()); // YL
        }else{ // Missing data
            painterMissingData.setPen(myPen);
            painterMissingData.drawPoint( (i- _displayStartIndex+1)*(1/_displayIncrement ), 20); //XR
        }

       myPen.setColor(QColor(255, 50, 0, 127));
        if (roughM(i,4 ) != -1){
            painter.setPen(myPen);
            painter.drawPoint( (i- _displayStartIndex)*(1/_displayIncrement ), roughM(i,4 ) * ui->lPanelRough->height()); // XR
        }else{ // Missing data
            painterMissingData.setPen(myPen);
            painterMissingData.drawPoint( (i- _displayStartIndex+1)*(1/_displayIncrement ), 30); //XR
        }

        myPen.setColor(QColor(0, 50, 128, 127));
        if (roughM(i,5 ) != -1){
            painter.setPen(myPen);
            painter.drawPoint( (i- _displayStartIndex)*(1/_displayIncrement ), roughM(i,5 ) * ui->lPanelRough->height()); // YR
        }else{ // Missing data
            painterMissingData.setPen(myPen);
            painterMissingData.drawPoint( (i- _displayStartIndex+1)*(1/_displayIncrement ), 40); //XR
        }
    }
    ui->lPanelRough->setPixmap(pixmap);
    ui->lPanelMissingData->setPixmap(pixmapMissingData);

    painter.end();
    painterMissingData.end();
}

void MainWindow::paintSmoothData()
{
    if (smoothM.is_empty()) return;

    //parameters for spacing flags
    int flInterpolate = 15;
    int flDisplacement = 35;
    int flminVariance = 55;
    int flminFixations = 75;

    QPixmap pixmap(ui->lPanelSmooth->width()-2,ui->lPanelSmooth->height()-2);
    QPixmap pixmapFlags(ui->lPanelFlags->width()-2,ui->lPanelFlags->height()-2);
    pixmap.fill(Qt::white);
    pixmapFlags.fill(Qt::white);

    QPainter painter(&pixmap);
    QPainter painterFlags(&pixmapFlags);
    QPen myPen(Qt::red, 1, Qt::SolidLine);

    for (uword i = _displayStartIndex; i < _displayStopIndex; ++i) {

        myPen.setColor(QColor(255, 0, 0, 127));
        myPen.setWidth(1);
        painter.setPen(myPen);
        painter.drawPoint((i-_displayStartIndex)*(1/_displayIncrement ), smoothM(i ,2 ) * ui->lPanelRough->height() / _expWidth); // XL

        myPen.setColor(QColor(0, 0, 128, 127));
        painter.setPen(myPen);
        painter.drawPoint((i- _displayStartIndex)*(1/_displayIncrement ), smoothM(i,3 ) * ui->lPanelRough->height() / _expHeight); // YL

        // PAINT FLAGS!
        if (smoothM.n_cols > 4){
            if (smoothM(i,6) == 1){ // Paint interpolated data
                myPen.setColor(QColor(0, 139, 69,127));
                myPen.setWidth(2);
                painter.setPen(myPen);
                painter.drawPoint((i-_displayStartIndex)*(1/_displayIncrement ), smoothM(i ,2 ) * ui->lPanelRough->height() / _expWidth); // XL
                painter.drawPoint((i- _displayStartIndex)*(1/_displayIncrement ), smoothM(i,3 ) * ui->lPanelRough->height() / _expHeight); // YL

                myPen.setWidth(5);
                painterFlags.setPen(myPen);
                painterFlags.drawPoint((i- _displayStartIndex)*(1/_displayIncrement ), flInterpolate);

            }
            if (smoothM.n_cols>=  10){  // Paint the rest of the flags
                myPen.setWidth(4);
                painterFlags.setPen(myPen);
                if (smoothM(i,7) == 1){ // Paint Displacement flag
                    myPen.setColor(QColor(100, 0, 69,127));
                    painterFlags.setPen(myPen);
                    painterFlags.drawPoint((i- _displayStartIndex)*(1/_displayIncrement ), flDisplacement);
                }
                if (smoothM(i,8) == 1){ // Paint min variance flag
                    myPen.setColor(QColor(181, 88, 228,127));
                    painterFlags.setPen(myPen);
                    painterFlags.drawPoint((i- _displayStartIndex)*(1/_displayIncrement ), flminVariance);
                }
                if (smoothM(i,9) == 1){ // Paint Min fixations flag
                    myPen.setColor(QColor(255, 99, 71,127));
                    painterFlags.setPen(myPen);
                    painterFlags.drawPoint((i- _displayStartIndex)*(1/_displayIncrement ), flminFixations);
                }
            }
        }
    }

    myPen.setColor(QColor(128, 128, 128,255));
    painterFlags.setPen(myPen);
    painterFlags.drawText(5,flInterpolate-5," - Interpolation");
    painterFlags.drawText(5,flDisplacement-5," - Consecutive fixations merged");
    painterFlags.drawText(5,flminVariance-5, " - High variance removed");
    painterFlags.drawText(5,flminFixations-5, " - Short fixations removed");

    ui->lPanelSmooth->setPixmap(pixmap);
    ui->lPanelFlags->setPixmap(pixmapFlags);

    painter.end();
    painterFlags.end();

}

void MainWindow::paintFixations()
{
    if (fixAllM.is_empty() && autoFixAllM.is_empty()) return;

    //this paints both the fixations and autofixations
    int fixTPos = 13;
    int fixPos = 15;
    int barHeight = 16;
    int autofixPos = 50;
    int autofixTPos = 48;

    QPixmap pixmapFixList(ui->lPanelFixList->width()-2,ui->lPanelFixList->height()-2);
    pixmapFixList.fill(Qt::transparent);
    QPixmap pixmap(ui->lPanelFixations->width()-2,ui->lPanelFixations->height()-2);
    pixmap.fill(Qt::white);

    QPainter painter(&pixmap);
    QPainter painterFixList(&pixmapFixList);

    int counter = 0;

    QFont font=painter.font() ;
    font.setPointSize ( 8 );
    painter.setFont(font);

    font.setPointSize ( 8 );
    painterFixList.setFont(font);

    std::stringstream number;
    number.str("");

    double posStart = 0;
    double posEnd = 0;

    // Just to optimize the code
    int auxIndex = 0;
    if (fixAllM.n_rows > 0){
        uvec fixIndex =  arma::find(fixAllM.col(1) >= _displayStartIndex);
        if (!fixIndex.empty()){
            auxIndex = fixIndex(0);
        }
    }


    for (uword i = auxIndex; i < fixAllM.n_rows  ; i++){

        if ((fixAllM(i,0) >= _displayStartIndex && fixAllM(i,0) <=  _displayStopIndex ) ||(fixAllM(i,0) <= _displayStartIndex && fixAllM(i,1) >= _displayStartIndex )){

            if (fixAllM(i,0) <= _displayStartIndex && fixAllM(i,1) >= _displayStartIndex ) // If it's a fixation that didn't end in the previous segment
                posStart = -1;
            else
                posStart = (fixAllM(i,0) - _displayStartIndex ) * (1/_displayIncrement);

            posEnd = ((fixAllM(i,1) - _displayStartIndex ) * (1/_displayIncrement)) - posStart;

            if (fixAllM(i,5) == Consts::SMOOTHP_YES){
                painter.setBrush(QBrush("#a32e0c"));
            }else{
                painter.setBrush(QBrush("#1ac500"));
            }
            painter.drawRect(QRect( posStart ,fixPos, posEnd,barHeight));
            number.str("");
            number << counter;
            painter.drawText( QPoint(posStart,fixTPos), number.str().c_str() );
            if (counter <= 47){ // Paint the fixations list
                painterFixList.drawText( QPoint(0,((counter +1) * 17)+ 20), number.str().c_str() );
                number.str("");
                number << fixAllM(i,0) ;
                painterFixList.drawText( QPoint(30,((counter +1) * 17)+ 20), number.str().c_str());
                number.str("");
                number << fixAllM(i,1) ;
                painterFixList.drawText( QPoint(90,((counter +1) * 17)+ 20), number.str().c_str());
                number.str("");
                number << fixAllM(i,2) ;
                painterFixList.drawText( QPoint(150,((counter +1) * 17)+ 20), number.str().c_str());
            }else if (counter == 48){
                painterFixList.drawText( QPoint(0,((counter +1) * 17)+ 20), "...");
            }

            counter ++; // Next fixation

        }else if (fixAllM(i,1) >= _displayStopIndex){
            break;
        }
    }

    //now paint auto fixations
   counter = 0;
   posStart = 0;
   posEnd = 0;

   // Just to optimize the code
   auxIndex = 0;
   if (autoFixAllM.n_rows > 0){
       uvec fixIndex =  arma::find(autoFixAllM.col(1) >= _displayStartIndex);
       if (!fixIndex.empty()){
           auxIndex = fixIndex(0);
       }
   }

   for (uword i = auxIndex; i < autoFixAllM.n_rows  ; i++){

       if ((autoFixAllM(i,0) >= _displayStartIndex && autoFixAllM(i,0) <=  _displayStopIndex ) ||(autoFixAllM(i,0) <= _displayStartIndex && autoFixAllM(i,1) >= _displayStartIndex )){

           if (autoFixAllM(i,0) <= _displayStartIndex && autoFixAllM(i,1) >= _displayStartIndex ) // If it's a fixation that didn't end in the previous segment
               posStart = -1;
           else
               posStart = (autoFixAllM(i,0) - _displayStartIndex ) * (1/_displayIncrement);

           posEnd = ((autoFixAllM(i,1) - _displayStartIndex ) * (1/_displayIncrement)) - posStart;

           if (autoFixAllM(i,5) == Consts::SMOOTHP_YES){
               painter.setBrush(QBrush("#a32e0c"));
           }else{
               painter.setBrush(QBrush("#ff8e00"));
           }
           painter.drawRect(QRect( posStart ,autofixPos, posEnd,barHeight));
           number.str("");
           number << counter;
           painter.drawText( QPoint(posStart,autofixTPos), number.str().c_str() );

           counter ++; // Next fixation

       }else if (autoFixAllM(i,1) >= _displayStopIndex){
           break;
       }
   }

    ui->lPanelFixations->setPixmap(pixmap);
    ui->lPanelFixList->setPixmap(pixmapFixList);
    painter.end();
    painterFixList.end();
}


void MainWindow::paintVelocity()
{
    if (smoothM.is_empty()) return;

    QPixmap pixmap(ui->lPanelVelocity->width()-2,ui->lPanelVelocity->height()-2);
    pixmap.fill(Qt::white);

    QPainter painter(&pixmap);
    QPen myPen(Qt::red, 1, Qt::SolidLine);

    double prevVel, curVel, prevAmp, curAmp = 0;
    int x1,x2 = 0;
    int x_2back, x_1back, x_cur, y_2back, y_1back, y_cur;

    for (uword i = _displayStartIndex+2; i < _displayStopIndex; ++i) {
        x1 = (i-_displayStartIndex-1)*(1/_displayIncrement );
        x2 = (i-_displayStartIndex)*(1/_displayIncrement );

        x_2back = smoothM(i-2,2);
        x_1back = smoothM(i-1,2);
        x_cur   = smoothM(i,2);
        y_2back = smoothM(i-2,3);
        y_1back = smoothM(i-1,3);
        y_cur   = smoothM(i,3);

        // oldAmp=sqrt(((XTwoBack-XOneBack)^2+(YTwoBack-YOneBack)^2)/2);
        prevAmp = sqrt((double)( ((x_2back - x_1back)* (x_2back - x_1back)) + ((y_2back - y_1back)* (y_2back - y_1back)))/2);
        curAmp = sqrt((double)(((x_1back - x_cur)*(x_1back - x_cur)) + ((y_1back - y_cur)*(y_1back - y_cur)))/2);

        prevVel = (double)prevAmp*_hz * _degPerPixel; // We calculate it x2 to see it better
        curVel = (double)curAmp*_hz * _degPerPixel;

        // Paint Smooth velocity
        myPen.setColor(QColor(255, 0, 0, 127));
        painter.setPen(myPen);
        painter.drawLine(QPoint(x1, prevVel), QPoint(x2,curVel)); // XL
    }

    ui->lPanelVelocity->setPixmap(pixmap);
    painter.end();
}

void MainWindow::paintPupilDilation()
{
    if (roughM.n_cols != 8) return;

    QPixmap pixmap(ui->lPanelPupilDilation->width()-2,ui->lPanelPupilDilation->height()-2);
    pixmap.fill(Qt::white);
    QPainter painter(&pixmap);
    QPen myPen(Qt::red, 1, Qt::SolidLine);

    for (uword i = _displayStartIndex; i < _displayStopIndex; ++i) {

        myPen.setColor(QColor(255, 0, 0, 127));
        myPen.setWidth(1);
        painter.setPen(myPen);
        painter.drawPoint((i-_displayStartIndex)*(1/_displayIncrement ), ui->lPanelPupilDilation->height() - (roughM(i ,6 ) * ui->lPanelPupilDilation->height()/_maxPupilDilation)); // Left dilation

        myPen.setColor(QColor(0, 0, 128, 127));
        painter.setPen(myPen);
        painter.drawPoint((i- _displayStartIndex)*(1/_displayIncrement ), ui->lPanelPupilDilation->height() - (roughM(i,7 ) * ui->lPanelPupilDilation->height()/_maxPupilDilation)); // Right dilation
    }

    painter.end();
    ui->lPanelPupilDilation->setPixmap(pixmap);
}


void MainWindow::paintCurrentFixation(int from, int to)
{
    // Paints the fixation that we are manipulating at this precise moment
    //QPixmap pixmap(ui->lPanelFixations->width()-2,ui->lPanelFixations->height()-2);
    //pixmap.fill(Qt::transparent);

    int fixPos = 15;
    int barHeight = 16;

    if (ui->lPanelFixations->pixmap() == NULL) return;
    QPixmap pixmap = (*ui->lPanelFixations->pixmap());
    QPainter painter(&pixmap);
    QPen myPen(Qt::green, 1, Qt::SolidLine);

    painter.setPen(myPen);
    painter.setBrush(QBrush("#CD00CD"));

    int posStart = (from - ((_currentFragment-1) * _secsFragment * _hz) ) * (1/_displayIncrement);
    int posEnd = ((to - ((_currentFragment-1) * _secsFragment * _hz) ) * (1/_displayIncrement)) - posStart;

    painter.drawRect(QRect( posStart ,fixPos, posEnd,barHeight));

    ui->lPanelFixations->setPixmap(pixmap);
    painter.end();
}


void MainWindow::paintCurrentFixationOnList(int from, int to)
{
    // Paint the fixation that we are manipulating at the list on the left.
    // If we are creating the fixation on top of other fixations, it paints them bold
    QPixmap pixmapFixList(ui->lPanelFixList->width()-2,ui->lPanelFixList->height()-2);
    pixmapFixList.fill(Qt::transparent);

    QPainter painterFixList(&pixmapFixList);

    int counter = 0;
    QFont font=painterFixList.font() ;

    std::stringstream number;
    number.str("");

    int auxIndex = 0;
    if (fixAllM.n_rows > 0){
        uvec fixIndex =  arma::find(fixAllM.col(1) >= _displayStartIndex);
        if (!fixIndex.empty()){
            auxIndex = fixIndex(0);
        }
    }
    for (uword i = auxIndex; i < fixAllM.n_rows  ; i++){
        if ((fixAllM(i,0) >= _displayStartIndex && fixAllM(i,0) <=  _displayStopIndex ) ||(fixAllM(i,0) <= _displayStartIndex && fixAllM(i,1) >= _displayStartIndex )){

            // The current fixations that are in the interval ==> bold
            if  ((fixAllM(i,0) >= from && fixAllM(i,0) <=  to ) ||(fixAllM(i,0) <= from && fixAllM(i,1) >= from )){
                font.setPointSize ( 12 );
                font.setBold(true);
                painterFixList.setFont(font);
            }else{
                font.setPointSize ( 10 );
                font.setBold(false);
                painterFixList.setFont(font);
            }
            number.str("");
            number << counter;
            if (counter <= 47){ // Paint the fixations list
                painterFixList.drawText( QPoint(0,((counter +1) * 17)+ 20), number.str().c_str() );
                number.str("");
                number << fixAllM(i,0) ;
                painterFixList.drawText( QPoint(30,((counter +1) * 17)+ 20), number.str().c_str());
                number.str("");
                number << fixAllM(i,1) ;
                painterFixList.drawText( QPoint(90,((counter +1) * 17)+ 20), number.str().c_str());
                number.str("");
                number << fixAllM(i,2) ;
                painterFixList.drawText( QPoint(150,((counter +1) * 17)+ 20), number.str().c_str());
            }else if (counter == 48){
                painterFixList.drawText( QPoint(0,((counter +1) * 17)+ 20), "...");
                break;
            }

            counter ++; // Next fixation

        }else if (fixAllM(i,1) >= _displayStopIndex){
            break;
        }
    }

    ui->lPanelFixList->setPixmap(pixmapFixList);

    painterFixList.end();
}

void MainWindow::paintLabels()
{

    if (p_active_participant == NULL) return;
    // Find Max Pupil dilation and paint the number
    if ( roughM.n_cols == 8){
        //mat aux = roughM.cols(6,7);
        //maxPupilDilation = aux.max() + 1;
        //float medPupilDialation = maxPupilDilation / 2;
        //ui->lDialMax->setText(QString::number(maxPupilDilation));
        //ui->lDialMed->setText(QString::number(medPupilDialation));
    }else{ // No pupil dilation
        //ui->lDialMax->setText("");
        //ui->lDialMed->setText("");
        //ui->lDialMin->setText("");
        //ui->ldegree->setText("");
        //ui->ldegree2->setText("");
    }


    // Paint Initial values:
    ui->lSubject->setText(p_active_participant->GetName());
    ui->lSegment->setText(QString::number(_currentFragment) + " / " + QString::number(_nFragments));

    //ui->lsecsSegment->setText(QString::number(secsFragment) + "s");

    int pn = this->_index_active_participant;
    ui->lineEditParticipant->setText(QString::number(++pn));
}


void MainWindow::paintClear()
{
    ui->lPanelRough->clear();
    ui->lPanelMissingData->clear();
    ui->lPanelSmooth->clear();
    ui->lPanelFlags->clear();
    ui->lPanelFixList->clear();
    ui->lPanelFixations->clear();
    ui->lPanelVelocity->clear();
    ui->lPanelPupilDilation->clear();
}


/**
 *          PUBLIC SLOTS METHODS
 */
void MainWindow::fncPress_bAcceptEstimation(){

    if (p_autoFixAllM->is_empty()){ //TODO: Check dates too
        QMessageBox msgBox;
        msgBox.setText("You have not done any estimation yet.");
        msgBox.setInformativeText("Please follow the steps above to detect fixations.");
        msgBox.exec();
    }else{
        fixAllM = autoFixAllM;
        GPMatrixFunctions::fncReturnFixationinSegments( p_fixAllM, p_experimentalSegmentsM);
    }

    if (!p_autoFixAllM->is_empty()) GPMatrixFunctions::saveFile((*p_autoFixAllM),p_active_participant->GetMatrixPath(Consts::MATRIX_AUTOFIXALL));
    if (!p_smoothM->is_empty()) GPMatrixFunctions::saveFile((*p_smoothM), p_active_participant->GetMatrixPath(Consts::MATRIX_SMOOTH));
    if (!p_fixAllM->is_empty()) GPMatrixFunctions::saveFile((*p_fixAllM), p_active_participant->GetMatrixPath(Consts::MATRIX_FIXALL));

    ui->stackedWidget->setCurrentWidget(ui->page_manual);

    paintSmoothData();
    paintFixations();

}

void MainWindow::fncPress_bInterpolate()
{
    DialogInterpolation w;
    w.setWindowTitle(tr("Interpolate smoothed data"));
    w.loadData(p_active_participant, (*p_smoothM));
    w.exec();

    //reload incase batch processed
    GPMatrixFunctions::readFile((*p_smoothM), p_active_participant->GetMatrixPath(Consts::MATRIX_SMOOTH));

    paintSmoothData();
    paintVelocity();
}

void MainWindow::fncPress_bSmooth()
{
    DialogSmoothXY win;
    win.loadData((*p_active_participant),(*p_roughM), (*p_smoothM));
    win.setWindowTitle(tr("Smooth data"));
    win.exec();

    //reload incase batch processed
    GPMatrixFunctions::readFile((*p_smoothM), p_active_participant->GetMatrixPath(Consts::MATRIX_SMOOTH));

    paintSmoothData();
    paintVelocity();

}

void MainWindow::fncPress_bEstimateFixations()
{
    DialogEstimateFixations efd;
    efd.loadData((*p_active_participant),(*p_roughM),(*p_smoothM), (*p_autoFixAllM));
    efd.exec();

    //reload incase batch processed
    GPMatrixFunctions::readFile((*p_smoothM), p_active_participant->GetMatrixPath(Consts::MATRIX_SMOOTH));
    GPMatrixFunctions::readFile((*p_fixAllM), p_active_participant->GetMatrixPath(Consts::MATRIX_FIXALL));
    GPMatrixFunctions::readFile((*p_autoFixAllM), p_active_participant->GetMatrixPath(Consts::MATRIX_AUTOFIXALL));

    // Paint fixations
    paintSmoothData();
    paintFixations();
}

void MainWindow::fncPress_bParticipantLeft()
{
    int pn1 = this->_index_active_participant;
    int pn = pn1-1;
    if (pn<0) pn = 0;
    if (pn>=_project.numParticipants()) pn = _project.numParticipants()-1;
    ui->lineEditParticipant->setText(QString::number(pn+1));
    if (pn1!=pn) this->fncSetActiveParticipant(pn);
}

void MainWindow::fncPress_bParticipantRight()
{
    int pn1 = this->_index_active_participant;
    int pn = pn1+1;
    if (pn<0) pn = 0;
    if (pn>=_project.numParticipants()) pn = _project.numParticipants()-1;
    ui->lineEditParticipant->setText(QString::number(pn+1));
    if (pn1!=pn) this->fncSetActiveParticipant(pn);
}

void MainWindow::fncPress_bExecuteManual()
{
    // Manual execution of Create/ delete / Merge
    int from = ui->txFrom->text().toInt(); // Get the converted data from the text box. If it's not numeric it return 0.
    int to =  ui->txTo->text().toInt();
    if (from >0 && to>0){
        this->fncPress_bExecuteManual(from, to); // Pass the event to mainwindow class
    }
}

void MainWindow::fncPress_bNotes()
{
    DialogParticipantEditNotes gpen(this->p_active_participant);
    gpen.exec();
}

void MainWindow::fncPress_bNext()
{
    // Go to the next fragment
    if ( _currentFragment != _nFragments)
    {
        _previousFragment = _currentFragment;
        _currentFragment = _currentFragment + 1;
        paintAll();
    }
}


void MainWindow::fncPress_bPrevious()
{
    // Go to the previous fragment
    if (_currentFragment != 1)
    {
        _currentFragment = _previousFragment;
        _previousFragment = _previousFragment - 1;
        paintAll();
    }
}


void MainWindow::fncPress_bCreate()
{
    // Activate/ deactivate Create mode
    if (this->_currentAction == Consts::AC_CREATE){
        ui->bCreate->setStyleSheet("");
    }else{
        ui->bCreate->setStyleSheet("* { background-color: rgb(255,125,100) }");
    }

    ui->bDelete->setStyleSheet("");
    ui->bMerge->setStyleSheet("");
    ui->bSmoothPursuit->setStyleSheet("");
    if (_currentAction == Consts::AC_CREATE){
        _currentAction = Consts::AC_NOACTION;
    }else{
        _currentAction = Consts::AC_CREATE;
    }
}

void MainWindow::fncPress_bDelete()
{
    // Activate/ deactivate Delete mode
    if (this->_currentAction == Consts::AC_DELETE){
        ui->bDelete->setStyleSheet("");
    }else{
        ui->bDelete->setStyleSheet("* { background-color: rgb(255,125,100) }");
    }
    ui->bCreate->setStyleSheet("");
    ui->bMerge->setStyleSheet("");
    ui->bSmoothPursuit->setStyleSheet("");
    if (_currentAction == Consts::AC_DELETE){
        _currentAction = Consts::AC_NOACTION;
    }else{
        _currentAction = Consts::AC_DELETE;
    }
}

void MainWindow::fncPress_bMerge()
{
    // ACTIVATE/ deactivate merge mode
    if (this->_currentAction == Consts::AC_MERGE){
        ui->bMerge->setStyleSheet("");
    }else{
        ui->bMerge->setStyleSheet("* { background-color: rgb(255,125,100) }");
    }
    ui->bCreate->setStyleSheet("");
    ui->bDelete->setStyleSheet("");
    ui->bSmoothPursuit->setStyleSheet("");
    if (_currentAction == Consts::AC_MERGE){
        _currentAction = Consts::AC_NOACTION;
    }else{
        _currentAction = Consts::AC_MERGE;
    }
}

void MainWindow::fncPress_bSmoothPursuit()
{
    // Activate. deactivate smooth pursuit mode.
    if (this->_currentAction == Consts::AC_SMOOTH_PURSUIT){
        ui->bSmoothPursuit->setStyleSheet("");
    }else{
        ui->bSmoothPursuit->setStyleSheet("* { background-color: rgb(255,125,100) }");
    }
    ui->bCreate->setStyleSheet("");
    ui->bDelete->setStyleSheet("");
    ui->bMerge->setStyleSheet("");
    if (_currentAction == Consts::AC_SMOOTH_PURSUIT){
        _currentAction = Consts::AC_NOACTION;
    }else{
        _currentAction = Consts::AC_SMOOTH_PURSUIT;
    }
}

void MainWindow::fncPress_bExecuteManual(int from, int to)
{
    // Ececute the action manually rather than with the mouse click
    fncManipulateFix(from, to);
}



/**
  *     MENU EVENTS
  */

void MainWindow::fncPress_subMenuDetectFixations()
{
    ui->stackedWidget->setCurrentWidget(ui->page_auto);
}

void MainWindow::fncPress_subMenuManualEditing()
{
    ui->stackedWidget->setCurrentWidget(ui->page_manual);
}

void MainWindow::fncPress_subMenuDialogConfig()
{
    fncWaitForLoad();

    //TODO: change to load participant settings
    DialogConfig w;
    w.loadData(&_project);
    w.setWindowTitle(tr("Configure Options"));
    w.exec();

    // update parameters
    fncLoadSettings(Consts::ACTIVE_CONFIGURATION());
    fncResetDisplayParams();
    fncSettingsChanged();
    fncSetActiveParticipant(0);
}

void MainWindow::fncPress_subMenuShowErrorLog()
{
    DialogGrafixError::ShowDialog();
}

void MainWindow::fncPress_subMenuShowStatistics()
{
    fncWaitForLoad();
    DialogStatistics w;
    w.setWindowTitle(tr("Statistics"));
    w.loadData(p_fixAllM,p_autoFixAllM, p_experimentalSegmentsM);
    w.exec();
}

void MainWindow::fncPress_subMenuHeatMap()
{
    fncWaitForLoad();
    DialogHeatMap w;
    w.setWindowTitle(tr("Heap Maps"));
    w.loadData(p_active_participant, roughM, p_fixAllM, p_experimentalSegmentsM);
    w.exec();
}

void MainWindow::fncPress_subMenuProjectOpen()
{
    // Open a new project
    fncWaitForLoad();

    //TODO change to open config
    DialogOpenProject w(&_project);
    w.setWindowTitle(tr("Select project or create new one"));
    w.exec();

    //saves directory of project returned
    QSettings settings("options.ini", QSettings::IniFormat);
    settings.setValue(Consts::SETTING_PROJECT,_project.GetFullDirectory());

    //_active_participant may no longer be ok (TODO: check if project changed)
    p_active_participant = NULL;
    if (_project.numParticipants()>0)
    {
        qDebug() << "Participants out:" << _project.numParticipants() << "\n";
        fncSetActiveParticipant(0);//TODO: Settings remember last participant
    }
}


void MainWindow::fncPress_subMenuVisualizeCurrentFragment()
{
    fncWaitForLoad();
    DialogVisualization w;
    w.setWindowTitle(tr("Visualize Fragment"));
    w.loadData(_currentFragment, p_active_participant, roughM, smoothM, fixAllM);
    w.exec();
}

void MainWindow::fncPress_subMenuVisualizeSegments()
{
    fncWaitForLoad();
    DialogVisualizationSegments w;
    w.setWindowTitle(tr("Visualize Segment"));
    w.loadData(p_active_participant, roughM, smoothM, fixAllM, experimentalSegmentsM);
    w.exec();
}

void MainWindow::fncPress_subMenuExport()
{
    // Export the fixations in the middle of the segments
    fncWaitForLoad();
    if (!fixAllM.is_empty()){
        DialogExport w;
        w.setWindowTitle(tr("Export data"));
        w.loadData(p_active_participant, roughM, smoothM, fixAllM, experimentalSegmentsM);
        w.exec();
    }
}


void MainWindow::fncPress_subMenuRecalculateFixations()
{
    // It recalcules all the values for the fixations
    fncWaitForLoad();
    fixAllM = GPFixationOperations::fncRecalculateFixations(roughM, fixAllM,
                                                            this->_project.GetProjectSetting(Consts::SETTING_EXP_WIDTH, Consts::ACTIVE_CONFIGURATION()).toInt(),
                                                            this->_project.GetProjectSetting(Consts::SETTING_EXP_HEIGHT, Consts::ACTIVE_CONFIGURATION()).toInt(),
                                                            this->_project.GetProjectSetting(Consts::SETTING_DEGREE_PER_PIX, Consts::ACTIVE_CONFIGURATION()).toInt());

    if (!fixAllM.is_empty())
        GPMatrixFunctions::saveFile(fixAllM, p_active_participant->GetMatrixPath(Consts::MATRIX_FIXALL));
}


void MainWindow::fncPress_subMenuCalculateSmooth()
{
    fncWaitForLoad();
    DialogSmoothXY w;
    w.setWindowTitle(tr("Smooth data"));
    w.loadData( (*p_active_participant), roughM, smoothM);
    w.exec();

    //load smooth data in case it was changed during batch processing
    GPMatrixFunctions::readFile(smoothM, p_active_participant->GetMatrixPath(Consts::MATRIX_SMOOTH));

    paintSmoothData();
    paintVelocity();
}

void MainWindow::fncPress_subMenuAdjustParameters()
{
    fncWaitForLoad();
    DialogTestSmoothParameters d;
    d.loadData(roughM,_displayStartIndex,_displayStopIndex,(*p_active_participant),_configuration);
    d.exec();
    fncLoadSettings(Consts::ACTIVE_CONFIGURATION());  //reload parameters after adjustment
}

void MainWindow::fncPress_subMenuChangeConfiguration()
{
    fncWaitForLoad();
    DialogSaveNewConfiguration d;
    d.loadData(&this->_configuration, &this->_project);
    d.exec();
    if (!(this->_configuration == Consts::ACTIVE_CONFIGURATION()))
    {
        _project.ActivateConfiguration(_configuration);
        fncLoadSettings(_configuration);
        this->_configuration_changed = false;
        ui->lConfigurationStatus->setText("Active Configuration: " + _configuration.first);
    }

}

void MainWindow::fncPress_subMenuClose()
{
    this->close();
}

void MainWindow::fncChange_tParticipantNumber()
{
    //check if number and in participant
    int number = ui->lineEditParticipant->text().toInt();
    if (number<1) number = 1;
    if (number>_project.numParticipants()) number = _project.numParticipants();
    ui->lineEditParticipant->setText(QString::number(number));

    if (number-1 != this->_index_active_participant)
        this->fncSetActiveParticipant(number-1);
}

void MainWindow::fncSettingsChanged()
{
    this->_configuration_changed = true;
    this->_configuration = Consts::ACTIVE_CONFIGURATION();
    ui->lConfigurationStatus->setText("Active Configuration: Unsaved.");
}

void MainWindow::fncChange_tInterpolation()
{
    _project.SetProjectSetting(Consts::SETTING_INTERP_LATENCY, Consts::ACTIVE_CONFIGURATION(),
                                 ui->leInterpolation->text().toInt());
    ui->sliderInterpolation->setValue((int)ui->leInterpolation->text().toInt());
    fncSettingsChanged();
}
void MainWindow::fncChange_tDisplacement()
{
    _project.SetProjectSetting(Consts::SETTING_POSTHOC_MERGE_CONSECUTIVE_VAL, Consts::ACTIVE_CONFIGURATION(),
                                 ui->leInterpolation->text().toDouble());
    ui->sliderDisplacement->setValue((int)(ui->leInterpolation->text().toDouble()*100));
    fncSettingsChanged();
}
void MainWindow::fncChange_tDisplacInterpolate()
{
    _project.SetProjectSetting(Consts::SETTING_INTERP_MAXIMUM_DISPLACEMENT, Consts::ACTIVE_CONFIGURATION(),
                                 ui->leDisplacInterpolate->text().toDouble());
    ui->sliderDisplacInterpolate->setValue((int)(ui->leDisplacInterpolate->text().toDouble()*100));
    fncSettingsChanged();
}
void MainWindow::fncChange_tMinFixation()
{
    _project.SetProjectSetting(Consts::SETTING_POSTHOC_MIN_DURATION_VAL, Consts::ACTIVE_CONFIGURATION(),
                                 ui->leMinFixation->text().toInt());
    ui->sliderMinFixation->setValue((int)ui->leMinFixation->text().toInt());
    fncSettingsChanged();
}
void MainWindow::fncChange_tVelocity()
{
    _project.SetProjectSetting(Consts::SETTING_INTERP_VELOCITY_THRESHOLD, Consts::ACTIVE_CONFIGURATION(),
                                 ui->leVelocity->text().toInt());
    ui->sliderVelocity->setValue((int)ui->leVelocity->text().toInt());
    fncSettingsChanged();
}
void MainWindow::fncChange_tVelocityVariance()
{
    _project.SetProjectSetting(Consts::SETTING_POSTHOC_LIMIT_RMS_VAL, Consts::ACTIVE_CONFIGURATION(),
                                 (ui->leVelocityVariance->text().toDouble()));
    ui->sliderVelocityVariance->setValue((int)(ui->leVelocityVariance->text().toDouble()*100));
    fncSettingsChanged();
}
void MainWindow::fncChange_tSigmaS()
{
    _project.SetProjectSetting(Consts::SETTING_SMOOTHING_SIGMA_S, Consts::ACTIVE_CONFIGURATION(),
                               (double) ui->leSigmaS->text().toDouble());
    ui->sliderSigmaS->setValue((int)ui->leSigmaS->text().toInt());
    fncSettingsChanged();
}
void MainWindow::fncChange_tSigmaR()
{
    _project.SetProjectSetting(Consts::SETTING_SMOOTHING_SIGMA_R, Consts::ACTIVE_CONFIGURATION(),
                               (double) ui->leSigmaR->text().toDouble());
    ui->sliderSigmaR->setValue((int)ui->leSigmaR->text().toInt());
    fncSettingsChanged();
}

void MainWindow::fncChange_sSigmaS()
{
    _project.SetProjectSetting(Consts::SETTING_SMOOTHING_SIGMA_S, Consts::ACTIVE_CONFIGURATION(),
                               (double) ui->sliderSigmaS->value());
    ui->leSigmaS->setText(QString::number(ui->sliderSigmaS->value()));
    fncSettingsChanged();
}
void MainWindow::fncChange_sSigmaR()
{
    _project.SetProjectSetting(Consts::SETTING_SMOOTHING_SIGMA_R, Consts::ACTIVE_CONFIGURATION(),
                               (double) ui->sliderSigmaR->value());
    ui->leSigmaR->setText(QString::number(ui->sliderSigmaR->value()));
    fncSettingsChanged();
}
void MainWindow::fncChange_sDisplacement(){
    ui->leDisplacement->clear();
    std::ostringstream o;
    o << ((double)ui->sliderDisplacement->value()/100);
    ui->leDisplacement->setText( o.str().c_str() );

    _project.SetProjectSetting(Consts::SETTING_POSTHOC_MERGE_CONSECUTIVE_VAL, Consts::ACTIVE_CONFIGURATION(),
                                 ((double)ui->sliderDisplacement->value()/100));
    fncSettingsChanged();
}

void MainWindow::fncChange_sDisplacInterpolate(){
    ui->leDisplacInterpolate->clear();
    std::ostringstream o;
    o << ((double)ui->sliderDisplacInterpolate->value()/100);
    ui->leDisplacInterpolate->setText( o.str().c_str() );

    _project.SetProjectSetting(Consts::SETTING_INTERP_MAXIMUM_DISPLACEMENT, Consts::ACTIVE_CONFIGURATION(),
                                 ((double)ui->sliderDisplacInterpolate->value()/100));
    fncSettingsChanged();
}

void MainWindow::fncChange_sInterpolation(){
    ui->leInterpolation->clear();
    std::ostringstream o;
    o << ui->sliderInterpolation->value();
    ui->leInterpolation->setText( o.str().c_str() );

    _project.SetProjectSetting(Consts::SETTING_INTERP_LATENCY, Consts::ACTIVE_CONFIGURATION(),
                                 ui->sliderInterpolation->value());
    fncSettingsChanged();
}

void MainWindow::fncChange_sMinFixation(){
    ui->leMinFixation->clear();
    std::ostringstream o;
    o << ui->sliderMinFixation->value();
    ui->leMinFixation->setText( o.str().c_str() );
    _project.SetProjectSetting(Consts::SETTING_POSTHOC_MIN_DURATION_VAL, Consts::ACTIVE_CONFIGURATION(),
                                 ui->sliderMinFixation->value());
    fncSettingsChanged();
}


void MainWindow::fncChange_sVelocity(){
    ui->leVelocity->clear();
    std::ostringstream o;
    o << ui->sliderVelocity->value();
    ui->leVelocity->setText( o.str().c_str() );

    /*/ Draw the line
    QPixmap pixmap(ui->lPanelVelocityAux->width(),ui->lPanelVelocityAux->height());
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    QPen myPen(Qt::green, 1, Qt::SolidLine);

    double y = (double)ui->sliderVelocity->value()   * maximizationIndex;

    painter.drawLine(0,y,ui->lPanelVelocity->width(),y);

    ui->lPanelVelocityAux->setPixmap(pixmap);

    painter.end();*/

    _project.SetProjectSetting(Consts::SETTING_INTERP_VELOCITY_THRESHOLD, Consts::ACTIVE_CONFIGURATION(),
                                 ui->sliderVelocity->value());

    fncSettingsChanged();
    paintVelocity();

}



void MainWindow::fncChange_sVelocityVariance(){
    ui->leVelocityVariance->clear();
    std::ostringstream o;
    o << ((double)ui->sliderVelocityVariance->value()/100);
    ui->leVelocityVariance->setText( o.str().c_str() );

    _project.SetProjectSetting(Consts::SETTING_POSTHOC_LIMIT_RMS_VAL, Consts::ACTIVE_CONFIGURATION(),
                                 ((double)ui->sliderVelocityVariance->value()/100));
    fncSettingsChanged();
}

void MainWindow::fncPress_cbDisplacement(){
    _project.SetProjectSetting(Consts::SETTING_POSTHOC_MERGE_CONSECUTIVE_FLAG, Consts::ACTIVE_CONFIGURATION(),
                                 ui->cb_displacement->checkState());
    fncSettingsChanged();
}

void MainWindow::fncPress_cbMinFixation(){
    _project.SetProjectSetting(Consts::SETTING_POSTHOC_MIN_DURATION_FLAG, Consts::ACTIVE_CONFIGURATION(),
                                 ui->cb_minFixation->checkState());
    fncSettingsChanged();
}

void MainWindow::fncPress_cbVelocityVariance(){
    _project.SetProjectSetting(Consts::SETTING_POSTHOC_LIMIT_RMS_FLAG, Consts::ACTIVE_CONFIGURATION(),
                                 ui->cb_velocityVariance->checkState());
    fncSettingsChanged();
}


