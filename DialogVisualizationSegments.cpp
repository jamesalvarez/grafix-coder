#include "DialogVisualizationSegments.h"
#include "ui_DialogVisualizationSegments.h"



DialogVisualizationSegments::DialogVisualizationSegments(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogVisualizationSegments)
{
    ui->setupUi(this);

    connect( ui->b_play, SIGNAL( clicked() ), this, SLOT( fncPress_bPlay() ) );
    connect( ui->b_cancel, SIGNAL( clicked() ), this, SLOT( fncPress_bCancel() ) );
    connect( ui->b_nextFragment, SIGNAL( clicked() ), this, SLOT( fncPress_bNextFragment() ) );
    connect( ui->b_prevFragment, SIGNAL( clicked() ), this, SLOT( fncPress_bPrevFragment() ) );
    connect( ui->b_nextSegment, SIGNAL( clicked() ), this, SLOT( fncPress_bNextSegment() ) );
    connect( ui->b_prevSegment, SIGNAL( clicked() ), this, SLOT( fncPress_bPrevSegment() ) );
    connect( ui->b_inputImage, SIGNAL( clicked() ), this, SLOT( fncPress_bInputImage() ) );

    connect(ui->cbFixations, SIGNAL(clicked()), this, SLOT(updateVisualizationSettings()));
    connect(ui->cbPupilDilation, SIGNAL(clicked()), this, SLOT(updateVisualizationSettings()));
    connect(ui->rad_rough, SIGNAL(clicked()), this, SLOT(updateVisualizationSettings()));
    connect(ui->rad_smooth, SIGNAL(clicked()), this, SLOT(updateVisualizationSettings()));

    ui->rad_rough->setChecked(true);
    playOnOff = 0; // Off
}

DialogVisualizationSegments::~DialogVisualizationSegments()
{
    playOnOff = 0;
    delete ui;

    if (_visualizationDrawer) {
        delete _visualizationDrawer;
    }
}


/**
  *    PUBLIC METHODS
  **/
void DialogVisualizationSegments::loadData(GrafixParticipant *participant, mat roughM, mat smoothM, mat fixAllM, mat segmentsM){

    this->_participant = participant;
    this->roughM = roughM;
    this->smoothM = smoothM;
    this->fixAllM = fixAllM;

    this->currentFragment = 1;
    this->currentSegment = 1;
    this->secsFragment = this->_participant->GetProject()->GetProjectSetting(Consts::SETTING_SECS_FRAGMENT, Consts::ACTIVE_CONFIGURATION()).toInt();
    this->hz = this->_participant->GetProject()->GetProjectSetting(Consts::SETTING_HZ, Consts::ACTIVE_CONFIGURATION()).toInt();
    this->expWidth = this->_participant->GetProject()->GetProjectSetting(Consts::SETTING_EXP_WIDTH, Consts::ACTIVE_CONFIGURATION()).toInt();
    this->expHeight = this->_participant->GetProject()->GetProjectSetting(Consts::SETTING_EXP_HEIGHT, Consts::ACTIVE_CONFIGURATION()).toInt();

    if (segmentsM.n_rows == 0) {
        //create temp segments files
        segmentsM.zeros(1,3);
        segmentsM(0,0) = 0;
        segmentsM(0,1) = 0;
        segmentsM(0,2) = roughM.n_rows-1;
    }

    this->nFragments = floor((segmentsM(currentSegment-1,2) - segmentsM(currentSegment-1,1))/ ( secsFragment * hz)) + 1 ;
    // order the segments
    uvec indices = sort_index(segmentsM.cols(1,1));
    this->segmentsM = segmentsM.rows(indices);




    std::ostringstream o;
    o << secsFragment << "s";
    ui->lsecsSegment->setText(o.str().c_str());


    _visualizationDrawer = new VisualizationDrawer(this->roughM, this->smoothM, this->fixAllM, this->segmentsM, participant);

    updateVisualizationSettings();
    updateVisualizationGeometry();
    paintFrame(0);

    loadImages();
    fncUpdateFragment();
    fncUpdateSegment();
    paintBackgroundImage();

}


