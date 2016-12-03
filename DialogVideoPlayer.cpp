#include "DialogVideoPlayer.h"
#include "ui_DialogVideoPlayer.h"

#include <time.h>
#include <sys/timeb.h>

DialogVideoPlayer::DialogVideoPlayer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogVideoPlayer) {
    ui->setupUi(this);


    connect(ui->buttonOpen, SIGNAL(clicked()), this, SLOT(openMoviePress()));
    connect(ui->buttonPlay, SIGNAL(clicked()), this, SLOT(playButtonPress()));
    connect(ui->buttonImage, SIGNAL(clicked()), this, SLOT(openImageFilePress()));
    connect(ui->checkBoxSmooth, SIGNAL(clicked(bool)), this, SLOT(settingChanged()));
    connect(ui->rbFragment, SIGNAL(clicked(bool)), this, SLOT(settingChanged()));
    connect(ui->rbSegment, SIGNAL(clicked(bool)), this, SLOT(settingChanged()));
    connect(ui->rbWholeFile, SIGNAL(clicked(bool)), this, SLOT(settingChanged()));
    connect(ui->checkBoxLoop, SIGNAL(clicked(bool)), this, SLOT(settingChanged()));
    connect(ui->checkBoxMovie, SIGNAL(clicked(bool)), this, SLOT(changeMovieMode()));
    connect(ui->spinBoxFragment, SIGNAL(valueChanged(int)), this, SLOT(spinBoxFragmentChanged(int)));
    connect(ui->spinBoxSegment, SIGNAL(valueChanged(int)), SLOT(spinBoxSegmentChanged(int)));
    connect(ui->spinBoxIndex, SIGNAL(valueChanged(int)), SLOT(spinBoxIndexChanged(int)));

    // Set up ui controls
    ui->checkBoxMovie->setEnabled(false);
    ui->buttonPlay->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));


    // Set up the main visualization view
    ui->visualizationView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->visualizationView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    visualizationScene = new QGraphicsScene(this);
    visualizationScene->setSceneRect(-10000,-10000,20000,20000); // to allow centering.....
    ui->visualizationView->setScene(visualizationScene);



    // Set up the media player (for videos)
    visualizationVideoItem = new QGraphicsVideoItem();
    visualizationVideoItem->setAspectRatioMode(Qt::KeepAspectRatio);
    mediaPlayer = new QMediaPlayer(this);
    mediaPlayer->setNotifyInterval(1000);
    mediaPlayer->setVideoOutput(visualizationVideoItem);
    visualizationScene->addItem(visualizationVideoItem);


    // Set up screen layer (mirrors screen size)
    screenLayerItem = new QGraphicsRectItem();
    visualizationScene->addItem(screenLayerItem);

    //Set up the background image Pixmap
    backgroundImageItem = new QGraphicsPixmapItem();
    visualizationScene->addItem(backgroundImageItem);

    // Set up the pixmap (to draw the view location)
    visualizationPixmapItem = new QGraphicsPixmapItem();
    visualizationScene->addItem(visualizationPixmapItem);

    // Media player connections
    connect(mediaPlayer, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(handleError()));


    // Set up the timeline
    ui->timeLineView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->timeLineView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    timeLineScene = new QGraphicsScene(this);
    ui->timeLineView->setScene(timeLineScene);


    // Set up timeline pixmap
    timeLinePixmapItem = new QGraphicsPixmapItem();
    timeLineScene->addItem(timeLinePixmapItem);

    // Set up the fixations view
    ui->fixationsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->fixationsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    fixationsScene = new QGraphicsScene(this);
    ui->fixationsView->setScene(fixationsScene);

    // Set up timeline pixmap
    fixationsPixmapItem = new QGraphicsPixmapItem();
    fixationsScene->addItem(fixationsPixmapItem);

    // Set up line indicating time on time line
    timeLineTimeItem = new QGraphicsLineItem();
    timeLineTimeItem->setPen(QPen(Qt::black, 1, Qt::DashLine));
    timeLineTimeItem->setPos(0, 0);
    timeLineScene->addItem(timeLineTimeItem);

    // Set up pens
    smoothPens = new QPen[2];
    roughPens = new QPen[4];

    smoothPens[0] = QPen(Qt::red, 1, Qt::SolidLine);
    smoothPens[1] = QPen(Qt::blue, 1, Qt::SolidLine);
    roughPens[0] = QPen(Qt::red, 1, Qt::SolidLine);
    roughPens[1] = QPen(Qt::red, 1, Qt::SolidLine);
    roughPens[2] = QPen(Qt::blue, 1, Qt::SolidLine);
    roughPens[3] = QPen(Qt::blue, 1, Qt::SolidLine);

}

