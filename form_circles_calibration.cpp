#include "form_circles_calibration.h"
#include "ui_form_circles_calibration.h"

const Size CircleDimensions=Size(15,7);
const float calibrationCircleDimension=0.0075f;

int flags=0;

Form_Circles_Calibration::Form_Circles_Calibration(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form_Circles_Calibration)
{
    ui->setupUi(this);
    timer1 = new QTimer(this);
    timer1->setInterval(200);
    connect(timer1,&QTimer::timeout,this,&Form_Circles_Calibration::on_Timeout);
    timer1->start();
}

Form_Circles_Calibration::~Form_Circles_Calibration()
{
    delete ui;
}
void Form_Circles_Calibration::on_Timeout()
{
    CapturingCamera();
}

void Form_Circles_Calibration::CapturingCamera()
{
    PylonAutoInitTerm autoinit;
    CTlFactory& tlFactory = CTlFactory::GetInstance();
    DeviceInfoList_t devices;
    while ( tlFactory.EnumerateDevices(devices) == 0 )
    {
        qDebug()<<"Unable to Find Camera";
    }
    CDeviceInfo info;
    Camera_t camera(CTlFactory::GetInstance().CreateFirstDevice(info));
    qDebug()<<"REading Camera Parameters";
    camera.Open();
    qDebug()<<"Is Camera Open"<<camera.IsOpen();
    qDebug()<<"Is Camera Grabbing"<<camera.IsGrabbing();
    int64_t camWidth = camera.Width.GetValue();
    int64_t camHeight = camera.Height.GetValue();
    int64_t offsetX = camera.OffsetX.GetValue();
    int64_t offsetY = camera.OffsetY.GetValue();
    camera.Width.SetValue(camera.Width.GetMax());
    camera.Height.SetValue(camera.Height.GetMax());
    qDebug()<<"Camera Width: "<<QString::number(camWidth)<<"Camera Height : "<<QString::number(camHeight);
    qDebug()<<"Camera Offset X : "<<QString::number(offsetX)<<" Camera Offset Y "<<QString::number(offsetY);
    camera.PixelFormat.SetValue(Basler_GigECameraParams::PixelFormat_Mono8);
    camera.AcquisitionFrameRateEnable.SetValue(true);
    camera.AcquisitionFrameRateAbs.SetValue(125.0);
    camera.GevSCPD.SetValue(100);

    int64_t i = camera.GevSCPD.GetValue();
    qDebug()<<" camera packet delay "<<i;
    qDebug()<<"Camera Acquisition Frame Rate"<<QString::number(camera.ResultingFrameRateAbs.GetValue());
    camera.MaxNumBuffer = 30;
    CImageFormatConverter formatConverter;
    formatConverter.OutputPixelFormat=PixelType_RGB8packed;

    camera.StartGrabbing(GrabStrategy_LatestImageOnly);
    camera.RetrieveResult(5000,GrabResult,TimeoutHandling_ThrowException);

    formatConverter.Convert(pylonImage,GrabResult);
    image = cv::Mat(GrabResult->GetHeight(),GrabResult->GetWidth(),CV_8UC3,(uint8_t *)pylonImage.GetBuffer());

    dest = QImage((uchar*)image.data, image.cols, image.rows, image.step, QImage::Format_RGB888);
    ui->liveCam->setPixmap(QPixmap::fromImage(dest));


    found = findCirclesGrid( image, CircleDimensions, pointBuf );
    qDebug()<<"Circles Found Status "<<found;



    vector< Point3f > obj;
    for (int i = 0; i < CircleDimensions.height; i++)
        for (int j = 0; j < CircleDimensions.width; j++)
            obj.push_back(Point3f((float)j * calibrationCircleDimension, (float)i * calibrationCircleDimension, 0));

    if(found)
    {
        //cornerSubPix(greyscale, pointBuf, Size(11, 11), Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
        drawChessboardCorners(image, CircleDimensions, pointBuf, found);
        ui->found->setText("true");


        image_points.push_back(pointBuf);
        object_points.push_back(obj);

        Mat M = cv::Mat::eye(3,3,CV_64F);
        Mat k = cv::Mat::zeros(1,5,CV_64F);
        vector<cv::Mat> rvecs;
        vector<cv::Mat> tvecs;

        flags |= CV_CALIB_FIX_K4;
        flags |= CV_CALIB_FIX_K5;
        qDebug()<<"1. obj,img "<<object_points.size()<<image_points.size();

        calibrateCamera(object_points, image_points, cvSize(GrabResult->GetWidth(),GrabResult->GetHeight()), M, k, rvecs, tvecs,flags);
        qDebug()<<"2. obj,img "<<object_points.size()<<image_points.size();


        cv::Mat R, map1, map2;
        cv::Mat newCameraMatrix;
        cv::Mat corrected;
        Corners drawnPoints;

        R = cv::Mat(3, 3, CV_64F);
        cv::setIdentity(R);
        cv::initUndistortRectifyMap(M, k, R, newCameraMatrix, cv::Size(GrabResult->GetWidth(), GrabResult->GetHeight()), CV_32FC1, map1, map2);


        cv::remap(image, corrected, map1, map2, cv::INTER_LINEAR);



//        cvtColor(image,greyscale,CV_BGR2GRAY);





//        vector< vector <Point> > contours;
//        vector< Vec4i > hierarchy;
//        int largest_contour_index=0;
//        int largest_area=0;

//        findContours( greyscale, contours, hierarchy,CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE ); // Find the contours in the image
//        qDebug()<<"No. of contours detected : "<<contours.size();
//        for( int i = 0; i< contours.size(); i++ )
//        {
//            double a=contourArea( contours[i],false);  //  Find the area of contour
//            if(a>largest_area)
//            {
//                largest_area=a;
//                largest_contour_index=i;
//                qDebug()<<"Largest Area"<<largest_area;//Store the index of largest contour
//            }
//        }

//        drawContours( greyscale,contours, largest_contour_index, Scalar(255,255,255),CV_FILLED, 8, hierarchy );


//        vector<vector<Point> > contours_poly(1);
//        approxPolyDP( Mat(contours[largest_contour_index]), contours_poly[0],8, true );
//        Rect boundRect=boundingRect(contours[largest_contour_index]);
//        qDebug()<<"Counturs "<<contours_poly[0].size();

//        if(contours_poly[0].size()==4){
//            std::vector<Point2f> quad_pts;
//            std::vector<Point2f> squre_pts;
//            quad_pts.push_back(Point2f(contours_poly[0][0].x,contours_poly[0][0].y));
//            quad_pts.push_back(Point2f(contours_poly[0][1].x,contours_poly[0][1].y));
//            quad_pts.push_back(Point2f(contours_poly[0][3].x,contours_poly[0][3].y));
//            quad_pts.push_back(Point2f(contours_poly[0][2].x,contours_poly[0][2].y));
//            squre_pts.push_back(Point2f(boundRect.x,boundRect.y));
//            squre_pts.push_back(Point2f(boundRect.x,boundRect.y+boundRect.height));
//            squre_pts.push_back(Point2f(boundRect.x+boundRect.width,boundRect.y));
//            squre_pts.push_back(Point2f(boundRect.x+boundRect.width,boundRect.y+boundRect.height));

//            Mat transmtx = getPerspectiveTransform(quad_pts,squre_pts);
//            transformed = Mat::zeros(image.rows, image.cols, CV_8UC3);

//            warpPerspective(image, transformed, transmtx, image.size());

//            Point P1=contours_poly[0][0];
//            Point P2=contours_poly[0][1];
//            Point P3=contours_poly[0][2];
//            Point P4=contours_poly[0][3];

//            line(image,P1,P2, Scalar(0,0,255),1,CV_AA,0);
//            line(image,P2,P3, Scalar(0,0,255),1,CV_AA,0);
//            line(image,P3,P4, Scalar(0,0,255),1,CV_AA,0);
//            line(image,P4,P1, Scalar(0,0,255),1,CV_AA,0);
//            rectangle(image,boundRect,Scalar(0,255,0),1,8,0);
//            rectangle(transformed,boundRect,Scalar(0,255,0),1,8,0);

//        }

        mydest = QImage((uchar*)transformed.data, transformed.cols, transformed.rows,transformed.step, QImage::Format_RGB888);
        ui->cal_image->setPixmap(QPixmap::fromImage(mydest));
//    }
//    else
//    {
//        ui->found->setText("false");
//    }
}
}
