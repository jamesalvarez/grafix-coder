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

    this->_checkboxes = QVector<QCheckBox*>() << ui->cb_xLeftRough <<
        ui->cb_yLeftRough << ui->cb_xRightRough <<
        ui->cb_yRightRough << ui->cb_xFilter <<
        ui->cb_yFilter << ui->cb_LeftPupil <<
        ui->cb_rightPupil << ui->cb_fixNumber <<
        ui->cb_fixDur << ui->cb_xAverage <<
        ui->cb_yAverage << ui->cb_euclideanDis <<
        ui->cb_sacNumber << ui->cb_sacDuration <<
        ui->cb_smoothpursuit <<ui->cb_sacAmplitude <<
        ui->cb_sacVelocityAv << ui->cb_sacVelocityPeak;
}

DialogExport::~DialogExport()
{
    delete ui;
}



void DialogExport::loadData(GrafixParticipant *participant, mat roughM,mat smoothM, mat fixAllM, mat experimentalSegmentsM){
    this->_participant = participant;
    this->roughM = roughM;
    this->smoothM = smoothM;
    this->fixAllM = fixAllM;
    this->experimentalSegmentsM = experimentalSegmentsM;

    this->_saved = false;
    this->_filename = "";

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
        return Consts::SETTING_EXPORT_FIXATION_DISTANCE;
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

    return "";

}

