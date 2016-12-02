#include "DialogVideoPlayer.h"
#include "ui_DialogVideoPlayer.h"

#include <time.h>
#include <sys/timeb.h>

DialogVideoPlayer::DialogVideoPlayer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogVideoPlayer) {
    ui->setupUi(this);


    connect(ui->buttonOpen, SIGNAL(clicked()), this, SLOT(openFile()));
    connect(ui->buttonPlay, SIGNAL(clicked()), this, SLOT(playButton()));
    connect(ui->sliderPosition, SIGNAL(sliderReleased()), this, SLOT(movieSliderReleased()), Qt::QueuedConnection);
    connect(ui->checkBoxSmooth, SIGNAL(clicked(bool)), this, SLOT(settingChanged()));
    connect(ui->rbFragment, SIGNAL(clicked(bool)), this, SLOT(settingChanged()));
    connect(ui->rbSegment, SIGNAL(clicked(bool)), this, SLOT(settingChanged()));
    connect(ui->rbWholeFile, SIGNAL(clicked(bool)), this, SLOT(settingChanged()));
    connect(ui->checkBoxLoop, SIGNAL(clicked(bool)), this, SLOT(settingChanged()));
    connect(ui->checkBoxMovie, SIGNAL(clicked(bool)), this, SLOT(changeMovieMode()));

    // Set up ui controls
    ui->checkBoxMovie->setEnabled(false);
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
    timeLineTimeItem->setPen(QPen(Qt::black, 1, Qt::DashLine));
    timeLineTimeItem->setPos(0, 0);
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

    _participant = participant;
    p_roughM = &p_roughM_in;
    p_smoothM = &p_smoothM_in;
    p_fixAllM = &p_fixAllM_in;
    secsFragment = this->_participant->GetProject()->GetProjectSetting(Consts::SETTING_SECS_FRAGMENT, Consts::ACTIVE_CONFIGURATION()).toInt();
    hz = this->_participant->GetProject()->GetProjectSetting(Consts::SETTING_HZ, Consts::ACTIVE_CONFIGURATION()).toInt();
    samplesPerFragment = secsFragment * hz;
    expWidth = this->_participant->GetProject()->GetProjectSetting(Consts::SETTING_EXP_WIDTH, Consts::ACTIVE_CONFIGURATION()).toInt();
    expHeight = this->_participant->GetProject()->GetProjectSetting(Consts::SETTING_EXP_HEIGHT, Consts::ACTIVE_CONFIGURATION()).toInt();

    if (p_smoothM->is_empty()) {
        ui->checkBoxSmooth->setChecked(false);
        ui->checkBoxSmooth->setEnabled(false);
    } else {
        ui->checkBoxSmooth->setChecked(true);
    }

    firstSampleMS = (*p_roughM).at(0, 0); //this is the ms of when the player first started
    currentIndex = 0;
    currentFragment = -1;

    settingChanged();

    updatePlaybackState(0, true);
}

void DialogVideoPlayer::resizeEvent (QResizeEvent *event) {
    QDialog::resizeEvent(event);
    fncCalculateAspectRatios();

    paintTimeLine();
    paintCurrentTimeLineLineFrame();
    paintCurrentVisualizationFrame();

}

void DialogVideoPlayer::mousePressEvent(QMouseEvent *mouseEvent) {
    bool leftButton = mouseEvent->buttons() & Qt::LeftButton;

    QRect timeLineRect = ui->timeLineView->geometry();

    bool onTimeLine = timeLineRect.contains(mouseEvent->pos());

    if (leftButton && onTimeLine) {
        // Set position
        double samples_over_time_line = (double)samplesPerFragment * (double)(mouseEvent->pos().x() - timeLineRect.x()) / (double)timeLineRect.width();
        int current_position = (currentFragment * samplesPerFragment) + (int)floor(samples_over_time_line);

        if (current_position > 0 && current_position < (int)p_roughM->n_rows) {
            updatePlaybackState(current_position, true);
        }
    }
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


    timeLineScene->setSceneRect(ui->timeLineView->rect());
}

