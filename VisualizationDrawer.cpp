#include "VisualizationDrawer.h"

VisualizationDrawer::VisualizationDrawer(const mat &roughM, const mat &smoothM, const mat &fixAllM, const mat &segmentsM, const GrafixParticipant *participant) {
    this->roughM = &roughM;
    this->smoothM = &smoothM;
    this->fixAllM = &fixAllM;
    this->segmentsM = &segmentsM;
    this->settingsLoader = new GrafixSettingsLoader(*participant);
    this->_increment = 0;
    this->_expHeight = 0;
    this->_expWidth = 0;
    this->_samplesPerFragment = 0;
    this->_playingSmooth = true;
    this->_paintFixationNumbers = true;
    this->_paintPupilDilation = true;

    this->_smoothPen = QPen(Qt::blue,1,Qt::SolidLine, Qt::RoundCap);
    this->_roughPen1 = QPen(Qt::red, 1.2, Qt::SolidLine, Qt::RoundCap);
    this->_roughPen2 = QPen(QColor(255,50,0), 1.2, Qt::SolidLine, Qt::RoundCap);
    this->_bluePen = QPen(Qt::blue, 1.2, Qt::SolidLine, Qt::RoundCap);
    this->_redPen = QPen(Qt::red, 1.2, Qt::SolidLine, Qt::RoundCap);
    this->_bluePen2 = QPen(QColor(0,50,255), 1.2, Qt::SolidLine, Qt::RoundCap);
    this->_redPen2 = QPen(QColor(255,50,0), 1.2, Qt::SolidLine, Qt::RoundCap);

    _currentIndex = 0;
    _currentFragment = ARMA_MAX_UWORD;
    _currentFragmentStartIndex = 0;
    _currentFragmentEndIndex = 0;
    _currentFixationIndex = 0;

    updateDrawingSettings();

}

VisualizationDrawer::~VisualizationDrawer() {
    delete this->settingsLoader;
}

void VisualizationDrawer::updateVisualizationFrame(int width, int height) {
    visualizationPixmap = QPixmap(width, height);
}

void VisualizationDrawer::updateFixationFrame(int width, int height) {
    fixationsPixmap = QPixmap(width, height);
}

void VisualizationDrawer::updateTimeLineFrame(int width, int height) {
    processLinePixmap = QPixmap(width, height);
    timeLinePixmap = QPixmap(width, height);
}

void VisualizationDrawer::updateDrawingSettings() {
    double secsFragment = settingsLoader->LoadSetting(Consts::SETTING_SECS_FRAGMENT).toDouble();
    double hz = settingsLoader->LoadSetting(Consts::SETTING_HZ).toDouble();
    _samplesPerFragment = secsFragment * hz;
    _increment = (double)processLinePixmap.width() / _samplesPerFragment;
    _expWidth = settingsLoader->LoadSetting(Consts::SETTING_EXP_WIDTH).toInt();
    _expHeight = settingsLoader->LoadSetting(Consts::SETTING_EXP_HEIGHT).toInt();


}

void VisualizationDrawer::paintCurrentFrame(uword currentIndex) {

    this->_currentIndex = currentIndex;

    // Calculate current fragment
    uword currentFragment = (uword)floor((double)currentIndex / _samplesPerFragment);

    // Check if fragment has changed
    if (currentFragment != _currentFragment) {

        // Update all fragment related variables
        this->_currentFragment = currentFragment;
        this->_currentFragmentStartIndex = this->_currentFragment * _samplesPerFragment;
        this->_currentFragmentEndIndex = this->_currentFragmentStartIndex = this->_currentFragmentStartIndex + _samplesPerFragment;

        if (_currentFragmentEndIndex > roughM->n_rows)
            _currentFragmentEndIndex = roughM->n_rows;

        currentFragmentFixations.clear();
        for (uword i = 0; i < fixAllM->n_rows; i++) {
            if (_currentFragmentStartIndex >= fixAllM->at(i, FIXCOL_START) && _currentFragmentEndIndex <= fixAllM->at(i, FIXCOL_END)) {
                currentFragmentFixations.insert_rows(currentFragmentFixations.n_rows, fixAllM->row(i));
                break;
            }
        }
    }


    // Calculate current fixation
    this->_currentFixationIndex = -1;

    for (uword i = 0; i < currentFragmentFixations.n_rows; i++) {
        if (currentIndex >= currentFragmentFixations(i, FIXCOL_START) && currentIndex <= currentFragmentFixations(i, FIXCOL_END)) {
            this->_currentFixationIndex = i;
            break;
        }
    }

    paintTimeLine();
    paintVisualization();

}



