#include "DialogHeatMap.h"
#include "ui_DialogHeatMap.h"
#include <fstream>

DialogHeatMap::DialogHeatMap(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogHeatMap)
{
    ui->setupUi(this);

    connect( ui->b_heatmap, SIGNAL( clicked() ), this, SLOT( fncPress_bCalculateHeatMap() ) );
    connect( ui->b_cancel, SIGNAL( clicked() ), this, SLOT( fncPress_bCancel() ) );
    connect( ui->b_nextSegment, SIGNAL( clicked() ), this, SLOT( fncPress_bNextSegment() ) );
    connect( ui->b_prevSegment, SIGNAL( clicked() ), this, SLOT( fncPress_bPrevSegment() ) );
    connect( ui->b_inputImage, SIGNAL( clicked() ), this, SLOT( fncPress_bInputImage() ) );

}

DialogHeatMap::~DialogHeatMap()
{
    delete ui;
}

/**
  *    PUBLIC METHODS
  **/
void DialogHeatMap::loadData(GrafixParticipant* participant,mat &roughM, mat* p_fixAllM, mat* p_segmentsM)
{
    this->p_fixAllM = p_fixAllM;
    this->SegmentsM = (*p_segmentsM);
    this->currentSegment = 1;
    this->_participant = participant;

    if (SegmentsM.n_rows == 0)
    {
        //create temp segments files
        SegmentsM.zeros(1,3);
        SegmentsM(0,0) = 0;
        SegmentsM(0,1) = 0;
        SegmentsM(0,2) = roughM.n_rows-1;
    }

    // order the segments
    uvec indices = sort_index((SegmentsM).cols(1,1));
    (SegmentsM) = (SegmentsM).rows(indices);
    this->showMaximized();
    loadImages();
    fncUpdateSegment();
    paintBackgroundImage();
}

/**
  *    PRIVATE METHODS
  **/

void DialogHeatMap::fncUpdateSegment()
{
    ui->lSegment->setText(QString::number(currentSegment) + " / " + QString::number(SegmentsM.n_rows));
}


void DialogHeatMap::paintBlankPanel(){
    // Paint the background white
    QPixmap pixmap(ui->lPanelBackground->width(),ui->lPanelBackground->height());
    pixmap.fill(Qt::white);

    ui->l_selectImage->setText("Select image...");

    ui->lPanelBackground->setPixmap(pixmap);
}

void DialogHeatMap::paintBackgroundImage(){
    deleteHeatMap();

    int number;
    number = (int)(SegmentsM)(currentSegment-1,0);

    // Find the correct id
    int index = -1;
    for (uword i = 0; i < pathsImages.size(); ++ i ){
        if (pathsImages.at(i).first == number){
            index = i;
            break;
        }
    }

    // If the id exists, paint the background!
    if (index != -1){
        QString pathImg = QString::fromStdString(pathsImages[index].second);
        //pathImg = pathImg.mid(1,pathImg.length());

        if (pathImg.length() > 33){
            ui->l_selectImage->setText((pathImg.mid(0,20) + "..." + pathImg.mid(pathImg.length()-15, pathImg.length())));
        }else{
            ui->l_selectImage->setText(pathImg);
        }


        QPixmap pixmap1(pathImg, 0, Qt::AutoColor);
        QSize size(ui->lPanelBackground->width() , ui->lPanelBackground->height());
        QPixmap pixmapBackground(pixmap1.scaled(size));
        ui->lPanelBackground->setPixmap(pixmapBackground);
    }else{
        paintBlankPanel();
    }

}


void DialogHeatMap::saveCurrentImage(int id, QString pathImg )
{
    pair<int,string> p;
    p.first = id;
    p.second = pathImg.toStdString();
    pathsImages[currentSegment-1] = p;

    ofstream myfile;
    string path = _participant->GetFullDirectory().toStdString() + "/pathImages.csv";
    myfile.open (path.c_str());

    for (uword i=0; i<pathsImages.size(); i++)
    {
        myfile << pathsImages.at(i).first << "," << pathsImages.at(i).second << endl;
    }
    myfile.close();
}

