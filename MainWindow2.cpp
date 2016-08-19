
#include "MainWindow2.h"
#include "ui_mainwindow2.h"

MainWindow2::MainWindow2(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow2)
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
    this->_copyEyes = Consts::DEFAULT_SETTING_SMOOTHING_USE_OTHER_EYE;
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
    this->_last_selected_configuration = Consts::ACTIVE_CONFIGURATION();
    this->_hold_changes = false;
    // Menu events
    connect( ui->actionConfiguration, SIGNAL( triggered() ), this, SLOT( fncPress_subMenuDialogConfig() ) );
    connect( ui->actionNew_Open, SIGNAL( triggered() ), this, SLOT( fncPress_subMenuProjectOpen() ) );
    connect( ui->actionCurrent_Fragment, SIGNAL( triggered() ), this, SLOT( fncPress_subMenuVisualizeCurrentFragment() ) );
    connect( ui->actionSegment, SIGNAL( triggered() ), this, SLOT( fncPress_subMenuVisualizeSegments() ) );
    connect( ui->actionExport_Data, SIGNAL( triggered() ), this, SLOT( fncPress_subMenuExport()));
    connect( ui->actionSmoothing, SIGNAL( triggered() ), this, SLOT( fncPress_subMenuCalculateSmooth()));
    connect( ui->actionRecalculate_Fixations, SIGNAL( triggered() ), this, SLOT( fncPress_subMenuRecalculateFixations())); // It recalculates the duration, variance,
    connect( ui->actionShow_Statistics, SIGNAL( triggered() ), this, SLOT( fncPress_subMenuShowStatistics()));
    connect( ui->actionHeat_Map, SIGNAL( triggered() ), this, SLOT( fncPress_subMenuHeatMap()));
    connect( ui->actionShow_Error_Log, SIGNAL(triggered()), this, SLOT(fncPress_subMenuShowErrorLog()));
    connect( ui->actionAuto_Fixation_Detection, SIGNAL(triggered()), this,SLOT(fncPress_subMenuDetectFixations()));
    connect( ui->actionManual_Fixation_Detection, SIGNAL(triggered()), this, SLOT(fncPress_subMenuManualEditing()));
    connect( ui->actionAdjust_Parameters, SIGNAL( triggered()), this, SLOT(fncPress_subMenuAdjustParameters()));
    connect( ui->actionChange_Configuration, SIGNAL(triggered()), this, SLOT(fncPress_subMenuChangeConfiguration()));
    connect( ui->actionClose, SIGNAL( triggered()), this, SLOT( fncPress_subMenuClose()));
    connect( ui->actionReload_Last_Configuration, SIGNAL( triggered()), this, SLOT(fncPress_subMenuReloadConfiguration()));
    connect( ui->actionMovie, SIGNAL( triggered() ), this, SLOT( fncPress_subMenuMovie()));

    // Main Buttons
    connect( ui->bNext, SIGNAL( clicked() ), this, SLOT( fncPress_bNext() ) );
    connect( ui->bPrevious, SIGNAL( clicked() ), this, SLOT( fncPress_bPrevious() ) );
    connect( ui->bCreate, SIGNAL( clicked()), this, SLOT(fncPress_bCreate()));
    connect( ui->bDelete, SIGNAL( clicked()), this, SLOT( fncPress_bDelete() ) );
    connect( ui->bMerge, SIGNAL( clicked()), this, SLOT( fncPress_bMerge() ) );
    connect( ui->bSmoothPursuit, SIGNAL( clicked()), this, SLOT( fncPress_bSmoothPursuit() ) );
    connect( ui->bResetToAuto, SIGNAL( clicked()), this, SLOT( fncPress_bResetFixations()));
    connect( ui->bResizeFixation, SIGNAL( clicked()), this, SLOT( fncPress_bResizeFixation()));
    connect( ui->bExecManual, SIGNAL( clicked()), this, SLOT( fncPress_bExecuteManual() ) );
    connect( ui->lineEditParticipant, SIGNAL(returnPressed()), this, SLOT ( fncChange_tParticipantNumber() ));
    connect( ui->lineEditSegment, SIGNAL( returnPressed()), this, SLOT( fncChange_tSegmentNumber()));
    connect( ui->pushButtonParticipantLeft, SIGNAL( clicked() ), this, SLOT( fncPress_bParticipantLeft() ) );
    connect( ui->pushButtonParticipantRight, SIGNAL( clicked() ), this, SLOT( fncPress_bParticipantRight() ) );
    connect( ui->bNotes,SIGNAL(clicked()), this, SLOT (fncPress_bNotes()));

    // Automatic fixation buttons:
    //connect( ui->b_acceptEstimation, SIGNAL( clicked() ), this, SLOT( fncPress_bAcceptEstimation() ) );
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
    ui->sliderDisplacInterpolate->setMaximum(Consts::MAXSLIDER_SETTING_INTERP_MAXIMUM_DISPLACEMENT);
    ui->sliderDisplacement->setMinimum(0);
    ui->sliderDisplacement->setMaximum(Consts::MAXSLIDER_SETTING_POSTHOC_MERGE_CONSECUTIVE_VAL);
    ui->sliderVelocityVariance->setMinimum(0);
    ui->sliderVelocityVariance->setMaximum(Consts::MAXSLIDER_SETTING_POSTHOC_LIMIT_RMS_VAL);
    ui->sliderVelocity->setMinimum(0);
    ui->sliderVelocity->setMaximum(Consts::MAXSLIDER_SETTING_INTERP_VELOCITY_THRESHOLD);
    ui->sliderInterpolation->setMaximum(Consts::MAXSLIDER_SETTING_INTERP_LATENCY);
    ui->sliderInterpolation->setMinimum(0);
    ui->sliderMinFixation->setMinimum(0);
    ui->sliderMinFixation->setMaximum(Consts::MAXSLIDER_SETTING_POSTHOC_MIN_DURATION_VAL);
    ui->sliderSigmaR->setMinimum(Consts::MINSLIDER_SETTING_SMOOTHING_SIGMA_R);
    ui->sliderSigmaR->setMaximum(Consts::MAXSLIDER_SETTING_SMOOTHING_SIGMA_R);
    ui->sliderSigmaS->setMinimum(Consts::MINSLIDER_SETTING_SMOOTHING_SIGMA_S);
    ui->sliderSigmaS->setMaximum(Consts::MAXSLIDER_SETTING_SMOOTHING_SIGMA_S);

    //start new label
    this->p_over_display_label = new QLabel(this);
    this->p_over_display_label->setAttribute(Qt::WA_TransparentForMouseEvents);
    this->p_over_display_label->raise();

    //zoom widgets
    this->p_bZoomout = new QPushButton(this);
    this->p_bZoomin = new QPushButton(this);
    this->p_lMagnification = new QLabel(this);
    this->p_lMagnification->setAlignment(Qt::AlignRight);
    connect(this->p_bZoomin, SIGNAL(clicked()), this, SLOT(fncPress_bZoomin()));
    connect(this->p_bZoomout, SIGNAL(clicked()), this, SLOT(fncPress_bZoomout()));

    this->_velocity_view_zoom = 1;
    this->p_bZoomout->setText("-");
    this->p_bZoomin->setText("+");
    this->p_lMagnification->setText("1x");


    //ui->lPanelMissingData->setAttribute(Qt::WA_TransparentForMouseEvents);
    ui->frameDataDisplay->setAttribute(Qt::WA_TransparentForMouseEvents);
    this->setAttribute(Qt::WA_TransparentForMouseEvents);
    // Allow the main widget to track the mouse
    //ui->centralWidget->setAttribute(Qt::WA_TransparentForMouseEvents);
    this->setMouseTracking(true);


    ui->stackedWidget->setCurrentWidget(ui->page_manual);

    //setup fixations list
    _fixationList = new FixationsListModel();
    ui->fixationTableView->setModel(_fixationList);



    qApp->installEventFilter(this);
    fncSetToolTips();

    //set up fixation table view
    ui->fixationTableView->setSelectionMode(QAbstractItemView::NoSelection);
    ui->fixationTableView->setColumnWidth(0,40);
    ui->fixationTableView->setColumnWidth(1,40);
    ui->fixationTableView->setColumnWidth(2,60);
    ui->fixationTableView->setColumnWidth(3,60);
}