void VisualizationDrawer::paintVisualization() {

    visualizationPixmap.fill(Qt::transparent);
    QPainter visualizationPainter(&visualizationPixmap);

    // Paint fixation number: Paint fixations option checked:
    if (_paintFixationNumbers && _currentFixationIndex > -1) {
        QString fixationNumber = QString::number(_currentFixationIndex + 1);
        QFont font = visualizationPainter.font();
        if (this->_playingSmooth) {
            font.setPointSize (35);
            visualizationPainter.setFont(font);
            visualizationPainter.drawText( QPoint( (smoothM->at(_currentIndex, 2 ) / _expWidth * visualizationPixmap.width()) + 20, smoothM->at(_currentIndex ,3 ) / _expHeight * visualizationPixmap.height()), fixationNumber );

        } else {
            font.setPointSize (85);
            visualizationPainter.setFont(font);
            visualizationPainter.drawText( QPoint( visualizationPixmap.width()/2, visualizationPixmap.height()/2 ), fixationNumber );
        }
    }

    if (this->_playingSmooth) {
        if (_paintPupilDilation && roughM->n_cols == 8){
            if (roughM->at(_currentIndex,6) > 0){
                _smoothPen.setWidth(roughM->at(_currentIndex ,6) * 10);
            }else if (roughM->at(_currentIndex,7) > 0){
                _smoothPen.setWidth(roughM->at(_currentIndex ,7) * 10);
            }
        }
        // Draw point
        visualizationPainter.setPen(_smoothPen);
        visualizationPainter.drawPoint( smoothM->at(_currentIndex ,2 ) / _expWidth * visualizationPixmap.width(), smoothM->at(_currentIndex ,3 ) / _expHeight * visualizationPixmap.height()); // XL

    } else {
        // Option 2 checked : Paint pupil dilation
        if (_paintPupilDilation && roughM->n_cols == 8){
            if (roughM->at(_currentIndex ,6) > 0){
                _roughPen1.setWidth(roughM->at(_currentIndex ,6) * 10);
            }
        }

        _roughPen1.setColor(QColor(255, 0, 0, 255));
        visualizationPainter.setPen(_roughPen1);
        visualizationPainter.drawPoint( roughM->at(_currentIndex ,2 )  * visualizationPixmap.width(), roughM->at(_currentIndex ,3 )  * visualizationPixmap.height()); // XL

        // Option 2 checked : Paint pupil dilation
        if (_paintPupilDilation  && roughM->n_cols == 8){
            if (roughM->at(_currentIndex ,7) > 0){
                _roughPen2.setWidth(roughM->at(_currentIndex ,7) * 10);
            }
        }

        _roughPen2.setColor(QColor(0, 50, 128, 255));
        visualizationPainter.setPen(_roughPen2);
        visualizationPainter.drawPoint( roughM->at(_currentIndex ,4 )  * visualizationPixmap.width(), roughM->at(_currentIndex ,5 )  * visualizationPixmap.height()); // XL

    }

    visualizationPainter.end();
}

