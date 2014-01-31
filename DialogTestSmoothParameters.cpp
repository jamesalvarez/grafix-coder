#include "DialogTestSmoothParameters.h"
#include "ui_DialogTestSmoothParameters.h"

DialogTestSmoothParameters::DialogTestSmoothParameters(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogTestSmoothParameters)
{
    _data_loaded = false;
    ui->setupUi(this);

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

    //value changed
    connect( ui->sliderSigmaR, SIGNAL(valueChanged(int)), this, SLOT( fncChange_sSmooth_R()) );
    connect( ui->sliderSigmaS, SIGNAL(valueChanged(int)), this, SLOT( fncChange_sSmooth_S()) );
    connect( ui->sliderInterpolation, SIGNAL( valueChanged(int)), this, SLOT( fncChange_sInterpolation() ) );
    connect( ui->sliderDisplacInterpolate, SIGNAL( valueChanged(int)), this, SLOT( fncChange_sDisplacInterpolate() ) );
    connect( ui->sliderDisplacement, SIGNAL( valueChanged(int)), this, SLOT( fncChange_sDisplacement() ) );
    connect( ui->sliderMinFixation, SIGNAL( valueChanged(int)), this, SLOT( fncChange_sMinFixation() ) );
    connect( ui->sliderVelocity, SIGNAL( valueChanged(int)), this, SLOT( fncChange_sVelocity() ) );
    connect( ui->sliderVelocityVariance, SIGNAL( valueChanged(int)), this, SLOT( fncChange_sVelocityVariance() ) );



}

DialogTestSmoothParameters::~DialogTestSmoothParameters()
{
    delete ui;
}

void DialogTestSmoothParameters::fncSetTempSetting(QString setting, QVariant value)
{
    p_project->SetProjectSetting(setting, Consts::ACTIVE_CONFIGURATION(), value, _temp_settings_path);
    fncUpdateLabels();
    fncSmoothData();
}

void DialogTestSmoothParameters::fncSetTempSettingFromSlider(QString setting, int slider_value, QLabel *label)
{
    double value = Consts::GetValueFromSlider(setting,slider_value);
    label->setText(QString::number(value,'f',3));
    fncSetTempSetting(setting,value);
}

void DialogTestSmoothParameters::fncChange_sDisplacement()
{
    fncSetTempSettingFromSlider(Consts::SETTING_POSTHOC_MERGE_CONSECUTIVE_VAL,
                                ui->sliderDisplacement->value(),
                                ui->lDisplacement);
}

void DialogTestSmoothParameters::fncChange_sDisplacInterpolate()
{
    fncSetTempSettingFromSlider(Consts::SETTING_INTERP_MAXIMUM_DISPLACEMENT,
                                ui->sliderDisplacInterpolate->value(),
                                ui->lDisplacInterpolate);
}

void DialogTestSmoothParameters::fncChange_sInterpolation()
{
    fncSetTempSettingFromSlider(Consts::SETTING_INTERP_LATENCY,
                                ui->sliderInterpolation->value(),
                                ui->lInterpolation);
}

void DialogTestSmoothParameters::fncChange_sMinFixation()
{
    fncSetTempSettingFromSlider(Consts::SETTING_POSTHOC_MIN_DURATION_VAL,
                                ui->sliderMinFixation->value(),
                                ui->lMinFixation);
}


void DialogTestSmoothParameters::fncChange_sVelocity()
{
    fncSetTempSettingFromSlider(Consts::SETTING_VELOCITY_THRESHOLD,
                                ui->sliderVelocity->value(),
                                ui->lVelocity);
}

void DialogTestSmoothParameters::fncChange_sVelocityVariance()
{
    fncSetTempSettingFromSlider(Consts::SETTING_POSTHOC_LIMIT_RMS_VAL,
                                ui->sliderVelocityVariance->value(),
                                ui->lVelocityVariance);
}

void DialogTestSmoothParameters::fncChange_sSmooth_R()
{
    fncSetTempSettingFromSlider(Consts::SETTING_SMOOTHING_SIGMA_R,
                                ui->sliderSigmaR->value(),
                                ui->lSigmaR);
}

void DialogTestSmoothParameters::fncChange_sSmooth_S()
{
    fncSetTempSettingFromSlider(Consts::SETTING_SMOOTHING_SIGMA_S,
                                ui->sliderSigmaS->value(),
                                ui->lSigmaS);
}

void DialogTestSmoothParameters::fncPress_cbDisplacement()
{
    fncSetTempSetting(Consts::SETTING_POSTHOC_MERGE_CONSECUTIVE_FLAG,
                                 ui->cb_displacement->checkState());
}

