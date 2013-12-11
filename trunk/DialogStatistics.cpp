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
    GPMatrixFunctions::fncReturnFixationinSegments( &fixAllM, p_experimentalSegmentsM);
    GPMatrixFunctions::fncReturnFixationinSegments( &autoFixAllM, p_experimentalSegmentsM);;

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
        o << mean(mean(fixAllM.col(5)));
        ui->l_variance_m->setText(o.str().c_str());

    }

    if (autoFixAllM.n_rows > 0){    // Automatic statistics
        //N
        o.str("");
        o << autoFixAllM.n_rows ;
        ui->l_N_a->setText(o.str().c_str());

        // Mean
        o.str("");
        o << mean(mean(autoFixAllM.col(2)));
        ui->l_mean_a->setText(o.str().c_str());

        // std
        o.str("");
        o << stddev(autoFixAllM.col(2));
        ui->l_std_a->setText(o.str().c_str());

        // mean Variance
        o.str("");
        o << mean(mean(autoFixAllM.col(5)));
        ui->l_variance_a->setText(o.str().c_str());
    }

   /** if (fixAllM.n_rows > 0 && autoFixAllM.n_rows > 0){  // Comparison between automatic/manual

        // HOw many fixations were correctly detected?
        int numberCorrect = 0;
        for (int i = 0; i < autoFixAllM.n_rows; ++i){

            fixIndex =  arma::find(fixAllM.col(0) == autoFixAllM(i,0));
            if (!fixIndex.is_empty()){
                aux = fixAllM.rows(fixIndex);
                fixIndex =  arma::find(fixAllM.col(1) == autoFixAllM(i,1));
                if (!fixIndex.is_empty()){
                    numberCorrect = numberCorrect + 1;
                }

            }

        }

        o.str("");
        o << numberCorrect;
        ui->l_N_correctFix->setText(o.str().c_str());

        o.str("");
        o << numberCorrect/autoFixAllM.n_rows * 100 ;
        ui->l_percent_correctFix->setText(o.str().c_str());



    } **/

}