void DialogExport::fncPress_bExport(){

    // Calculate saccades
    mat saccadesM;
    GrafixSettingsLoader settingsLoader(*(this->_participant->GetProject()));
    GPMatrixFunctions::fncCalculateSaccades(&saccadesM, &fixAllM, &roughM, settingsLoader);

    //Save state
    for (int i = 0; i < _checkboxes.size(); ++i) {
        settingsLoader.SetSetting(this->getSettingForCheckBox(_checkboxes[i]), _checkboxes[i]->isChecked());
    }

    // Collect data to export in matrix
    mat exportM = roughM.col(0);

    if (ui->cb_xLeftRough->isChecked()){    // X left rough
        join_rows(exportM, roughM.col(2));
    }
    if (ui->cb_yLeftRough->isChecked()){    // Y left rough
        join_rows(exportM, roughM.col(3));
    }
    if (ui->cb_xRightRough->isChecked()){    // x right rough
        join_rows(exportM, roughM.col(4));
    }
    if (ui->cb_yRightRough->isChecked()){    // Y right rough
        join_rows(exportM, roughM.col(5));
    }
    if (ui->cb_LeftPupil->isChecked()){    // Pupil dilation left
        join_rows(exportM, roughM.col(6));
    }
    if (ui->cb_rightPupil->isChecked()){    // Pupil dilation right
        join_rows(exportM, roughM.col(7));
    }
    if (ui->cb_xFilter->isChecked()){    // Flitered X
        join_rows(exportM, smoothM.col(2));
    }
    if (ui->cb_yFilter->isChecked()){    // Flitered Y
        join_rows(exportM, roughM.col(3));
    }
    if (ui->cb_fixNumber->isChecked()){    // Fixation number!

        int new_col_index = exportM.n_cols;
        exportM.insert_cols(new_col_index,1,true);

        // Calculate fixation number
        for (uword i = 0; i < fixAllM.n_rows ; i ++) {
            exportM.rows(fixAllM(i,0), fixAllM(i,1)).col(new_col_index).fill(i+1);
        }
    }

    if (ui->cb_fixDur->isChecked()){    // Fixation duration!
        int new_col_index = exportM.n_cols;
        exportM.insert_cols(new_col_index,1,true);

        for (uword i = 0; i < fixAllM.n_rows ; i ++) {
            exportM.rows(fixAllM(i,0), fixAllM(i,1)).col(new_col_index).fill(fixAllM(i,2));
        }
    }

    if (ui->cb_xAverage->isChecked()){    // Average X
        int new_col_index = exportM.n_cols;
        exportM.insert_cols(new_col_index,1,true);

        for (uword i = 0; i < fixAllM.n_rows ; i ++) {
            exportM.rows(fixAllM(i,0), fixAllM(i,1)).col(new_col_index).fill(fixAllM(i,3));
        }
    }

    if (ui->cb_yAverage->isChecked()){    // Average y
        int new_col_index = exportM.n_cols;
        exportM.insert_cols(new_col_index,1,true);

        for (uword i = 0; i < fixAllM.n_rows ; i ++) {
            exportM.rows(fixAllM(i,0), fixAllM(i,1)).col(new_col_index).fill(fixAllM(i,4));
        }
    }

    if (ui->cb_euclideanDis->isChecked()){    // Euclidean distance
        int new_col_index = exportM.n_cols;
        exportM.insert_cols(new_col_index,1,true);

        for (uword i = 0; i < fixAllM.n_rows ; i ++) {
            exportM.rows(fixAllM(i,0), fixAllM(i,1)).col(new_col_index).fill(fixAllM(i,5));
        }
    }

    if (ui->cb_smoothpursuit->isChecked()){    // Is smooth pursuit?
        int new_col_index = exportM.n_cols;
        exportM.insert_cols(new_col_index,1,true);

        for (uword i = 0; i < fixAllM.n_rows ; i ++) {
            exportM.rows(fixAllM(i,0), fixAllM(i,1)).col(new_col_index).fill(fixAllM(i,6));
        }
    }

    if (ui->cb_sacNumber->isChecked()){    // Saccade number!
        int new_col_index = exportM.n_cols;
        exportM.insert_cols(new_col_index,1,true);

        for (uword i = 0; i < saccadesM.n_rows ; i ++) {
            exportM.rows(saccadesM(i,0), saccadesM(i,1)).col(new_col_index).fill(i + 1);
        }
    }

    if (ui->cb_sacDuration->isChecked()){    // Saccade Amplitude
        int new_col_index = exportM.n_cols;
        exportM.insert_cols(new_col_index,1,true);

        for (uword i = 0; i < saccadesM.n_rows ; i ++) {
            exportM.rows(saccadesM(i,0), saccadesM(i,1)).col(new_col_index).fill(saccadesM(i,2));
        }
    }

    if (ui->cb_sacAmplitude->isChecked()){    // Saccade Amplitude
        int new_col_index = exportM.n_cols;
        exportM.insert_cols(new_col_index,1,true);

        for (uword i = 0; i < saccadesM.n_rows ; i ++) {
            exportM.rows(saccadesM(i,0), saccadesM(i,1)).col(new_col_index).fill(saccadesM(i,3));
        }
    }

    if (ui->cb_sacVelocityAv->isChecked()){    // Saccade Amplitude
        int new_col_index = exportM.n_cols;
        exportM.insert_cols(new_col_index,1,true);

        for (uword i = 0; i < saccadesM.n_rows ; i ++) {
            exportM.rows(saccadesM(i,0), saccadesM(i,1)).col(new_col_index).fill(saccadesM(i,4));
        }
    }

    if (ui->cb_sacVelocityPeak->isChecked()){    // Saccade Amplitude
        int new_col_index = exportM.n_cols;
        exportM.insert_cols(new_col_index,1,true);

        for (uword i = 0; i < saccadesM.n_rows ; i ++) {
            exportM.rows(saccadesM(i,0), saccadesM(i,1)).col(new_col_index).fill(saccadesM(i,5));
        }
    }


    // **** Save the file
    QString fn = QFileDialog::getSaveFileName(this,"Save exported data","","*.csv");

    //check that fn has extension
    if (QFileInfo(fn).suffix() == "") {
        fn += ".csv";
    }

    if (fn != "" && GPMatrixFunctions::saveFileSafe(exportM,fn)) {
        _saved = true;
        _filename = fn;
    }

    this->close();
}

bool DialogExport::Saved()
{
    return _saved;
}

QString DialogExport::Filename()
{
    return _filename;
}