bool MainWindow2::LoadProject()
{
    //loads settings (info of previous project
    QSettings settings("options.ini", QSettings::IniFormat);

    // If there is not a project opened (in the settings file), we open the openProject dialog.
    while (settings.value(Consts::SETTING_PROJECT).isNull() ||
            !(_project.LoadProjectSettings(settings.value(Consts::SETTING_PROJECT).toString())))
    {
        QMessageBox msgBox;
        msgBox.setText("No project loaded.");
        msgBox.setInformativeText("It was not possible to load a valid project - please load or create a new one.");
        msgBox.setStandardButtons(QMessageBox::Ok| QMessageBox::Close);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.setButtonText(QMessageBox::Close,"Quit GraFIX");
        int ret = msgBox.exec();
        if (ret == QMessageBox::Close)
        {
            return false;
        }

        DialogOpenProject w(&_project);
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
    return true;
}

bool MainWindow2::eventFilter(QObject *obj, QEvent *event)
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
        } else {
            this->p_over_display_label->clear();
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
            paintFixations();
            if (_currentAction == Consts::AC_DRAG_ENDS)
            {
                if (_selectedFixationEnd && file_posX > _fixStartPos)
                {
                   paintCurrentFixation(_fixStartPos, file_posX);
                   ui->lFrom->setText(QString::number(_fixStartPos));
                   ui->lTo->setText(QString::number(file_posX));
                }
                else if (_selectedFixationEnd == false && file_posX < _fixStartPos)
                {
                    paintCurrentFixation(file_posX, _fixStartPos);
                    ui->lTo->setText(QString::number(_fixStartPos));
                    ui->lFrom->setText(QString::number(file_posX));
                }
            }
            else
            {
                // If we are manipulating a fixation

                if (_fixStartPos >= file_posX)
                {
                    // If "to" is smaller than "from", do nothing
                    file_posX = 0;
                }
                else
                {
                    // "from" is bigger than "to". Paint rectangle
                   paintCurrentFixation(_fixStartPos, file_posX);
                }

                ui->lTo->setText(QString::number(file_posX));
            }
        }
    }

    if (event->type() == QEvent::MouseButtonRelease)
    {
        if (_fixInAction == Consts::FIX_OFF) return false;
        _fixInAction = Consts::FIX_OFF;
        int fixStopPos = _displayStartIndex + (posX * _displayIncrement);
        if (this->_currentAction == Consts::AC_DRAG_ENDS)
        {
            if(this->_selectedFixationEnd  && fixStopPos > _fixStartPos)
            {
                if (fixAllM.n_rows > (_selectedFixationRow + 1) &&
                    fixAllM.at(_selectedFixationRow + 1, 0) < fixStopPos)
                {
                    ui->statusbar->showMessage("Error: Fixations cannot overlap");
                }
                else
                {
                    fixAllM.at(_selectedFixationRow,1) = fixStopPos;
                    GPFixationOperations::fncRecalculateFixationValues(roughM,&fixAllM, _selectedFixationRow,_expWidth,_expHeight,_degPerPixel, _copyEyes);
                }
            }
            else if (_selectedFixationEnd == false && fixStopPos < _fixStartPos)
            {
                if (_selectedFixationRow > 0 &&
                    fixAllM.at(_selectedFixationRow - 1, 1) > fixStopPos)
                {
                    ui->statusbar->showMessage("Error: Fixations cannot overlap");
                }
                else
                {
                    fixAllM.at(_selectedFixationRow,0) = fixStopPos;
                    GPFixationOperations::fncRecalculateFixationValues(roughM,&fixAllM, _selectedFixationRow,_expWidth,_expHeight,_degPerPixel, _copyEyes);
                }
            }

            // Save file
            if (!fixAllM.is_empty() && p_active_participant != NULL)
                GPMatrixFunctions::saveFile(fixAllM, p_active_participant->GetMatrixPath(Consts::MATRIX_FIXALL));

            // Paint fixations again
            paintFixations();
        }
        else
        {
             // function for create/ merge/ delete depending in the action.
             fncManipulateFix(_fixStartPos, fixStopPos);

        }
        ui->lTo->setText("0");
        _fixStartPos = -1;
    }

    if (event->type() == QEvent::MouseButtonPress)
    {

        if (this->_currentAction == Consts::AC_DRAG_ENDS)
        {
            //find closest beginning or end of fixation
            int current_pos = _displayStartIndex + (posX * _displayIncrement);
            uword closest_fixation = 0;
            bool select_end = false;
            int closest_value = fixAllM.at(0,0);

            for (uword fix_row = 0; fix_row < fixAllM.n_rows; ++fix_row)
            {
                int from = fixAllM.at(fix_row,0);
                int to = fixAllM.at(fix_row,1);
                if (std::abs(from - current_pos) < std::abs(closest_value - current_pos))
                {
                    closest_value = from;
                    closest_fixation = fix_row;
                    select_end = false;
                }
                if (std::abs(to - current_pos) < std::abs(closest_value - current_pos))
                {
                    closest_value = to;
                    closest_fixation = fix_row;
                    select_end = true;
                }
            }

            if (closest_value - current_pos > 5) return false;

            _fixInAction = Consts::FIX_ON;
            _selectedFixationRow = closest_fixation;
            _selectedFixationEnd = select_end;

            _fixStartPos = _selectedFixationEnd ? this->fixAllM.at(_selectedFixationRow, 0) :
                                                  this->fixAllM.at(_selectedFixationRow, 1);

            paintCurrentFixation(this->fixAllM.at(_selectedFixationRow, 0), this->fixAllM.at(_selectedFixationRow, 1));
            ui->lFrom->setText(QString::number(this->fixAllM.at(_selectedFixationRow, 0)));
            ui->lTo->setText(QString::number(this->fixAllM.at(_selectedFixationRow, 1)));
        }
        else
        {
           // store start value
            _fixInAction = Consts::FIX_ON;
           _fixStartPos = _displayStartIndex + (posX * _displayIncrement);
           //statusBar()->showMessage(QString("Mouse clicked on ") + obj->objectName());
        }
    }
    return false;
}

