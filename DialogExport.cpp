#include "DialogExport.h"
#include "ui_DialogExport.h"



DialogExport::DialogExport(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogExport) {
    ui->setupUi(this);

    connect( ui->b_export, SIGNAL( clicked() ), this, SLOT( fncPress_bExport() ) );
    connect( ui->b_cancel, SIGNAL( clicked() ), this, SLOT( fncPress_bCancel() ) );
    connect( ui->b_export_all, SIGNAL( clicked() ), this, SLOT( fncPress_bExportAll()));
    connect( ui->b_select_all, SIGNAL( clicked() ), this, SLOT( fncPress_bSelectAll()));

    connect( ui->rb_timestamped, SIGNAL( clicked() ), this, SLOT( fncChangeExportType()));
    connect( ui->rb_saccades, SIGNAL( clicked() ), this, SLOT( fncChangeExportType()));
    connect( ui->rb_fixations, SIGNAL( clicked() ), this, SLOT( fncChangeExportType()));

    this->_checkboxes = QVector<QCheckBox*>() << ui->cb_xLeftRough <<
                        ui->cb_yLeftRough << ui->cb_xRightRough <<
                        ui->cb_yRightRough << ui->cb_xFilter <<
                        ui->cb_yFilter << ui->cb_LeftPupil <<
                        ui->cb_rightPupil << ui->cb_fixNumber <<
                        ui->cb_fixDur << ui->cb_xAverage <<
                        ui->cb_yAverage << ui->cb_euclideanDis <<
                        ui->cb_sacNumber << ui->cb_sacDuration <<
                        ui->cb_smoothpursuit << ui->cb_sacAmplitude <<
                        ui->cb_sacVelocityAv << ui->cb_sacVelocityPeak << ui->cb_velocity;
}

DialogExport::~DialogExport() {
    delete ui;
}



void DialogExport::loadData(GrafixParticipant *participant, mat roughM, mat smoothM, mat fixAllM) {
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

    Consts::EXPORT_TYPE exportType = (Consts::EXPORT_TYPE)(settingsLoader.LoadSetting(Consts::SETTING_EXPORT_TYPE).toInt());

    switch(exportType) {
        case Consts::EXPORT_TIMESTAMPED:
            ui->rb_timestamped->setChecked(true);
            break;
        case Consts::EXPORT_FIXATIONS:
            ui->rb_fixations->setChecked(true);
            break;
        case Consts::EXPORT_SACCADES:
            ui->rb_saccades->setChecked(true);
            break;
    }

    fncChangeExportType();
}


void DialogExport::fncPress_bCancel() {
    close();
}

void DialogExport::fncPress_bExportAll() {
    //Save state
    GrafixSettingsLoader settingsLoader(*(this->_participant->GetProject()));
    for (int i = 0; i < _checkboxes.size(); ++i) {
        settingsLoader.SetSetting(this->getSettingForCheckBox(_checkboxes[i]), _checkboxes[i]->isChecked());
    }
    settingsLoader.SetSetting(Consts::SETTING_EXPORT_TYPE,(int)fncGetCurrentExportTypeFromRadioButtons());

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

    GrafixProject *project = this->_participant->GetProject();
    GrafixSettingsLoader settingsLoader(*project);

    //Save state
    for (int i = 0; i < _checkboxes.size(); ++i) {
        settingsLoader.SetSetting(this->getSettingForCheckBox(_checkboxes[i]), _checkboxes[i]->isChecked());
    }

    settingsLoader.SetSetting(Consts::SETTING_EXPORT_TYPE,(int)fncGetCurrentExportTypeFromRadioButtons());

    // **** Save the file

    // Get current path of project
    QString projectPath = project->GetFullDirectory();
    QString filename = QFileDialog::getSaveFileName(this, "Save exported data", projectPath, "*.csv");

    //check that fn has extension
    if (QFileInfo(filename).suffix() == "") {
        filename += ".csv";
    }

    GPMatrixFiles::exportFile(roughM, smoothM, fixAllM, filename, settingsLoader);

    this->close();
}

void DialogExport::fncPress_bSelectAll() {
    for (int i = 0; i < _checkboxes.size(); ++i) {
        _checkboxes[i]->setChecked(true);
    }
}

Consts::EXPORT_TYPE DialogExport::fncGetCurrentExportTypeFromRadioButtons() {


    if (ui->rb_fixations->isChecked()) {
        return Consts::EXPORT_FIXATIONS;
    } else if (ui->rb_saccades->isChecked()) {
        return Consts::EXPORT_SACCADES;
    }

    return Consts::EXPORT_TIMESTAMPED;

}

void DialogExport::fncChangeExportType() {

    Consts::EXPORT_TYPE exportType = fncGetCurrentExportTypeFromRadioButtons();

    bool canSelectSaccades = false;
    bool canSelectFixations = false;
    bool canSelectOthers = false;

    switch(exportType) {
        case Consts::EXPORT_TIMESTAMPED:
            canSelectSaccades = true;
            canSelectFixations = true;
            canSelectOthers = true;
            break;
        case Consts::EXPORT_FIXATIONS:
            canSelectFixations = true;
            break;
        case Consts::EXPORT_SACCADES:
            canSelectSaccades = true;
            break;
    }

    ui->cb_xLeftRough->setEnabled(canSelectOthers);
    ui->cb_yLeftRough->setEnabled(canSelectOthers);
    ui->cb_xRightRough->setEnabled(canSelectOthers);
    ui->cb_yRightRough->setEnabled(canSelectOthers);
    ui->cb_xFilter->setEnabled(canSelectOthers);
    ui->cb_yFilter->setEnabled(canSelectOthers);
    ui->cb_LeftPupil->setEnabled(canSelectOthers);
    ui->cb_rightPupil->setEnabled(canSelectOthers);
    ui->cb_fixNumber->setEnabled(canSelectFixations);
    ui->cb_fixDur->setEnabled(canSelectFixations);
    ui->cb_xAverage->setEnabled(canSelectFixations);
    ui->cb_yAverage->setEnabled(canSelectFixations);
    ui->cb_euclideanDis->setEnabled(canSelectFixations);
    ui->cb_sacNumber->setEnabled(canSelectSaccades);
    ui->cb_sacDuration->setEnabled(canSelectSaccades);
    ui->cb_smoothpursuit->setEnabled(canSelectFixations);
    ui->cb_sacAmplitude->setEnabled(canSelectSaccades);
    ui->cb_sacVelocityAv->setEnabled(canSelectSaccades);
    ui->cb_sacVelocityPeak->setEnabled(canSelectSaccades);
    ui->cb_velocity->setEnabled(canSelectOthers);

}
