#include "DialogSmoothXY.h"
#include "ui_DialogSmoothXY.h"



DialogSmoothXY::DialogSmoothXY(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSmoothXY)
{
    ui->setupUi(this);

    // Events
    connect( ui->b_accept, SIGNAL( clicked() ), this, SLOT( fncPress_bAccept() ) );
    connect( ui->b_cancel, SIGNAL( clicked() ), this, SLOT( fncPress_bClose() ) );
    connect( ui->b_Batch, SIGNAL( clicked() ), this, SLOT( fncPress_bBatch()));
    connect( ui->cb_eyes, SIGNAL( clicked() ), this, SLOT( fncPress_cbUseBothEyes()));
    // By default, we use both eyes.
    ui->cb_eyes->setChecked(true);
}

DialogSmoothXY::~DialogSmoothXY()
{
    delete ui;
}

/**
  *     PRIVATE METHODS
  */

void DialogSmoothXY::loadData(GrafixParticipant &participant,mat const &roughM, mat &smoothM){


    this->p_roughM = &roughM;
    this->p_smoothM = &smoothM;

    this->_participant = &participant;
    this->_project = _participant->GetProject();
    this->_configuration = Consts::ACTIVE_CONFIGURATION();

    ui->cb_eyes->setChecked(_project->GetProjectSetting(Consts::SETTING_SMOOTHING_USE_OTHER_EYE, _configuration).toBool());

    if (!p_smoothM->is_empty()){
        ui->lMsg->setText("The data is smoothed already. \n \n Would you like to smooth it again?");
    }
}


/**
  *     PUBLIC SLOTS
  */
void DialogSmoothXY::fncPress_bAccept(){

    ui->lMsg->setText("Smoothing Data");
    _project->SetProjectSetting(Consts::SETTING_SMOOTHING_USE_OTHER_EYE, _configuration, ui->cb_eyes->isChecked());
    GPMatrixProgressBar gpProgress(this);

    GPMatrixFunctions::smoothRoughMatrixFBF((*this->p_roughM),
                                            (*this->_participant),
                                              this->_configuration,
                                              this->p_smoothM,
                                              &gpProgress);

    if (GPMatrixFunctions::saveFile((*p_smoothM), _participant->GetMatrixPath(Consts::MATRIX_SMOOTH)))
        _participant->SetParticipantSetting(Consts::PSETTING_SMOOTHED_DATE, QDateTime::currentDateTime());
    else
    {
        //TODO
    }


    close();
}

 void DialogSmoothXY::fncPress_bClose()
 {
     close();
 }

 void DialogSmoothXY::fncPress_bBatch()
 {
     //Begin batch processing, send project.
     DialogBatchProcess gpbp(_project, Consts::TASK_SMOOTH);
     gpbp.exec();
     close();
 }

 void DialogSmoothXY::fncPress_cbUseBothEyes()
 {
     _project->SetProjectSetting(Consts::SETTING_SMOOTHING_USE_OTHER_EYE, _configuration,
                                  ui->cb_eyes->checkState());
 }


