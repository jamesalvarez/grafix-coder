#include "DialogConfig.h"
#include "ui_DialogConfig.h"



using namespace std;

DialogConfig::DialogConfig(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogConfig)
{
    ui->setupUi(this);
}


void DialogConfig::loadData(GrafixProject *project)
{
    _project        = project;
    _configuration  = Consts::ACTIVE_CONFIGURATION();
    _expHeight      = _project->GetProjectSetting(Consts::SETTING_EXP_HEIGHT,
                                                  _configuration).toInt();
    _expWidth       = _project->GetProjectSetting(Consts::SETTING_EXP_WIDTH,
                                                  _configuration).toInt();
    _hz             = _project->GetProjectSetting(Consts::SETTING_HZ,
                                                  _configuration).toInt();
    _secsSegment    = _project->GetProjectSetting(Consts::SETTING_SECS_FRAGMENT,
                                                  _configuration).toInt();
    _degreePerPixel = _project->GetProjectSetting(Consts::SETTING_DEGREE_PER_PIX,
                                                  _configuration).toDouble();
    ui->cbEyes->setChecked(_project->GetProjectSetting(Consts::SETTING_SMOOTHING_USE_OTHER_EYE,
                                                  _configuration).toBool());


    ui->combo_HZ->addItem("50", Consts::HZ_50);
    ui->combo_HZ->addItem("60", Consts::HZ_60);
    ui->combo_HZ->addItem("120", Consts::HZ_120);
    ui->combo_HZ->addItem("300", Consts::HZ_300);
    ui->combo_HZ->addItem("1000", Consts::HZ_1000);

    int index = ui->combo_HZ->findData( _hz);            // Set Value
    if ( index != -1 ) { // -1 for not found
       ui->combo_HZ->setCurrentIndex(index);
    }

    ui->combo_secsSegment->addItem("10", Consts::FRAGMENT_DURATION_10);
    ui->combo_secsSegment->addItem("11", Consts::FRAGMENT_DURATION_11);
    ui->combo_secsSegment->addItem("12", Consts::FRAGMENT_DURATION_12);
    ui->combo_secsSegment->addItem("13", Consts::FRAGMENT_DURATION_13);
    ui->combo_secsSegment->addItem("14", Consts::FRAGMENT_DURATION_14);
    ui->combo_secsSegment->addItem("15", Consts::FRAGMENT_DURATION_15);
    ui->combo_secsSegment->addItem("16", Consts::FRAGMENT_DURATION_16);
    ui->combo_secsSegment->addItem("17", Consts::FRAGMENT_DURATION_17);
    ui->combo_secsSegment->addItem("18", Consts::FRAGMENT_DURATION_18);
    ui->combo_secsSegment->addItem("19", Consts::FRAGMENT_DURATION_19);
    ui->combo_secsSegment->addItem("20", Consts::FRAGMENT_DURATION_20);
    ui->combo_secsSegment->addItem("21", Consts::FRAGMENT_DURATION_21);
    ui->combo_secsSegment->addItem("22", Consts::FRAGMENT_DURATION_22);
    ui->combo_secsSegment->addItem("23", Consts::FRAGMENT_DURATION_23);
    ui->combo_secsSegment->addItem("24", Consts::FRAGMENT_DURATION_24);
    ui->combo_secsSegment->addItem("25", Consts::FRAGMENT_DURATION_25);

    index = ui->combo_secsSegment->findData( _secsSegment);  // Set value
    if ( index != -1 ) { // -1 for not found
       ui->combo_secsSegment->setCurrentIndex(index);
    }

    ui->tWidth->setText(QString::number(_expWidth));
    ui->tHeight->setText(QString::number(_expHeight));
    ui->tDegreePerPixel->setText(QString::number(_degreePerPixel));


    // Events
    connect(ui->combo_HZ , SIGNAL(currentIndexChanged(int)),this,SLOT(fncChange_HZ(int)));
    connect(ui->combo_secsSegment , SIGNAL(currentIndexChanged(int)),this,SLOT(fncChange_secsSegment(int)));
    connect( ui->b_accept, SIGNAL( clicked() ), this, SLOT( fncPress_bAccept() ) );
    connect( ui->b_cancel, SIGNAL( clicked() ), this, SLOT( close() ) );

}

DialogConfig::~DialogConfig()
{
    delete ui;
}


void DialogConfig::fncChange_HZ(int index)
{
    int value = ui->combo_HZ->itemData(index).toInt();
    _hz = value;
}

void DialogConfig::fncChange_secsSegment(int index)
{
    int value = ui->combo_secsSegment->itemData(index).toInt();
    _secsSegment = value;
}

void DialogConfig::fncPress_bAccept()
{
    // Store settings file and close dialog
    _project->SetProjectSetting(Consts::SETTING_HZ, _configuration, _hz);
    _project->SetProjectSetting(Consts::SETTING_EXP_WIDTH, _configuration, ui->tWidth->text().toInt());
    _project->SetProjectSetting(Consts::SETTING_EXP_HEIGHT, _configuration, ui->tHeight->text().toInt());
    _project->SetProjectSetting(Consts::SETTING_DEGREE_PER_PIX, _configuration, ui->tDegreePerPixel->text().toDouble());
    _project->SetProjectSetting(Consts::SETTING_SECS_FRAGMENT, _configuration, _secsSegment);
    _project->SetProjectSetting(Consts::SETTING_SMOOTHING_USE_OTHER_EYE, _configuration, ui->cbEyes->isChecked());
    this->close();
}


