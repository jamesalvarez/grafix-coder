#include "DialogVisualization.h"
#include "ui_DialogVisualization.h"



DialogVisualization::DialogVisualization(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogVisualization)
{
    ui->setupUi(this);

    connect( ui->b_play, SIGNAL( clicked() ), this, SLOT( fncPress_bPlay() ) );
    connect( ui->b_cancel, SIGNAL( clicked() ), this, SLOT( fncPress_bCancel() ) );

    connect(ui->rad_rough,SIGNAL(clicked(bool)),this,SLOT(fncChange_Visualization(bool)));
    connect(ui->rad_smooth,SIGNAL(clicked(bool)),this,SLOT(fncChange_Visualization(bool)));
    connect(ui->rad_rough_smooth,SIGNAL(clicked(bool)),this,SLOT(fncChange_Visualization(bool)));

    ui->rad_rough->setChecked(true);
    playOnOff = 0; // Off
}


DialogVisualization::~DialogVisualization()
{
    playOnOff = 0;
    delete ui;
}

/**
  *    PUBLIC METHODS
  **/
void DialogVisualization::loadData(int currentFragment, GrafixParticipant* participant, mat &p_roughM_in, mat &p_smoothM_in, mat &p_fixAllM_in){

    this->_participant = participant;
    this->p_roughM = &p_roughM_in;
    this->p_smoothM = &p_smoothM_in;
    this->p_fixAllM = &p_fixAllM_in;
    this->currentFragment = currentFragment;
    this->secsFragment = this->_participant->GetProject()->GetProjectSetting(Consts::SETTING_SECS_FRAGMENT, Consts::ACTIVE_CONFIGURATION()).toInt();
    this->hz = this->_participant->GetProject()->GetProjectSetting(Consts::SETTING_HZ, Consts::ACTIVE_CONFIGURATION()).toInt();
    this->expWidth = this->_participant->GetProject()->GetProjectSetting(Consts::SETTING_EXP_WIDTH, Consts::ACTIVE_CONFIGURATION()).toInt();
    this->expHeight = this->_participant->GetProject()->GetProjectSetting(Consts::SETTING_EXP_HEIGHT, Consts::ACTIVE_CONFIGURATION()).toInt();

    std::ostringstream o;
    o << secsFragment << "s";
    ui->lsecsSegment->setText(o.str().c_str());

    paintXYAxes();
    paintFixations();
    paintBlankPanel();

}


/**
  *    PRIVATE METHODS
  **/

void DialogVisualization::paintXYAxes(){
    if (ui->rad_rough->isChecked()){ // Paint the rough data
        paintRoughXY();
    }else if (ui->rad_smooth->isChecked()){   // Paint the smooth data
        paintSmoothXY();
    }else{
        paintRoughAndSmooth();
    }

}

void DialogVisualization::paintBlankPanel(){
    // Paint the background white
    QPixmap pixmap(ui->lPanelVisualization->width(),ui->lPanelVisualization->height());
    pixmap.fill(Qt::white);

    ui->lPanelVisualization->setPixmap(pixmap);
}