void DialogTestSmoothParameters::fncPress_cbMinFixation()
{
    fncSetTempSetting(Consts::SETTING_POSTHOC_MIN_DURATION_FLAG,
                                 ui->cb_minFixation->checkState());
}

void DialogTestSmoothParameters::fncPress_cbVelocityVariance()
{
    fncSetTempSetting(Consts::SETTING_POSTHOC_LIMIT_RMS_FLAG,
                                 ui->cb_velocityVariance->checkState());
}

void DialogTestSmoothParameters::fncPress_bPlotRMS()
{
    DialogPlot dp;
    dp.loadRMSData(p_participant);
    dp.exec();
}

void DialogTestSmoothParameters::fncPress_bPlotDur()
{
    DialogPlot dp;
    dp.loadDurData(p_participant);
    dp.exec();
}

void DialogTestSmoothParameters::fncPress_bPlotVelocities()
{
    DialogPlot dp;
    dp.loadVelData(p_participant);
    dp.exec();
}

void DialogTestSmoothParameters::fncPress_bLeft()
{
    fncGetSubmat(_displayStartIndex - _secsFragmentHz, _displayStartIndex);
    paintRoughData();
    fncSmoothData();
}

void DialogTestSmoothParameters::fncPress_bRight()
{
    fncGetSubmat(_displayStartIndex + _secsFragmentHz, _displayStopIndex + _secsFragmentHz);
    paintRoughData();
    fncSmoothData();
}

void DialogTestSmoothParameters::apply()
{
    QMessageBox qmb;
    qmb.setText("Do you want to overwrite project settings?");
    qmb.setInformativeText("Warning: there is no way to undo this.");
    qmb.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    qmb.setDefaultButton(QMessageBox::No);
    if(qmb.exec() == QMessageBox::Yes)
    {
        //overwrite;
        QFile(p_participant->GetProject()->GetProjectSettingsPath()).remove();
        QFile::copy(_temp_settings_path, p_participant->GetProject()->GetProjectSettingsPath());
    }
}

void DialogTestSmoothParameters::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e);
    paintRoughData();
    paintSmoothData();
    paintFixations();
    paintVelocity();
}



