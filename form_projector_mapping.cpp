#include "form_projector_mapping.h"
#include "ui_form_projector_mapping.h"
#include <iostream>
#include <fstream>
#include <iterator>

//Point2f center(0,0);

typedef Pylon::CBaslerGigEInstantCamera Camera_t;

Mat SourceImage,IntrinsicImage,DistortionImage;
Mat CorrectedImage;
vector<cv::Mat> rvecs;
vector<cv::Mat> tvecs;


Form_Projector_Mapping::Form_Projector_Mapping(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form_Projector_Mapping)
{
    ui->setupUi(this);

//    qDebug()<<" test "

    IntrinsicImage = cv::Mat::eye(3,3,CV_64F);
    DistortionImage = cv::Mat::zeros(1,5,CV_64F);

    FileStorage filesrc("../CalibrationValues/CalibratedValuess.xml",FileStorage::READ);
    filesrc["IntrinsicCoefficient"] >> IntrinsicImage;
    filesrc["DistortionCoefficient"] >> DistortionImage;
    filesrc["RVector"] >> rvecs;
    filesrc["TVector"] >> tvecs;
    filesrc["CorrectedImage"] >> CorrectedImage;



//    getline(filesrc,line);
//    IntrinsicImage=line.find("IntrinsicCoefficient");
//    DistortionImage=line.find("DistortionCoefficient");
//    rvecs=line.find("RVector");
//    tvecs=line.find("TVector");
//    CorrectedImage=line.find("CorrectedImage");

    filesrc.release();



    timer1 = new QTimer(this);
    timer1->setInterval(500);
    connect(timer1,&QTimer::timeout,this,&Form_Projector_Mapping::on_Timeout);
    timer1->start();

}

Form_Projector_Mapping::~Form_Projector_Mapping()
{

    delete ui;
}
void Form_Projector_Mapping::on_Timeout()
{
   // qDebug()<<" Timer Time Out ";
    CaptureCamera();
    //CaptureCameraUsingHoughLinesLogic();
    timer1->stop();
}

