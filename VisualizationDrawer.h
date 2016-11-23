   #ifndef VISUALIZATIONDRAWER_H
#define VISUALIZATIONDRAWER_H

#include "MyConstants.h"
#include "GrafixProject.h"
#include <qpainter.h>
#include "armadillo"

using namespace arma;

class VisualizationDrawer
{
    // Participant Settings
    GrafixSettingsLoader *settingsLoader;

    // Matrices
    const mat *roughM;
    const mat *smoothM;
    const mat *fixAllM;
    const mat *segmentsM;
    mat currentFragmentFixations;

    // Settings
    double _increment;
    QPen _smoothPen;
    QPen _roughPen1;
    QPen _roughPen2;
    QPen _redPen;
    QPen _redPen2;
    QPen _bluePen;
    QPen _bluePen2;
    int _expWidth;
    int _expHeight;
    double _samplesPerFragment;

    //Pens

    // Display state
    uword _currentIndex;
    uword _currentFragment;
    uword _currentFragmentStartIndex;
    uword _currentFragmentEndIndex;
    int _currentFixationIndex;

    // Flags
    bool _playingSmooth;
    bool _paintFixationNumbers;
    bool _paintPupilDilation;

    void updateDrawingSettings();

    void paintVisualization();
    void paintTimeLine();
    void paintProcessLine();
    void paintFixations();

public:
    VisualizationDrawer(const mat &roughM, const mat &smoothM, const mat &fixAllM, const mat &segmentsM, const GrafixParticipant *participant);
    ~VisualizationDrawer();

    QPixmap visualizationPixmap;
    QPixmap processLinePixmap;
    QPixmap timeLinePixmap;
    QPixmap fixationsPixmap;

    void updateVisualizationFrame(int width, int height);
    void updateFixationFrame(int width, int height);
    void updateTimeLineFrame(int width, int height);

    void paintCurrentFrame(uword _currentIndex);
};

#endif // VISUALIZATIONDRAWER_H