void DialogVideoPlayer::closeEvent(QCloseEvent *event) {
    QDialog::closeEvent(event);
    mediaPlayer->stop();
}

void DialogVideoPlayer::openFile() {

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Movie"), QDir::homePath());

    if (!fileName.isEmpty()) {
        mediaPlayer->setMedia(QUrl::fromLocalFile(fileName));

        if (mediaPlayer->isVideoAvailable()) {
            fncCalculateAspectRatios();
            ui->checkBoxMovie->setEnabled(true);
            ui->checkBoxMovie->setChecked(true);
            return;
        } else {
            QMessageBox messageBox;
            messageBox.critical(0,"Error","An error has occured when loading this movie!");
            messageBox.setFixedSize(500,200);
        }
    }
}

void DialogVideoPlayer::playButton() {

    if (playing) {
        stopPlaying();

    } else {
        startPlaying();

    }
}

void DialogVideoPlayer::stopPlaying() {
    playing = false;
    ui->buttonPlay->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
}



void DialogVideoPlayer::startPlaying() {
    playing = true;
    ui->buttonPlay->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    if (ui->checkBoxMovie->isChecked()) {
        //wait to start (buggy otherwise)
        while(mediaPlayer->mediaStatus() != QMediaPlayer::LoadedMedia) {
            qApp->processEvents();
        }

        mediaPlayer->setPosition(currentTimeMS - firstSampleMS);
        mediaPlayer->play();

        while(mediaPlayer->state() == QMediaPlayer::PlayingState && playing) {

            // Get position in ms with offset since start of movie and map to data
            double current_movie_time = (double)(mediaPlayer->position()) + firstSampleMS;
            updatePlaybackStateTime(current_movie_time);
            qApp->processEvents();
        }

        mediaPlayer->stop();
    } else {
        clockStartTime = getMilliCount();
        playStartSampleMS = p_roughM->at(qMax(0, currentIndex), 0 );
        while(playing) {
            double current_movie_time = playStartSampleMS + getMilliCount() - clockStartTime;
            updatePlaybackStateTime(current_movie_time);
            qApp->processEvents();
        }
    }
}

int DialogVideoPlayer::getMilliCount(){
    timeb tb;
    ftime(&tb);
    int nCount = tb.millitm + (tb.time & 0xfffff) * 1000;
    return nCount;
}

void DialogVideoPlayer::updatePlaybackStateTime(double newTimeMS) {

    qDebug() << "playing: " << newTimeMS;

    currentTimeMS = newTimeMS;

    //Search for current index

    int last_index = (*p_roughM).n_rows - 1;

    int previousIndex = qMax(currentIndex,0);

    if (p_roughM->at(previousIndex,0) > currentTimeMS ) {
        previousIndex = 0;
    }

    int current_index = -1;

    for(int search_index = previousIndex; search_index <= last_index; search_index++) {
        double searched_time = (*p_roughM).at(search_index, 0);
        if (searched_time >= currentTimeMS) {
            //found latest frame - paint
            current_index = search_index;

            break;
        }
    }

    updatePlaybackState(current_index, false);

}