DialogVideoPlayer::~DialogVideoPlayer() {
    playing = false;
    mediaPlayer->stop();
    delete[] smoothPens;
    delete[] roughPens;
    delete fixationsPixmapItem;
    delete fixationsScene;
    delete timeLineTimeItem;
    delete timeLinePixmapItem;
    delete timeLineScene;
    delete visualizationPixmapItem;
    delete visualizationVideoItem;
    delete visualizationScene;
    delete mediaPlayer;

    delete ui;
}

void DialogVideoPlayer::loadData(GrafixParticipant* participant, mat &p_roughM_in, mat &p_smoothM_in, mat &p_fixAllM_in, mat &p_segmentsM_in) {

    _participant = participant;

    p_roughM = &p_roughM_in;
    p_smoothM = &p_smoothM_in;
    p_fixAllM = &p_fixAllM_in;

    segmentsM = p_segmentsM_in;
    if (segmentsM.n_rows == 0) {
        segmentsM.zeros(1,3);
        segmentsM(0,0) = 0;
        segmentsM(0,1) = 0;
        segmentsM(0,2) = p_roughM->n_rows - 1;
    }
    // order the segments
    uvec indices = sort_index(segmentsM.cols(1,1));
    this->segmentsM = segmentsM.rows(indices);



    secsFragment = this->_participant->GetProject()->GetProjectSetting(Consts::SETTING_SECS_FRAGMENT, Consts::ACTIVE_CONFIGURATION()).toInt();
    hz = this->_participant->GetProject()->GetProjectSetting(Consts::SETTING_HZ, Consts::ACTIVE_CONFIGURATION()).toInt();
    samplesPerFragment = secsFragment * hz;
    expWidth = this->_participant->GetProject()->GetProjectSetting(Consts::SETTING_EXP_WIDTH, Consts::ACTIVE_CONFIGURATION()).toInt();
    expHeight = this->_participant->GetProject()->GetProjectSetting(Consts::SETTING_EXP_HEIGHT, Consts::ACTIVE_CONFIGURATION()).toInt();

    if (p_smoothM->is_empty()) {
        settingPlaySmooth = false;
        ui->checkBoxSmooth->setChecked(false);
        ui->checkBoxSmooth->setEnabled(false);
    } else {
        ui->checkBoxSmooth->setChecked(true);
    }

    firstSampleMS = (*p_roughM).at(0, 0); //this is the ms of when the player first started
    currentIndex = 0;
    currentFragment = -1;
    currentSegment = 0;

    settingChanged();

    updatePlaybackState(0, true);
    resizeDisplay();
}

