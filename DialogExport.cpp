#include "DialogExport.h"
#include "ui_DialogExport.h"



DialogExport::DialogExport(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogExport)
{
    ui->setupUi(this);

    connect( ui->b_export, SIGNAL( clicked() ), this, SLOT( fncPress_bExport() ) );
    connect( ui->b_cancel, SIGNAL( clicked() ), this, SLOT( fncPress_bCancel() ) );
    connect( ui->b_export_all, SIGNAL( clicked() ), this, SLOT( fncPress_bExportAll()));
    connect( ui->b_select_all, SIGNAL( clicked() ), this, SLOT( fncPress_bSelectAll()));

    this->_checkboxes = QVector<QCheckBox*>() << ui->cb_xLeftRough <<
        ui->cb_yLeftRough << ui->cb_xRightRough <<
        ui->cb_yRightRough << ui->cb_xFilter <<
        ui->cb_yFilter << ui->cb_LeftPupil <<
        ui->cb_rightPupil << ui->cb_fixNumber <<
        ui->cb_fixDur << ui->cb_xAverage <<
        ui->cb_yAverage << ui->cb_euclideanDis <<
        ui->cb_sacNumber << ui->cb_sacDuration <<
        ui->cb_smoothpursuit <<ui->cb_sacAmplitude <<
        ui->cb_sacVelocityAv << ui->cb_sacVelocityPeak << ui->cb_velocity;
}

DialogExport::~DialogExport()
{
    delete ui;
}



void DialogExport::loadData(GrafixParticipant *participant, mat roughM,mat smoothM, mat fixAllM){
    this->_participant = participant;
    this->roughM = roughM;
    this->smoothM = smoothM;
    this->fixAllM = fixAllM;

    // Load checkbox states
    GrafixSettingsLoader settingsLoader(*(this->_participant->GetProject()));
    for (int i = 0; i < _checkboxes.size(); ++i) {
        QString setting = this->getSettingForCheckBox(_checkboxes[i]);
        bool checkBoxState = settingsLoader.LoadSetting(setting).toBool();
        _checkboxes[i]->setChecked(checkBoxState);
    }

}


void DialogExport::fncPress_bCancel(){
    close();
}

void DialogExport::fncPress_bExportAll() {
    //Save state
    GrafixSettingsLoader settingsLoader(*(this->_participant->GetProject()));
    for (int i = 0; i < _checkboxes.size(); ++i) {
        settingsLoader.SetSetting(this->getSettingForCheckBox(_checkboxes[i]), _checkboxes[i]->isChecked());
    }

    //Begin batch processing, send project.
    DialogBatchProcess gpbp(this->_participant->GetProject(), Consts::TASK_EXPORT);
    gpbp.exec();
    close();
}

QString DialogExport::getSettingForCheckBox(QCheckBox* checkBox) {
    if (checkBox == ui->cb_xLeftRough)
        return Consts::SETTING_EXPORT_LEFT_X_ROUGH;
    if (checkBox == ui->cb_yLeftRough)
        return Consts::SETTING_EXPORT_LEFT_Y_ROUGH;
    if (checkBox == ui->cb_xRightRough)
        return Consts::SETTING_EXPORT_RIGHT_X_ROUGH;
    if (checkBox == ui->cb_yRightRough)
        return Consts::SETTING_EXPORT_RIGHT_Y_ROUGH;
    if (checkBox == ui->cb_xFilter)
        return Consts::SETTING_EXPORT_X_SMOOTH;
    if (checkBox == ui->cb_yFilter)
        return Consts::SETTING_EXPORT_Y_SMOOTH;
    if (checkBox == ui->cb_LeftPupil)
        return Consts::SETTING_EXPORT_LEFT_PUPIL;
    if (checkBox == ui->cb_rightPupil)
        return Consts::SETTING_EXPORT_RIGHT_PUPIL;
    if (checkBox == ui->cb_fixNumber)
        return Consts::SETTING_EXPORT_FIXATION_NUMBER;
    if (checkBox == ui->cb_fixDur)
        return Consts::SETTING_EXPORT_FIXATION_DURATION;
    if (checkBox == ui->cb_xAverage)
        return Consts::SETTING_EXPORT_FIXATION_X_AVERAGE;
    if (checkBox == ui->cb_yAverage)
        return Consts::SETTING_EXPORT_FIXATION_Y_AVERAGE;
    if (checkBox == ui->cb_euclideanDis)
        return Consts::SETTING_EXPORT_FIXATION_RMS;
    if (checkBox == ui->cb_sacNumber)
        return Consts::SETTING_EXPORT_SACCADE_NUMBER;
    if (checkBox == ui->cb_sacDuration)
        return Consts::SETTING_EXPORT_SACCADE_DURATION;
    if (checkBox == ui->cb_smoothpursuit)
        return Consts::SETTING_EXPORT_FIXATION_IS_SMOOTH_PURSUIT;
    if (checkBox == ui->cb_sacAmplitude)
        return Consts::SETTING_EXPORT_SACCADE_DISTANCE;
    if (checkBox == ui->cb_sacVelocityAv)
        return Consts::SETTING_EXPORT_SACCADE_VELOCITY_AVERAGE;
    if (checkBox == ui->cb_sacVelocityPeak)
        return Consts::SETTING_EXPORT_SACCADE_VELOCITY_PEAK;
    if (checkBox == ui->cb_velocity)
        return Consts::SETTING_EXPORT_VELOCITY;
    return "";

}

void DialogExport::fncPress_bExport() {

    GrafixSettingsLoader settingsLoader(*(this->_participant->GetProject()));

    //Save state
    for (int i = 0; i < _checkboxes.size(); ++i) {
        settingsLoader.SetSetting(this->getSettingForCheckBox(_checkboxes[i]), _checkboxes[i]->isChecked());
    }

    // **** Save the file
    QString filename = QFileDialog::getSaveFileName(this,"Save exported data",QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),"*.csv");

    //check that fn has extension
    if (QFileInfo(filename).suffix() == "") {
        filename += ".csv";
    }

    GPMatrixFunctions::exportFile(roughM, smoothM, fixAllM, filename, settingsLoader);

    this->close();
}

void DialogExport::fncPress_bSelectAll() {
    for (int i = 0; i < _checkboxes.size(); ++i) {
        _checkboxes[i]->setChecked(true);
    }
}
