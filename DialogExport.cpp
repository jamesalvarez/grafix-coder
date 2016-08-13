#include "DialogExport.h"
#include "ui_DialogExport.h"



DialogExport::DialogExport(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogExport)
{
    ui->setupUi(this);

    connect( ui->b_export, SIGNAL( clicked() ), this, SLOT( fncPress_bExport() ) );
    connect( ui->b_cancel, SIGNAL( clicked() ), this, SLOT( fncPress_bCancel() ) );
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
    _saved = false;
    _filename = "";

}


void DialogExport::fncPress_bCancel(){
    close();
}

void DialogExport::fncPress_bExport(){

    // Calculate saccades
    mat saccadesM;
    GPMatrixFunctions::fncCalculateSaccades(&saccadesM, &fixAllM, &roughM, GrafixSettingsLoader(*(this->_participant->GetProject())));

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
