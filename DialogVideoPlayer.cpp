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

    connect(ui->buttonPlay, SIGNAL(clicked()), this, SLOT(playButton()));

    ui->sliderPosition->setRange(0, 0);

    connect(ui->sliderPosition, SIGNAL(sliderMoved(int)),
            this, SLOT(setPosition(int)));


    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    mediaPlayer = new QMediaPlayer(this);
    mediaPlayer->setNotifyInterval(1000); //updates slider every 1000 ms
    scene = new QGraphicsScene(this);
    item = new QGraphicsVideoItem();
    item->setAspectRatioMode(Qt::KeepAspectRatio);
    ui->graphicsView->setScene(scene);
    mediaPlayer->setVideoOutput(item);
    scene->addItem(item);

    rect_overlay = new QGraphicsRectItem();
    //rect_overlay->setPen
    scene->addItem(rect_overlay);

    pixmap_overlay = new QGraphicsPixmapItem();
    scene->addItem(pixmap_overlay);


    //ui->graphicsView->setViewport(new QGLWidget);

    connect(mediaPlayer, SIGNAL(stateChanged(QMediaPlayer::State)),
            this, SLOT(mediaStateChanged(QMediaPlayer::State)));
    connect(mediaPlayer, SIGNAL(positionChanged(qint64)), this, SLOT(positionChanged(qint64)));
    connect(mediaPlayer, SIGNAL(durationChanged(qint64)), this, SLOT(durationChanged(qint64)));
    connect(mediaPlayer, SIGNAL(error(QMediaPlayer::Error)), this, SLOT(handleError()));
}

DialogVideoPlayer::~DialogVideoPlayer()
{
    mediaPlayer->stop();
    delete pixmap_overlay;
    delete item;
    delete scene;
    delete mediaPlayer;
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

    if (this->p_smoothM->is_empty())
    {
        ui->checkBoxSmooth->setChecked(false);
        ui->checkBoxSmooth->setEnabled(false);
    }
    else
    {
        ui->checkBoxSmooth->setChecked(true);
    }
}

void DialogVideoPlayer::resizeEvent (QResizeEvent *event)
{
    fncCalculateAspectRatios();
}

void DialogVideoPlayer::fncCalculateAspectRatios()
{
    // Calculate resize ratios for resizing
    vid_width = item->size().width();
    vid_height = item->size().height();
    double ratioW = expWidth / vid_width;
    double ratioH = expHeight / vid_height;

    // smaller ratio will ensure that the image fits in the view
    if (ratioW > ratioH)
    {
        //data is wider than video

        //make heights the same,
        display_height = vid_height;
        display_width =  expWidth * vid_height / expHeight;
        vid_offset_y = 0;
        vid_offset_x = (display_width - vid_width ) / 2;

    }
    else
    {
        //data is taller than data (or the same)

        //make widths the same
        display_width = vid_width;
        display_height = expHeight * vid_width / expWidth;
        vid_offset_x = 0;
        vid_offset_y = (display_height - vid_height) / 2;
    }

    rect_overlay->setRect(0,0,display_width,display_height);
    item->setPos(vid_offset_x, vid_offset_y);
    ui->graphicsView->fitInView(this->rect_overlay, Qt::KeepAspectRatio);
}

void DialogVideoPlayer::closeEvent(QCloseEvent *event)
{
    mediaPlayer->stop();
}

void DialogVideoPlayer::openFile()
{

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Movie"),QDir::homePath());

    if (!fileName.isEmpty()) {
        mediaPlayer->setMedia(QUrl::fromLocalFile(fileName));

        fncCalculateAspectRatios();


        double video_aspect = (double)vid_width / (double)vid_height;
        double data_aspect = (double)expWidth / (double)expHeight;

        if (video_aspect == data_aspect)
        {
            ui->statusLabel->setText("Video loaded.");
        }
        else
        {
            if (video_aspect > data_aspect)
            {
                ui->statusLabel->setText("Video loaded: Video aspect is wider than data.");
            }
            else
            {
                ui->statusLabel->setText("Video loaded: Video aspect is taller than data.");
            }
        }

        ui->buttonPlay->setEnabled(true);
    }
}

void DialogVideoPlayer::playButton()
{
    switch(mediaPlayer->state())
    {
    case QMediaPlayer::PlayingState:
        mediaPlayer->pause();
        break;
    default:
        playMovie();
        break;
    }
}