void DialogVideoPlayer::resizeEvent (QResizeEvent *event) {
    QDialog::resizeEvent(event);
    resizeDisplay();

    paintTimeLine();
    paintFixations();
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

        return;
    }

    QRect fixationsRect = ui->fixationsView->geometry();
    bool onFixations = fixationsRect.contains(mouseEvent->pos());

    if (leftButton && onFixations) {
        // Set position to beginning of fixation (if on one)
        double samples_over_fixations = (double)samplesPerFragment * (double)(mouseEvent->pos().x() - fixationsRect.x()) / (double)fixationsRect.width();
        int selected_position = (currentFragment * samplesPerFragment) + (int)floor(samples_over_fixations);
        for (uword i = 0; i < p_fixAllM->n_rows  ; i++) {
            bool fixation_starts_before_selected = p_fixAllM->at(i, FIXCOL_START) <= selected_position;
            bool fixation_ends_after_selected = p_fixAllM->at(i, FIXCOL_END) >= selected_position;

            //go to start of fixation
            if (fixation_starts_before_selected && fixation_ends_after_selected) {
                updatePlaybackState(p_fixAllM->at(i, FIXCOL_START), true);
            }
        }
    }
}

void DialogVideoPlayer::resizeDisplay() {
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

    //qDebug() << "exp width: " << expWidth << " exp height: " << expHeight;
    //qDebug() << "vid width: " << vid_width << " vid height: " << vid_height;
    //qDebug() << "disp width: " << display_width << " disp height: " << display_height;
    //qDebug() << "offsetts: " << vid_offset_x << " " << vid_offset_y;

    screenLayerItem->setRect(0, 0, display_width, display_height);
    ui->visualizationView->fitInView(screenLayerItem, Qt::KeepAspectRatio);
    ui->visualizationView->centerOn(display_width / 2, display_height / 2);

    // Move everything to centre
    int x = ui->visualizationView->width() - display_width;

    timeLineScene->setSceneRect(ui->timeLineView->rect());
}

void DialogVideoPlayer::spinBoxSegmentChanged(int value) {

    int target_index = qBound(0, value, (int)segmentsM.n_rows);

    if (target_index != value) {
        ui->spinBoxSegment->setValue(target_index);
    }

    if (target_index == 0) {
        updatePlaybackState(0, true);
    } else if (target_index - 1 < (int)segmentsM.n_rows) {
        updatePlaybackState(segmentsM(FIXCOL_START,target_index - 1), true);
    }


}

void DialogVideoPlayer::spinBoxIndexChanged(int value) {
    int target_index = qBound(0, value, (int)p_roughM->n_rows - 1);

    if (target_index != value) {
        ui->spinBoxIndex->setValue(target_index);
    }

    updatePlaybackState(target_index, true);
}

void DialogVideoPlayer::spinBoxFragmentChanged(int value) {
    int target_index = qBound(0, value * samplesPerFragment, (int)p_roughM->n_rows - 1);

    updatePlaybackState(target_index, true);
}



void DialogVideoPlayer::closeEvent(QCloseEvent *event) {
    mediaPlayer->stop();
    playing = false;
    QDialog::closeEvent(event);
}

void DialogVideoPlayer::openMoviePress() {

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Movie"), QDir::homePath());

    if (!fileName.isEmpty()) {
        mediaPlayer->setMedia(QUrl::fromLocalFile(fileName));

        // Wait to load...
        while(mediaPlayer->mediaStatus() != QMediaPlayer::LoadedMedia && mediaPlayer->mediaStatus() != QMediaPlayer::InvalidMedia) {
            qApp->processEvents();
        }

        if (mediaPlayer->mediaStatus() != QMediaPlayer::InvalidMedia) {
            resizeDisplay();
            ui->checkBoxMovie->setEnabled(true);
            ui->checkBoxMovie->setChecked(true);
            return;
        } else {
            QMessageBox messageBox;
            messageBox.critical(0, "Error", "An error has occured when loading this movie!");
            messageBox.setFixedSize(500, 200);
        }
    }
}

