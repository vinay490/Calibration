#ifndef CALIBRATION_MAIN_H
#define CALIBRATION_MAIN_H

#include <QMainWindow>
#include "QImage"
#include "QTimer"
#include "QDesktopWidget"
#include "QRect"
#include "stdio.h"
//#include "QCamera"

//#include "qopticaldevice.h"



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

#include <QProcess>
#include <QDesktopWidget>

#include "QDebug"
using namespace cv;
using namespace Pylon;
using namespace std;
//namespace fisheye;
#include <QProcess>
namespace Ui {
class Calibration_Main;
}

class Calibration_Main : public QMainWindow
{
    Q_OBJECT

public:
    explicit Calibration_Main(QWidget *parent = 0);
    ~Calibration_Main();
signals:
    void Calibration_Completed();

private slots:

    void CapturingCamera();
    void on_Timeout();

    void on_pbtn_calibrate_clicked();
    bool saveCameraCalibration(String name, Mat cameraMatrix, Mat distanceCoefficients, vector<Mat> rvecs, vector<Mat> tvecs);


private:
    Ui::Calibration_Main *ui;
    QProcess *ProcessCalibUE;

    QTimer *timer1;

    CPylonImage pylonImage;
    CGrabResultPtr GrabResult;

    Mat image;
    Mat greyscale;
    Mat initUndistort;


    QImage dest;
    QImage mydest;

    bool found;
    vector<Point2f> pointBuf;

    QProcess *process;

    vector<Mat> images;
    //vector< Point2f > corners;
    vector <vector <Point2f>> allFoundCorners;
    vector< vector< Point3f > > object_points;
    vector< vector< Point2f > > image_points;
    Mat imageUndistort;
    typedef std::vector<cv::Point2f>    Corners;

};

#endif // CALIBRATION_MAIN_H