void VisualizationDrawer::paintTimeLine() {

    timeLinePixmap.fill(Qt::white);
    QPainter painter(&timeLinePixmap);

    int height = timeLinePixmap.height();
    double height2 = (double)height / (double)_expWidth;

    for (uword i = _currentFragmentStartIndex; i < _currentFragmentEndIndex; ++i) {

        int x = (i - _currentFragmentStartIndex ) * _samplesPerFragment;

        if (_playingSmooth) {
            if (roughM->at(i,2 ) != -1){
                painter.setPen(_redPen);
                painter.drawPoint(x, roughM->at(i,2 ) * height); // XL
            }

            if (roughM->at(i,3 ) != -1){
                painter.setPen(_redPen2);
                painter.drawPoint(x, roughM->at(i,3 ) * height); // YL
            }

            if (roughM->at(i,4 ) != -1){
                painter.setPen(_bluePen);
                painter.drawPoint(x, roughM->at(i,4 ) * height); // XR
            }

            if (roughM->at(i,5 ) != -1){
                painter.setPen(_bluePen2);
                painter.drawPoint(x, roughM->at(i,5 ) * height); // YR
            }
        } else {
            if (smoothM->at(i,2 ) != -1){
                painter.setPen(_redPen);
                painter.drawPoint(x, smoothM->at(i,2 ) * height2);
            }

            if (smoothM->at(i,3 ) != -1){
                painter.setPen(_bluePen);
                painter.drawPoint(x, smoothM->at(i,3 ) * height2);
            }
        }
    }

    painter.end();
}

void VisualizationDrawer::paintProcessLine() {

    processLinePixmap.fill(Qt::transparent);
    QPainter processLinePainter(&processLinePixmap);

    uword currentTimeLinePosition = floor((_currentIndex - _currentFragmentStartIndex ) * _increment);
    processLinePainter.drawLine(currentTimeLinePosition, 0, currentTimeLinePosition, processLinePixmap.height());
    processLinePainter.end();
}



void VisualizationDrawer::paintFixations(){

    fixationsPixmap.fill(Qt::white);
    QPainter painter(&fixationsPixmap);

    double increment = _samplesPerFragment / fixationsPixmap.height();

    int counter = 0;
    QFont font=painter.font() ;
    font.setPointSize ( 8 );
    painter.setFont(font);

    std::stringstream number;
    number.str("");

    double posStart = 0;
    double posEnd = 0;

    int auxIndex = 0;
    if (fixAllM->n_rows > 0){
        uvec fixIndex =  arma::find(fixAllM->col(FIXCOL_END) >= _currentFragmentStartIndex);

        if (!fixIndex.is_empty()){
            auxIndex = fixIndex(0);
        }else{
            auxIndex = fixAllM->n_rows ;
        }

    }

    for (uword i = auxIndex; i < fixAllM->n_rows  ; i++){
        if ((fixAllM->at(i,FIXCOL_START) >= _currentFragmentStartIndex && fixAllM->at(i,FIXCOL_START) <=  _currentFragmentEndIndex ) ||(fixAllM->at(i,FIXCOL_START) <= _currentFragmentStartIndex && fixAllM->at(i,FIXCOL_END) >= _currentFragmentStartIndex )){

            if (fixAllM->at(i,FIXCOL_START) <= _currentFragmentStartIndex && fixAllM->at(i,FIXCOL_END) >= _currentFragmentStartIndex ) // If it's a fixation that didn't end in the previous segment
                posStart = -1;
            else
                posStart = (fixAllM->at(i,FIXCOL_START) - _currentFragmentStartIndex ) * (1/increment);

            posEnd = ((fixAllM->at(i,FIXCOL_END) - _currentFragmentStartIndex ) * (1/increment)) - posStart;

            if (fixAllM->at(i,FIXCOL_SMOOTH_PURSUIT) == Consts::SMOOTHP_YES){
                painter.setBrush(QBrush("#ffbc00"));
            }else{
                painter.setBrush(QBrush("#1ac500"));
            }

            painter.drawRect(QRect( posStart ,25, posEnd,16));
            number.str("");
            number << counter;
            painter.drawText( QPoint(posStart,23), number.str().c_str() );

            counter ++; // Next fixation

        }else if (fixAllM->at(i,FIXCOL_END) >= _currentFragmentEndIndex){
            break;
        }
    }

    painter.end();
}
