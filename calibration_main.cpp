#include "calibration_main.h"
#include "ui_calibration_main.h"
#include <iostream>
#include <fstream>
#include <iterator>

typedef Pylon::CBaslerGigEInstantCamera Camera_t;


const Size chessboardDimensions=Size(9,17);
Size ImageSizeBasler;
const float calibrationSquareDimension=0.01f;
bool showPoints=false;

int num_of_frames = 1;

int flag = 0;


int thresh = 100;
int max_thresh = 255;
RNG rng(12345);

Calibration_Main::Calibration_Main(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Calibration_Main)
{
    ui->setupUi(this);

    timer1 = new QTimer(this);
    timer1->setInterval(200);
    connect(timer1,&QTimer::timeout,this,&Calibration_Main::on_Timeout);
    timer1->start();
}
Calibration_Main::~Calibration_Main()
{

    timer1->stop();
    //PylonTerminate();
    delete ui;
}

void Calibration_Main::on_Timeout()
{
    CapturingCamera();
    timer1->stop();

}

void Calibration_Main::CapturingCamera()
{

    //    PylonInitialize();
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

    int64_t offsetX = camera.OffsetX.GetValue();
    int64_t offsetY = camera.OffsetY.GetValue();
    camera.Width.SetValue(camera.Width.GetMax());
    camera.Height.SetValue(camera.Height.GetMax());
//    camera.Width.SetValue(599);
//    camera.Height.SetValue(431);
//    camera.OffsetX.SetValue(29);
//    camera.OffsetY.SetValue(13);

    int64_t camWidth = camera.Width.GetValue();
    int64_t camHeight = camera.Height.GetValue();

//        int64_t offsetX = camera.OffsetX.GetValue();
//        int64_t offsetY = camera.OffsetY.GetValue();


    qDebug()<<"Camera Width: "<<QString::number(camWidth)<<"Camera Height : "<<QString::number(camHeight);
    qDebug()<<"Camera Offset X : "<<QString::number(offsetX)<<" Camera Offset Y "<<QString::number(offsetY);
    camera.PixelFormat.SetValue(Basler_GigECameraParams::PixelFormat_Mono8);
    camera.AcquisitionFrameRateEnable.SetValue(true);
    camera.AcquisitionFrameRateAbs.SetValue(125.0);
    camera.GevSCPD.SetValue(0);

    int64_t i = camera.GevSCPD.GetValue();
    qDebug()<<" camera packet delay "<<i;
    qDebug()<<"Camera Acquisition Frame Rate"<<QString::number(camera.ResultingFrameRateAbs.GetValue());
    camera.MaxNumBuffer = 30;
    CImageFormatConverter formatConverter;
    formatConverter.OutputPixelFormat=PixelType_RGB8packed;
    Mat M = cv::Mat::eye(3,3,CV_64F);
    Mat k = cv::Mat::zeros(1,5,CV_64F);
    vector<cv::Mat> rvecs;
    vector<cv::Mat> tvecs;

    camera.StartGrabbing(15,GrabStrategy_OneByOne);


    while(camera.IsGrabbing())
    {

        camera.RetrieveResult(5000,GrabResult,TimeoutHandling_ThrowException);

        formatConverter.Convert(pylonImage,GrabResult);
        //Pylon::DisplayImage(1, GrabResult);
        image = cv::Mat(GrabResult->GetHeight(),GrabResult->GetWidth(),CV_8UC3,(uint8_t *)pylonImage.GetBuffer());
        //       cvtColor(image,greyscale,CV_BGR2GRAY);

        dest = QImage((uchar*)image.data, image.cols, image.rows, image.step, QImage::Format_RGB888);
        ui->liveCam->setPixmap(QPixmap::fromImage(dest));

        found = findChessboardCorners(image,chessboardDimensions,pointBuf,CV_CALIB_CB_ADAPTIVE_THRESH|CV_CALIB_CB_NORMALIZE_IMAGE|CV_CALIB_CB_FAST_CHECK|CV_CALIB_CB_FILTER_QUADS);
        qDebug()<<"Chessboard Corners Found"<<found<<GrabResult->GetImageNumber();

        ImageSizeBasler=cvSize(GrabResult->GetHeight(),GrabResult->GetWidth());

        //qDebug()<<" Image Size Basler "<<ImageSizeBasler;

        vector< Point3f > obj;

        for (int i = 0; i < chessboardDimensions.height; i++)
            for (int j = 0; j < chessboardDimensions.width; j++)
                obj.push_back(Point3f((float)j * calibrationSquareDimension, (float)i * calibrationSquareDimension, 0));

        if(found)
        {
            ui->found->setText("true");

            //           cornerSubPix(greyscale, pointBuf, cv::Size(5, 5), cv::Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));

            //                     cornerSubPix(greyscale, pointBuf, Size(11, 11), Size(-1, -1), TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));

            //             drawChessboardCorners(greyscale,ImageSizeBasler,pointBuf,found);

            image_points.push_back(pointBuf);
            object_points.push_back(obj);



            flag |= CV_CALIB_FIX_K4;
            flag |= CV_CALIB_FIX_K5;
//            flag |= CV_CALIB_FIX_ASPECT_RATIO;
//            flag |= CV_CALIB_FIX_FOCAL_LENGTH;
//            flag |= CV_CALIB_ZERO_TANGENT_DIST;
//            flag |= CV_CALIB_FIX_PRINCIPAL_POINT;
//            flag |= CV_CALIB_USE_INTRINSIC_GUESS;
            // flag |= CV_CALIB_FIX_ASPECT_RATIO;
            //            qDebug()<<"1. obj,img "<<object_points.size()<<image_points.size();



        }
    }

    calibrateCamera(object_points, image_points, chessboardDimensions, M, k, rvecs, tvecs,flag);

    cv::Mat R, map1, map2;
    cv::Mat newCameraMatrix;
    cv::Mat corrected;


        R = cv::Mat(3, 3, CV_64F);
        cv::setIdentity(R);
    //    cv::initUndistortRectifyMap(M, k, R, newCameraMatrix, cvSize(GrabResult->GetWidth(),GrabResult->GetHeight()), CV_32FC1, map1, map2);

    initUndistortRectifyMap(M, k, R,
                            getOptimalNewCameraMatrix(M, k, cvSize(658,492), 1,  cvSize(658,492), 0),
                            cvSize(658,492), CV_32FC1, map1, map2);
    cv::remap(image, corrected, map1, map2, cv::INTER_LINEAR);





//        undistort(image,corrected,M,k);




    //projectPoints();
    mydest = QImage((uchar*)corrected.data, corrected.cols, corrected.rows,corrected.step, QImage::Format_RGB888);
    ui->cal_image->setPixmap(QPixmap::fromImage(mydest));

    FileStorage file1("../CalibrationValues/CalibratedValuess.xml",FileStorage::WRITE);
    file1<<"IntrinsicCoefficient" << M;
    file1<<"DistortionCoefficient"<< k;
    file1<<"RVector"<<rvecs;
    file1<<"TVector"<<tvecs;
    file1<<"Resoultion"<<Size(GrabResult->GetWidth(),GrabResult->GetHeight());
    file1<<"ChessBoardDimensions"<<chessboardDimensions;
    file1<<"SourceImage"<<image;
    file1<<"CorrectedImage"<<corrected;
    file1.release();
}