void DialogTestSmoothParameters::loadData(const mat &RoughM, uword displayStartIndex, uword displayStopIndex, const GrafixParticipant &p)
{
    this->p_participant = &p;
    this->p_project = p.GetProject();
    _temp_settings_path = "tempsettings";
    QFile(_temp_settings_path).remove();
    QFile::copy(p.GetProject()->GetProjectSettingsPath(),_temp_settings_path);

    GrafixSettingsLoader loader(_temp_settings_path);

    this->_expHeight = loader.LoadSetting(Consts::SETTING_EXP_HEIGHT).toInt();
    this->_expWidth = loader.LoadSetting(Consts::SETTING_EXP_WIDTH).toInt();
    this->_hz = loader.LoadSetting(Consts::SETTING_HZ).toInt();

    int secsFragment = loader.LoadSetting(Consts::SETTING_SECS_FRAGMENT).toInt();

    this->_secsFragmentHz = secsFragment * _hz;
    this->_displayIncrement = (double) _secsFragmentHz / ui->lPanelRough->width();
    this->_degreePerPixel = loader.LoadSetting(Consts::SETTING_DEGREE_PER_PIX).toDouble();

    ui->sliderSigmaS->setValue(
                loader.LoadSliderSetting(
                        Consts::SETTING_SMOOTHING_SIGMA_S));

    ui->sliderSigmaR->setValue(
                loader.LoadSliderSetting(
                        Consts::SETTING_SMOOTHING_SIGMA_R));

    ui->sliderInterpolation->setValue(
                loader.LoadSliderSetting(
                        Consts::SETTING_INTERP_LATENCY));

    ui->sliderMinFixation->setValue(
                loader.LoadSliderSetting(
                        Consts::SETTING_POSTHOC_MIN_DURATION_VAL));

    ui->sliderVelocity->setValue(
                loader.LoadSliderSetting(
                        Consts::SETTING_VELOCITY_THRESHOLD));

    ui->sliderVelocityVariance->setValue(
                loader.LoadSliderSetting(
                        Consts::SETTING_POSTHOC_LIMIT_RMS_VAL));

    ui->sliderDisplacement->setValue(
                loader.LoadSliderSetting(
                          Consts::SETTING_POSTHOC_MERGE_CONSECUTIVE_VAL));

    ui->sliderDisplacInterpolate->setValue(
                loader.LoadSliderSetting(
                          Consts::SETTING_INTERP_MAXIMUM_DISPLACEMENT));

    ui->cb_displacement->setChecked(loader.LoadSetting(Consts::SETTING_POSTHOC_MERGE_CONSECUTIVE_FLAG).toBool());
    ui->cb_minFixation->setChecked(loader.LoadSetting(Consts::SETTING_POSTHOC_MIN_DURATION_FLAG).toBool());
    ui->cb_velocityVariance->setChecked(loader.LoadSetting(Consts::SETTING_POSTHOC_LIMIT_RMS_FLAG).toBool());
    this->p_participant = &p;
    this->p_original_roughM = &RoughM;

    fncUpdateLabels();

    fncGetSubmat(displayStartIndex, displayStopIndex);

    _data_loaded = true;



    //moved
    connect( ui->sliderSigmaR, SIGNAL(sliderMoved(int)), this, SLOT( fncUpdateLabels()) );
    connect( ui->sliderSigmaS, SIGNAL(sliderMoved(int)), this, SLOT( fncUpdateLabels()) );
    connect( ui->sliderInterpolation, SIGNAL( sliderMoved(int)), this, SLOT( fncUpdateLabels() ) );
    connect( ui->sliderDisplacInterpolate, SIGNAL( sliderMoved(int)), this, SLOT( fncUpdateLabels() ) );
    connect( ui->sliderDisplacement, SIGNAL( sliderMoved(int)), this, SLOT( fncUpdateLabels() ) );
    connect( ui->sliderMinFixation, SIGNAL( sliderMoved(int)), this, SLOT( fncUpdateLabels() ) );
    connect( ui->sliderVelocity, SIGNAL( sliderMoved(int)), this, SLOT( fncUpdateLabels() ) );
    connect( ui->sliderVelocityVariance, SIGNAL( sliderMoved(int)), this, SLOT( fncUpdateLabels() ) );


    connect( ui->cb_displacement, SIGNAL( clicked()), this, SLOT( fncPress_cbDisplacement() ) );
    connect( ui->cb_minFixation, SIGNAL( clicked()), this, SLOT( fncPress_cbMinFixation() ) );
    connect( ui->cb_velocityVariance, SIGNAL( clicked()), this, SLOT( fncPress_cbVelocityVariance() ) );
    connect( ui->bApply, SIGNAL( clicked()), this, SLOT(apply()) );
    connect( ui->bClose, SIGNAL( clicked()), this, SLOT(close()) );
    connect( ui->bPlotRMS, SIGNAL( clicked()), this, SLOT( fncPress_bPlotRMS()));
    connect( ui->bPlotDurations, SIGNAL( clicked()), this, SLOT(fncPress_bPlotDur()));
    connect( ui->bPlotVelocities, SIGNAL( clicked()), this, SLOT(fncPress_bPlotVelocities()));

    connect( ui->bLeft, SIGNAL( clicked()), this, SLOT(fncPress_bLeft()));
    connect( ui->bRight, SIGNAL( clicked()), this, SLOT(fncPress_bRight()));

    this->showMaximized();
    paintRoughData();
    fncSmoothData();

}

void DialogTestSmoothParameters::fncGetSubmat(uword displayStartIndex, uword displayStopIndex)
{
    if (displayStopIndex>(*p_original_roughM).n_rows) displayStopIndex = (*p_original_roughM).n_rows - 1;
    if (displayStartIndex >= displayStopIndex) return;

    this->_displayStartIndex = displayStartIndex;
    this->_displayStopIndex = displayStopIndex;
    this->_roughM = (*p_original_roughM).submat(_displayStartIndex,0,_displayStopIndex,(*p_original_roughM).n_cols-1);
}

