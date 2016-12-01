#include "DialogVideoPlayer.h"
#include "ui_DialogVideoPlayer.h"

DialogVideoPlayer::DialogVideoPlayer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogVideoPlayer) {
    ui->setupUi(this);


    connect(ui->buttonOpen, SIGNAL(clicked()), this, SLOT(openFile()));
    connect(ui->buttonPlay, SIGNAL(clicked()), this, SLOT(playButton()));
    connect(ui->sliderPosition, SIGNAL(sliderReleased()), this, SLOT(movieSliderReleased()), Qt::QueuedConnection);


    // Set up ui controls
    ui->buttonPlay->setEnabled(false);
    ui->buttonPlay->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui->sliderPosition->setRange(0, 0);


    // Set up the main visualization view
    ui->visualizationView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->visualizationView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    visualizationScene = new QGraphicsScene(this);
    ui->visualizationView->setScene(visualizationScene);



    // Set up the media player (for videos)
    visualizationVideoItem = new QGraphicsVideoItem();
    visualizationVideoItem->setAspectRatioMode(Qt::KeepAspectRatio);
    mediaPlayer = new QMediaPlayer(this);
    mediaPlayer->setNotifyInterval(1000); //updates slider every 1000 ms
    mediaPlayer->setVideoOutput(visualizationVideoItem);
    visualizationScene->addItem(visualizationVideoItem);


    // Set up screen layer (mirrors screen size)
    screenLayerItem = new QGraphicsRectItem();
    visualizationScene->addItem(screenLayerItem);


    // Set up the pixmap (to draw the view location)
    visualizationPixmapItem = new QGraphicsPixmapItem();
    visualizationScene->addItem(visualizationPixmapItem);



    // Media player connections
    connect(mediaPlayer, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(mediaStateChanged(QMediaPlayer::State)));
    connect(mediaPlayer, SIGNAL(positionChanged(qint64)), this, SLOT(positionChanged(qint64)));
    connect(mediaPlayer, SIGNAL(durationChanged(qint64)), this, SLOT(durationChanged(qint64)));
    connect(mediaPlayer, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(handleError()));


    // Set up the timeline
    ui->timeLineView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->timeLineView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    timeLineScene = new QGraphicsScene(this);
    ui->timeLineView->setScene(timeLineScene);


    // Set up timeline pixmap
    timeLinePixmapItem = new QGraphicsPixmapItem();
    timeLineScene->addItem(timeLinePixmapItem);


    // Set up line indicating time on time line
    timeLineTimeItem = new QGraphicsLineItem();
    timeLineScene->addItem(timeLineTimeItem);

}

DialogVideoPlayer::~DialogVideoPlayer() {
    mediaPlayer->stop();
    delete timeLineTimeItem;
    delete timeLinePixmapItem;
    delete timeLineScene;
    delete visualizationPixmapItem;
    delete visualizationVideoItem;
    delete visualizationScene;
    delete mediaPlayer;
    delete ui;
}

void DialogVideoPlayer::loadData(GrafixParticipant* participant, mat &p_roughM_in, mat &p_smoothM_in, mat &p_fixAllM_in) {

    this->_participant = participant;
    this->p_roughM = &p_roughM_in;
    this->p_smoothM = &p_smoothM_in;
    this->p_fixAllM = &p_fixAllM_in;
    this->secsFragment = this->_participant->GetProject()->GetProjectSetting(Consts::SETTING_SECS_FRAGMENT, Consts::ACTIVE_CONFIGURATION()).toInt();
    this->hz = this->_participant->GetProject()->GetProjectSetting(Consts::SETTING_HZ, Consts::ACTIVE_CONFIGURATION()).toInt();
    this->expWidth = this->_participant->GetProject()->GetProjectSetting(Consts::SETTING_EXP_WIDTH, Consts::ACTIVE_CONFIGURATION()).toInt();
    this->expHeight = this->_participant->GetProject()->GetProjectSetting(Consts::SETTING_EXP_HEIGHT, Consts::ACTIVE_CONFIGURATION()).toInt();

    if (this->p_smoothM->is_empty()) {
        ui->checkBoxSmooth->setChecked(false);
        ui->checkBoxSmooth->setEnabled(false);
    } else {
        ui->checkBoxSmooth->setChecked(true);
    }

    timerOffset = (*p_roughM).at(0, 0); //this is the ms of when the player first started
    currentIndex = 0;
}

