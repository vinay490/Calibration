#ifndef FORM_CONTOURS_ROI_H
#define FORM_CONTOURS_ROI_H

#include <QWidget>

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

#include <QDebug>
using namespace cv;
using namespace Pylon;
using namespace std;

namespace Ui {
class Form_Contours_ROI;
}

class Form_Contours_ROI : public QWidget
{
    Q_OBJECT

public:
    explicit Form_Contours_ROI(QWidget *parent = 0);
    ~Form_Contours_ROI();

private slots:
    void on_Timeout();
    void Accessing_Camera();
private:
    Ui::Form_Contours_ROI *ui;

    QTimer *timer1;

    CInstantCamera *camera;

    CPylonImage pylonImage;
    CGrabResultPtr GrabResult;
    Mat image;
    Mat greyscale;

    QImage SourceImage;
    QImage ContourImage;

};

#endif // FORM_CONTOURS_ROI_H