void DialogVisualization::playRough(){
    QPixmap pixmap(ui->lPanelVisualization->width(),ui->lPanelVisualization->height());
    QPixmap pixmapProcessLine(ui->lPanelXY_2->width(),ui->lPanelXY_2->height());

    QPainter painter(&pixmap);
    pixmap.fill(Qt::white);
    QPen myPen(Qt::red, 15, Qt::SolidLine);

    uword startIndex = ((currentFragment-1) * secsFragment * hz);
    uword stopIndex =  ((currentFragment) * secsFragment * hz);
    if (stopIndex > (*p_roughM).n_rows)
        stopIndex = (*p_roughM).n_rows;
    double increment = (double) secsFragment * hz / ui->lPanelXY->width();

    int firstIndexSeg = 0; // Just to figure out which is the fixation id
    if ((*p_fixAllM).n_rows > 0){
        uvec fixIndex =  arma::find((*p_fixAllM).col(1) >= startIndex);
        if (!fixIndex.is_empty()){
            firstIndexSeg = fixIndex(0);
        }

    }

    QFont font=painter.font() ;
    font.setPointSize ( 65 );
    painter.setFont(font);

    myPen.setColor(QColor(255, 0, 0, 255));
    myPen.setCapStyle(Qt::RoundCap);
    painter.setPen(myPen);

    double velocity =  ((double)1/hz) * 1000;   // velocity in miliseconds

    std::stringstream number;
    int startTime;
    mat aux;
    for (uword i = startIndex; i < stopIndex; ++i) {
        if (playOnOff == 0)
            break;


        pixmapProcessLine.fill(Qt::transparent);
        QPainter painterProcessLine(&pixmapProcessLine);

        painterProcessLine.drawLine((i- startIndex )*(1/increment ),0,(i- startIndex )*(1/increment ),ui->lPanelXY_2->height());

        pixmap.fill(Qt::white);

        // Option 1 checked: Paint fixation numbers
        if ( ui->cbFixations->isChecked()){
            uvec fixIndex =  arma::find((*p_fixAllM).col(FIXCOL_START) <= i);
            aux = (*p_fixAllM).rows(fixIndex);
            fixIndex =  arma::find(aux.col(FIXCOL_END) >= i);
            if (fixIndex.n_rows != 0){   // If there is a fixation between the values, paint it!
                number.str("");
                number << fixIndex(0) - firstIndexSeg;
                myPen.setColor(QColor(0, 50, 128, 255));
                painter.setPen(myPen);
                painter.drawText( QPoint( ui->lPanelVisualization->width()/2, ui->lPanelVisualization->height()/2 ), number.str().c_str() );

            }
        }

        // Option 2 checked : Paint pupil dilation
        if (ui->cbPupilDilation->isChecked() && (*p_roughM).n_cols == 8){
            if ((*p_roughM)(i ,6) > 0){
                myPen.setWidth((*p_roughM)(i ,6) * 10);
            }
        }

        myPen.setColor(QColor(255, 0, 0, 255));
        painter.setPen(myPen);
        painter.drawPoint( (*p_roughM)(i ,2 )  * ui->lPanelVisualization->width(), (*p_roughM)(i ,3 )  * ui->lPanelVisualization->height()); // XL

        // Option 2 checked : Paint pupil dilation
        if (ui->cbPupilDilation->isChecked()  && (*p_roughM).n_cols == 8){
            if ((*p_roughM)(i ,7) > 0){
                myPen.setWidth((*p_roughM)(i ,7) * 10);
            }
        }

        myPen.setColor(QColor(0, 50, 128, 255));
        painter.setPen(myPen);
        painter.drawPoint( (*p_roughM)(i ,4 )  * ui->lPanelVisualization->width(), (*p_roughM)(i ,5 )  * ui->lPanelVisualization->height()); // XL


        ui->lPanelXY_2->setPixmap(pixmapProcessLine);
        ui->lPanelVisualization->setPixmap(pixmap);
        ui->lPanelVisualization->repaint();

        painterProcessLine.end();

        qApp->processEvents();

        // Timer !
        startTime = getMilliCount();
        while (getMilliSpan(startTime) < velocity){
        }
    }

     painter.end();

}