void DialogVideoPlayer::resizeEvent (QResizeEvent *event) {
    QDialog::resizeEvent(event);
    fncCalculateAspectRatios();

}

void DialogVideoPlayer::fncCalculateAspectRatios() {
    // Calculate resize ratios for resizing
    vid_width = visualizationVideoItem->size().width();
    vid_height = visualizationVideoItem->size().height();
    double ratioW = expWidth / vid_width;
    double ratioH = expHeight / vid_height;



    // smaller ratio will ensure that the image fits in the view
    if (ratioW > ratioH) {
        //data is wider than video

        //make heights the same,
        display_height = vid_height;
        display_width =  expWidth * vid_height / expHeight;
        vid_offset_y = 0;
        vid_offset_x = (display_width - vid_width ) / 2;

    } else {
        //data is taller than data (or the same)

        //make widths the same
        display_width = vid_width;
        display_height = expHeight * vid_width / expWidth;
        vid_offset_x = 0;
        vid_offset_y = (display_height - vid_height) / 2;
    }

    qDebug() << "exp width: " << expWidth << " exp height: " << expHeight;
    qDebug() << "vid width: " << vid_width << " vid height: " << vid_height;
    qDebug() << "disp width: " << display_width << " disp height: " << display_height;
    qDebug() << "offsetts: " << vid_offset_x << " " << vid_offset_y;

    screenLayerItem->setRect(0, 0, display_width, display_height);
    visualizationVideoItem->setPos(vid_offset_x, vid_offset_y);
    ui->visualizationView->fitInView(this->screenLayerItem, Qt::KeepAspectRatio);
}

void DialogVideoPlayer::closeEvent(QCloseEvent *event) {
    QDialog::closeEvent(event);
    mediaPlayer->stop();
}

void DialogVideoPlayer::openFile() {

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Movie"), QDir::homePath());

    if (!fileName.isEmpty()) {
        mediaPlayer->setMedia(QUrl::fromLocalFile(fileName));

        fncCalculateAspectRatios();


        double video_aspect = (double)vid_width / (double)vid_height;
        double data_aspect = (double)expWidth / (double)expHeight;

        if (video_aspect == data_aspect) {
            ui->statusLabel->setText("Video loaded.");
        } else {
            if (video_aspect > data_aspect) {
                ui->statusLabel->setText("Video loaded: Video aspect is wider than data.");
            } else {
                ui->statusLabel->setText("Video loaded: Video aspect is taller than data.");
            }
        }

        ui->buttonPlay->setEnabled(true);
    }
}

void DialogVideoPlayer::playButton() {
    switch(mediaPlayer->state()) {
        case QMediaPlayer::PlayingState:
            mediaPlayer->pause();
            break;
        default:
            playMovie();
            break;
    }
}

void DialogVideoPlayer::playTimer() {

}

void DialogVideoPlayer::playMovie() {
    //wait to start (buggy otherwise)
    while(mediaPlayer->mediaStatus() != QMediaPlayer::LoadedMedia) {
        qApp->processEvents();
    }

    mediaPlayer->play();

    while(mediaPlayer->state() == QMediaPlayer::PlayingState) {

        // Get position in ms with offset since start of movie and map to data
        double current_movie_time = (double)(mediaPlayer->position()) + timerOffset;
        updatePlaybackState(current_movie_time);
        qApp->processEvents();
    }

    mediaPlayer->stop();
}

void DialogVideoPlayer::updatePlaybackState(double newTimeMS) {

    qDebug() << "playing: " << newTimeMS;

    currentTimeMS = newTimeMS;

    //Search for current index

    int last_index = (*p_roughM).n_rows - 1;

    int previousIndex = qMax(currentIndex,0);
    currentIndex = -1;

    for(int search_index = previousIndex; search_index <= last_index; search_index++) {
        double searched_time = (*p_roughM).at(search_index, 0);
        if (searched_time >= currentTimeMS) {
            //found latest frame - paint
            currentIndex = search_index;

            //use previous frame
            if (currentIndex > 0) currentIndex--;

            break;
        }
    }

    if (currentIndex == -1) {
        paintNoFrameFound();
        qDebug() << "no frame";
    } else {
        paintCurrentVisualizationFrame();
    }
}

