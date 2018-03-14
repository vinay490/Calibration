#-------------------------------------------------
#
# Project created by QtCreator 2017-12-20T11:46:00
#
#-------------------------------------------------

QT       += core gui concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Calibration
TEMPLATE = app


SOURCES += main.cpp\
        calibration_main.cpp \
    form_projector.cpp \
    form_projector_mapping.cpp \
    form_circles_calibration.cpp \
    form_contours_roi.cpp

HEADERS  += calibration_main.h \
    form_projector.h \
    form_projector_mapping.h \
    form_circles_calibration.h \
    form_contours_roi.h

FORMS    += calibration_main.ui \
    form_projector.ui \
    form_projector_mapping.ui \
    form_circles_calibration.ui \
    form_contours_roi.ui


INCLUDEPATH += C:/Basler/Development/include \
               C:/opencv_2.4.9/opencv/build/install/include \

LIBS += -LC:/Basler/Development/lib/x64\
    -lGenApi_MD_VC120_v3_0_Basler_pylon_v5_0 \
    -lGCBase_MD_VC120_v3_0_Basler_pylon_v5_0 \
    -lPylonBase_MD_VC120_v5_0 \

#Libraries of Opencv

LIBS += -LC:/opencv_2.4.9/opencv/build/install/x64/vc12/lib
LIBS += -lopencv_core249
LIBS += -lopencv_imgproc249
LIBS += -lopencv_highgui249
LIBS += -lopencv_ml249
LIBS += -lopencv_features2d249
LIBS += -lopencv_calib3d249
LIBS += -lopencv_objdetect249
LIBS += -lopencv_flann249

RESOURCES += \
    myresource.qrc

