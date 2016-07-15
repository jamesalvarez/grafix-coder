#include "DialogPlot.h"
#include "ui_DialogPlot.h"

DialogPlot::DialogPlot(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogPlot)
{
    ui->setupUi(this);
}

DialogPlot::~DialogPlot()
{
    delete ui;
}

void DialogPlot::loadSpikeData(const GrafixParticipant *p)
{
    p_participant = p;
    GPMatrixFunctions::readFileSafe(_fixAllM,p_participant->GetMatrixPath(Consts::MATRIX_FIXALL));
    mat _dataM;
    GPMatrixFunctions::readFileSafe(_dataM,p_participant->GetMatrixPath(Consts::MATRIX_SMOOTH));

    int n_fixations = (int)_fixAllM.n_rows;

    mat ts = zeros(n_fixations,1);
    for (uword i = 0; i<_fixAllM.n_rows; ++i)
    {
        mat velocities = zeros(_fixAllM.at(i,1)-_fixAllM.at(i,0)-1,1);
        double average_vel = 0;
        int counter = 0;
        for (uword i2 = 1+_fixAllM.at(i,0); i2 < _fixAllM.at(i,1); ++i2)
        {
            double x_1back = _dataM(i2-1,2);
            double x_cur   = _dataM(i2,2);
            double y_1back = _dataM(i2-1,3);
            double y_cur   = _dataM(i2,3);

            double curAmp = sqrt((double)(((x_1back - x_cur)*(x_1back - x_cur)) + ((y_1back - y_cur)*(y_1back - y_cur)))/2);
            average_vel += curAmp;
            velocities.at(counter,0) = curAmp;
            counter++;
        }

        average_vel /= velocities.size();

        double E=0;
        for(uword i3=0;i3<velocities.size();i3++)
               E+=pow((velocities[i3] - average_vel),2);
        double std =  sqrt(1/velocities.size()*E);

        double max = velocities.max();

        ts.at(i,0) = (average_vel - max) / (std / (sqrt(velocities.size())));
        //if (ts.at(i, 0)
    }
    //mat ts2 = sort(ts,1);
    QVector<double> x(n_fixations), y(n_fixations);
    for (int i=0; i<n_fixations; ++i)
    {
      x[i] = i+1; // x goes from -1 to 1
      y[i] = ts.at(i,0); // let's plot a quadratic function
    }

    double max = ts.max();

    // create graph and assign data to it:
    ui->customPlot->addGraph();
    ui->customPlot->graph(0)->setData(x, y);
    // give the axes some labels:
    ui->customPlot->xAxis->setLabel("fixation");
    ui->customPlot->yAxis->setLabel("T of max velocity");
    // set axes ranges, so we see all data:
    ui->customPlot->xAxis->setRange(1, n_fixations);
    ui->customPlot->yAxis->setRange(0, max);
    ui->customPlot->replot();
}

void DialogPlot::loadRMSData(const GrafixParticipant *p)
{
    p_participant = p;
    GPMatrixFunctions::readFileSafe(_fixAllM,p_participant->GetMatrixPath(Consts::MATRIX_FIXALL));

    int n_fixations = (int)_fixAllM.n_rows;

    mat RMS = sort(_fixAllM.col(5),1);

    // generate some data:
    QVector<double> x(n_fixations), y(n_fixations); //
    for (int i=1; i<=n_fixations; ++i)
    {
      x[i-1] = i; // x goes from -1 to 1
      y[i-1] = RMS.at(i-1,0); // let's plot a quadratic function
    }

    double max = RMS.max();

    // create graph and assign data to it:
    ui->customPlot->addGraph();
    ui->customPlot->graph(0)->setData(x, y);
    // give the axes some labels:
    ui->customPlot->xAxis->setLabel("fixation");
    ui->customPlot->yAxis->setLabel("RMS");
    // set axes ranges, so we see all data:
    ui->customPlot->xAxis->setRange(1, n_fixations);
    ui->customPlot->yAxis->setRange(0, max);
    ui->customPlot->replot();
}

void DialogPlot::loadDurData(const GrafixParticipant *p)
{
    p_participant = p;
    GPMatrixFunctions::readFileSafe(_fixAllM,p_participant->GetMatrixPath(Consts::MATRIX_FIXALL));

    int n_fixations = (int)_fixAllM.n_rows;

    mat durations = sort(_fixAllM.col(2),1);

    int sub200index = 0;
    for (int i=0; i<n_fixations; ++i)
    {
        //search for sub 200ms
        if (durations.at(i,0) <=0.5)
        {
            sub200index = i;
            break;
        }
    }
    // generate some data:

    int n_sub200_fixations = n_fixations - sub200index;

    QVector<double> x(n_sub200_fixations), y(n_sub200_fixations); //
    for (int i=sub200index; i<n_fixations; ++i)
    {
      x[i-sub200index] = 1+i-sub200index; // x goes from -1 to 1
      y[i-sub200index] = durations.at(i,0); // let's plot a quadratic function
    }

    double max = y[0];

    // create graph and assign data to it:
    ui->customPlot->addGraph();
    ui->customPlot->graph(0)->setData(x, y);
    // give the axes some labels:
    ui->customPlot->xAxis->setLabel("Fixation");
    ui->customPlot->yAxis->setLabel("Duration");
    // set axes ranges, so we see all data:
    ui->customPlot->xAxis->setRange(1,n_sub200_fixations);
    ui->customPlot->yAxis->setRange(0, max);
    ui->customPlot->replot();
}

void DialogPlot::loadVelData(const GrafixParticipant *p)
{
    p_participant = p;
    GPMatrixFunctions::readFileSafe(_fixAllM,p_participant->GetMatrixPath(Consts::MATRIX_SMOOTH));

    uword n_fixations = _fixAllM.n_rows;



    // oldAmp=sqrt(((XTwoBack-XOneBack)^2+(YTwoBack-YOneBack)^2)/2);
    mat velocities = zeros(n_fixations-1,1);

    for (uword i = 1;i<n_fixations;++i)
    {
        double x_1back = _fixAllM(i-1,2);
        double x_cur   = _fixAllM(i,2);
        double y_1back = _fixAllM(i-1,3);
        double y_cur   = _fixAllM(i,3);

        double curAmp = sqrt((double)(((x_1back - x_cur)*(x_1back - x_cur)) + ((y_1back - y_cur)*(y_1back - y_cur)))/2);
        velocities.at(i-1,0) = curAmp;
    }

    velocities = sort(velocities,1);


    // generate some data:
    QVector<double> x(n_fixations-1), y(n_fixations-1); //
    for (uword i=0; i<n_fixations-1; ++i)
    {
      x[i] = 1+i; // x goes from -1 to 1
      y[i] = velocities.at(i,0); // let's plot a quadratic function
    }

    double max = y[0];
    ui->customPlot->setInteraction(QCP::iRangeDrag, true);
    ui->customPlot->setInteraction(QCP::iRangeZoom , true);
    QCPAxisRect qar(ui->customPlot);
    qar.setRangeDrag(Qt::Horizontal);
    // create graph and assign data to it:
    ui->customPlot->addGraph();
    ui->customPlot->graph(0)->setData(x, y);
    // give the axes some labels:
    ui->customPlot->xAxis->setLabel("Fixation");
    ui->customPlot->yAxis->setLabel("Velocity");
    // set axes ranges, so we see all data:
    ui->customPlot->xAxis->setRange(1,n_fixations-1);
    ui->customPlot->yAxis->setRange(0, max);
    ui->customPlot->replot();
}