void Calibration_Main::on_pbtn_calibrate_clicked()
{
    //ui->cal_image->setPixmap(QPixmap::fromImage(finaldest));
}

bool Calibration_Main::saveCameraCalibration(String name,Mat cameraMatrix,Mat distanceCoefficients,vector<cv::Mat> rvecs,vector<cv::Mat> tvecs)
{
    ofstream  outStream(name);
    if(outStream)
    {
        uint16_t  rows=cameraMatrix.rows;
        uint16_t  columns=cameraMatrix.cols;

        outStream<<"camera Martix Rows :"<<rows<<endl;
        outStream<<"\ncamera Martix columns :"<<columns<<endl;
        outStream<<"Camera Matrix\n";
        for(int r=0; r<rows;r++)
        {
            for(int c=0;c<columns;c++)
            {
                double  value=cameraMatrix.at<double>(r,c);
                outStream<<" value at  "<<r <<", "<<c  << "   ->  "<<value<<endl;
            }
        }


        rows=distanceCoefficients.rows;
        columns=distanceCoefficients.cols;

        outStream<< " DistanceCoefficients \n";
        for(int r=0; r<rows;r++)
        {
            for(int c=0;c<columns;c++)
            {
                double  value1=distanceCoefficients.at<double>(r,c);
                outStream<<" value at  "<<r <<" , "<<c << "     ->  " <<value1<<endl;
            }
        }


        outStream<< " rvecs \n";


        outStream.close();
        return true;
    }
    return false;


}

