#-------------------------------------------------
#
# Project created by QtCreator 2012-11-16T10:17:27
#
#-------------------------------------------------

TARGET = FixationsCoder
TEMPLATE = app

QT += core gui widgets printsupport

QMAKE_CXXFLAGS += -Wno-unused-local-typedefs

SOURCES += main.cpp \
    MyConstants.cpp \
    GrafixProject.cpp \
    GPTableModel.cpp \
    GPGeneralStaticLibrary.cpp \
    GPMatrixFunctions.cpp \
    DialogTestSmoothParameters.cpp \
    qcustomplot.cpp \
    DialogPlot.cpp \
    DialogConfig.cpp \
    DialogEstimateFixations.cpp \
    DialogExport.cpp \
    DialogBatchProcess.cpp \
    DialogGrafixError.cpp \
    DialogParticipantEditNotes.cpp \
    DialogParticipantPaths.cpp \
    DialogOpenProject.cpp \
    DialogHeatMap.cpp \
    DialogInterpolation.cpp \
    GPFixationOperations.cpp \
    DialogSmoothXY.cpp \
    DialogStatistics.cpp \
    DialogVisualization.cpp \
    DialogVisualizationSegments.cpp \
    MainWindow.cpp \
    DialogSaveNewConfiguration.cpp \
    MainWindow2.cpp

HEADERS  += \
    MyConstants.h \
    GrafixProject.h \
    GPTableModel.h \
    GPGeneralStaticLibrary.h \
    GPMatrixFunctions.h \
    DialogTestSmoothParameters.h \
    qcustomplot.h \
    DialogPlot.h \
    DialogConfig.h \
    DialogEstimateFixations.h \
    DialogExport.h \
    DialogBatchProcess.h \
    DialogGrafixError.h \
    DialogParticipantEditNotes.h \
    DialogParticipantPaths.h \
    DialogOpenProject.h \
    DialogHeatMap.h \
    DialogInterpolation.h \
    GPFixationOperations.h \
    DialogSmoothXY.h \
    DialogStatistics.h \
    DialogVisualization.h \
    DialogVisualizationSegments.h \
    MainWindow.h \
    DialogSaveNewConfiguration.h \
    MainWindow2.h

FORMS    += \
    DialogTestSmoothParameters.ui \
    DialogPlot.ui \
    DialogConfig.ui \
    DialogEstimateFixations.ui \
    DialogBatchProcess.ui \
    DialogGrafixError.ui \
    DialogExport.ui \
    DialogParticipantEditNotes.ui \
    DialogParticipantPaths.ui \
    DialogOpenProject.ui \
    DialogInterpolation.ui \
    DialogHeatMap.ui \
    DialogSmoothXY.ui \
    DialogStatistics.ui \
    DialogVisualization.ui \
    DialogVisualizationSegments.ui \
    MainWindow.ui \
    DialogSaveNewConfiguration.ui \
    MainWindow2.ui


INCLUDEPATH += $$PWD/TRUNCATED_KERNEL_BF


INCLUDEPATH += $$PWD/../../armadillo-3.930.1/include


macx: LIBS += -L$$PWD/../../../Codes/libraries/armadillo-3.4.4/ -larmadillo
macx: LIBS += $$PWD/../armadillo-3.910.0/include -larmadillo

macx: INCLUDEPATH += $$PWD/../../../Codes/libraries/armadillo-3.4.4
macx: DEPENDPATH += $$PWD/../../../Codes/libraries/armadillo-3.4.4

win32: INCLUDEPATH += C:/Qt/boost_1_54_0/boost

win32: INCLUDEPATH += $$PWD/../armadillo-3.910.0/include
win32: DEPENDPATH += $$PWD/../armadillo-3.910.0/include

win32: INCLUDEPATH += $$PWD/../armadillo-3.910.0/examples/lib_win32
win32: DEPENDPATH += $$PWD/../armadillo-3.910.0/examples/lib_win32


win32: LIBS += -L$$PWD/../armadillo-3.910.0/examples/lib_win32/ -lblas_win32_MT


win32: LIBS += -L$$PWD/../armadillo-3.910.0/examples/lib_win32/ -llapack_win32_MT