void DialogHeatMap::loadImages(){


    ifstream file ( (_participant->GetFullDirectory().toStdString() + "/pathImages.csv").c_str() ); // declare file stream: http://www.cplusplus.com/reference/iostream/ifstream/
    string line;
    while ( file.good() )
    {
        pair<int,string> p;
        getline ( file, line, ',' );
        p.first = atoi(line.c_str()); // read a string until next comma: http://www.cplusplus.com/reference/string/getline/
        getline ( file, line );
        p.second = line;
        pathsImages.push_back(p);
    }

    file.close();



    if (pathsImages.empty() || pathsImages.size() != (SegmentsM).n_rows ){

        //create empty one
        qDebug() << "Creating new one\n";
        for (uword i=0;i<(SegmentsM).n_rows;i++)
        {
            pair<int,string> p(-1,"-1");
            pathsImages.push_back(p);
        }
    }
}

void DialogHeatMap::paintHeatMap(mat* p_fixations){
    QPixmap map(ui->lPanel_heatmap->width(),ui->lPanel_heatmap->height());
    map.fill(Qt::transparent);
    QPainter painter(&map);
    painter.setRenderHint(QPainter::HighQualityAntialiasing);

    // Create Intensity map
    for(int j = 1, c = (*p_fixations).n_rows; j < c; ++j) {
        double dm =  (*p_fixations)(j,2) * 150 / 500; // Vary the diameter it has to be a number from 30 to 150
        //int alpha = dm;
        if (dm < 0){
            dm = 0;
            //alpha = 0;
        }else if(dm > 255){
            dm = 150;
            //alpha = 255;
        }else if(dm >150){
            dm = 150;
        }

        QPen g_pen(QColor(0, 0, 0, 0));
        g_pen.setWidth(0);
        QRadialGradient grad((*p_fixations)(j,3) *ui->lPanel_heatmap->width() , (*p_fixations)(j,4)*ui->lPanel_heatmap->height(), dm/2); // Create Gradient
        grad.setColorAt(0, QColor(255, 0, 0, dm)); // Black, varying alpha
        grad.setColorAt(1, QColor(0, 0, 0, 0)); // Black, completely transparent
        QBrush g_brush(grad); // Gradient QBrush
        painter.setPen(g_pen);
        painter.setBrush(g_brush);
        painter.drawEllipse((*p_fixations)(j,3) *ui->lPanel_heatmap->width() -dm/2,  (*p_fixations)(j,4)*ui->lPanel_heatmap->height()-dm/2, dm, dm); // Draw circle

    }

    for(int j = 1, c = (*p_fixations).n_rows; j < c; ++j) { // Paint the circles for each fixation
        QPen g_pen(QColor(255, 255, 255, 255));
        g_pen.setWidth(0);
        painter.setPen(g_pen);
        painter.drawEllipse((*p_fixations)(j,3) *ui->lPanel_heatmap->width() -10,  (*p_fixations)(j,4)*ui->lPanel_heatmap->height()-10, 20, 20); // Draw circle

    }

   // QPixmap pixmap1(map);
    ui->lPanel_heatmap->setPixmap(map);
    painter.end();

}

void DialogHeatMap::deleteHeatMap(){
    QPixmap map(ui->lPanel_heatmap->width(),ui->lPanel_heatmap->height());
    map.fill(Qt::transparent);
    ui->lPanel_heatmap->setPixmap(map);
}

/**
  *     PUBLIC SLOTS
  **/

void DialogHeatMap::fncPress_bCalculateHeatMap(){
    uvec fixIndex =  arma::find((*p_fixAllM).col(0) <= (SegmentsM)(currentSegment-1,2));
    mat aux = (*p_fixAllM).rows(fixIndex);
    fixIndex =  arma::find(aux.col(0) >= (SegmentsM)(currentSegment-1,1));
    aux = aux.rows(fixIndex);
    paintHeatMap(&aux);
}

void DialogHeatMap::fncPress_bCancel(){
    close();
}

void DialogHeatMap::fncPress_bInputImage(){
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),tr("Files (*.*)"));
    if (!fileName.isEmpty()){
        ui->l_selectImage->setText(fileName);
        saveCurrentImage((SegmentsM)(currentSegment-1,0), fileName);
        paintBackgroundImage();
    }
}


void DialogHeatMap::fncPress_bNextSegment(){
    if (currentSegment != (int)(SegmentsM).n_rows){

        currentSegment = currentSegment + 1;

        fncUpdateSegment();
        paintBackgroundImage();
        deleteHeatMap();
    }
}

void DialogHeatMap::fncPress_bPrevSegment(){
    if (currentSegment != 1){

        currentSegment = currentSegment - 1;

        fncUpdateSegment();
        paintBackgroundImage();
        deleteHeatMap();
    }

}

