#ifndef FORM_PROJECTOR_MAPPING_H
#define FORM_PROJECTOR_MAPPING_H

#include <QWidget>

#include "pylon/PylonIncludes.h"
#include "pylon/PylonGUI.h"
#include <pylon/gige/BaslerGigEInstantCamera.h>
#include <pylon/gige/BaslerGigECamera.h>
#include <pylon/gige/GigETransportLayer.h>
#include <pylon/gige/_GigEStreamParams.h>
#include <pylon/gige/GigETransportLayer.h>
#include "pylon/gige/PylonGigEDevice.h"

#include <pylon/TransportLayer.h>
#include <pylon/gige/_BaslerGigECameraParams.h>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/video/video.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "Qpoint"
#include <QDebug>
#include "QtConcurrent"

#include "calibration_main.h"
using namespace cv;
using namespace Pylon;
using namespace std;


namespace Ui {
class Form_Projector_Mapping;
}

class Form_Projector_Mapping : public QWidget
{
    Q_OBJECT

public:
    explicit Form_Projector_Mapping(QWidget *parent = 0);
    ~Form_Projector_Mapping();

private slots:
    void on_Timeout();

    void CaptureCamera();
    void CaptureCameraUsingHoughLinesLogic();
    void After_Calibration();
    Point2f computeIntersect(cv::Vec4i a, cv::Vec4i b);
    void sortCorners(std::vector<cv::Point2f> &corners, cv::Point2f center);
private:
    Ui::Form_Projector_Mapping *ui;
    Calibration_Main  *Calib;

    bool mStop;
    QTimer *timer1;

    CInstantCamera *camera;
    Mat image;
    Mat greyscale;
    Mat gray;
    Mat transformed;
    QImage dest;
    QImage finaldest;


    CImageFormatConverter converter;
    CPylonImage pylonImage;
    CGrabResultPtr GrabResult;


    vector< Point2f > corners;


    typedef std::vector<cv::Point2f>    Corners;


};

#endif // FORM_PROJECTOR_MAPPING_H
