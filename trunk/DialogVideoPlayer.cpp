#include "DialogVideoPlayer.h"
#include "ui_DialogVideoPlayer.h"

DialogVideoPlayer::DialogVideoPlayer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogVideoPlayer)
{
    ui->setupUi(this);


    connect(ui->buttonOpen, SIGNAL(clicked()), this, SLOT(openFile()));
    ui->buttonPlay->setEnabled(false);
    ui->buttonPlay->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));

    connect(ui->buttonPlay, SIGNAL(clicked()), this, SLOT(play()));

    ui->sliderPosition->setRange(0, 0);

    connect(ui->sliderPosition, SIGNAL(sliderMoved(int)),
            this, SLOT(setPosition(int)));


    mediaPlayer.setVideoOutput(ui->videoWidget);

    mediaPlayer.setNotifyInterval(1000); //updates slider every 1000 ms

    connect(&mediaPlayer, SIGNAL(stateChanged(QMediaPlayer::State)),
            this, SLOT(mediaStateChanged(QMediaPlayer::State)));
    connect(&mediaPlayer, SIGNAL(positionChanged(qint64)), this, SLOT(positionChanged(qint64)));
    connect(&mediaPlayer, SIGNAL(durationChanged(qint64)), this, SLOT(durationChanged(qint64)));
    connect(&mediaPlayer, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(handleError()));
}

DialogVideoPlayer::~DialogVideoPlayer()
{
    delete ui;
}

void DialogVideoPlayer::loadData(GrafixParticipant* participant, mat &p_roughM_in, mat &p_smoothM_in, mat &p_fixAllM_in){

    this->_participant = participant;
    this->p_roughM = &p_roughM_in;
    this->p_smoothM = &p_smoothM_in;
    this->p_fixAllM = &p_fixAllM_in;
    this->secsFragment = this->_participant->GetProject()->GetProjectSetting(Consts::SETTING_SECS_FRAGMENT, Consts::ACTIVE_CONFIGURATION()).toInt();
    this->hz = this->_participant->GetProject()->GetProjectSetting(Consts::SETTING_HZ, Consts::ACTIVE_CONFIGURATION()).toInt();
    this->expWidth = this->_participant->GetProject()->GetProjectSetting(Consts::SETTING_EXP_WIDTH, Consts::ACTIVE_CONFIGURATION()).toInt();
    this->expHeight = this->_participant->GetProject()->GetProjectSetting(Consts::SETTING_EXP_HEIGHT, Consts::ACTIVE_CONFIGURATION()).toInt();

}

void DialogVideoPlayer::openFile()
{

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Movie"),QDir::homePath());

    if (!fileName.isEmpty()) {
        mediaPlayer.setMedia(QUrl::fromLocalFile(fileName));
        ui->buttonPlay->setEnabled(true);
    }
}

void DialogVideoPlayer::play()
{
    switch(mediaPlayer.state()) {
    case QMediaPlayer::PlayingState:
        mediaPlayer.pause();
        break;
    default:

        playRough();
        break;
    }
}

void DialogVideoPlayer::playRough()
{
    QGraphicsScene *scene = new QGraphicsScene(this);
    QGraphicsVideoItem *item = new QGraphicsVideoItem;
    ui->graphicsView->setScene(scene);
    mediaPlayer.setVideoOutput(item);
    scene->addItem(item);
    //QGraphicsTextItem *label = new QGraphicsTextItem("Text Over Video!");

    //scene->addItem(label);



    int display_width = ui->graphicsView->width();
    int display_height = ui->graphicsView->height();
    QPixmap pixmap(display_width, display_height);

    pixmap.fill(Qt::transparent);

    //QGraphicsPixmapItem *overlay = new QGraphicsPixmapItem(pixmap);
    //scene->addItem(overlay);


    //ui->graphicsView->setViewport(new QGLWidget);
    //graphicsView->show();

    mediaPlayer.play();

    double timer_offset = (*p_roughM).at(0,0); //this is the ms of when the player first started
    uword current_index = 0;
    uword last_index = (*p_roughM).n_rows - 1;


    while(mediaPlayer.state() == QMediaPlayer::PlayingState)
    {
        qApp->processEvents();
    }

    while(mediaPlayer.state() == QMediaPlayer::PlayingState)
    {
        //get position in ms with offset since start of movie and map to our data
        double current_movie_time = mediaPlayer.position() + timer_offset;

        for(uword search_index = current_index; search_index <= last_index; search_index++)
        {
            double searched_time = (*p_roughM).at(search_index,0);
            if (searched_time >= current_movie_time)
            {
                //found latest frame - paint
                current_index = search_index;

                //use previous frame
                if (current_index > 0) current_index--;

                pixmap.fill(Qt::transparent);
                QPainter painter(&pixmap);
                QPen myPen(Qt::red, 2, Qt::SolidLine);
                myPen.setCapStyle(Qt::RoundCap);
                //set pupil dilation
                if ((*p_roughM)(current_index ,6) > 0) myPen.setWidth((*p_roughM)(current_index ,6) * 2);

                myPen.setColor(QColor(255, 0, 0, 255));
                painter.setPen(myPen);
                painter.drawPoint( (*p_roughM)(current_index ,2 )  * display_width, (*p_roughM)(current_index ,3 )  * display_height);

                // Option 2 checked : Paint pupil dilation
                if ((*p_roughM)(current_index ,7) > 0) myPen.setWidth((*p_roughM)(current_index ,7) * 2);

                myPen.setColor(QColor(0, 50, 128, 255));
                painter.setPen(myPen);
                painter.drawPoint( (*p_roughM)(current_index ,4 )  * display_width, (*p_roughM)(current_index ,5 )  * display_height);
                painter.end();

                break;
            }
        }

        //overlay->setPixmap(pixmap);

        //ui->gazePanel->setPixmap(pixmap);
        //ui->gazePanel->repaint();

        qApp->processEvents();
    }


}

    /*
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
            uvec fixIndex =  arma::find((*p_fixAllM).col(0) <= i);
            aux = (*p_fixAllM).rows(fixIndex);
            fixIndex =  arma::find(aux.col(1) >= i);
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
    }*/




void DialogVideoPlayer::mediaStateChanged(QMediaPlayer::State state)
{
    switch(state) {
    case QMediaPlayer::PlayingState:
        ui->buttonPlay->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        break;
    default:
        ui->buttonPlay->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        break;
    }
}

void DialogVideoPlayer::positionChanged(qint64 position)
{
    ui->sliderPosition->setValue(position);
}

void DialogVideoPlayer::durationChanged(qint64 duration)
{
    ui->sliderPosition->setRange(0, duration);
}

void DialogVideoPlayer::setPosition(int position)
{
    mediaPlayer.setPosition(position);
}

void DialogVideoPlayer::handleError()
{
    ui->buttonPlay->setEnabled(false);
    //errorLabel->setText("Error: " + mediaPlayer.errorString());
}