void MainWindow2::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event);
   //QMainWindow2::resizeEvent(event);
   int w = ui->lPanelMissingData->width();
   int h = ui->lPanelVelocity->y() + ui->lPanelVelocity->height() - ui->lPanelMissingData->y();

   QPoint new_pos = ui->lPanelMissingData->pos() + ui->frameDataDisplay->pos() + ui->centralwidget->pos();
   //int y = new_pos.y() - (this->frameGeometry().height() - this->geometry().height());
   this->p_over_display_label->setGeometry(new_pos.x(),
                                           new_pos.y(),
                                           w,
                                           h);

   this->p_over_display_label->raise();

   //Now for zoom buttons
   int button_size = 20;
   int spacing = 3;
   new_pos = ui->lPanelVelocity->pos() + ui->frameDataDisplay->pos() + ui->centralwidget->pos();
   this->p_bZoomout->setGeometry(new_pos.x()+ ui->lPanelVelocity->width() - button_size, new_pos.y(), button_size, button_size);
   this->p_bZoomout->raise();
   this->p_bZoomin->setGeometry(new_pos.x()+ ui->lPanelVelocity->width() - spacing - (2 *button_size), new_pos.y(), button_size, button_size);
   this->p_bZoomin->raise();
   this->p_lMagnification->setGeometry(new_pos.x()+ ui->lPanelVelocity->width() - (2*spacing) - (7 *button_size), new_pos.y(), button_size*5, button_size);
   this->p_lMagnification->raise();
   statusBar()->showMessage("Resized");
   paintAll();
}


MainWindow2::~MainWindow2()
{
    _project.SaveSettings();
    delete this->p_over_display_label;
    delete ui;
}


void MainWindow2::fncWaitForLoad()
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

