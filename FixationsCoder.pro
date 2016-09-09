#-------------------------------------------------
#
# Project created by QtCreator 2012-11-16T10:17:27
#
#-------------------------------------------------

TARGET = FixationsCoder
TEMPLATE = app

QT += core gui widgets printsupport multimediawidgets opengl

QMAKE_CXXFLAGS += -Wno-unused-local-typedefs -Wno-unnamed-type-template-args
QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-local-typedefs -Wno-unnamed-type-template-args

#osx deployment target
macx: QMAKE_CFLAGS += -mmacosx-version-min=10.6
macx: QMAKE_CXXFLAGS += -mmacosx-version-min=10.6

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
    DialogSaveNewConfiguration.cpp \
    MainWindow2.cpp \
    GrafixSettingsLoader.cpp \
    Trilateral/Trilateral2003.cpp \
    DialogVideoPlayer.cpp \
    FixationsListModel.cpp

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
    DialogSaveNewConfiguration.h \
    MainWindow2.h \
    GrafixSettingsLoader.h \
    Trilateral/Trilateral2003.h \
    DialogVideoPlayer.h \
    FixationsListModel.h

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
    DialogSaveNewConfiguration.ui \
    MainWindow2.ui \
    DialogVideoPlayer.ui


INCLUDEPATH += $$PWD/TRUNCATED_KERNEL_BF


#to deploy on mac use macdeployqt

#needed for armadillo (installed with cmake)
macx: INCLUDEPATH += /usr/local/include


#win32: INCLUDEPATH += C:/Qt/boost_1_54_0/boost

#win32: INCLUDEPATH += $$PWD/../armadillo-7.200.2/include
#win32: DEPENDPATH += $$PWD/../armadillo-7.200.2/include

#win32: INCLUDEPATH += $$PWD/../armadillo-7.200.2/examples/lib_win32
#win32: DEPENDPATH += $$PWD/../armadillo-7.200.2/examples/lib_win32


#win32: LIBS += -L$$PWD/../armadillo-7.200.2/examples/lib_win32/ -lblas_win32_MT

#win32: LIBS += -L$$PWD/../armadillo-7.200.2/examples/lib_win32/ -llapack_win32_MT


#-------------------------------------------------
#
# Paths for ARMADILLO - put armadillo in the same directory as grafix-coder.
# (But not it the actual directory)
# Update the paths below to point to the correct directory names
#
#-------------------------------------------------
win64: INCLUDEPATH += C:/Qt/boost_1_54_0/boost

win64: INCLUDEPATH += $$PWD/../armadillo-7.200.2/include
win64: DEPENDPATH += $$PWD/../armadillo-7.200.2/include

win64: INCLUDEPATH += $$PWD/../armadillo-7.200.2/examples/lib_win64
win64: DEPENDPATH += $$PWD/../armadillo-7.200.2/examples/lib_win64


win64: LIBS += -L$$PWD/../armadillo-7.200.2/examples/lib_win64/ -lblas_win64_MT

win64: LIBS += -L$$PWD/../armadillo-7.200.2/examples/lib_win64/ -llapack_win64_MT