void DialogVideoPlayer::playButtonPress() {

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
        if (mediaPlayer->mediaStatus() == QMediaPlayer::InvalidMedia) {
            ui->checkBoxMovie->setChecked(false);
            ui->checkBoxMovie->setEnabled(false);
            stopPlaying();
            return;
        }

        while(mediaPlayer->mediaStatus() == QMediaPlayer::LoadingMedia) {
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

int DialogVideoPlayer::getMilliCount() {
    timeb tb;
    ftime(&tb);
    int nCount = tb.millitm + (tb.time & 0xfffff) * 1000;
    return nCount;
}

void DialogVideoPlayer::updatePlaybackStateTime(double newTimeMS) {

    currentTimeMS = newTimeMS;

    //Search for current index

    int last_index = (*p_roughM).n_rows - 1;

    int previousIndex = qMax(currentIndex, 0);

    if (p_roughM->at(previousIndex, 0) > currentTimeMS ) {
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

void DialogVideoPlayer::updatePlaybackState(int index, bool resetTimeSource) {

    bool trigger_fragment_mode = settingPlayMode == PlayModeFragment &&
                                 (index >= ((currentFragment + 1) * samplesPerFragment) || index == -1);

    if (trigger_fragment_mode) {
        if (settingLoop) {
            updatePlaybackState(currentFragment * samplesPerFragment, true);
        } else {
            stopPlaying();
        }
        return;
    }

    bool trigger_whole_file_mode = settingPlayMode == PlayModeWholeFile &&
                                   (index >= (int)p_roughM->n_rows || index == -1);

    if (trigger_whole_file_mode) {
        if (settingLoop) {
            updatePlaybackState(0, true);
        } else {
            stopPlaying();
        }
        return;
    }

    bool segment_in_bounds = currentSegment > 0 && currentSegment - 1 < (int)segmentsM.n_rows;
    bool trigger_segment_mode = segment_in_bounds && settingPlayMode == PlayModeSegment &&
            (index >= segmentsM(currentSegment - 1, SEGCOL_END) || index == -1);
    if (trigger_segment_mode) {

        if (settingLoop) {
            updatePlaybackState(segmentsM(currentSegment - 1, SEGCOL_START), true);
        } else {
            stopPlaying();
        }
        return;
    }

    // Display index
    currentIndex = index;
    currentTimeMS = p_roughM->at(qMax(0, currentIndex), 0 );

    ui->labelTime->setText(QString("Time: %1").arg(currentTimeMS / 1000));
    ui->spinBoxIndex->setValue(currentIndex);

    // Calculate current fragment
    int current_fragment = currentIndex > -1 ? (int)floor((double)currentIndex / samplesPerFragment) : -1;

    if (current_fragment != currentFragment) {
        currentFragment = current_fragment;
        paintTimeLine();
        paintFixations();
    }

    ui->spinBoxFragment->blockSignals(true);
    ui->spinBoxFragment->setValue(currentFragment);
    ui->spinBoxFragment->blockSignals(false);

    // Calculate current segment
    int current_segment = 0; // 0 means at start of file
    for (uword i = 0; i < segmentsM.n_rows -1; ++i) {
        if (currentIndex >= segmentsM(i, SEGCOL_START)) {
            current_segment = i;
        }
    }

    if (currentSegment != current_segment + 1) {
        currentSegment = current_segment + 1;


        if (!ui->checkBoxMovie->isChecked()) {
            paintBackgroundImage();
        }
    }

    ui->spinBoxSegment->blockSignals(true);
    ui->spinBoxSegment->setValue(currentSegment);
    ui->spinBoxSegment->blockSignals(false);

    paintCurrentVisualizationFrame();
    paintCurrentTimeLineLineFrame();

    if (resetTimeSource) {
        if (ui->checkBoxMovie->isChecked()) {
            mediaPlayer->setPosition(currentTimeMS - firstSampleMS);
        } else {
            clockStartTime = getMilliCount();
            playStartSampleMS = p_roughM->at(qMax(0, currentIndex), 0 );
        }
    }
}

void DialogVideoPlayer::paintCurrentVisualizationFrame() {


    bool show_fixation_numbers = true;

    QPixmap pixmap(display_width, display_height);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);


    QPen myPen(QColor(0,0,0,127), 1, Qt::SolidLine);
    myPen.setCapStyle(Qt::RoundCap);
    painter.setPen(myPen);

    if (show_fixation_numbers) {
        QFont serifFont("Times", 64, QFont::Bold);
        painter.setFont(serifFont);
        uvec fixIndex =  arma::find((*p_fixAllM).col(FIXCOL_START) <= currentIndex);
        mat aux = (*p_fixAllM).rows(fixIndex);
        fixIndex =  arma::find(aux.col(FIXCOL_END) >= currentIndex);
        if (fixIndex.n_rows != 0) {
            painter.drawText( QRect(0, 0, display_width, display_height), Qt::AlignCenter, QString::number(fixIndex(0)) );
        }
    }


    mat *matrixToUse = settingPlaySmooth ? p_smoothM : p_roughM;

    if (matrixToUse->is_empty()) return;

    QPen *pensToUse = settingPlaySmooth ? smoothPens : roughPens;
    int nPensToUse = settingPlaySmooth ? 1 : 2;
    double width_multi = settingPlaySmooth ? (double)display_width / (double)expWidth : display_width;
    double height_multi = settingPlaySmooth ? (double)display_height / (double)expHeight : display_height;
    double pen_size_multi = 20;

    for(int i = 0; i < nPensToUse; ++i) {
        myPen.setColor(pensToUse[i].color());

        // Pupil width
        if (p_roughM->n_cols >= 8 && p_roughM->at(currentIndex, 6) > 0 && p_roughM->at(currentIndex, 7) > 0) {
            double width = (p_roughM->at(currentIndex, 6) + p_roughM->at(currentIndex, 7)) / 6;
            int penWidth = (int)floor(width * pen_size_multi);
            myPen.setWidth(penWidth);
        } else {
            myPen.setWidth(pen_size_multi);
        }

        painter.setPen(myPen);

        uword index = 2 * (i + 1);
        painter.drawPoint(matrixToUse->at(currentIndex, index) * width_multi, matrixToUse->at(currentIndex, index + 1) * height_multi);
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
    timeLineTimeItem->setLine(x, 0, x, y);
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

    QPixmap timeLinePixmap = QPixmap(ui->timeLineView->width(), ui->timeLineView->height());
    timeLinePixmap.fill(Qt::white);
    QPainter painter(&timeLinePixmap);

    double width = timeLinePixmap.width();
    double height = settingPlaySmooth ? timeLinePixmap.height() / (double)expWidth : timeLinePixmap.height();
    mat *matrixToUse = settingPlaySmooth ? p_smoothM : p_roughM;
    QPen *pensToUse = settingPlaySmooth ? smoothPens : roughPens;
    int nPensToUse = settingPlaySmooth ? 2 : 4;

    // Get starting index
    uword startIndex = currentFragment * samplesPerFragment;

    // Sanity check for number of rows
    if (startIndex >= matrixToUse->n_rows) return;

    // Get draw limit
    uword limit = qMin(matrixToUse->n_rows - 1, startIndex + samplesPerFragment) - startIndex;


    for (uword i = 0; i < limit; ++i) {
        uword index = i + startIndex;
        double x = (double)i * width / (double)samplesPerFragment;

        for(int j = 0; j < nPensToUse; ++j) {
            uword col = (uword)j + 2;
            if (matrixToUse->at(index, col) != -1) {
                painter.setPen(pensToUse[j]);
                painter.drawPoint(x, matrixToUse->at(index, col) * height);
            }
        }
    }

    painter.end();

    timeLinePixmapItem->setPixmap(timeLinePixmap);
}

void DialogVideoPlayer::paintFixations() {

    QPixmap fixationsPixmap = QPixmap(ui->fixationsView->width(), ui->fixationsView->height());
    fixationsPixmap.fill(Qt::white);
    QPainter painter(&fixationsPixmap);

    double positionMultiplier = (double)ui->fixationsView->width() / (double)samplesPerFragment;

    int counter = 0;
    QFont font = painter.font() ;
    font.setPointSize ( 8 );
    painter.setFont(font);

    std::stringstream number;
    number.str("");

    double posStart = 0;
    double posEnd = 0;

    uword start_index = currentFragment * samplesPerFragment;
    uword end_index = start_index + samplesPerFragment;

    int auxIndex = 0;
    if (p_fixAllM->n_rows > 0) {
        uvec fixIndex =  arma::find(p_fixAllM->col(FIXCOL_END) >= start_index);

        if (!fixIndex.is_empty()) {
            auxIndex = fixIndex(0);
        } else {
            auxIndex = p_fixAllM->n_rows ;
        }

    }

    for (uword i = auxIndex; i < p_fixAllM->n_rows  ; i++) {

        bool fixation_starts_after_fragment_start = p_fixAllM->at(i, FIXCOL_START) >= start_index;
        bool fixation_starts_before_fragment_end = p_fixAllM->at(i, FIXCOL_START) <=  end_index;
        bool fixation_starts_before_fragment_start = p_fixAllM->at(i, FIXCOL_START) <= start_index;
        bool fixation_ends_after_fragment_start = p_fixAllM->at(i, FIXCOL_END) >= start_index;

        if ((fixation_starts_after_fragment_start && fixation_starts_before_fragment_end) ||
                (fixation_starts_before_fragment_start && fixation_ends_after_fragment_start)) {

            // If it's a fixation that didn't end in the previous segment
            if (fixation_starts_before_fragment_start && fixation_ends_after_fragment_start )
                posStart = -1;
            else
                posStart = (p_fixAllM->at(i, FIXCOL_START) - start_index ) * positionMultiplier;


            posEnd = ((p_fixAllM->at(i, FIXCOL_END) - start_index ) * positionMultiplier) - posStart;

            if (p_fixAllM->at(i, FIXCOL_SMOOTH_PURSUIT) == Consts::SMOOTHP_YES) {
                painter.setBrush(QBrush("#ffbc00"));
            } else {
                painter.setBrush(QBrush("#1ac500"));
            }

            painter.drawRect(QRect(posStart, 12, posEnd, 16));
            number.str("");
            number << counter;
            painter.drawText( QPoint(posStart, 9), number.str().c_str() );

            counter ++; // Next fixation

        } else if (p_fixAllM->at(i, FIXCOL_END) >= end_index) {
            break;
        }
    }

    painter.end();
    fixationsPixmapItem->setPixmap(fixationsPixmap);
}

void DialogVideoPlayer::paintBackgroundImage(){

    if (currentSegment <= 0 || currentSegment > (int)segmentsM.n_rows || segmentsM.is_empty()) {
        clearBackgroundImage();
        return;
    }

    int number =  segmentsM(currentSegment - 1, 0);

    // Find the correct id
    if (pathsImages.contains(number)) {
        QString path = pathsImages.value(number);
        QPixmap pixmap = QPixmap(path);

        QSize size(display_width , display_height);
        pixmap = pixmap.scaled(size);
        backgroundImageItem->setPixmap(pixmap);
    } else {
        clearBackgroundImage();
    }


}

void DialogVideoPlayer::clearBackgroundImage() {
    //clear the background
    QPixmap clear = QPixmap(expWidth, expHeight);
    clear.fill(Qt::transparent);
    backgroundImageItem->setPixmap(clear);
}

void DialogVideoPlayer::loadImages(){

}

void DialogVideoPlayer::openImageFilePress(){
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),tr("Files (*.*)"));
    if (!fileName.isEmpty()){
        int number =  segmentsM(currentSegment - 1, 0);
        pathsImages[number] = fileName;
        paintBackgroundImage();
    }
}


void DialogVideoPlayer::changeMovieMode() {
    stopPlaying();
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

    if (!ui->checkBoxMovie->isChecked()) {
        paintBackgroundImage();
    }
}


void DialogVideoPlayer::handleError() {
    mediaPlayer->stop();
    ui->buttonPlay->setEnabled(false);
}
