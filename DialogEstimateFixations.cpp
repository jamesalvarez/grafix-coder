#include "DialogEstimateFixations.h"
#include "ui_DialogEstimateFixations.h"

DialogEstimateFixations::DialogEstimateFixations(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogEstimateFixations)
{
    ui->setupUi(this);

    // Events
    connect( ui->b_accept, SIGNAL( clicked() ), this, SLOT( fncPress_bAccept() ) );
    connect( ui->b_cancel, SIGNAL( clicked() ), this, SLOT( fncPress_bClose() ) );
    connect( ui->b_Batch, SIGNAL( clicked() ), this, SLOT( fncPress_bBatch()));
}

DialogEstimateFixations::~DialogEstimateFixations()
{
    delete ui;
}

/**
  *     PRIVATE METHODS
  */

void DialogEstimateFixations::loadData(GrafixParticipant &participant, mat &roughM, mat &smoothM, mat &autoFixAllM){


    this->p_roughM = &roughM;
    this->p_smoothM = &smoothM;
    this->p_autoFixAllM = &autoFixAllM;

    this->_participant = &participant;
    this->_project = _participant->GetProject();


    if (!p_autoFixAllM->is_empty()){
        ui->lMsg->setText("The fixations are estimated already. \n \n Would you like to estimate them again?");
    }
}


/**
  *     PUBLIC SLOTS
  */
void DialogEstimateFixations::fncPress_bAccept(){

    ui->lMsg->setText("Estimating Fixations");

    GPMatrixProgressBar gpProgress(this);

    GrafixSettingsLoader settingsLoader((*_project)); //uses active configuration by default

    GPMatrixFunctions::estimateFixations((*p_roughM),
                                         (*p_smoothM),
                                         (*p_autoFixAllM),
                                         settingsLoader,
                                         gpProgress);

    if (GPMatrixFunctions::saveFile((*p_smoothM), _participant->GetMatrixPath(Consts::MATRIX_SMOOTH)) &
        GPMatrixFunctions::saveFile((*p_autoFixAllM), _participant->GetMatrixPath(Consts::MATRIX_AUTOFIXALL)))
    {

        _participant->SetParticipantSetting(Consts::PSETTING_ESTIMATED_FIX_DATE, QDateTime::currentDateTime());
    }



    close();
}

 void DialogEstimateFixations::fncPress_bClose()
 {
     close();
 }

 void DialogEstimateFixations::fncPress_bBatch()
 {
     //Begin batch processing, send project.
     DialogBatchProcess gpbp(_project, Consts::TASK_ESTIMATE_FIXATIONS);
     gpbp.exec();
     close();
 }
