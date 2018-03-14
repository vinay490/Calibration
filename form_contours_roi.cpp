#include "form_contours_roi.h"
#include "ui_form_contours_roi.h"


typedef Pylon::CBaslerGigEInstantCamera Camera_t;



Form_Contours_ROI::Form_Contours_ROI(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form_Contours_ROI)
{
    ui->setupUi(this);

    timer1 = new QTimer(this);
    timer1->setInterval(2000);
    connect(timer1,&QTimer::timeout,this,&Form_Contours_ROI::on_Timeout);
    timer1->start();
}

Form_Contours_ROI::~Form_Contours_ROI()
{
    timer1->stop();
    delete ui;

}

void Form_Contours_ROI::on_Timeout()
{
    Accessing_Camera();
    //timer1->stop();
}

void Form_Contours_ROI::Accessing_Camera()
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

    int64_t offsetX = camera.OffsetX.GetValue();
    int64_t offsetY = camera.OffsetY.GetValue();
    camera.Width.SetValue(camera.Width.GetMax());
    camera.Height.SetValue(camera.Height.GetMax());

    int64_t camWidth = camera.Width.GetValue();
    int64_t camHeight = camera.Height.GetValue();


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

    camera.StartGrabbing(15,GrabStrategy_OneByOne);


    while(camera.IsGrabbing())
    {

        camera.RetrieveResult(5000,GrabResult,TimeoutHandling_ThrowException);

        formatConverter.Convert(pylonImage,GrabResult);
        //Pylon::DisplayImage(1, GrabResult);
        image = cv::Mat(GrabResult->GetHeight(),GrabResult->GetWidth(),CV_8UC3,(uint8_t *)pylonImage.GetBuffer());
        SourceImage = QImage((uchar*)image.data, image.cols, image.rows, image.step, QImage::Format_RGB888);
        ui->liveCam->setPixmap(QPixmap::fromImage(SourceImage));


        cvtColor(image,greyscale,CV_BGR2GRAY);

        threshold(greyscale, greyscale,25, 255,THRESH_BINARY); //Threshold the gray

        //Canny(greyscale, greyscale, 100, 200, 3);





        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;
//        RNG rng(12345);
        findContours( greyscale, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );


     //   Mat drawing = Mat::zeros( greyscale.size(), CV_8UC3 );


//        if(contours.size()!=0)
//        {
//            qDebug()<<" Contours Found "<<contours.size();
//            for( int i = 0; i< contours.size(); i++ )
//            {

//                Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
//                drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );

//                Rect box = boundingRect(contours[i]);
//                rectangle(drawing, box, color);
//                qDebug()<<" Rect Area"<<box.area();
//                qDebug()<<" box Height and Width  "<<box.height<<box.width;

//                //  qDebug()<<" Rectangle Data "<<box;
//            }
//        }
//        else
//        {
//            qDebug()<<" Contours NOT Found ";
//        }

    }
}

