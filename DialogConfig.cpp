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
    _secsFragment    = _project->GetProjectSetting(Consts::SETTING_SECS_FRAGMENT,
                                                  _configuration).toInt();
    _degreePerPixel = _project->GetProjectSetting(Consts::SETTING_DEGREE_PER_PIX,
                                                  _configuration).toDouble();
    ui->cbEyes->setChecked(_project->GetProjectSetting(Consts::SETTING_SMOOTHING_USE_OTHER_EYE,
                                                  _configuration).toBool());




    ui->spinBoxHz->setValue(_hz);
    ui->spinBoxFragment->setValue(_secsFragment);

    ui->tWidth->setText(QString::number(_expWidth));
    ui->tHeight->setText(QString::number(_expHeight));
    ui->tDegreePerPixel->setText(QString::number(_degreePerPixel));


    // Events
    connect(ui->spinBoxHz , SIGNAL(valueChanged(int)),this,SLOT(fncChange_HZ(int)));
    connect(ui->spinBoxFragment , SIGNAL(valueChanged(int)),this,SLOT(fncChange_secsSegment(int)));
    connect( ui->b_accept, SIGNAL( clicked() ), this, SLOT( fncPress_bAccept() ) );
    connect( ui->b_cancel, SIGNAL( clicked() ), this, SLOT( close() ) );

}

DialogConfig::~DialogConfig()
{
    delete ui;
}


void DialogConfig::fncChange_HZ(int index)
{
    _hz = ui->spinBoxHz->value();
}

void DialogConfig::fncChange_secsSegment(int index)
{
    _secsFragment = ui->spinBoxFragment->value();
}

void DialogConfig::fncPress_bAccept()
{
    // Store settings file and close dialog
    _project->SetProjectSetting(Consts::SETTING_HZ, _configuration, _hz);
    _project->SetProjectSetting(Consts::SETTING_EXP_WIDTH, _configuration, ui->tWidth->text().toInt());
    _project->SetProjectSetting(Consts::SETTING_EXP_HEIGHT, _configuration, ui->tHeight->text().toInt());
    _project->SetProjectSetting(Consts::SETTING_DEGREE_PER_PIX, _configuration, ui->tDegreePerPixel->text().toDouble());
    _project->SetProjectSetting(Consts::SETTING_SECS_FRAGMENT, _configuration, _secsFragment);
    _project->SetProjectSetting(Consts::SETTING_SMOOTHING_USE_OTHER_EYE, _configuration, ui->cbEyes->isChecked());
    this->close();
}


