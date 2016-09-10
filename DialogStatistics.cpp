#include "DialogStatistics.h"
#include "ui_DialogStatistics.h"

DialogStatistics::DialogStatistics(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogStatistics)
{
    ui->setupUi(this);
}

DialogStatistics::~DialogStatistics()
{
    delete ui;
}

void DialogStatistics::loadData(mat* p_fixAllM_in, mat* p_autoFixAllM_in, mat* p_experimentalSegmentsM){
    fixAllM = (*p_fixAllM_in);
    autoFixAllM = (*p_autoFixAllM_in);

    // Remove fixations that are not in the segments
    GPMatrixFunctions::returnFixationinSegments( &fixAllM, p_experimentalSegmentsM);
    GPMatrixFunctions::returnFixationinSegments( &autoFixAllM, p_experimentalSegmentsM);;

    computeStatistics();

}

void DialogStatistics::computeStatistics(){
    std::ostringstream o;
    uvec fixIndex;
    mat aux;

    if (fixAllM.n_rows > 0){  // Manual statistics
        //N
        o << fixAllM.n_rows ;
        ui->l_N_m->setText(o.str().c_str());

        // Mean
        o.str("");
        o << mean(mean(fixAllM.col(2)));
        ui->l_mean_m->setText(o.str().c_str());

        // std
        o.str("");
        o << stddev(fixAllM.col(2));
        ui->l_std_m->setText(o.str().c_str());

        // mean Variance
        o.str("");

        mat variances = fixAllM.col(FIXCOL_RMS);
        uvec aboveOrEqualToZeroRMS = arma::find(variances > 0);
        variances = variances.elem(aboveOrEqualToZeroRMS);

        o << mean(mean(variances));
        ui->l_variance_m->setText(o.str().c_str());

    }

    if (autoFixAllM.n_rows > 0){    // Automatic statistics
        //N
        o.str("");
        o << autoFixAllM.n_rows ;
        ui->l_N_a->setText(o.str().c_str());

        // Mean
        o.str("");
        o << mean(mean(autoFixAllM.col(FIXCOL_DURATION)));
        ui->l_mean_a->setText(o.str().c_str());

        // std
        o.str("");
        o << stddev(autoFixAllM.col(FIXCOL_DURATION));
        ui->l_std_a->setText(o.str().c_str());

        // mean Variance
        o.str("");
        o << mean(mean(autoFixAllM.col(FIXCOL_RMS)));
        ui->l_variance_a->setText(o.str().c_str());
    }

}