void Form_Projector_Mapping::CaptureCamera()
{
    //        PylonAutoInitTerm  autoinit;
    //        CTlFactory& tlFactory = CTlFactory::GetInstance();
    //        DeviceInfoList_t devices;
    //        while ( tlFactory.EnumerateDevices(devices) == 0 )
    //        {
    //            qDebug()<<"Unable to Find Camera";
    //        }
    //        CDeviceInfo info;
    //        Camera_t camera(CTlFactory::GetInstance().CreateFirstDevice(info));
    //        qDebug()<<"REading Camera Parameters";
    //        camera.Open();
    //        qDebug()<<"Is Camera Open"<<camera.IsOpen();
    //        qDebug()<<"Is Camera Grabbing"<<camera.IsGrabbing();
    //        int64_t camWidth = camera.Width.GetValue();
    //        int64_t camHeight = camera.Height.GetValue();
    //        int64_t offsetX = camera.OffsetX.GetValue();
    //        int64_t offsetY = camera.OffsetY.GetValue();
    //        camera.Width.SetValue(camera.Width.GetMax());
    //        camera.Height.SetValue(camera.Height.GetMax());
    //        qDebug()<<"Camera Width: "<<QString::number(camWidth)<<"Camera Height : "<<QString::number(camHeight);
    //        qDebug()<<"Camera Offset X : "<<QString::number(offsetX)<<" Camera Offset Y "<<QString::number(offsetY);
    //        camera.PixelFormat.SetValue(Basler_GigECameraParams::PixelFormat_Mono8);
    //        camera.AcquisitionFrameRateEnable.SetValue(true);
    //        camera.AcquisitionFrameRateAbs.SetValue(125.0);
    //        camera.GevSCPD.SetValue(100);

    //        int64_t i = camera.GevSCPD.GetValue();
    //        qDebug()<<" camera packet delay "<<i;
    //        qDebug()<<"Camera Acquisition Frame Rate"<<QString::number(camera.ResultingFrameRateAbs.GetValue());
    //        camera.MaxNumBuffer = 30;
    //        CImageFormatConverter formatConverter;
    //        formatConverter.OutputPixelFormat=PixelType_RGB8packed;

    //        //    camera.GrabOne(200,GrabResult,TimeoutHandling_Return);
    //        camera.StartGrabbing(15,GrabStrategy_LatestImageOnly);

    qDebug()<<" 1 ";

    qDebug()<<" Loading Calibrated Details Done ";

    cv::Mat R, map1, map2;

    cv::Mat newCameraMatrix;
    cv::Mat corrected;
    Corners drawnPoints;

    R = cv::Mat(3, 3, CV_64F);
    cv::setIdentity(R);


//    while(camera.IsGrabbing())
//    {

//        camera.RetrieveResult(5000,GrabResult,TimeoutHandling_ThrowException);

//        formatConverter.Convert(pylonImage,GrabResult);
//        //Pylon::DisplayImage(1, GrabResult);
//        image = cv::Mat(GrabResult->GetHeight(),GrabResult->GetWidth(),CV_8UC3,(uint8_t *)pylonImage.GetBuffer());




//        cvtColor(CorrectedImage,greyscale,CV_BGR2GRAY);
//        threshold( greyscale, greyscale, 150, 255,CV_THRESH_BINARY );
//        dest = QImage((uchar*)image.data, image.cols,  image.rows,  image.step, QImage::Format_RGB888);
//        ui->liveCam->setPixmap(QPixmap::fromImage(dest));
//        // GaussianBlur(greyscale,greyscale,Size(9,9),5,10);


        vector< vector <Point> > contours;
        vector< Vec4i > hierarchy;
        int largest_contour_index=0;
        int largest_area=0;

        findContours( greyscale, contours, hierarchy,CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE ); // Find the contours in the image
        qDebug()<<"No. of contours detected : "<<contours.size();
        for( int i = 0; i< contours.size(); i++ )
        {
            double a=contourArea( contours[i],false);  //  Find the area of contour
            if(a>largest_area)
            {
                largest_area=a;
                largest_contour_index=i;
                qDebug()<<"Largest Area"<<largest_area;//Store the index of largest contour
            }
        }

        drawContours( greyscale,contours, largest_contour_index, Scalar(255,255,255),CV_FILLED, 8, hierarchy );



        vector<vector<Point> > contours_poly(1);
        approxPolyDP( Mat(contours[largest_contour_index]), contours_poly[0],8, true );
        Rect boundRect=boundingRect(contours[largest_contour_index]);
        qDebug()<<"Counturs "<<contours_poly[0].size();

        Point2f inputQuad[4];
        // Output Quadilateral or World plane coordinates
        Point2f outputQuad[4];


        if(contours_poly[0].size()==4)
        {
            std::vector<Point2f> quad_pts;
            std::vector<Point2f> squre_pts;
            quad_pts.push_back(Point2f(contours_poly[0][0].x,contours_poly[0][0].y));
            quad_pts.push_back(Point2f(contours_poly[0][1].x,contours_poly[0][1].y));
            quad_pts.push_back(Point2f(contours_poly[0][3].x,contours_poly[0][3].y));
            quad_pts.push_back(Point2f(contours_poly[0][2].x,contours_poly[0][2].y));
            squre_pts.push_back(Point2f(boundRect.x,boundRect.y));
            squre_pts.push_back(Point2f(boundRect.x,boundRect.y+boundRect.height));
            squre_pts.push_back(Point2f(boundRect.x+boundRect.width,boundRect.y));
            squre_pts.push_back(Point2f(boundRect.x+boundRect.width,boundRect.y+boundRect.height));

            //            inputQuad[0] = Point2f( -30,-60 );
            //               inputQuad[1] = Point2f( image.cols+50,-50);
            //               inputQuad[2] = Point2f( image.cols+100,image.rows+50);
            //               inputQuad[3] = Point2f( -50,image.rows+50  );
            //               // The 4 points where the mapping is to be done , from top-left in clockwise order
            //               outputQuad[0] = Point2f( 0,0 );
            //               outputQuad[1] = Point2f( image.cols-1,0);
            //               outputQuad[2] = Point2f( image.cols-1,image.rows-1);
            //               outputQuad[3] = Point2f( 0,image.rows-1  );

            // Get the Perspective Transform Matrix i.e. lambda
            //               lambda = getPerspectiveTransform( inputQuad, outputQuad );

            Mat transmtx = getPerspectiveTransform(inputQuad,outputQuad);
            transformed = Mat::zeros( image.rows,  image.cols,  image.type());
            warpPerspective( image, transformed, transmtx, Size(659,494));

            Point P1=contours_poly[0][0];
            Point P2=contours_poly[0][1];
            Point P3=contours_poly[0][2];
            Point P4=contours_poly[0][3];


            line( image,P1,P2, Scalar(0,0,255),1,CV_AA,0);
            line( image,P2,P3, Scalar(0,0,255),1,CV_AA,0);
            line( image,P3,P4, Scalar(0,0,255),1,CV_AA,0);
            line( image,P4,P1, Scalar(0,0,255),1,CV_AA,0);
            rectangle( image,boundRect,Scalar(0,255,0),1,8,0);
            rectangle(transformed,boundRect,Scalar(0,255,0),1,8,0);

            finaldest = QImage((uchar*)transformed.data, transformed.cols, transformed.rows, transformed.step, QImage::Format_RGB888);
            ui->cal_image->setPixmap(QPixmap::fromImage(finaldest));



        }
        else
            qDebug()<<"Make sure that your are getting 4 corner using approxPolyDP..."<<endl;
//    }
}