void MainWindow2::fncSetActiveParticipant(int position)
{
    //check position is available
    if (!_project.HasParticipant(position)) return;

    this->_files_next_position = position;
    ui->statusbar->showMessage("Status: Closing participant...");
    _files_stop_flag = true; //sets flag telling current file loading process to end
    while(!_files_future.isFinished() && position == _files_next_position)
    {
        qApp->processEvents();
    }

    //check if target position is still this one before setting future
    if (position == _files_next_position)
    {
        ui->statusbar->showMessage("Status: Loading...");
        _files_future = QtConcurrent::run(this, &MainWindow2::fncSetActiveParticipantThread, position);

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

void MainWindow2::fncSetActiveParticipantThread(int position)
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

void MainWindow2::fncLoadSettings(GrafixConfiguration configuration)
{
    // Read the Configuration info from settings.

    _expHeight = _project.GetProjectSetting(Consts::SETTING_EXP_HEIGHT, configuration).toInt();
    _expWidth = _project.GetProjectSetting(Consts::SETTING_EXP_WIDTH, configuration).toInt();
    _hz = _project.GetProjectSetting(Consts::SETTING_HZ, configuration).toInt();
    _secsFragment = _project.GetProjectSetting(Consts::SETTING_SECS_FRAGMENT, configuration).toInt();
    _degPerPixel = _project.GetProjectSetting(Consts::SETTING_DEGREE_PER_PIX, configuration).toDouble();
    _copyEyes = _project.GetProjectSetting(Consts::SETTING_SMOOTHING_USE_OTHER_EYE, configuration).toBool();
    ui->sliderSigmaS->setValue(
                Consts::GetSliderValue(Consts::SETTING_SMOOTHING_SIGMA_S,
                _project.GetProjectSetting(
                        Consts::SETTING_SMOOTHING_SIGMA_S, configuration).toDouble()));

    ui->sliderSigmaR->setValue(
                Consts::GetSliderValue(Consts::SETTING_SMOOTHING_SIGMA_R,
                _project.GetProjectSetting(
                        Consts::SETTING_SMOOTHING_SIGMA_R, configuration).toDouble()));


    ui->sliderInterpolation->setValue(
                Consts::GetSliderValue(Consts::SETTING_INTERP_LATENCY,
                _project.GetProjectSetting(
                        Consts::SETTING_INTERP_LATENCY, configuration).toDouble()));

    ui->sliderMinFixation->setValue(
                Consts::GetSliderValue(Consts::SETTING_POSTHOC_MIN_DURATION_VAL,
                _project.GetProjectSetting(
                        Consts::SETTING_POSTHOC_MIN_DURATION_VAL, configuration).toDouble()));

    ui->sliderVelocity->setValue(
                Consts::GetSliderValue(Consts::SETTING_VELOCITY_THRESHOLD,
                _project.GetProjectSetting(
                        Consts::SETTING_VELOCITY_THRESHOLD, configuration).toDouble()));

    ui->sliderVelocityVariance->setValue(
                Consts::GetSliderValue(Consts::SETTING_POSTHOC_LIMIT_RMS_VAL,
                _project.GetProjectSetting(
                        Consts::SETTING_POSTHOC_LIMIT_RMS_VAL, configuration).toDouble()));

    ui->sliderDisplacement->setValue(
                Consts::GetSliderValue(Consts::SETTING_POSTHOC_MERGE_CONSECUTIVE_VAL,
                _project.GetProjectSetting(
                          Consts::SETTING_POSTHOC_MERGE_CONSECUTIVE_VAL, configuration).toDouble()));

    ui->sliderDisplacInterpolate->setValue(
                Consts::GetSliderValue(Consts::SETTING_INTERP_MAXIMUM_DISPLACEMENT,
                _project.GetProjectSetting(
                          Consts::SETTING_INTERP_MAXIMUM_DISPLACEMENT, configuration).toDouble()));

    ui->cb_displacement->setChecked(_project.GetProjectSetting(
                                        Consts::SETTING_POSTHOC_MERGE_CONSECUTIVE_FLAG, configuration).toBool());
    ui->cb_minFixation->setChecked(_project.GetProjectSetting(
                                        Consts::SETTING_POSTHOC_MIN_DURATION_FLAG, configuration).toBool());
    ui->cb_velocityVariance->setChecked(_project.GetProjectSetting(
                                        Consts::SETTING_POSTHOC_LIMIT_RMS_FLAG, configuration).toBool());
}

void MainWindow2::fncResetDisplayParams()
{
    _currentFragment = 1;
    _previousFragment = 0;

    fncSetDisplayParams();
}

void MainWindow2::fncSetDisplayParams()
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
    _displayIncrement = (double) _secsFragment * _hz / ui->lPanelRough->width();
}



// function for create/ merge/ delete depending in the selected action.
void MainWindow2::fncManipulateFix(int from, int to)
{
    if (from < to){ // If the end of the fixation is bigger than the start.
        switch (_currentAction)
        {
            case Consts::AC_CREATE:
               fixAllM = GPFixationOperations::fncCreateFixation(fixAllM, roughM,   _hz,  _secsFragment,  _currentFragment, from, to, _expWidth, _expHeight, _degPerPixel, _copyEyes);
               break;
            case Consts::AC_DELETE:
                fixAllM = GPFixationOperations::fncDeleteFixations(fixAllM, _hz,  _secsFragment,  _currentFragment, from,  to);
                break;
            case Consts::AC_MERGE:
                fixAllM = GPFixationOperations::fncMergeFixations(fixAllM, roughM, _hz,  _secsFragment,  _currentFragment, from,  to, _expWidth, _expHeight, _degPerPixel, _copyEyes);
                break;
            case Consts::AC_SMOOTH_PURSUIT:
                fixAllM = GPFixationOperations::fncSmoothPursuitFixation(fixAllM, _hz,  _secsFragment,  _currentFragment, from,  to);
                break;
            case Consts::AC_RESET_TO_AUTO:
                GPFixationOperations::fncResetFixation(p_fixAllM,autoFixAllM,roughM,from,to,_expWidth,_expHeight,_degPerPixel, _copyEyes);
                break;
            //case Consts::AC_DRAG_ENDS:
            //    GPFixationOperations::fncResizeFixation(this->p_fixAllM, from, to);
            //    break;
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

bool MainWindow2::fncReadAllFiles(GrafixParticipant* participant)
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


void MainWindow2::fncSaveAllFiles(GrafixParticipant *participant)
{
    if (!roughM.is_empty()) GPMatrixFunctions::saveFile(roughM,participant->GetMatrixPath(Consts::MATRIX_ROUGH));
    if (!smoothM.is_empty()) GPMatrixFunctions::saveFile(smoothM, participant->GetMatrixPath(Consts::MATRIX_SMOOTH));
    if (!fixAllM.is_empty()) GPMatrixFunctions::saveFile(fixAllM, participant->GetMatrixPath(Consts::MATRIX_FIXALL));
    if (!autoFixAllM.is_empty()) GPMatrixFunctions::saveFile(autoFixAllM, participant->GetMatrixPath(Consts::MATRIX_AUTOFIXALL));
    if (!experimentalSegmentsM.is_empty()) GPMatrixFunctions::saveFile(experimentalSegmentsM, participant->GetMatrixPath(Consts::MATRIX_SEGMENTS));
}


void MainWindow2::paintAll()
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

    ui->statusbar->showMessage("Status: Ready.");
}


void MainWindow2::paintExperimentalSegments()
{
    // The method paints grey boxes in the areas that don't need to be coded
    //TODO: Expand to all other boxes too

    QPixmap pixmap2(this->p_over_display_label->width(), this->p_over_display_label->height());
    pixmap2.fill(Qt::transparent);
    //pixmap2.fill(Qt::white);
    if (!experimentalSegmentsM.is_empty())
    {
        QPainter painter(&pixmap2);


        int pen_correction = painter.pen().width();

        painter.setBrush(QBrush(Qt::transparent));
        painter.drawRect(QRect( 0 ,0,pixmap2.width()-pen_correction, pixmap2.height()-pen_correction));
        painter.setBrush(QBrush(QColor(192, 192, 192, 127)));

        // Order the segments depending on time
        uvec indices = sort_index(experimentalSegmentsM.cols(1,1));
        mat expSegAux = experimentalSegmentsM.rows(indices);

        // find the segments that may be involved in the current fragment
        uvec fixIndex =  arma::find(expSegAux.col(2) >= _displayStartIndex);
        mat startSAux = expSegAux.rows(fixIndex);
        fixIndex =  arma::find(startSAux.col(1) <= _displayStopIndex);

        if (fixIndex.is_empty()){ // If it's empty that means that there is not segment in this fragment, hence, all is grey
            painter.drawRect(QRect( 0 ,0,pixmap2.width()-pen_correction, pixmap2.height()-pen_correction));
        }else{

            startSAux =  startSAux.rows(fixIndex); // in this matrix we have all the segments that start in the current fragment
            fixIndex = arma::find(expSegAux.col(0) == startSAux(0,0)); // We find the first segment that starts in the current fragment

            bool isNextSegment = false;
            uword segmentNumber = fixIndex(0,0);
            while (!isNextSegment && segmentNumber < expSegAux.n_rows){
                if (fixIndex(0,0) <= 0 || expSegAux(fixIndex(0,0)-1,2) <= _displayStartIndex){ // this is the very first segment or it is the first segment of the fragment
                    painter.drawRect(QRect( 0 ,0,(expSegAux(0,1)- _displayStartIndex) * (1/_displayIncrement ) - pen_correction, pixmap2.height()-pen_correction));

                }
                if (segmentNumber > 0){
                    if (expSegAux(segmentNumber,1) >=_displayStopIndex){
                        painter.drawRect(QRect( (expSegAux(segmentNumber-1,2)- _displayStartIndex) * (1/_displayIncrement ) ,0,pixmap2.width() - pen_correction, pixmap2.height() - pen_correction));
                    }else{
                        int x1 = (expSegAux(segmentNumber-1,2)- _displayStartIndex) * (1/_displayIncrement );
                        int x2 = ((expSegAux(segmentNumber,1)- _displayStartIndex) * (1/_displayIncrement )) - x1;

                        painter.drawRect(QRect( x1 ,0,x2 - pen_correction, pixmap2.height() - pen_correction));
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

void MainWindow2::paintRoughData()
{
    if (roughM.is_empty()) return;
    QPixmap pixmapMissingData(ui->lPanelMissingData->width()-2,ui->lPanelMissingData->height()-2);
    pixmapMissingData.fill(Qt::white);
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



    painter.setPen(QColor(0,0,0,127));
    int start_position_secs = _secsFragment * (_currentFragment - 1);
    int end_position_secs = start_position_secs + _secsFragment;
    painter.drawText(QPoint(10, 10),
                     "Raw Data: " +
                     QString::number(start_position_secs) +
                     "s - " + QString::number(end_position_secs) +
                     "s");
    painterMissingData.drawText(QPoint(10,10),"Missing Data:");

    ui->lPanelRough->setPixmap(pixmap);
    ui->lPanelMissingData->setPixmap(pixmapMissingData);

    painter.end();
    painterMissingData.end();
}

void MainWindow2::paintSmoothData()
{

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
    painter.drawText(QPoint(10,10),"Smoothed Data:");

    if (smoothM.n_rows < (_displayStopIndex - 1)) {
        //no smoothM matrix so exit
        ui->lPanelSmooth->setPixmap(pixmap);
        ui->lPanelFlags->setPixmap(pixmapFlags);

        painter.end();
        painterFlags.end();
        return;
    }

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

void MainWindow2::paintFixations()
{

    //this paints both the fixations and autofixations
    int fixTPos = 13;
    int fixPos = 15;
    int barHeight = 16;
    int autofixPos = 50;
    int autofixTPos = 48;

    QPixmap pixmap(ui->lPanelFixations->width()-2,ui->lPanelFixations->height()-2);
    pixmap.fill(Qt::white);

    QPainter painter(&pixmap);

    int counter = 0;

    QFont font=painter.font() ;
    font.setPointSize ( 8 );
    painter.setFont(font);

    std::stringstream number;
    number.str("");

    double posStart = 0;
    double posEnd = 0;

    int startFixationIndex = -1;
    int endFixationIndex = -1;

    //search for the fixations that appear in this segment
    for (uword i = 0; i < fixAllM.n_rows  ; i++) {

        // Fixations that start anywhere in this segment
        bool fixationStartsInSegment = (fixAllM(i,0) >= _displayStartIndex && fixAllM(i,0) <=  _displayStopIndex );

        // Fixations that start before the segment begins, and end in this one.
        bool fixationStartsBeforeSegment = (fixAllM(i,0) <= _displayStartIndex && fixAllM(i,1) >= _displayStartIndex );

        bool displayFixation = fixationStartsInSegment || fixationStartsBeforeSegment;

        if (!displayFixation) {
            continue;
        } else {

            if (startFixationIndex == -1) {
                startFixationIndex = (int)i;
            }

            endFixationIndex = (int)i;

            if (fixationStartsBeforeSegment )
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
            counter ++; // Next fixation

        }
    }

    //update model
    mat currentFixations;
    if (startFixationIndex > -1 && endFixationIndex > -1) {
        currentFixations = fixAllM.rows(startFixationIndex, endFixationIndex);
    } else {
        currentFixations = mat();
    }

    _fixationList->updateWithFixations(currentFixations, -1);


    //now paint auto fixations
   counter = 0;
   posStart = 0;
   posEnd = 0;

   // Just to optimize the code
   uword auxIndex = 0;
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
    painter.end();
}


void MainWindow2::paintVelocity()
{

    QPixmap pixmap(ui->lPanelVelocity->width()-2,ui->lPanelVelocity->height()-2);
    pixmap.fill(Qt::white);

    QPainter painter(&pixmap);
    painter.drawText(QPoint(10,10),"Velocity:");
    QPen myPen(Qt::red, 1, Qt::SolidLine);
    if(!smoothM.is_empty())
    {
        double prevVel, curVel, prevAmp, curAmp = 0;
        int x1,x2 = 0;
        double x_2back, x_1back, x_cur, y_2back, y_1back, y_cur;

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

            prevVel = (double)prevAmp*_hz * _degPerPixel * this->_velocity_view_zoom; // We calculate it x2 to see it better
            curVel = (double)curAmp*_hz * _degPerPixel * this->_velocity_view_zoom;

            // Paint Smooth velocity
            myPen.setColor(QColor(255, 0, 0, 127));
            painter.setPen(myPen);
            painter.drawLine(QPoint(x1, prevVel), QPoint(x2,curVel)); // XL
        }

        //now draw halfway line
        double halfway_pixels = pixmap.height() / 2;
        painter.setPen(QColor(0,0,0,127));
        painter.drawLine(QPoint(0,halfway_pixels),
                         QPoint(pixmap.width(), halfway_pixels));
        painter.drawText(QPoint(10,halfway_pixels),QString::number(halfway_pixels / (_degPerPixel * _hz * this->_velocity_view_zoom)));

        //and draw velocity threshold line
        double vel_thresh_line = _project.GetProjectSetting(Consts::SETTING_VELOCITY_THRESHOLD,Consts::ACTIVE_CONFIGURATION()).toDouble() *
                                 _hz * _degPerPixel * this->_velocity_view_zoom;
        painter.setPen(QColor(0,0,255,127));
        painter.drawLine(QPoint(0,vel_thresh_line),
                         QPoint(pixmap.width(), vel_thresh_line));
        painter.drawText(QPoint(10,vel_thresh_line),"Velocity Threshold");
    }

    ui->lPanelVelocity->setPixmap(pixmap);
    painter.end();
}

void MainWindow2::paintPupilDilation()
{
    QPixmap pixmap(ui->lPanelPupilDilation->width()-2,ui->lPanelPupilDilation->height()-2);
    pixmap.fill(Qt::white);
    QPainter painter(&pixmap);
    painter.drawText(QPoint(10,10),"Pupil Dilation:");
    QPen myPen(Qt::red, 1, Qt::SolidLine);

    if (roughM.n_cols == 8)
    {
        for (uword i = _displayStartIndex; i < _displayStopIndex; ++i) {

            myPen.setColor(QColor(255, 0, 0, 127));
            myPen.setWidth(1);
            painter.setPen(myPen);
            painter.drawPoint((i-_displayStartIndex)*(1/_displayIncrement ), ui->lPanelPupilDilation->height() - (roughM(i ,6 ) * ui->lPanelPupilDilation->height()/_maxPupilDilation)); // Left dilation

            myPen.setColor(QColor(0, 0, 128, 127));
            painter.setPen(myPen);
            painter.drawPoint((i- _displayStartIndex)*(1/_displayIncrement ), ui->lPanelPupilDilation->height() - (roughM(i,7 ) * ui->lPanelPupilDilation->height()/_maxPupilDilation)); // Right dilation
        }
    }

    painter.end();
    ui->lPanelPupilDilation->setPixmap(pixmap);
}


void MainWindow2::paintCurrentFixation(int from, int to)
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




void MainWindow2::paintLabels()
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

    // Paint Total values:
    ui->lSubject->setText(" / " + QString::number(_project.numParticipants()));
    ui->lSegment->setText(" / " + QString::number(_nFragments));

    // Paint current values
    int pn = this->_index_active_participant;
    ui->lineEditParticipant->setText(QString::number(++pn));
    ui->lineEditSegment->setText(QString::number(_currentFragment));

    //Participant name
    ui->lSubjectName->setText(this->p_active_participant->GetName());

    //Velocity zoom level
    this->p_lMagnification->setText("x" + QString::number(this->_velocity_view_zoom,'f',2));
}


void MainWindow2::paintClear()
{
    ui->lPanelRough->clear();
    ui->lPanelMissingData->clear();
    ui->lPanelSmooth->clear();
    ui->lPanelFlags->clear();
    ui->lPanelFixations->clear();
    ui->lPanelVelocity->clear();
    ui->lPanelPupilDilation->clear();
}


/**
 *          PUBLIC SLOTS METHODS
 */

void MainWindow2::fncPress_bInterpolate()
{
    DialogInterpolation w;
    w.setWindowTitle(tr("Interpolate smoothed data"));
    w.loadData(p_active_participant, (*p_smoothM));
    w.exec();

    //reload incase batch processed
    GPMatrixFunctions::readFileSafe((*p_smoothM), p_active_participant->GetMatrixPath(Consts::MATRIX_SMOOTH));

    paintSmoothData();
    paintVelocity();
}

void MainWindow2::fncPress_bSmooth()
{
    DialogSmoothXY win;
    win.loadData((*p_active_participant),(*p_roughM), (*p_smoothM));
    win.setWindowTitle(tr("Smooth data"));
    win.exec();

    //reload incase batch processed
    GPMatrixFunctions::readFileSafe((*p_smoothM), p_active_participant->GetMatrixPath(Consts::MATRIX_SMOOTH));

    paintSmoothData();
    paintVelocity();

}

void MainWindow2::fncPress_bEstimateFixations()
{
    DialogEstimateFixations efd;
    efd.loadData((*p_active_participant),(*p_roughM),(*p_smoothM), (*p_autoFixAllM));
    efd.exec();

    //reload incase batch processed
    GPMatrixFunctions::readFileSafe((*p_smoothM), p_active_participant->GetMatrixPath(Consts::MATRIX_SMOOTH));
    GPMatrixFunctions::readFileSafe((*p_fixAllM), p_active_participant->GetMatrixPath(Consts::MATRIX_FIXALL));
    GPMatrixFunctions::readFileSafe((*p_autoFixAllM), p_active_participant->GetMatrixPath(Consts::MATRIX_AUTOFIXALL));

    if (!p_autoFixAllM->is_empty())
    {
        fixAllM = autoFixAllM;
        GPMatrixFunctions::fncReturnFixationinSegments( p_fixAllM, p_experimentalSegmentsM);
        if (!p_autoFixAllM->is_empty()) GPMatrixFunctions::saveFileSafe((*p_autoFixAllM),p_active_participant->GetMatrixPath(Consts::MATRIX_AUTOFIXALL));
        if (!p_fixAllM->is_empty()) GPMatrixFunctions::saveFileSafe((*p_fixAllM), p_active_participant->GetMatrixPath(Consts::MATRIX_FIXALL));
    }

    paintAll();
}

void MainWindow2::fncPress_bNext()
{
    // Go to the next fragment
    if ( _currentFragment != _nFragments)
    {
        _previousFragment = _currentFragment;
        _currentFragment = _currentFragment + 1;
        paintAll();
    }
}


void MainWindow2::fncPress_bPrevious()
{
    // Go to the previous fragment
    if (_currentFragment != 1)
    {
        _currentFragment = _previousFragment;
        _previousFragment = _previousFragment - 1;
        paintAll();
    }
}

void MainWindow2::fncPress_bParticipantLeft()
{
    int pn1 = this->_index_active_participant;
    int pn = pn1-1;
    if (pn<0) pn = 0;
    if (pn>=_project.numParticipants()) pn = _project.numParticipants()-1;
    ui->lineEditParticipant->setText(QString::number(pn+1));
    if (pn1!=pn) this->fncSetActiveParticipant(pn);
}

void MainWindow2::fncPress_bParticipantRight()
{
    int pn1 = this->_index_active_participant;
    int pn = pn1+1;
    if (pn<0) pn = 0;
    if (pn>=_project.numParticipants()) pn = _project.numParticipants()-1;
    ui->lineEditParticipant->setText(QString::number(pn+1));
    if (pn1!=pn) this->fncSetActiveParticipant(pn);
}

void MainWindow2::fncPress_bExecuteManual()
{
    // Manual execution of Create/ delete / Merge
    int from = ui->txFrom->text().toInt(); // Get the converted data from the text box. If it's not numeric it return 0.
    int to =  ui->txTo->text().toInt();
    if (from >0 && to>0){
        this->fncPress_bExecuteManual(from, to); // Pass the event to MainWindow2 class
    }
}

void MainWindow2::fncPress_bNotes()
{
    DialogParticipantEditNotes gpen(this->p_active_participant);
    gpen.exec();
    this->paintLabels();
}




void MainWindow2::fncPress_bCreate()
{
    fncPressActionButton(Consts::AC_CREATE, ui->bCreate, Consts::HELP_CREATE_MODE);
}

void MainWindow2::fncPress_bDelete()
{
    fncPressActionButton(Consts::AC_DELETE, ui->bDelete, Consts::HELP_DELETE_MODE);
}

void MainWindow2::fncPress_bMerge()
{
    fncPressActionButton(Consts::AC_MERGE, ui->bMerge, Consts::HELP_MERGE_MODE);
}

void MainWindow2::fncPress_bSmoothPursuit()
{
    fncPressActionButton(Consts::AC_SMOOTH_PURSUIT,ui->bSmoothPursuit,Consts::HELP_PURSUIT_MODE);
}

void MainWindow2::fncPress_bResetFixations()
{
    fncPressActionButton(Consts::AC_RESET_TO_AUTO,ui->bResetToAuto,Consts::HELP_RESET_MODE);
}

void MainWindow2::fncPress_bResizeFixation()
{
    fncPressActionButton(Consts::AC_DRAG_ENDS, ui->bResizeFixation, Consts::HELP_DRAG_MODE);
}

void MainWindow2::fncPressActionButton(int action, QPushButton* button, QString message)
{
    // Activate. deactivate an action mode.
    ui->bCreate->setStyleSheet("");
    ui->bDelete->setStyleSheet("");
    ui->bMerge->setStyleSheet("");
    ui->bResetToAuto->setStyleSheet("");
    ui->bSmoothPursuit->setStyleSheet("");
    ui->bResizeFixation->setStyleSheet("");

    if (this->_currentAction == action)
    {
        button->setStyleSheet("");
        _currentAction = Consts::AC_NOACTION;
        ui->statusbar->showMessage(Consts::HELP_STATUS_READY);
    }
    else
    {
        button->setStyleSheet("* { background-color: rgb(255,125,100) }");
        _currentAction = action;
        ui->statusbar->showMessage(message);
    }
}

void MainWindow2::fncPress_bExecuteManual(int from, int to)
{
    // Ececute the action manually rather than with the mouse click
    fncManipulateFix(from, to);
}

void MainWindow2::fncPress_bZoomin()
{
    this->_velocity_view_zoom *= 2;
    paintVelocity();
    paintLabels();
}

void MainWindow2::fncPress_bZoomout()
{
    this->_velocity_view_zoom /= 2;
    paintVelocity();
    paintLabels();
}


/**
  *     MENU EVENTS
  */

void MainWindow2::fncPress_subMenuDetectFixations()
{
    ui->stackedWidget->setCurrentWidget(ui->page_auto);
}

void MainWindow2::fncPress_subMenuManualEditing()
{
    ui->stackedWidget->setCurrentWidget(ui->page_manual);
}

void MainWindow2::fncPress_subMenuDialogConfig()
{
    fncWaitForLoad();

    //TODO: change to load participant settings
    DialogConfig w;
    w.loadData(&_project);
    //w.setWindowTitle(tr("Configure Options"));
    w.exec();

    // update parameters
    fncLoadSettings(Consts::ACTIVE_CONFIGURATION());
    fncResetDisplayParams();
    fncSettingsChanged();
    fncSetActiveParticipant(0);
}

void MainWindow2::fncPress_subMenuShowErrorLog()
{
    DialogGrafixError::ShowDialog();
}

void MainWindow2::fncPress_subMenuShowStatistics()
{
    fncWaitForLoad();
    DialogStatistics w;
    w.setWindowTitle(tr("Statistics"));
    w.loadData(p_fixAllM,p_autoFixAllM, p_experimentalSegmentsM);
    w.exec();
}

void MainWindow2::fncPress_subMenuHeatMap()
{
    fncWaitForLoad();
    DialogHeatMap w;
    w.setWindowTitle(tr("Heap Maps"));
    w.loadData(p_active_participant, roughM, p_fixAllM, p_experimentalSegmentsM);
    w.exec();
}

void MainWindow2::fncPress_subMenuProjectOpen()
{
    // Open a new project
    fncWaitForLoad();

    DialogOpenProject w(&_project);
    w.exec();

    //saves directory of project returned
    QSettings settings("options.ini", QSettings::IniFormat);
    settings.setValue(Consts::SETTING_PROJECT,_project.GetFullDirectory());

    p_active_participant = NULL;
    if (_project.numParticipants()>0)
    {
        qDebug() << "Participants out:" << _project.numParticipants() << "\n";
        fncSetActiveParticipant(0);//TODO: Settings remember last participant
    }
}


void MainWindow2::fncPress_subMenuVisualizeCurrentFragment()
{
    fncWaitForLoad();
    DialogVisualization w;
    w.setWindowTitle(tr("Visualize Fragment"));
    w.loadData(_currentFragment, p_active_participant, roughM, smoothM, fixAllM);
    w.exec();
}

void MainWindow2::fncPress_subMenuVisualizeSegments()
{
    fncWaitForLoad();
    DialogVisualizationSegments w;
    w.setWindowTitle(tr("Visualize Segment"));
    w.loadData(p_active_participant, roughM, smoothM, fixAllM, experimentalSegmentsM);
    w.exec();
}

void MainWindow2::fncPress_subMenuExport()
{
    // Export the fixations in the middle of the segments
    fncWaitForLoad();
    DialogExport w;
    w.setWindowTitle(tr("Export data"));
    w.loadData(p_active_participant, roughM, smoothM, fixAllM);
    w.exec();
}


void MainWindow2::fncPress_subMenuRecalculateFixations()
{
    // It recalcules all the values for the fixations
    fncWaitForLoad();
    GPFixationOperations::fncRecalculateFixations(roughM, &fixAllM, _expWidth, _expHeight, _degPerPixel, _copyEyes);

    if (!fixAllM.is_empty())
        GPMatrixFunctions::saveFile(fixAllM, p_active_participant->GetMatrixPath(Consts::MATRIX_FIXALL));

    this->paintFixations(); //redraw list
}


void MainWindow2::fncPress_subMenuCalculateSmooth()
{
    fncWaitForLoad();
    DialogSmoothXY w;
    w.setWindowTitle(tr("Smooth data"));
    w.loadData( (*p_active_participant), roughM, smoothM);
    w.exec();

    //load smooth data in case it was changed during batch processing
    GPMatrixFunctions::readFileSafe(smoothM, p_active_participant->GetMatrixPath(Consts::MATRIX_SMOOTH));

    paintSmoothData();
    paintVelocity();
}

void MainWindow2::fncPress_subMenuAdjustParameters()
{
    fncWaitForLoad();
    DialogTestSmoothParameters d;
    d.loadData(roughM,_displayStartIndex,_displayStopIndex,(*p_active_participant));

    d.exec();
    fncLoadSettings(Consts::ACTIVE_CONFIGURATION());  //reload parameters after adjustment
}

void MainWindow2::fncPress_subMenuChangeConfiguration()
{
    fncWaitForLoad();
    DialogSaveNewConfiguration d;
    d.loadData(this->_last_selected_configuration, this->_project);
    d.exec();

    GrafixConfiguration selected_config = d.Selected();
    if (!(selected_config == Consts::ACTIVE_CONFIGURATION()))
    {
        //not active configuration, so load the settings.
        _project.ActivateConfiguration(selected_config);
        fncLoadSettings(selected_config);
        this->_configuration_changed = false;
        ui->lConfigurationStatus->setText("Active Configuration: " + selected_config.first);
        ui->statusbar->showMessage("Status: The active configuration '" + selected_config.first +"' has been selected.");
        this->_last_selected_configuration = selected_config;
    }
}

void MainWindow2::fncPress_subMenuReloadConfiguration()
{
    if (_last_selected_configuration == Consts::ACTIVE_CONFIGURATION()) return;
    _project.ActivateConfiguration(_last_selected_configuration);
    fncLoadSettings(_last_selected_configuration);
    this->_configuration_changed = false;
    ui->lConfigurationStatus->setText("Active Configuration: " + _last_selected_configuration.first);
    ui->statusbar->showMessage("Status: The active configuration '" + _last_selected_configuration.first +"' has been re-instantiated.");
}

void MainWindow2::fncPress_subMenuClose()
{
    this->close();
}

void MainWindow2::fncPress_subMenuMovie()
{
    DialogVideoPlayer dvp;
    dvp.loadData(p_active_participant, roughM, smoothM, fixAllM);
    dvp.exec();
}

void MainWindow2::fncChange_tParticipantNumber()
{
    //check if number and in participant
    int number = ui->lineEditParticipant->text().toInt();
    if (number<1) number = 1;
    if (number>_project.numParticipants()) number = _project.numParticipants();
    ui->lineEditParticipant->setText(QString::number(number));

    if (number-1 != this->_index_active_participant)
        this->fncSetActiveParticipant(number-1);
}

void MainWindow2::fncChange_tSegmentNumber()
{
    int number = ui->lineEditSegment->text().toInt();
    if (number<1) number = 1;
    if (number>_nFragments) number = _nFragments;
    ui->lineEditSegment->setText(QString::number(number));

    if (number != _currentFragment)
    {
        _currentFragment = number;
        this->paintAll();
    }
}

void MainWindow2::fncSettingsChanged()
{
    this->_configuration_changed = true;
    ui->lConfigurationStatus->setText("Active Configuration: Unsaved.");
    ui->statusbar->showMessage("Status: The active configuration has been changed, save configuration now if you want to be able to re-use this configuration in the future");
}

void MainWindow2::fncSetToolTips()
{
    this->ui->lPanelMissingData->setToolTip("Missing Data");
    this->ui->lPanelFlags->setToolTip("Flags for various actions performed:\n"
                                      "Interpolation - missing data has been interpolated.\n"
                                      "Consecutive fixations merged - post-hoc merging has occurred.\n"
                                      "High variance removed - post-hoc removal of high variance fixation.\n"
                                      "Short fixations removed - post-hoc removal of short fixation.\n");

    //manual page
    this->ui->bCreate->setToolTip(Consts::HELP_CREATE_MODE);
    this->ui->bDelete->setToolTip(Consts::HELP_DELETE_MODE);
    this->ui->bSmoothPursuit->setToolTip(Consts::HELP_PURSUIT_MODE);
    this->ui->bMerge->setToolTip(Consts::HELP_MERGE_MODE);
    this->ui->bResetToAuto->setToolTip(Consts::HELP_RESET_MODE);
    this->ui->bResizeFixation->setToolTip(Consts::HELP_DRAG_MODE);
    this->ui->txTo->setToolTip(Consts::HELP_MANUAL_EXEC);
    this->ui->txFrom->setToolTip(Consts::HELP_MANUAL_EXEC);
    this->ui->bExecManual->setToolTip(Consts::HELP_MANUAL_EXEC);
    this->ui->lFrom->setToolTip(Consts::HELP_POSITION);
    this->ui->lTo->setToolTip(Consts::HELP_POSITION);

    //auto page
    this->ui->sliderSigmaR->setToolTip(Consts::HELP_SETTING_SIGMA_R);
    this->ui->sliderSigmaS->setToolTip(Consts::HELP_SETTING_SIGMA_S);
    this->ui->sliderVelocity->setToolTip(Consts::HELP_SETTING_VEL);
    this->ui->sliderInterpolation->setToolTip(Consts::HELP_SETTING_INTERP_TIME);
    this->ui->sliderDisplacInterpolate->setToolTip(Consts::HELP_SETTING_INTERP_DISP);
    this->ui->sliderMinFixation->setToolTip(Consts::HELP_SETTING_MIN_DUR);
    this->ui->sliderVelocityVariance->setToolTip(Consts::HELP_SETTING_VARIANCE);
    this->ui->sliderDisplacement->setToolTip(Consts::HELP_SETTING_MERGE);

    this->ui->b_interpolate->setToolTip(Consts::HELP_BUTTON_INTERP);
    this->ui->b_smooth->setToolTip(Consts::HELP_BUTTON_FILTER);
    this->ui->b_estimateFixations->setToolTip(Consts::HELP_BUTTON_EST);

    this->ui->leSigmaR->setToolTip(Consts::HELP_SETTING_SIGMA_R);
    this->ui->leSigmaS->setToolTip(Consts::HELP_SETTING_SIGMA_S);
    this->ui->leVelocity->setToolTip(Consts::HELP_SETTING_VEL);
    this->ui->leInterpolation->setToolTip(Consts::HELP_SETTING_INTERP_TIME);
    this->ui->leDisplacInterpolate->setToolTip(Consts::HELP_SETTING_INTERP_DISP);
    this->ui->leMinFixation->setToolTip(Consts::HELP_SETTING_MIN_DUR);
    this->ui->leVelocityVariance->setToolTip(Consts::HELP_SETTING_VARIANCE);
    this->ui->leDisplacement->setToolTip(Consts::HELP_SETTING_MERGE);

}

void MainWindow2::fncChange_tInterpolation()
{
    fncChangeLineEdit(Consts::SETTING_INTERP_LATENCY, ui->leInterpolation, ui->sliderInterpolation);
}
void MainWindow2::fncChange_tDisplacement()
{
    fncChangeLineEdit(Consts::SETTING_POSTHOC_MERGE_CONSECUTIVE_VAL, ui->leDisplacement, ui->sliderDisplacement);
}
void MainWindow2::fncChange_tDisplacInterpolate()
{
    fncChangeLineEdit(Consts::SETTING_INTERP_MAXIMUM_DISPLACEMENT, ui->leDisplacInterpolate,ui->sliderDisplacInterpolate);
}
void MainWindow2::fncChange_tMinFixation()
{
    fncChangeLineEdit(Consts::SETTING_POSTHOC_MIN_DURATION_VAL, ui->leMinFixation, ui->sliderMinFixation);
}
void MainWindow2::fncChange_tVelocity()
{
    fncChangeLineEdit(Consts::SETTING_VELOCITY_THRESHOLD, ui->leVelocity, ui->sliderVelocity);
    paintVelocity();//to paint line
}
void MainWindow2::fncChange_tVelocityVariance()
{
    fncChangeLineEdit(Consts::SETTING_POSTHOC_LIMIT_RMS_VAL, ui->leVelocityVariance, ui->sliderVelocityVariance);
}
void MainWindow2::fncChange_tSigmaS()
{
    fncChangeLineEdit(Consts::SETTING_SMOOTHING_SIGMA_S, ui->leSigmaS, ui->sliderSigmaS);
}
void MainWindow2::fncChange_tSigmaR()
{
    fncChangeLineEdit(Consts::SETTING_SMOOTHING_SIGMA_R, ui->leSigmaR, ui->sliderSigmaR);
}

void MainWindow2::fncChangeSlider(QString setting, QSlider* sl, QLineEdit* le)
{
    if (_hold_changes) return;
    _hold_changes = true;
    double new_value = Consts::GetValueFromSlider(setting, sl->value());
    _project.SetProjectSetting(setting,Consts::ACTIVE_CONFIGURATION(),new_value);
    le->setText(QString::number(new_value,'f',3));
    fncSettingsChanged();
    _hold_changes = false;
}

void MainWindow2::fncChangeLineEdit(QString setting, QLineEdit* le, QSlider* sl)
{
    if (_hold_changes) return;
    _hold_changes = true;
    double new_value = le->text().toDouble();
    _project.SetProjectSetting(setting, Consts::ACTIVE_CONFIGURATION(),new_value);
    sl->setValue(Consts::GetSliderValue(setting, new_value));
    fncSettingsChanged();
    _hold_changes = false;
}

void MainWindow2::fncChange_sSigmaS()
{
    fncChangeSlider(Consts::SETTING_SMOOTHING_SIGMA_S, ui->sliderSigmaS, ui->leSigmaS);
}
void MainWindow2::fncChange_sSigmaR()
{
    fncChangeSlider(Consts::SETTING_SMOOTHING_SIGMA_R, ui->sliderSigmaR, ui->leSigmaR);
}
void MainWindow2::fncChange_sDisplacement()
{
    fncChangeSlider(Consts::SETTING_POSTHOC_MERGE_CONSECUTIVE_VAL, ui->sliderDisplacement, ui->leDisplacement);
}

void MainWindow2::fncChange_sDisplacInterpolate()
{
    fncChangeSlider(Consts::SETTING_INTERP_MAXIMUM_DISPLACEMENT, ui->sliderDisplacInterpolate, ui->leDisplacInterpolate);
}

void MainWindow2::fncChange_sInterpolation()
{
    fncChangeSlider(Consts::SETTING_INTERP_LATENCY, ui->sliderInterpolation, ui->leInterpolation);
}

void MainWindow2::fncChange_sMinFixation()
{
    fncChangeSlider(Consts::SETTING_POSTHOC_MIN_DURATION_VAL, ui->sliderMinFixation, ui->leMinFixation);
}


void MainWindow2::fncChange_sVelocity()
{
    fncChangeSlider(Consts::SETTING_VELOCITY_THRESHOLD, ui->sliderVelocity, ui->leVelocity);
    paintVelocity(); //to paint line
}

void MainWindow2::fncChange_sVelocityVariance()
{
    fncChangeSlider(Consts::SETTING_POSTHOC_LIMIT_RMS_VAL, ui->sliderVelocityVariance, ui->leVelocityVariance);
}

void MainWindow2::fncPress_cbDisplacement(){
    _project.SetProjectSetting(Consts::SETTING_POSTHOC_MERGE_CONSECUTIVE_FLAG, Consts::ACTIVE_CONFIGURATION(),
                                 ui->cb_displacement->checkState());
    fncSettingsChanged();
}

void MainWindow2::fncPress_cbMinFixation(){
    _project.SetProjectSetting(Consts::SETTING_POSTHOC_MIN_DURATION_FLAG, Consts::ACTIVE_CONFIGURATION(),
                                 ui->cb_minFixation->checkState());
    fncSettingsChanged();
}

void MainWindow2::fncPress_cbVelocityVariance(){
    _project.SetProjectSetting(Consts::SETTING_POSTHOC_LIMIT_RMS_FLAG, Consts::ACTIVE_CONFIGURATION(),
                                 ui->cb_velocityVariance->checkState());
    fncSettingsChanged();
}