void DialogTestSmoothParameters::paintFixations()
{
    int col0, col1, col2, col3;

    col0 = 0;
    col1 = 30;
    col2 = 90;
    col3 = 150;
    if (_fixAllM.is_empty()) return;

    int fixTPos = 13;
    int fixPos = 15;
    int barHeight = 16;

    QPixmap pixmap(ui->lPanelFixations->width()-2,ui->lPanelFixations->height()-2);
    pixmap.fill(Qt::white);
    QPainter painter(&pixmap);

    QPixmap pixmapFixList(ui->lPanelFixationList->width()-2,ui->lPanelFixationList->height()-2);
    pixmapFixList.fill(Qt::transparent);
    QPainter painterFixList(&pixmapFixList);


    int counter = 0;

    QFont font=painter.font() ;
    font.setPointSize ( 8 );
    painter.setFont(font);

    std::stringstream number;
    number.str("");

    double posStart = 0;
    double posEnd = 0;

    // Just to optimize the code
    int auxIndex = 0;

    int displayStartIndex = 0;
    int displayStopIndex = _roughM.n_rows;
    for (uword i = auxIndex; i < _fixAllM.n_rows  ; i++)
    {
        if ((_fixAllM(i,0) >= displayStartIndex && _fixAllM(i,0) <=  displayStopIndex ) ||(_fixAllM(i,0) <= displayStartIndex && _fixAllM(i,1) >= displayStartIndex )){

            // DRAW FIXATION
            if (_fixAllM(i,0) <= displayStartIndex && _fixAllM(i,1) >= displayStartIndex ) // If it's a fixation that didn't end in the previous segment
                posStart = -1;
            else
                posStart = (_fixAllM(i,0) - displayStartIndex ) * (1/_displayIncrement);

            posEnd = ((_fixAllM(i,1) - displayStartIndex ) * (1/_displayIncrement)) - posStart;

            if (_fixAllM(i,5) == Consts::SMOOTHP_YES){
                painter.setBrush(QBrush("#a32e0c"));
            }else{
                painter.setBrush(QBrush("#1ac500"));
            }
            painter.drawRect(QRect( posStart ,fixPos, posEnd,barHeight));
            number.str("");
            number << counter;
            painter.drawText( QPoint(posStart,fixTPos), number.str().c_str() );


            //UDPATE TABLE
            painterFixList.drawText( QPoint(col0,((counter +1) * 17)+ 20), QString::number(counter) );
            // RMS
            double variance = _fixAllM.at(i,5);
            painterFixList.drawText( QPoint(col1,((counter +1) * 17)+ 20), QString::number(variance,'f', 2) );

            //DURATION
            double dur = _fixAllM.at(i,2) * 1000;
            painterFixList.drawText( QPoint(col2,((counter +1) * 17)+ 20), QString::number(dur,'f', 0));

            if (i>0)
            {
                //positions in degrees
                double avXp = _fixAllM.at(i-1,3) * _expWidth * _degreePerPixel;
                double avYp = _fixAllM.at(i-1,4) * _expHeight * _degreePerPixel;
                double avX = _fixAllM.at(i,3) * _expWidth * _degreePerPixel;
                double avY = _fixAllM.at(i,4) * _expHeight * _degreePerPixel;

                //distance
                double distance = sqrt(
                                        pow((avX - avXp), 2)
                                      + pow((avY - avYp), 2));

                painterFixList.drawText( QPoint(col3,((counter +1) * 17)+ 20), QString::number(distance,'f', 2));
            }
            else
                painterFixList.drawText( QPoint(col3,((counter +1) * 17)+ 20), "-");


            counter ++; // Next fixation

        }else if (_fixAllM(i,1) >= displayStopIndex){
            break;
        }
    }
    painterFixList.drawText( QPoint(col0,20),"No.");
    painterFixList.drawText( QPoint(col1,20),"Variance");
    painterFixList.drawText( QPoint(col2,20),"Duration");
    painterFixList.drawText( QPoint(col3,20),"Distance");
    ui->lPanelFixationList->setPixmap(pixmapFixList);
    ui->lPanelFixations->setPixmap(pixmap);

    painterFixList.end();
    painter.end();
}

void DialogTestSmoothParameters::paintRoughData()
{
    if (_roughM.is_empty()) return;
    QPixmap pixmap(ui->lPanelRough->width()-2,ui->lPanelRough->height()-2);
    pixmap.fill(Qt::white);
    QPainter painter(&pixmap);
    QPen myPen(Qt::red, 1.2, Qt::SolidLine);

    for (uword i = 0; i < _roughM.n_rows; ++i) {
        myPen.setColor(QColor(255, 0, 0, 127));

        if (_roughM(i,2 ) != -1){
            painter.setPen(myPen);
            painter.drawPoint( (i )*(1/_displayIncrement ), _roughM(i,2 ) * ui->lPanelRough->height()); // XL
        }

        myPen.setColor(QColor(0, 0, 128, 127));
        if (_roughM(i,3 ) != -1){
            painter.setPen(myPen);
            painter.drawPoint( (i)*(1/_displayIncrement ), _roughM(i,3 ) * ui->lPanelRough->height()); // YL
        }
       myPen.setColor(QColor(255, 50, 0, 127));
        if (_roughM(i,4 ) != -1){
            painter.setPen(myPen);
            painter.drawPoint( (i)*(1/_displayIncrement ), _roughM(i,4 ) * ui->lPanelRough->height()); // XR
        }

        myPen.setColor(QColor(0, 50, 128, 127));
        if (_roughM(i,5 ) != -1){
            painter.setPen(myPen);
            painter.drawPoint( (i)*(1/_displayIncrement ), _roughM(i,5 ) * ui->lPanelRough->height()); // YR
        }
    }
    ui->lPanelRough->setPixmap(pixmap);


    painter.end();

}

