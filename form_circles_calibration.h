#ifndef FORM_CIRCLES_CALIBRATION_H
#define FORM_CIRCLES_CALIBRATION_H

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
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d//calib3d.hpp"
#include "opencv2/nonfree/nonfree.hpp"

#include "QDebug"
#include "QTimer"

using namespace cv;
using namespace Pylon;
using namespace std;

typedef Pylon::CBaslerGigEInstantCamera Camera_t;

namespace Ui {
class Form_Circles_Calibration;
}

class Form_Circles_Calibration : public QWidget
{
    Q_OBJECT

public:
    explicit Form_Circles_Calibration(QWidget *parent = 0);
    ~Form_Circles_Calibration();

private slots:
    void on_Timeout();

    void CapturingCamera();
private:
    Ui::Form_Circles_Calibration *ui;

    CImageFormatConverter formatConverter;
    CPylonImage pylonImage;
    CGrabResultPtr GrabResult;

    vector<Point2f> pointBuf;

    vector< vector< Point3f > > object_points;
    vector< vector< Point2f > > image_points;
    Mat imageUndistort;
    Mat transformed;

    typedef std::vector<cv::Point2f>    Corners;

    Mat greyscale;

    Mat image;
    QTimer *timer1;
    QImage dest;
    QImage mydest;
    bool found;


};

#endif // FORM_CIRCLES_CALIBRATION_H