void DialogVideoPlayer::updatePlaybackState(int index, bool resetAll) {
    currentIndex = index;
    currentTimeMS = p_roughM->at(qMax(0, currentIndex), 0 );
    //  If time is greater that biggest time in rough matrix...
    if (currentIndex == -1) {

        if (settingLoop) {

            switch (settingPlayMode) {
             case DialogVideoPlayer::PlayModeSegment:
                stopPlaying();
                break;
             case DialogVideoPlayer::PlayModeFragment: {
                uword fragment_start = currentFragment * samplesPerFragment;
                updatePlaybackState(fragment_start, true);}
                break;
             case DialogVideoPlayer::PlayModeWholeFile:
                updatePlaybackState(0, true);
                break;

            }
        }  else {
            stopPlaying();
            return;
        }
    }

    // Calculate current fragment
    int current_fragment = currentIndex > -1 ? (int)floor((double)currentIndex / samplesPerFragment) : -1;

    if (current_fragment != currentFragment) {
        currentFragment = current_fragment;
        paintTimeLine();
    }
    paintCurrentVisualizationFrame();

    paintCurrentTimeLineLineFrame();

    if (resetAll) {
        if (ui->checkBoxMovie->isChecked()) {
            mediaPlayer->setPosition(currentTimeMS - firstSampleMS);
        } else {
            clockStartTime = getMilliCount();
            playStartSampleMS = p_roughM->at(qMax(0, currentIndex), 0 );
        }
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



    if (settingPlaySmooth) {
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

    // Get how far in fragment currentIndex is
    int index_in_fragment = currentIndex - (currentFragment * samplesPerFragment);
    double pos_in_fragment = (double)index_in_fragment / (double)samplesPerFragment;

    qreal x = pos_in_fragment * ui->timeLineView->width();
    qreal y = ui->timeLineView->height();
    timeLineTimeItem->setLine(x,0,x,y);
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

    // Clear time line
    QPixmap timeLinePixmap = QPixmap(ui->timeLineView->width(), ui->timeLineView->height());
    timeLinePixmap.fill(Qt::white);
    timeLinePixmapItem->setPixmap(timeLinePixmap);


}

void DialogVideoPlayer::paintTimeLine() {
    qDebug() << "painting time line";
    QPixmap timeLinePixmap = QPixmap(ui->timeLineView->width(), ui->timeLineView->height());
    timeLinePixmap.fill(Qt::white);

    QPainter painter(&timeLinePixmap);

    double height = timeLinePixmap.height();
    double width = timeLinePixmap.width();
    double height2 = height / (double)expWidth;

    QPen redPen(Qt::red, 1, Qt::SolidLine);
    QPen bluePen(Qt::blue, 1, Qt::SolidLine);

    uword startIndex = currentFragment * samplesPerFragment;

    qDebug() << "width" << width;
    for (uword i = 0; i < (uword)samplesPerFragment; ++i) {

        uword index = i + startIndex;
        double x = (double)i * width / (double)samplesPerFragment;

        if (settingPlaySmooth) {
            if (p_roughM->at(index,2 ) != -1){
                painter.setPen(redPen);
                painter.drawPoint(x, p_roughM->at(i,2 ) * height); // XL
            }

            if (p_roughM->at(index,3 ) != -1){
                painter.setPen(redPen);
                painter.drawPoint(x, p_roughM->at(i,3 ) * height); // YL
            }

            if (p_roughM->at(index,4 ) != -1){
                painter.setPen(bluePen);
                painter.drawPoint(x, p_roughM->at(i,4 ) * height); // XR
            }

            if (p_roughM->at(index,5 ) != -1){
                painter.setPen(bluePen);
                painter.drawPoint(x, p_roughM->at(i,5 ) * height); // YR
            }
        } else {
            if (p_smoothM->at(index,2 ) != -1){
                painter.setPen(redPen);
                painter.drawPoint(x, p_smoothM->at(i,2 ) * height2);
            }

            if (p_smoothM->at(index,3 ) != -1){
                painter.setPen(bluePen);
                painter.drawPoint(x, p_smoothM->at(i,3 ) * height2);
            }
        }
    }

    painter.end();

    timeLinePixmapItem->setPixmap(timeLinePixmap);
}

void DialogVideoPlayer::changeMovieMode() {

}

void DialogVideoPlayer::settingChanged() {
    if (ui->rbFragment->isChecked()) {
        settingPlayMode = DialogVideoPlayer::PlayModeFragment;
    } else if (ui->rbSegment->isChecked()) {
        settingPlayMode = DialogVideoPlayer::PlayModeSegment;
    } else if (ui->rbWholeFile->isChecked()) {
        settingPlayMode = DialogVideoPlayer::PlayModeWholeFile;
    }

    settingPlaySmooth = ui->checkBoxSmooth->isChecked();
    settingLoop = ui->checkBoxLoop->isChecked();
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