void DialogTestSmoothParameters::paintVelocity()
{
    if (_smoothM.is_empty()) return;

    QPixmap pixmap(ui->lPanelVelocity->width()-2,ui->lPanelVelocity->height()-2);
    pixmap.fill(Qt::white);

    QPainter painter(&pixmap);
    painter.drawText(QPoint(10,10),"Velocity:");
    QPen myPen(Qt::red, 1, Qt::SolidLine);

    double prevVel, curVel, prevAmp, curAmp = 0;
    int x1,x2 = 0;
    int x_2back, x_1back, x_cur, y_2back, y_1back, y_cur;

    for (uword i = 2; i < _smoothM.n_rows-1; ++i) {
        x1 = (i-1)*(1/_displayIncrement );
        x2 = (i)*(1/_displayIncrement );

        x_2back = _smoothM(i-2,2);
        x_1back = _smoothM(i-1,2);
        x_cur   = _smoothM(i,2);
        y_2back = _smoothM(i-2,3);
        y_1back = _smoothM(i-1,3);
        y_cur   = _smoothM(i,3);

        // oldAmp=sqrt(((XTwoBack-XOneBack)^2+(YTwoBack-YOneBack)^2)/2);
        prevAmp = sqrt((double)( ((x_2back - x_1back)* (x_2back - x_1back)) + ((y_2back - y_1back)* (y_2back - y_1back)))/2);
        curAmp = sqrt((double)(((x_1back - x_cur)*(x_1back - x_cur)) + ((y_1back - y_cur)*(y_1back - y_cur)))/2);

        prevVel = (double)prevAmp*_hz * _degreePerPixel; // We calculate it x2 to see it better
        curVel = (double)curAmp*_hz * _degreePerPixel;

        // Paint Smooth velocity
        myPen.setColor(QColor(255, 0, 0, 127));
        painter.setPen(myPen);
        painter.drawLine(QPoint(x1, prevVel), QPoint(x2,curVel)); // XL
    }

    ui->lPanelVelocity->setPixmap(pixmap);
    painter.end();
}

void DialogTestSmoothParameters::paintSmoothData()
{
    if (_smoothM.is_empty()) return;


    QPixmap pixmap(ui->lPanelSmooth->width()-2,ui->lPanelSmooth->height()-2);
    pixmap.fill(Qt::white);

    QPainter painter(&pixmap);
    QPen myPen(Qt::red, 1, Qt::SolidLine);

    for (uword i = 0; i < _smoothM.n_rows; ++i) {

        myPen.setColor(QColor(255, 0, 0, 127));
        myPen.setWidth(1);
        painter.setPen(myPen);
        painter.drawPoint((i)*(1/_displayIncrement ), _smoothM(i ,2 ) * ui->lPanelRough->height() / _expWidth); // XL

        myPen.setColor(QColor(0, 0, 128, 127));
        painter.setPen(myPen);
        painter.drawPoint((i)*(1/_displayIncrement ), _smoothM(i,3 ) * ui->lPanelRough->height() / _expHeight); // YL

    }

    ui->lPanelSmooth->setPixmap(pixmap);

    painter.end();
}

void DialogTestSmoothParameters::fncUpdateLabels()
{

}

void DialogTestSmoothParameters::fncSmoothData()
{
    if (!_data_loaded) return;
    this->_displayIncrement = (double) _secsFragmentHz / ui->lPanelRough->width();

    GrafixSettingsLoader settingsLoader(_temp_settings_path);
    GPMatrixFunctions::smoothRoughMatrixFBF(_roughM, this->_temp_settings_path, Consts::ACTIVE_CONFIGURATION(), &_smoothM);
    //GPMatrixFunctions::smoothRoughMatrixTrilateral(_roughM,settingsLoader,&_smoothM);
    GPMatrixFunctions::estimateFixations(_roughM,_smoothM,_fixAllM, settingsLoader );
    paintRoughData();
    paintSmoothData();
    paintFixations();
    paintVelocity();
}