void DialogVisualization::playSmooth(){
    QPixmap pixmap(ui->lPanelVisualization->width(),ui->lPanelVisualization->height());
    QPixmap pixmapProcessLine(ui->lPanelXY_2->width(),ui->lPanelXY_2->height());

    QPainter painter(&pixmap);
    pixmap.fill(Qt::white);
    QPen myPen(Qt::red, 15, Qt::SolidLine);

    double increment = (double) secsFragment * hz / ui->lPanelXY->width();

    uword startIndex = ((currentFragment-1) * secsFragment * hz);
    uword stopIndex =  ((currentFragment) * secsFragment * hz);
    if (stopIndex > (*p_smoothM).n_rows)
        stopIndex = (*p_smoothM).n_rows;

    int firstIndexSeg = 0; // Just to figure out which is the fixation id
    if ((*p_fixAllM).n_rows > 0){
        uvec fixIndex =  arma::find((*p_fixAllM).col(FIXCOL_END) >= startIndex);
        if (!fixIndex.is_empty()){
            firstIndexSeg = fixIndex(0);
        }

    }

    QFont font=painter.font() ;
    font.setPointSize ( 25 );
    painter.setFont(font);

    myPen.setColor(QColor(255, 0, 0, 255));
    myPen.setCapStyle(Qt::RoundCap);
    painter.setPen(myPen);

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

        // Option 1 checked : Paint pupil dilation
        if (ui->cbPupilDilation->isChecked() && (*p_roughM).n_cols == 8){
            if ((*p_roughM)(i,6) > 0){
                myPen.setWidth((*p_roughM)(i ,6) * 10);
            } else if ((*p_roughM)(i,7) > 0){
                myPen.setWidth((*p_roughM)(i ,7) * 10);
            }
        }

        // Draw point
        pixmap.fill(Qt::white);
        myPen.setColor(QColor(203, 29, 209, 255));
        painter.setPen(myPen);
        painter.drawPoint( (*p_smoothM)(i ,2 ) / expWidth * ui->lPanelVisualization->width(), (*p_smoothM)(i ,3 ) / expHeight * ui->lPanelVisualization->height()); // XL
       // painter.drawText( QPoint( ((*p_smoothM)(i ,2 ) / expWidth * ui->lPanelVisualization->width()), (*p_smoothM)(i ,3 ) / expHeight * ui->lPanelVisualization->height()), "X");


        // Option 2 paint fixation number: Paint fixations option checked:
        if ( ui->cbFixations->isChecked()){
            uvec fixIndex =  arma::find((*p_fixAllM).col(FIXCOL_START) <= i);
            aux = (*p_fixAllM).rows(fixIndex);
            fixIndex =  arma::find(aux.col(FIXCOL_END) >= i);
            if (fixIndex.n_rows != 0){   // If there is a fixation between the values, paint it!
                number.str("");
                number << fixIndex(0) - firstIndexSeg;
                painter.drawText( QPoint( ((*p_smoothM)(i ,2 ) / expWidth * ui->lPanelVisualization->width())+20, (*p_smoothM)(i ,3 ) / expHeight * ui->lPanelVisualization->height()), number.str().c_str() );

            }
        }

        ui->lPanelXY_2->setPixmap(pixmapProcessLine);
        ui->lPanelVisualization->setPixmap(pixmap);
        ui->lPanelVisualization->repaint();

        painterProcessLine.end();

        qApp->processEvents();

        // Timer !
        startTime = getMilliCount();
        while (getMilliSpan(startTime) < velocity){
        }

    }
    painter.end();


}


void DialogVisualization::playRoughAndSmooth(){
    QPixmap pixmap(ui->lPanelVisualization->width(),ui->lPanelVisualization->height());
    QPixmap pixmapProcessLine(ui->lPanelXY_2->width(),ui->lPanelXY_2->height());

    QPainter painter(&pixmap);
    pixmap.fill(Qt::white);
    QPen myPen(Qt::red, 15, Qt::SolidLine);

    double increment = (double) secsFragment * hz / ui->lPanelXY->width();

    uword startIndex = ((currentFragment-1) * secsFragment * hz);
    uword stopIndex =  ((currentFragment) * secsFragment * hz);
    if (stopIndex > (*p_smoothM).n_rows)
        stopIndex = (*p_smoothM).n_rows;

    int firstIndexSeg = 0; // Just to figure out which is the fixation id
    if ((*p_fixAllM).n_rows > 0){
        uvec fixIndex =  arma::find((*p_fixAllM).col(1) >= startIndex);
        if (!fixIndex.is_empty()){
            firstIndexSeg = fixIndex(0);
        }

    }

    QFont font=painter.font() ;
    font.setPointSize ( 25 );
    painter.setFont(font);

    myPen.setColor(QColor(255, 0, 0, 255));
    myPen.setCapStyle(Qt::RoundCap);
    painter.setPen(myPen);

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

        // Option 1 checked : Paint pupil dilation
        if (ui->cbPupilDilation->isChecked() && (*p_roughM).n_cols == 8){
            if ((*p_roughM)(i,6) > 0){
                myPen.setWidth((*p_roughM)(i ,6) * 10);
            }else if ((*p_roughM)(i,7) > 0){
                myPen.setWidth((*p_roughM)(i ,7) * 10);
            }
        }

        pixmap.fill(Qt::white);

        // Draw rough data
        myPen.setColor(QColor(255, 0, 0, 255));
        painter.setPen(myPen);
        painter.drawPoint( (*p_roughM)(i ,2 )  * ui->lPanelVisualization->width(), (*p_roughM)(i ,3 )  * ui->lPanelVisualization->height()); // XL

        myPen.setColor(QColor(0, 50, 128, 255));
        painter.setPen(myPen);
        painter.drawPoint( (*p_roughM)(i ,4 )  * ui->lPanelVisualization->width(), (*p_roughM)(i ,5 )  * ui->lPanelVisualization->height()); // XL

        // Draw smooth data
        myPen.setColor(QColor(203, 29, 209, 255));
        painter.setPen(myPen);
        painter.drawPoint( (*p_smoothM)(i ,2 ) / expWidth * ui->lPanelVisualization->width(), (*p_smoothM)(i ,3 ) / expHeight * ui->lPanelVisualization->height()); // XL



        // Option 2 paint fixation number: Paint fixations option checked:
        if ( ui->cbFixations->isChecked()){
            uvec fixIndex =  arma::find((*p_fixAllM).col(FIXCOL_START) <= i);
            aux = (*p_fixAllM).rows(fixIndex);
            fixIndex =  arma::find(aux.col(FIXCOL_END) >= i);
            if (fixIndex.n_rows != 0){   // If there is a fixation between the values, paint it!
                number.str("");
                number << fixIndex(0) - firstIndexSeg;
                painter.drawText( QPoint( ((*p_smoothM)(i ,2 ) / expWidth * ui->lPanelVisualization->width())+20, (*p_smoothM)(i ,3 ) / expHeight * ui->lPanelVisualization->height()), number.str().c_str() );

            }
        }

        ui->lPanelXY_2->setPixmap(pixmapProcessLine);
        ui->lPanelVisualization->setPixmap(pixmap);
        ui->lPanelVisualization->repaint();

        painterProcessLine.end();

        qApp->processEvents();

        // Timer !
        startTime = getMilliCount();
        while (getMilliSpan(startTime) < velocity){
        }

    }
    painter.end();


}