/**
  *    PRIVATE METHODS
  **/



void DialogVisualizationSegments::paintBlankPanel(){
    // Paint the background white
    QPixmap pixmap(ui->lPanelBackground->width(),ui->lPanelBackground->height());
    pixmap.fill(Qt::white);

    ui->l_selectImage->setText("Select image...");

    ui->lPanelBackground->setPixmap(pixmap);
}

void DialogVisualizationSegments::paintBackgroundImage(){

    int number =  segmentsM(currentSegment-1,0);

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
        std::string pathImg = pathsImages.at(index).second;
        pathImg = pathImg.substr(1,pathImg.length());

        if (pathImg.length() > 33){
            ui->l_selectImage->setText((pathImg.substr(0,20) + "..." + pathImg.substr(pathImg.length()-15, pathImg.length())).c_str());
        }else{
            ui->l_selectImage->setText(pathImg.c_str());
        }


        QPixmap pixmap1(pathImg.c_str(), 0, Qt::AutoColor);
        QSize size(ui->lPanelBackground->width() , ui->lPanelBackground->height());
        QPixmap pixmapBackground(pixmap1.scaled(size));
        ui->lPanelBackground->setPixmap(pixmapBackground);
    }else{
        paintBlankPanel();
    }

}

void DialogVisualizationSegments::saveCurrentImage(int id, QString pathImg )
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

void DialogVisualizationSegments::loadImages(){
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



    if (pathsImages.empty() || pathsImages.size() != segmentsM.n_rows ){

        //create empty one
        qDebug() << "Creating new one\n";
        for (uword i=0;i<segmentsM.n_rows;i++)
        {
            pair<int,string> p(-1,"-1");
            pathsImages.push_back(p);
        }
    }
}


void DialogVisualizationSegments::playVisualization(){

    startIndex = (segmentsM(currentSegment-1,1) ) + ((currentFragment-1) * secsFragment * hz );
    stopIndex =  (segmentsM(currentSegment-1,2) );
    if (stopIndex > roughM.n_rows)
        stopIndex = roughM.n_rows;

    int firstIndexSeg = 0; // Just to figure out which is the fixation id
    if (fixAllM.n_rows > 0){
        uvec fixIndex =  arma::find(fixAllM.col(1) >= startIndex);
        if (!fixIndex.is_empty()){
            firstIndexSeg = fixIndex(0);
        }

    }


    double velocity =  ((double)1/hz) * 1000;   // velocity in miliseconds


    int startTime;

    for (uword i = startIndex; i < stopIndex  ; ++i) {
        if (playOnOff == 0)
            break;


        paintFrame(i);

        qApp->processEvents();

        // Timer !
        startTime = getMilliCount();
        while (getMilliSpan(startTime) < velocity){
        }

    }

}

void DialogVisualizationSegments::paintFrame(uword frame) {
    _visualizationDrawer->paintCurrentFrame(frame);
    ui->lPanelFixations->setPixmap(_visualizationDrawer->fixationsPixmap);
    //ui->lPanelXY->setPixmap(_visualizationDrawer->timeLinePixmap);
    //ui->lPanelVisualization->setPixmap(_visualizationDrawer->visualizationPixmap);
    //ui->lPanelXY_2->setPixmap(_visualizationDrawer->processLinePixmap);

    ui->lPanelFixations->repaint();
    //ui->lPanelXY->repaint();
    //ui->lPanelVisualization->repaint();
    //ui->lPanelXY_2->repaint();
}


int DialogVisualizationSegments::getMilliCount(){
    timeb tb;
    ftime(&tb);
    int nCount = tb.millitm + (tb.time & 0xfffff) * 1000;
    return nCount;
}

int DialogVisualizationSegments::getMilliSpan(int nTimeStart){
    int nSpan = getMilliCount() - nTimeStart;
    if(nSpan < 0)
        nSpan += 0x100000 * 1000;
    return nSpan;
}


