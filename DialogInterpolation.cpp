#include "DialogInterpolation.h"
#include "ui_DialogInterpolation.h"


DialogInterpolation::DialogInterpolation(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogInterpolation)
{
    ui->setupUi(this);

    // Events
    connect( ui->b_accept, SIGNAL( clicked() ), this, SLOT( fncPress_bAccept() ) );
    connect( ui->b_cancel, SIGNAL( clicked() ), this, SLOT( close() ) );
    connect( ui->b_Batch, SIGNAL( clicked() ), this, SLOT( fncPress_bBatch() ) );

}

DialogInterpolation::~DialogInterpolation()
{
    delete ui;
}

void DialogInterpolation::loadData(GrafixParticipant* participant, mat &SmoothM)
{
    this->p_smoothM = (&SmoothM);
    this->_participant = participant;
    this->_project = _participant->GetProject();

    if ((*p_smoothM).is_empty()){
        ui->lMsg->setText("The data must be smoothed before interpolating it. \n \n Please smooth it first.");
    }else{
        //
    }

}

void DialogInterpolation::fncPress_bAccept(){

    if ((*p_smoothM).is_empty())
    {
        // The data is still not smoothed, we cannot proceed.
        close();
    }else
    {
        // Interpolate!
        ui->lMsg->setText("Processing ... ");
        qApp->processEvents();
        GPMatrixProgressBar gpProgressBar(this);
        GPMatrixFunctions::interpolateData((*p_smoothM), _project->GetProjectSettingsPath(), gpProgressBar);

    }

    if (!p_smoothM->is_empty() && GPMatrixFunctions::saveFile((*p_smoothM), _participant->GetMatrixPath(Consts::MATRIX_SMOOTH)))
        _participant->SetParticipantSetting(Consts::PSETTING_INTERPOLATED_DATE,QDateTime::currentDateTime());
    else
    {
        //TODO
    }

    close();

}

void DialogInterpolation::fncPress_bBatch()
{
    DialogBatchProcess gpbp(_project,Consts::TASK_INTERPOLATE);
    gpbp.exec();
    close();
}