void Form_Projector_Mapping::CaptureCameraUsingHoughLinesLogic()
{
    //    PylonInitialize();
    //    CTlFactory& tlFactory = CTlFactory::GetInstance();
    //    DeviceInfoList_t devices;
    //    while ( tlFactory.EnumerateDevices(devices) == 0 )
    //    {
    //        qDebug()<<"Unable to Find Camera";
    //    }
    //    CDeviceInfo info;
    //    Camera_t camera(CTlFactory::GetInstance().CreateFirstDevice(info));
    //    qDebug()<<"REading Camera Parameters";
    //    camera.Open();
    //    qDebug()<<"Is Camera Open"<<camera.IsOpen();
    //    qDebug()<<"Is Camera Grabbing"<<camera.IsGrabbing();
    //    int64_t camWidth = camera.Width.GetValue();
    //    int64_t camHeight = camera.Height.GetValue();
    //    int64_t offsetX = camera.OffsetX.GetValue();
    //    int64_t offsetY = camera.OffsetY.GetValue();
    //    camera.Width.SetValue(camera.Width.GetMax());
    //    camera.Height.SetValue(camera.Height.GetMax());
    //    qDebug()<<"Camera Width: "<<QString::number(camWidth)<<"Camera Height : "<<QString::number(camHeight);
    //    qDebug()<<"Camera Offset X : "<<QString::number(offsetX)<<" Camera Offset Y "<<QString::number(offsetY);
    //    camera.PixelFormat.SetValue(Basler_GigECameraParams::PixelFormat_Mono8);
    //    camera.AcquisitionFrameRateEnable.SetValue(true);
    //    camera.AcquisitionFrameRateAbs.SetValue(125.0);
    //    camera.GevSCPD.SetValue(0);

    //    int64_t i = camera.GevSCPD.GetValue();
    //    qDebug()<<" camera packet delay "<<i;
    //    qDebug()<<"Camera Acquisition Frame Rate"<<QString::number(camera.ResultingFrameRateAbs.GetValue());
    //    camera.MaxNumBuffer = 30;
    //    CImageFormatConverter formatConverter;
    //    formatConverter.OutputPixelFormat=PixelType_RGB8packed;

    //    //    camera.GrabOne(200,GrabResult,TimeoutHandling_Return);
    //    camera.StartGrabbing(15,GrabStrategy_LatestImageOnly);

    //    while(camera.IsGrabbing())
    //    {

    //        camera.RetrieveResult(5000,GrabResult,TimeoutHandling_ThrowException);

    //        formatConverter.Convert(pylonImage,GrabResult);
    //        Pylon::DisplayImage(1, GrabResult);
    //        image = cv::Mat(GrabResult->GetHeight(),GrabResult->GetWidth(),CV_8UC3,(uint8_t *)pylonImage.GetBuffer());

    //        cv :: cvtColor (image,gray, CV_BGR2GRAY);

    //        cv :: blur (gray, gray, cv :: Size (3, 3));
    //        cv :: Canny (gray, gray, 100, 100, 3);

    //        //        threshold( gray, gray, 30, 255,CV_THRESH_BINARY );
    //        dest = QImage((uchar*)image.data, image.cols, image.rows, image.step, QImage::Format_RGB888);
    //        ui->liveCam->setPixmap(QPixmap::fromImage(dest));




    //        std :: vector <cv :: Vec4i> lines;
    //        cv :: HoughLinesP (gray, lines, 1, CV_PI / 180, 70, 30, 10);

    //        //Expand the lines
    //        for  ( int  i = 0; i <lines.size (); i ++)
    //        {
    //            cv :: Vec4i v = lines [i];
    //            lines [i] [0] = 0;
    //            lines [i] [1] = ((( float ) v [1] - v [3]) / v [0] - v [2]) * -v [0] + v [1];
    //            lines [i] [2] = image.cols;
    //            lines [i] [3] = (( float ) v [1] - v [3]) / (v [0] - v [2]) * (image.cols - v [2]) + v [3] ;
    //        }

    //        std :: vector <cv :: Point2f> corners;
    //        for  ( int  i = 0; i <lines.size (); i ++)
    //        {
    //            for  ( int  j = i + 1; j <lines.size (); j ++)
    //            {
    //                cv :: Point2f pt = computeIntersect(lines [i], lines [j]);
    //                if(( pt.x  >= 0)&&( pt.y >= 0))
    //                    corners.push_back (pt);
    //            }
    //        }

    //        std :: vector <cv :: Point2f> approx;
    //        cv :: approxPolyDP (cv :: Mat (corners), approx, cv :: arcLength (cv :: Mat (corners),  true ) * 0.02,  true );
    //        qDebug()<<"Approx Size : "<<approx.size();
    //        if(approx.size()!= 4)
    //        {
    //            qDebug()<<  "The object is not quadrilateral!" ;
    //            //return  -1;
    //        }
    //        else
    //        {
    //            qDebug()<< "Quadrilateral ";

    //        }

    //        finaldest = QImage((uchar*)gray.data, gray.cols, gray.rows, gray.step, QImage::Format_Grayscale8);
    //        ui->cal_image->setPixmap(QPixmap::fromImage(finaldest));


    //    }

}


