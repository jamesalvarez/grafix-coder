#ifndef OPERATIONS_H
#define OPERATIONS_H

#include "MyConstants.h"
#include "GPMatrixFunctions.h"

#include <QtWidgets/QMainWindow>
#include <QtGui>
#include <QtWidgets/QWidget>

#include <sstream>
#include <string>
#include <stdlib.h>
#include <iostream>

#include "armadillo"




using namespace arma;
using namespace std;

class GPFixationOperations
{
public:
     static mat fncCreateFixation(mat fixAllM, mat allETDataM, int hz, int secsSegment, int segment, int from, int to, int expWidth,int expHeight, double degPerPixel); // Creates a fixation from "from" to "to"
     static mat fncDeleteFixations(mat fixAllM, int hz, int secsSegment, int segment, int from, int to); // Deletes all the fixations from "from" to "to"
     static mat fncMergeFixations(mat fixAllM,  mat allETDataM, int hz, int secsSegment, int segment, int from, int to,int expWidth,int expHeight, double degPerPixel); // Merges all the fixations from "from" to "to"
     static mat fncSmoothPursuitFixation(mat fixAllM, int hz, int secsSegment, int segment, int from, int to);  // Identifies selected fixations as smooth pursuit/not smooth pursuit
     static mat fncRecalculateFixations(mat roughM, mat fixAllM,int expWidth,int expHeight, double degPerPixel);
};

#endif // OPERATIONS_H