/**
  *     PUBLIC SLOTS
  **/

void DialogVisualizationSegments::fncPress_bPlay(){

    if (playOnOff == 0){
        playOnOff = 1; // Playing gaze
        ui->b_play->setText("Stop");
        playVisualization();
        playOnOff = 0; // Stop playing gaze
        ui->b_play->setText("Play");
    }else{
        playOnOff = 0;
        ui->b_play->setText("Play");
    }
}

void DialogVisualizationSegments::updateVisualizationSettings() {

    _visualizationDrawer->setSetting(VIS_PLAY_SMOOTH, ui->rad_smooth->isChecked());
    _visualizationDrawer->setSetting(VIS_PAINT_FIXATION_NUMBERS, ui->cbFixations->isChecked());
    _visualizationDrawer->setSetting(VIS_PAINT_PUPIL_DILATION, ui->cbPupilDilation->isChecked());

}

void DialogVisualizationSegments::updateVisualizationGeometry() {

    _visualizationDrawer->updateTimeLineFrame(ui->lPanelXY->width(), ui->lPanelXY->height());
    _visualizationDrawer->updateVisualizationFrame(ui->lPanelVisualization->width(), ui->lPanelVisualization->height());
    _visualizationDrawer->updateFixationFrame(ui->lPanelFixations->width(), ui->lPanelFixations->height());
}

void DialogVisualizationSegments::fncUpdateFragment(){
    std::ostringstream o;
    o << currentFragment << " / " << nFragments;
    ui->lFragment->setText( o.str().c_str());
}

void DialogVisualizationSegments::fncUpdateSegment(){
    std::ostringstream o;
    o << currentSegment << " / " << segmentsM.n_rows;
    ui->lSegment->setText( o.str().c_str());
}

// Methods to close the window and stop the current processes.
void DialogVisualizationSegments::fncPress_bCancel(){
    playOnOff = 0;
    close();
}

void DialogVisualizationSegments::fncPress_bInputImage(){
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),tr("Files (*.*)"));
    if (!fileName.isEmpty()){
        ui->l_selectImage->setText(fileName);
        saveCurrentImage(segmentsM(currentSegment-1,0), fileName);
        paintBackgroundImage();
    }

}

void DialogVisualizationSegments::fncPress_bNextFragment(){
    if (currentFragment != nFragments){

        currentFragment = currentFragment + 1;

        if (playOnOff == 1)
            fncPress_bPlay();
        fncUpdateFragment();
    }

}

void DialogVisualizationSegments::fncPress_bPrevFragment(){
    if (currentFragment != 1){

        currentFragment = currentFragment - 1;

        if (playOnOff == 1)
            fncPress_bPlay();

        fncUpdateFragment();
    }

}

void DialogVisualizationSegments::fncPress_bNextSegment(){
    if (currentSegment != (int)segmentsM.n_rows){

        currentSegment = currentSegment + 1;
        currentFragment = 1;
        nFragments = floor((segmentsM(currentSegment-1,2) - segmentsM(currentSegment-1,1))/ ( secsFragment * hz)) + 1 ;


        if (playOnOff == 1)
            fncPress_bPlay();

        fncUpdateFragment();
        fncUpdateSegment();

        paintBackgroundImage();
    }
}

void DialogVisualizationSegments::fncPress_bPrevSegment(){
    if (currentSegment != 1){

        currentSegment = currentSegment - 1;
        currentFragment = 1;
        nFragments = floor((segmentsM(currentSegment-1,2) - segmentsM(currentSegment-1,1))/ ( secsFragment * hz)) + 1 ;


        if (playOnOff == 1)
            fncPress_bPlay();

        fncUpdateFragment();
        fncUpdateSegment();

        paintBackgroundImage();
    }

}

void DialogVisualizationSegments::closeEvent(QCloseEvent *){
    playOnOff = 0;
    close();
}

