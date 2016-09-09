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

    connect(ui->rad_rough,SIGNAL(clicked(bool)),this,SLOT(fncChange_Visualization(bool)));
    connect(ui->rad_smooth,SIGNAL(clicked(bool)),this,SLOT(fncChange_Visualization(bool)));

    ui->rad_rough->setChecked(true);
    playOnOff = 0; // Off
}

DialogVisualizationSegments::~DialogVisualizationSegments()
{
    playOnOff = 0;
    delete ui;
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

    if (segmentsM.n_rows == 0)
    {
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

    loadImages();
    paintXYAxes();
    paintFixations();
    fncUpdateFragment();
    fncUpdateSegment();
    paintBackgroundImage();

}


/**
  *    PRIVATE METHODS
  **/

void DialogVisualizationSegments::paintXYAxes(){
    if (ui->rad_rough->isChecked()){ // Paint the rough data
        paintRoughXY();
    }else{   // Paint the smooth data
        paintSmoothXY();
    }

}

void DialogVisualizationSegments::paintBlankPanel(){
    // Paint the background white
    QPixmap pixmap(ui->lPanelBackground->width(),ui->lPanelBackground->height());
    pixmap.fill(Qt::white);

    ui->l_selectImage->setText("Select image...");

    ui->lPanelBackground->setPixmap(pixmap);
}

void DialogVisualizationSegments::paintBackgroundImage(){

    int number;
    number =  segmentsM(currentSegment-1,0);

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

void DialogVisualizationSegments::playRough(){

    QPixmap pixmap(ui->lPanelVisualization->width(),ui->lPanelVisualization->height());
    QPixmap pixmapProcessLine(ui->lPanelXY_2->width(),ui->lPanelXY_2->height());

    QPen myPen(Qt::red, 15, Qt::SolidLine);

    uword startIndex = (segmentsM(currentSegment-1,1) ) + ((currentFragment-1) * secsFragment * hz );
    uword stopIndex =  (segmentsM(currentSegment-1,2) );
    if (stopIndex > roughM.n_rows)
        stopIndex = roughM.n_rows;
    double increment = (double) secsFragment * hz / ui->lPanelXY->width();

    int firstIndexSeg = 0; // Just to figure out which is the first fixation id
    if (fixAllM.n_rows > 0){
        uvec fixIndex =  arma::find(fixAllM.col(FIXCOL_END) >= startIndex);
        if (!fixIndex.is_empty()){
            firstIndexSeg = fixIndex(0);
        }

    }

    myPen.setColor(QColor(255, 0, 0, 255));
    myPen.setCapStyle(Qt::RoundCap);

    double velocity =  ((double)1/hz) * 1000;   // velocity in miliseconds

    std::stringstream number;
    int startTime;
    mat aux;
    for (uword i = startIndex; i < stopIndex; ++i) {
        if (playOnOff == 0)
            break;

        //switch to next fragment if playing through
        if ((i - startIndex) >= (secsFragment * hz))
        {
            if (currentFragment != nFragments)
            {
                currentFragment = currentFragment + 1;

                startIndex = (segmentsM(currentSegment-1,1) ) + ((currentFragment-1) * secsFragment * hz );

                paintXYAxes();
                paintFixations();

                fncUpdateFragment();
            }
        }


        pixmapProcessLine.fill(Qt::transparent);
        QPainter painterProcessLine(&pixmapProcessLine);

        // Draw time line
        painterProcessLine.drawLine((i- startIndex )*(1/increment ),0,(i- startIndex )*(1/increment ),ui->lPanelXY_2->height());


        pixmap.fill(Qt::transparent);
         QPainter painter(&pixmap);

         // Option 1 checked: Paint fixation numbers
        if ( ui->cbFixations->isChecked()){
            uvec fixIndex =  arma::find(fixAllM.col(FIXCOL_START) <= i);
            aux = fixAllM.rows(fixIndex);
            fixIndex =  arma::find(aux.col(1) >= i);

            if (fixIndex.n_rows != 0){   // If there is a fixation between the values, paint it!
                number.str("");
                number << fixIndex(0) - firstIndexSeg;
                myPen.setColor(QColor(0, 100, 29, 255));

                QFont font=painter.font() ;
                font.setPointSize ( 85 );
                painter.setFont(font);

                painter.setPen(myPen);
                painter.drawText( QPoint( ui->lPanelVisualization->width()/2, ui->lPanelVisualization->height()/2 ), number.str().c_str() );

            }
        }

        // Option 2 checked : Paint pupil dilation
        if (ui->cbPupilDilation->isChecked() && roughM.n_cols == 8){
            if (roughM(i ,6) > 0){
                myPen.setWidth(roughM(i ,6) * 10);
            }
        }

        myPen.setColor(QColor(255, 0, 0, 255));
        painter.setPen(myPen);
        painter.drawPoint( roughM(i ,2 )  * ui->lPanelVisualization->width(), roughM(i ,3 )  * ui->lPanelVisualization->height()); // XL

        // Option 2 checked : Paint pupil dilation
        if (ui->cbPupilDilation->isChecked()  && roughM.n_cols == 8){
            if (roughM(i ,7) > 0){
                myPen.setWidth(roughM(i ,7) * 10);
            }
        }

        myPen.setColor(QColor(0, 50, 128, 255));
        painter.setPen(myPen);
        painter.drawPoint( roughM(i ,4 )  * ui->lPanelVisualization->width(), roughM(i ,5 )  * ui->lPanelVisualization->height()); // XL


        ui->lPanelXY_2->setPixmap(pixmapProcessLine);
        ui->lPanelVisualization->setPixmap(pixmap);
        ui->lPanelVisualization->repaint();

        painterProcessLine.end();
        painter.end();

        qApp->processEvents();

        // Timer !
        startTime = getMilliCount();
        while (getMilliSpan(startTime) < velocity){
        }
    }

    ui->lPanelXY_2->setPixmap(pixmapProcessLine);
    ui->lPanelVisualization->setPixmap(pixmap);
    ui->lPanelVisualization->repaint();



}
void DialogVisualizationSegments::playSmooth(){
    QPixmap pixmap(ui->lPanelVisualization->width(),ui->lPanelVisualization->height());
    QPixmap pixmapProcessLine(ui->lPanelXY_2->width(),ui->lPanelXY_2->height());

    QPen myPen(Qt::red, 15, Qt::SolidLine);

    uword startIndex = (segmentsM(currentSegment-1,1) ) + ((currentFragment-1) * secsFragment * hz );
    uword stopIndex =  (segmentsM(currentSegment-1,2) );
    if (stopIndex > roughM.n_rows)
        stopIndex = roughM.n_rows;
    double increment = (double) secsFragment * hz / ui->lPanelXY->width();

    int firstIndexSeg = 0; // Just to figure out which is the fixation id
    if (fixAllM.n_rows > 0){
        uvec fixIndex =  arma::find(fixAllM.col(1) >= startIndex);
        if (!fixIndex.is_empty()){
            firstIndexSeg = fixIndex(0);
        }

    }

    myPen.setColor(QColor(255, 0, 0, 255));
    myPen.setCapStyle(Qt::RoundCap);

    double velocity =  ((double)1/hz) * 1000;   // velocity in miliseconds

    std::stringstream number;
    int startTime;
    mat aux;
    for (uword i = startIndex; i < stopIndex  ; ++i) {
        if (playOnOff == 0)
            break;

        pixmapProcessLine.fill(Qt::transparent);
        QPainter painterProcessLine(&pixmapProcessLine);

        // Draw time line
        painterProcessLine.drawLine((i- startIndex )*(1/increment ),0,(i- startIndex )*(1/increment ),ui->lPanelXY_2->height());

        pixmap.fill(Qt::transparent);
        QPainter painter(&pixmap);

        // Option 1 checked : Paint pupil dilation
        if (ui->cbPupilDilation->isChecked() && roughM.n_cols == 8){
            if (roughM(i,6) > 0){
                myPen.setWidth(roughM(i ,6) * 10);
            }else if (roughM(i,7 > 0)){
                myPen.setWidth(roughM(i ,7) * 10);
            }
        }


        // Draw point
        painter.setPen(myPen);
        painter.drawPoint( smoothM(i ,2 ) / expWidth * ui->lPanelVisualization->width(), smoothM(i ,3 ) / expHeight * ui->lPanelVisualization->height()); // XL

        // Option 2 paint fixation number: Paint fixations option checked:
        if ( ui->cbFixations->isChecked()){
            uvec fixIndex =  arma::find(fixAllM.col(FIXCOL_START) <= i);
            aux = fixAllM.rows(fixIndex);
            fixIndex =  arma::find(aux.col(1) >= i);
            if (fixIndex.n_rows != 0){   // If there is a fixation between the values, paint it!
                number.str("");
                number << fixIndex(0) - firstIndexSeg;

                QFont font=painter.font() ;
                font.setPointSize ( 35 );
                painter.setFont(font);
                painter.drawText( QPoint( (smoothM(i ,2 ) / expWidth * ui->lPanelVisualization->width())+20, smoothM(i ,3 ) / expHeight * ui->lPanelVisualization->height()), number.str().c_str() );

            }
        }

        ui->lPanelXY_2->setPixmap(pixmapProcessLine);
        ui->lPanelVisualization->setPixmap(pixmap);
        ui->lPanelVisualization->repaint();

        painterProcessLine.end();
        painter.end();

        qApp->processEvents();

        // Timer !
        startTime = getMilliCount();
        while (getMilliSpan(startTime) < velocity){
        }

    }

}


void DialogVisualizationSegments::paintRoughXY(){

    double increment = (double) secsFragment * hz / ui->lPanelXY->width();

    QPixmap pixmapMissingData(ui->lPanelMissingData->width(),ui->lPanelMissingData->height());
    pixmapMissingData.fill(Qt::transparent);
    QPixmap pixmap(ui->lPanelXY->width(),ui->lPanelXY->height());
    pixmap.fill(Qt::white);

    QPainter painter(&pixmap);
    QPainter painterMissingData(&pixmapMissingData);
    QPen myPen(Qt::red, 1.2, Qt::SolidLine);

    uword startIndex = (segmentsM(currentSegment-1,1) ) + ((currentFragment-1) * secsFragment * hz );
    uword stopIndex =  (segmentsM(currentSegment-1,2) );
    if (stopIndex > roughM.n_rows)
        stopIndex = roughM.n_rows;

   // also so el la capacidad del array es menor que stop index
    for (uword i = startIndex; i < stopIndex; ++i) {
        myPen.setColor(QColor(255, 0, 0, 255));

        if (roughM(i,2 ) != -1){
            painter.setPen(myPen);
            painter.drawPoint( (i- startIndex )*(1/increment ), roughM(i,2 ) * ui->lPanelXY->height()); // XL
        }else{ // Missing data
            painterMissingData.setPen(myPen);
            painterMissingData.drawPoint( (i- startIndex+1)*(1/increment ), 10); // XL
        }

        myPen.setColor(QColor(0, 0, 128, 255));
        if (roughM(i,3 ) != -1){
            painter.setPen(myPen);
            painter.drawPoint( (i- startIndex)*(1/increment ), roughM(i,3 ) * ui->lPanelXY->height()); // YL
        }else{ // Missing data
            painterMissingData.setPen(myPen);
            painterMissingData.drawPoint( (i- startIndex+1)*(1/increment ), 20); //XR
        }

       myPen.setColor(QColor(255, 50, 0, 255));
        if (roughM(i,4 ) != -1){
            painter.setPen(myPen);
            painter.drawPoint( (i- startIndex)*(1/increment ), roughM(i,4 ) * ui->lPanelXY->height()); // XR
        }else{ // Missing data
            painterMissingData.setPen(myPen);
            painterMissingData.drawPoint( (i- startIndex+1)*(1/increment ), 30); //XR
        }

        myPen.setColor(QColor(0, 50, 128, 255));
        if (roughM(i,5 ) != -1){
            painter.setPen(myPen);
            painter.drawPoint( (i- startIndex)*(1/increment ), roughM(i,5 ) * ui->lPanelXY->height()); // YR
        }else{ // Missing data
            painterMissingData.setPen(myPen);
            painterMissingData.drawPoint( (i- startIndex+1)*(1/increment ), 40); //XR
        }
    }

    ui->lPanelXY->setPixmap(pixmap);
    ui->lPanelMissingData->setPixmap(pixmapMissingData);

     painter.end();
     painterMissingData.end();

}

void DialogVisualizationSegments::paintSmoothXY(){

    QPixmap pixmap(ui->lPanelXY->width(),ui->lPanelXY->height());
    pixmap.fill(Qt::white);

    QPainter painter(&pixmap);
    QPen myPen(Qt::red, 1, Qt::SolidLine);

    uword startIndex = (segmentsM(currentSegment-1,1) ) + ((currentFragment-1) * secsFragment * hz );
    uword stopIndex =  (segmentsM(currentSegment-1,2) );
    if (stopIndex > smoothM.n_rows)
        stopIndex = smoothM.n_rows;
    double increment = (double) secsFragment * hz / ui->lPanelXY->width();

    for (uword i = startIndex; i < stopIndex; ++i) {
        myPen.setColor(QColor(255, 0, 0, 255));
        painter.setPen(myPen);
        painter.drawPoint((i-startIndex)*(1/increment ), smoothM(i ,2 ) / expWidth * ui->lPanelXY->height()); // XL

        myPen.setColor(QColor(0, 0, 128, 255));
        painter.setPen(myPen);
        painter.drawPoint((i- startIndex)*(1/increment ), smoothM(i,3 )/ expHeight * ui->lPanelXY->height()); // YL

    }

    ui->lPanelXY->setPixmap(pixmap);
     painter.end();

}

void DialogVisualizationSegments::paintFixations(){
    QPixmap pixmap(ui->lPanelFixations->width(),ui->lPanelFixations->height());
    pixmap.fill(Qt::white);

    QPainter painter(&pixmap);

    int startIndex = (segmentsM(currentSegment-1,1) ) + ((currentFragment-1) * secsFragment * hz );
    int stopIndex =  (segmentsM(currentSegment-1,2) );
    double increment = (double) secsFragment * hz / ui->lPanelXY->width();
    int counter = 0;

    QFont font=painter.font() ;
    font.setPointSize ( 8 );
    painter.setFont(font);

    std::stringstream number;
    number.str("");

    double posStart = 0;
    double posEnd = 0;

    int auxIndex = 0;
    if (fixAllM.n_rows > 0){
        uvec fixIndex =  arma::find(fixAllM.col(FIXCOL_END) >= startIndex);

        if (!fixIndex.is_empty()){
            auxIndex = fixIndex(0);
        }else{
            auxIndex = fixAllM.n_rows ;
        }

    }

    for (uword i = auxIndex; i < fixAllM.n_rows  ; i++){
        if ((fixAllM(i,FIXCOL_START) >= startIndex && fixAllM(i,FIXCOL_START) <=  stopIndex ) ||(fixAllM(i,FIXCOL_START) <= startIndex && fixAllM(i,FIXCOL_END) >= startIndex )){

            if (fixAllM(i,FIXCOL_START) <= startIndex && fixAllM(i,FIXCOL_END) >= startIndex ) // If it's a fixation that didn't end in the previous segment
                posStart = -1;
            else
                posStart = (fixAllM(i,FIXCOL_START) - startIndex ) * (1/increment);

            posEnd = ((fixAllM(i,FIXCOL_END) - startIndex ) * (1/increment)) - posStart;

            if (fixAllM(i,FIXCOL_SMOOTH_PURSUIT) == Consts::SMOOTHP_YES){
                painter.setBrush(QBrush("#ffbc00"));
            }else{
                painter.setBrush(QBrush("#1ac500"));
            }

            painter.drawRect(QRect( posStart ,25, posEnd,16));
            number.str("");
            number << counter;
            painter.drawText( QPoint(posStart,23), number.str().c_str() );

            counter ++; // Next fixation

        }else if (fixAllM(i,FIXCOL_END) >= stopIndex){
            break;
        }
    }

    ui->lPanelFixations->setPixmap(pixmap);
    painter.end();
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
        if (ui->rad_rough->isChecked()){ // Paint the rough data
            playRough();
        }else{   // Paint the smooth data

            playSmooth();
        }
        playOnOff = 0; // Stop playing gaze
        ui->b_play->setText("Play");
    }else{
        playOnOff = 0;
        ui->b_play->setText("Play");
        if (ui->rad_rough->isChecked()){ // Paint the rough data
            paintRoughXY();
        }else{   // Paint the smooth data
            paintSmoothXY();
        }

    }

}

void DialogVisualizationSegments::fncChange_Visualization(bool type){
    Q_UNUSED(type);
    playOnOff = 0;
    ui->b_play->setText("Play");
    paintXYAxes();
    paintBackgroundImage();
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
        paintXYAxes();
        paintFixations();

        fncUpdateFragment();
    }

}

void DialogVisualizationSegments::fncPress_bPrevFragment(){
    if (currentFragment != 1){

        currentFragment = currentFragment - 1;

        if (playOnOff == 1)
            fncPress_bPlay();

        paintXYAxes();
        paintFixations();

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
        paintXYAxes();
        paintFixations();

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
        paintXYAxes();
        paintFixations();

        fncUpdateFragment();
        fncUpdateSegment();

        paintBackgroundImage();
    }

}

void DialogVisualizationSegments::closeEvent(QCloseEvent *){
    playOnOff = 0;
    close();
}