Point2f Form_Projector_Mapping::computeIntersect(cv :: Vec4i a , cv :: Vec4i b)
{
    //    int  x1 = a[0];
    //    int y1 = a[.1];
    //    int x2 = a[2], y2 = a[.3], x3 = b[0], y3 = b[.1], x4 = b[2], y4 = b[3];
    //    float  denom;

    //    if( float  d = (( float )(x1 - x2)*(y3 - y4)) - ((y1 - y2) * (x3 - x4)))
    //    {
    //        cv :: Point2f pt;
    //        pt.x = ((x1*y2 - y1*x2) * (x3-x4) - (x1-x2) * (x3*y4 - y3*x4)) / d;
    //        pt.y = ((x1*y2 - y1*x2) * (y3-y4) - (y1-y2) * (x3*y4 - y3*x4)) / d;

    //        return  pt;
    //    }
    //    else
    return  cv :: Point2f (-1, -1);
}

void Form_Projector_Mapping::sortCorners (std :: vector <cv :: Point2f> & corners,
                                          cv :: Point2f center)
{
    //    std :: vector <cv :: Point2f> top, bot;

    //    for  ( int  i = 0; i <corners.size (); i ++)
    //    {
    //        if  (corners [i] .y <center.y)
    //            top.push_back (corners [i]);
    //        else
    //            bot.push_back (corners [i]);
    //    }
    //    corners.clear ();

    //    if  (top.size () == 2 && bot.size () == 2) {
    //        cv :: Point2f tl = top [0] .x> top [1] .x? top [1]: top [0];
    //        cv :: Point2f tr = top [0] .x> top [1] .x? top [0]: top [1];
    //        cv :: Point2f bl = bot [0] .x> bot [1] .x? bot [1]: bot [0];
    //        cv :: Point2f br = bot [0] .x> bot [1] .x? bot [0]: bot [1];


    //        corners.push_back (tl);
    //        corners.push_back (tr);
    //        corners.push_back (br);
    //        corners.push_back (bl);
    //    }
}
void Form_Projector_Mapping::After_Calibration()
{
    //    timer1 = new QTimer(this);
    //    timer1->setInterval(500);
    //    connect(timer1,&QTimer::timeout,this,&Form_Projector_Mapping::on_Timeout);
    //    timer1->start();

    //    qDebug()<<"After_Calibration";
}
