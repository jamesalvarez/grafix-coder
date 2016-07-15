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
    mat exportM = roughM.col(0);
    mat aux;

    if (ui->cb_xLeftRough->isChecked()){    // X left rough
        aux = zeros(roughM.n_rows, exportM.n_cols + 1);
        aux.cols(0,exportM.n_cols-1) = exportM.cols(0,exportM.n_cols-1);
        aux.col(exportM.n_cols) = roughM.col(2);
        exportM = aux;
    }
    if (ui->cb_yLeftRough->isChecked()){    // Y left rough
        aux = zeros(roughM.n_rows, exportM.n_cols + 1);
        aux.cols(0,exportM.n_cols-1) = exportM.cols(0,exportM.n_cols-1);
        aux.col(exportM.n_cols) = roughM.col(3);
        exportM = aux;
    }
    if (ui->cb_xRightRough->isChecked()){    // x right rough
        aux = zeros(roughM.n_rows, exportM.n_cols + 1);
        aux.cols(0,exportM.n_cols-1) = exportM.cols(0,exportM.n_cols-1);
        aux.col(exportM.n_cols) = roughM.col(4);
        exportM = aux;
    }
    if (ui->cb_yRightRough->isChecked()){    // Y right rough
        aux = zeros(roughM.n_rows, exportM.n_cols + 1);
        aux.cols(0,exportM.n_cols-1) = exportM.cols(0,exportM.n_cols-1);
        aux.col(exportM.n_cols) = roughM.col(5);
        exportM = aux;
    }
    if (ui->cb_LeftPupil->isChecked()){    // Pupil dilation left
        aux = zeros(roughM.n_rows, exportM.n_cols + 1);
        aux.cols(0,exportM.n_cols-1) = exportM.cols(0,exportM.n_cols-1);
        aux.col(exportM.n_cols) = roughM.col(6);
        exportM = aux;
    }
    if (ui->cb_rightPupil->isChecked()){    // Pupil dilation right
        aux = zeros(roughM.n_rows, exportM.n_cols + 1);
        aux.cols(0,exportM.n_cols-1) = exportM.cols(0,exportM.n_cols-1);
        aux.col(exportM.n_cols) = roughM.col(7);
        exportM = aux;
    }
    if (ui->cb_xFilter->isChecked()){    // Flitered X
        aux = zeros(roughM.n_rows, exportM.n_cols + 1);
        aux.cols(0,exportM.n_cols-1) = exportM.cols(0,exportM.n_cols-1);
        aux.col(exportM.n_cols) = smoothM.col(2);
        exportM = aux;
    }
    if (ui->cb_yFilter->isChecked()){    // Flitered Y
        aux = zeros(roughM.n_rows, exportM.n_cols + 1);
        aux.cols(0,exportM.n_cols-1) = exportM.cols(0,exportM.n_cols-1);
        aux.col(exportM.n_cols) = smoothM.col(3);
        exportM = aux;
    }
    if (ui->cb_fixNumber->isChecked()){    // Fixation number!
        aux = zeros(roughM.n_rows, exportM.n_cols + 1);
        aux.cols(0,exportM.n_cols-1) = exportM.cols(0,exportM.n_cols-1);

        // Calculate fixation number
        for (uword i = 0; i < fixAllM.n_rows ; i ++){ // run all the fixations
            aux.rows(fixAllM(i,0), fixAllM(i,1)).col(exportM.n_cols).fill(i+1);
        }
        exportM = aux;
    }

    if (ui->cb_fixDur->isChecked()){    // Fixation duration!
        aux = zeros(roughM.n_rows, exportM.n_cols + 1);
        aux.cols(0,exportM.n_cols-1) = exportM.cols(0,exportM.n_cols-1);

        // Calculate fixation number
        for (uword i = 0; i < fixAllM.n_rows ; i ++){ // run all the fixations
            aux.rows(fixAllM(i,0), fixAllM(i,1)).col(exportM.n_cols).fill(fixAllM(i,2));
        }
        exportM = aux;
    }

    if (ui->cb_xAverage->isChecked()){    // Average X
        aux = zeros(roughM.n_rows, exportM.n_cols + 1);
        aux.cols(0,exportM.n_cols-1) = exportM.cols(0,exportM.n_cols-1);

        // Calculate fixation number
        for (uword i = 0; i < fixAllM.n_rows ; i ++){ // run all the fixations
            aux.rows(fixAllM(i,0), fixAllM(i,1)).col(exportM.n_cols).fill(fixAllM(i,3));
        }
        exportM = aux;
    }

    if (ui->cb_yAverage->isChecked()){    // Average y
        aux = zeros(roughM.n_rows, exportM.n_cols + 1);
        aux.cols(0,exportM.n_cols-1) = exportM.cols(0,exportM.n_cols-1);

        // Calculate fixation number
        for (uword i = 0; i < fixAllM.n_rows ; i ++){ // run all the fixations
            aux.rows(fixAllM(i,0), fixAllM(i,1)).col(exportM.n_cols).fill(fixAllM(i,4));
        }
        exportM = aux;
    }

    if (ui->cb_euclideanDis->isChecked()){    // Euclidean distance
        aux = zeros(roughM.n_rows, exportM.n_cols + 1);
        aux.cols(0,exportM.n_cols-1) = exportM.cols(0,exportM.n_cols-1);

        // Calculate fixation number
        for (uword i = 0; i < fixAllM.n_rows ; i ++){ // run all the fixations
            aux.rows(fixAllM(i,0), fixAllM(i,1)).col(exportM.n_cols).fill(fixAllM(i,5));
        }
        exportM = aux;
    }

    if (ui->cb_smoothpursuit->isChecked()){    // Is smooth pursuit?
        aux = zeros(roughM.n_rows, exportM.n_cols + 1);
        aux.cols(0,exportM.n_cols-1) = exportM.cols(0,exportM.n_cols-1);

        // Calculate fixation number
        for (uword i = 0; i < fixAllM.n_rows ; i ++){ // run all the fixations
            aux.rows(fixAllM(i,0), fixAllM(i,1)).col(exportM.n_cols).fill(fixAllM(i,6));
        }
        exportM = aux;
    }

    // **** Save the file
    QString fn = QFileDialog::getSaveFileName(this,"Save exported data","","*.csv");

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