void DialogVisualization::paintRoughXY(){

    double increment = (double) secsFragment * hz / ui->lPanelXY->width();

    QPixmap pixmapMissingData(ui->lPanelMissingData->width(),ui->lPanelMissingData->height());
    pixmapMissingData.fill(Qt::transparent);
    QPixmap pixmap(ui->lPanelXY->width(),ui->lPanelXY->height());
    pixmap.fill(Qt::white);

    QPainter painter(&pixmap);
    QPainter painterMissingData(&pixmapMissingData);
    QPen myPen(Qt::red, 1.2, Qt::SolidLine);

    uword startIndex = ((currentFragment-1) * secsFragment * hz);
    uword stopIndex =  ((currentFragment) * secsFragment * hz);
    if (stopIndex > (*p_roughM).n_rows)
        stopIndex = (*p_roughM).n_rows;

   // also so el la capacidad del array es menor que stop index
    for (uword i = startIndex; i < stopIndex; ++i) {
        myPen.setColor(QColor(255, 0, 0, 255));

        if ((*p_roughM)(i,2 ) != -1){
            painter.setPen(myPen);
            painter.drawPoint( (i- startIndex )*(1/increment ), (*p_roughM)(i,2 ) * ui->lPanelXY->height()); // XL
        }else{ // Missing data
            painterMissingData.setPen(myPen);
            painterMissingData.drawPoint( (i- startIndex+1)*(1/increment ), 10); // XL
        }

        myPen.setColor(QColor(0, 0, 128, 255));
        if ((*p_roughM)(i,3 ) != -1){
            painter.setPen(myPen);
            painter.drawPoint( (i- startIndex)*(1/increment ), (*p_roughM)(i,3 ) * ui->lPanelXY->height()); // YL
        }else{ // Missing data
            painterMissingData.setPen(myPen);
            painterMissingData.drawPoint( (i- startIndex+1)*(1/increment ), 20); //XR
        }

       myPen.setColor(QColor(255, 50, 0, 255));
        if ((*p_roughM)(i,4 ) != -1){
            painter.setPen(myPen);
            painter.drawPoint( (i- startIndex)*(1/increment ), (*p_roughM)(i,4 ) * ui->lPanelXY->height()); // XR
        }else{ // Missing data
            painterMissingData.setPen(myPen);
            painterMissingData.drawPoint( (i- startIndex+1)*(1/increment ), 30); //XR
        }

        myPen.setColor(QColor(0, 50, 128, 255));
        if ((*p_roughM)(i,5 ) != -1){
            painter.setPen(myPen);
            painter.drawPoint( (i- startIndex)*(1/increment ), (*p_roughM)(i,5 ) * ui->lPanelXY->height()); // YR
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

void DialogVisualization::paintRoughAndSmooth(){

    double increment = (double) secsFragment * hz / ui->lPanelXY->width();

    QPixmap pixmapMissingData(ui->lPanelMissingData->width(),ui->lPanelMissingData->height());
    pixmapMissingData.fill(Qt::transparent);
    QPixmap pixmap(ui->lPanelXY->width(),ui->lPanelXY->height());
    pixmap.fill(Qt::white);

    QPainter painter(&pixmap);
    QPainter painterMissingData(&pixmapMissingData);
    QPen myPen(Qt::red, 1.2, Qt::SolidLine);

    uword startIndex = ((currentFragment-1) * secsFragment * hz);
    uword stopIndex =  ((currentFragment) * secsFragment * hz);
    if (stopIndex > (*p_roughM).n_rows)
        stopIndex = (*p_roughM).n_rows;

   // also so el la capacidad del array es menor que stop index
    for (uword i = startIndex; i < stopIndex; ++i) {
        myPen.setColor(QColor(255, 0, 0, 255));

        if ((*p_roughM)(i,2 ) != -1){
            painter.setPen(myPen);
            painter.drawPoint( (i- startIndex )*(1/increment ), (*p_roughM)(i,2 ) * ui->lPanelXY->height()); // XL
        }else{ // Missing data
            painterMissingData.setPen(myPen);
            painterMissingData.drawPoint( (i- startIndex+1)*(1/increment ), 10); // XL
        }

        myPen.setColor(QColor(0, 0, 128, 255));
        if ((*p_roughM)(i,3 ) != -1){
            painter.setPen(myPen);
            painter.drawPoint( (i- startIndex)*(1/increment ), (*p_roughM)(i,3 ) * ui->lPanelXY->height()); // YL
        }else{ // Missing data
            painterMissingData.setPen(myPen);
            painterMissingData.drawPoint( (i- startIndex+1)*(1/increment ), 20); //XR
        }

       myPen.setColor(QColor(255, 50, 0, 255));
        if ((*p_roughM)(i,4 ) != -1){
            painter.setPen(myPen);
            painter.drawPoint( (i- startIndex)*(1/increment ), (*p_roughM)(i,4 ) * ui->lPanelXY->height()); // XR
        }else{ // Missing data
            painterMissingData.setPen(myPen);
            painterMissingData.drawPoint( (i- startIndex+1)*(1/increment ), 30); //XR
        }

        myPen.setColor(QColor(0, 50, 128, 255));
        if ((*p_roughM)(i,5 ) != -1){
            painter.setPen(myPen);
            painter.drawPoint( (i- startIndex)*(1/increment ), (*p_roughM)(i,5 ) * ui->lPanelXY->height()); // YR
        }else{ // Missing data
            painterMissingData.setPen(myPen);
            painterMissingData.drawPoint( (i- startIndex+1)*(1/increment ), 40); //XR
        }

        // Paint smooth data!
        myPen.setColor(QColor(50, 179, 58, 255));
        painter.setPen(myPen);
        painter.drawPoint((i-startIndex)*(1/increment ), (*p_smoothM)(i ,2 ) / expWidth * ui->lPanelXY->height()); // XL

        myPen.setColor(QColor(162, 50, 179, 255));
        painter.setPen(myPen);
        painter.drawPoint((i- startIndex)*(1/increment ), (*p_smoothM)(i,3 )/ expHeight * ui->lPanelXY->height()); // YL

    }

    ui->lPanelXY->setPixmap(pixmap);
    ui->lPanelMissingData->setPixmap(pixmapMissingData);

     painter.end();
     painterMissingData.end();

}

void DialogVisualization::paintSmoothXY(){

    QPixmap pixmap(ui->lPanelXY->width(),ui->lPanelXY->height());
    pixmap.fill(Qt::white);

    QPainter painter(&pixmap);
    QPen myPen(Qt::red, 1, Qt::SolidLine);

    uword startIndex = ((currentFragment-1) * secsFragment * hz);
    uword stopIndex =  ((currentFragment) * secsFragment * hz);
    if (stopIndex > (*p_smoothM).n_rows)
        stopIndex = (*p_smoothM).n_rows;
    double increment = (double) secsFragment * hz / ui->lPanelXY->width();

    for (uword i = startIndex; i < stopIndex; ++i) {
        myPen.setColor(QColor(50, 179, 58, 255));
        painter.setPen(myPen);
        painter.drawPoint((i-startIndex)*(1/increment ), (*p_smoothM)(i ,2 ) / expWidth * ui->lPanelXY->height()); // XL

        myPen.setColor(QColor(162, 50, 179, 255));
        painter.setPen(myPen);
        painter.drawPoint((i- startIndex)*(1/increment ), (*p_smoothM)(i,3 )/ expHeight * ui->lPanelXY->height()); // YL

    }

    ui->lPanelXY->setPixmap(pixmap);
     painter.end();

}

void DialogVisualization::paintFixations(){
    QPixmap pixmap(ui->lPanelFixations->width(),ui->lPanelFixations->height());
    pixmap.fill(Qt::white);

    QPainter painter(&pixmap);

    int startIndex = ((currentFragment-1) * secsFragment * hz);
    int stopIndex =  ((currentFragment) * secsFragment * hz);
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
    if ((*p_fixAllM).n_rows > 0){
        uvec fixIndex =  arma::find((*p_fixAllM).col(FIXCOL_END) >= startIndex);

        if (!fixIndex.is_empty()){
            auxIndex = fixIndex(0);
        }else{
            auxIndex = (*p_fixAllM).n_rows ;
        }

    }

    for (uword i = auxIndex; i < (*p_fixAllM).n_rows  ; i++){
        if (((*p_fixAllM)(i,FIXCOL_START) >= startIndex && (*p_fixAllM)(i,FIXCOL_START) <=  stopIndex ) ||((*p_fixAllM)(i,FIXCOL_START) <= startIndex && (*p_fixAllM)(i,FIXCOL_END) >= startIndex )){

            if ((*p_fixAllM)(i,FIXCOL_START) <= startIndex && (*p_fixAllM)(i,FIXCOL_END) >= startIndex ) // If it's a fixation that didn't end in the previous segment
                posStart = -1;
            else
                posStart = ((*p_fixAllM)(i,FIXCOL_START) - startIndex ) * (1/increment);

            posEnd = (((*p_fixAllM)(i,FIXCOL_END) - startIndex ) * (1/increment)) - posStart;

            if ((*p_fixAllM)(i,FIXCOL_SMOOTH_PURSUIT) == Consts::SMOOTHP_YES){
                painter.setBrush(QBrush("#ffbc00"));
            }else{
                painter.setBrush(QBrush("#1ac500"));
            }

            painter.drawRect(QRect( posStart ,25, posEnd,16));
            number.str("");
            number << counter;
            painter.drawText( QPoint(posStart,23), number.str().c_str() );

            counter ++; // Next fixation

        }else if ((*p_fixAllM)(i,FIXCOL_END) >= stopIndex){
            break;
        }
    }

    ui->lPanelFixations->setPixmap(pixmap);
    painter.end();
}

int DialogVisualization::getMilliCount(){
    timeb tb;
    ftime(&tb);
    int nCount = tb.millitm + (tb.time & 0xfffff) * 1000;
    return nCount;
}

int DialogVisualization::getMilliSpan(int nTimeStart){
    int nSpan = getMilliCount() - nTimeStart;
    if(nSpan < 0)
        nSpan += 0x100000 * 1000;
    return nSpan;
}


/**
  *     PUBLIC SLOTS
  **/

void DialogVisualization::fncPress_bPlay(){

    if (playOnOff == 0){
        playOnOff = 1; // Playing gaze
        ui->b_play->setText("Stop");
        if (ui->rad_rough->isChecked()){ // Paint the rough data
            playRough();
        }else if(ui->rad_smooth->isChecked()){   // Paint the smooth data
            playSmooth();
        }else{
            playRoughAndSmooth();
        }
        playOnOff = 0; // Stop playing gaze
        ui->b_play->setText("Play");
    }else{
        playOnOff = 0;
        ui->b_play->setText("Play");
        if (ui->rad_rough->isChecked()){ // Paint the rough data
            paintRoughXY();
        }else if(ui->rad_smooth->isChecked()){   // Paint the smooth data
            paintSmoothXY();
        }else{
            playRoughAndSmooth();
        }

    }

}

void DialogVisualization::fncChange_Visualization(bool type){
    Q_UNUSED(type)
    playOnOff = 0;
    ui->b_play->setText("Play");
    paintXYAxes();
    paintBlankPanel();
}

// Methods to close the window and stop the current processes.
void DialogVisualization::fncPress_bCancel(){
    playOnOff = 0;
    close();
}

void DialogVisualization::closeEvent(QCloseEvent *){
    playOnOff = 0;
    close();
}