void DialogVideoPlayer::playMovie()
{
    //wait to start (buggy otherwise)
    while(mediaPlayer->mediaStatus() != QMediaPlayer::LoadedMedia)
    {
        qApp->processEvents();
    }

    mediaPlayer->play();

    double timer_offset = (*p_roughM).at(0,0); //this is the ms of when the player first started
    uword current_index = 0;
    uword last_index = (*p_roughM).n_rows - 1;

    //bool show_fixation_numbers = !this->p_fixAllM->is_empty();
    bool show_fixation_numbers = false;

    double width_multi = (double)display_width / (double)expWidth;
    double height_multi = (double)display_height / (double)expHeight;
    double pen_size_multi = display_width / 100;

    QFont serifFont("Times", 64, QFont::Bold);

    while(mediaPlayer->state() == QMediaPlayer::PlayingState)
    {
        //get position in ms with offset since start of movie and map to our data
        double current_movie_time = (double)(mediaPlayer->position()) + timer_offset;

        QPixmap pixmap(display_width, display_height);
        pixmap.fill(Qt::transparent);

        for(uword search_index = current_index; search_index <= last_index; search_index++)
        {
            double searched_time = (*p_roughM).at(search_index,0);
            if (searched_time >= current_movie_time)
            {
                //found latest frame - paint
                current_index = search_index;

                //use previous frame
                if (current_index > 0) current_index--;


                QPainter painter(&pixmap);



                if (ui->checkBoxSmooth->isChecked())
                {
                    QPen myPen(Qt::green, pen_size_multi * 4, Qt::SolidLine);
                    myPen.setColor(QColor(0, 250, 0, 255));
                    myPen.setCapStyle(Qt::RoundCap);

                    if ((*p_roughM)(current_index ,6) > 0 && (*p_roughM)(current_index ,7) > 0)
                    {
                        myPen.setWidth(((*p_roughM)(current_index ,6) + (*p_roughM)(current_index ,7)) * pen_size_multi);
                    }

                    painter.setPen(myPen);
                    painter.drawPoint((*p_smoothM)(current_index ,2 ) * width_multi, (*p_smoothM)(current_index ,3 ) * height_multi); // XL

                    // Option 1 checked: Paint fixation numbers
                    if (show_fixation_numbers)
                    {
                        painter.setFont(serifFont);


                        uvec fixIndex =  arma::find((*p_fixAllM).col(FIXCOL_START) <= current_index);
                        mat aux = (*p_fixAllM).rows(fixIndex);
                        fixIndex =  arma::find(aux.col(FIXCOL_END) >= current_index);
                        if (fixIndex.n_rows != 0)
                        {
                            // If there is a fixation between the values, paint it!

                            myPen.setColor(QColor(0, 50, 128, 255));
                            painter.setPen(myPen);

                            painter.drawText( QPoint( display_width/2,display_height/2 ),QString::number(fixIndex(0)) );

                        }
                    }


                }
                else
                {
                    QPen myPen(Qt::red, pen_size_multi * 4, Qt::SolidLine);
                    myPen.setCapStyle(Qt::RoundCap);
                    //set pupil dilation
                    if ((*p_roughM)(current_index ,6) > 0) myPen.setWidth((*p_roughM)(current_index ,6) * pen_size_multi);

                    myPen.setColor(QColor(255, 0, 0, 255));
                    painter.setPen(myPen);
                    painter.drawPoint( (*p_roughM)(current_index ,2 )  * display_width, (*p_roughM)(current_index ,3 )  * display_height);

                    // Option 2 checked : Paint pupil dilation
                    if ((*p_roughM)(current_index ,7) > 0) myPen.setWidth((*p_roughM)(current_index ,7) * pen_size_multi);

                    myPen.setColor(QColor(0, 50, 128, 255));
                    painter.setPen(myPen);
                    painter.drawPoint( (*p_roughM)(current_index ,4 )  * display_width, (*p_roughM)(current_index ,5 )  * display_height);
                }


                painter.end();

                break;
            }
        }

        pixmap_overlay->setPixmap(pixmap);
        qApp->processEvents();
    }

    mediaPlayer->stop();


}


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
    mediaPlayer->setPosition(position);
}

void DialogVideoPlayer::handleError()
{
    ui->buttonPlay->setEnabled(false);
    //errorLabel->setText("Error: " + mediaPlayer.errorString());
}