void DialogVideoPlayer::paintCurrentVisualizationFrame() {

    QFont serifFont("Times", 64, QFont::Bold);
    bool show_fixation_numbers = false;

    double width_multi = (double)display_width / (double)expWidth;
    double height_multi = (double)display_height / (double)expHeight;
    double pen_size_multi = display_width / 100;

    QPixmap pixmap(display_width, display_height);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);



    if (ui->checkBoxSmooth->isChecked()) {
        QPen myPen(Qt::green, pen_size_multi * 4, Qt::SolidLine);
        myPen.setColor(QColor(0, 250, 0, 255));
        myPen.setCapStyle(Qt::RoundCap);

        if ((*p_roughM)(currentIndex , 6) > 0 && (*p_roughM)(currentIndex , 7) > 0) {
            myPen.setWidth(((*p_roughM)(currentIndex , 6) + (*p_roughM)(currentIndex , 7)) * pen_size_multi);
        }

        painter.setPen(myPen);
        painter.drawPoint((*p_smoothM)(currentIndex , 2 ) * width_multi, (*p_smoothM)(currentIndex , 3 ) * height_multi); // XL

        // Option 1 checked: Paint fixation numbers
        if (show_fixation_numbers) {
            painter.setFont(serifFont);


            uvec fixIndex =  arma::find((*p_fixAllM).col(FIXCOL_START) <= currentIndex);
            mat aux = (*p_fixAllM).rows(fixIndex);
            fixIndex =  arma::find(aux.col(FIXCOL_END) >= currentIndex);
            if (fixIndex.n_rows != 0) {
                // If there is a fixation between the values, paint it!

                myPen.setColor(QColor(0, 50, 128, 255));
                painter.setPen(myPen);

                painter.drawText( QPoint( display_width / 2, display_height / 2 ), QString::number(fixIndex(0)) );

            }
        }


    } else {
        QPen myPen(Qt::red, pen_size_multi * 4, Qt::SolidLine);
        myPen.setCapStyle(Qt::RoundCap);
        //set pupil dilation
        if ((*p_roughM)(currentIndex , 6) > 0) myPen.setWidth((*p_roughM)(currentIndex , 6) * pen_size_multi);

        myPen.setColor(QColor(255, 0, 0, 255));
        painter.setPen(myPen);
        painter.drawPoint( (*p_roughM)(currentIndex , 2 )  * display_width, (*p_roughM)(currentIndex , 3 )  * display_height);

        // Option 2 checked : Paint pupil dilation
        if ((*p_roughM)(currentIndex , 7) > 0) myPen.setWidth((*p_roughM)(currentIndex , 7) * pen_size_multi);

        myPen.setColor(QColor(0, 50, 128, 255));
        painter.setPen(myPen);
        painter.drawPoint( (*p_roughM)(currentIndex , 4 )  * display_width, (*p_roughM)(currentIndex , 5 )  * display_height);
    }


    painter.end();
    visualizationPixmapItem->setPixmap(pixmap);

}

void DialogVideoPlayer::paintCurrentTimeLineLineFrame() {

}

void DialogVideoPlayer::paintNoFrameFound() {
    QPixmap pixmap(display_width, display_height);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);

    QFont serifFont("Times", 12, QFont::Bold);

    painter.setFont(serifFont);

    painter.drawText( QPoint( 5, 20 ), "No valid data found!" );

    painter.end();
    visualizationPixmapItem->setPixmap(pixmap);
}

void DialogVideoPlayer::paintTimeLine() {

}


void DialogVideoPlayer::mediaStateChanged(QMediaPlayer::State state) {
    switch(state) {
        case QMediaPlayer::PlayingState:
            ui->buttonPlay->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
            break;
        default:
            ui->buttonPlay->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
            break;
    }
}

void DialogVideoPlayer::positionChanged(qint64 position) {
    ui->sliderPosition->setValue(position);
}

void DialogVideoPlayer::durationChanged(qint64 duration) {
    ui->sliderPosition->setRange(0, duration);
}

void DialogVideoPlayer::movieSliderReleased() {
    mediaPlayer->setPosition(ui->sliderPosition->sliderPosition());
}

void DialogVideoPlayer::handleError() {
    ui->buttonPlay->setEnabled(false);
    //errorLabel->setText("Error: " + mediaPlayer.errorString());
}
