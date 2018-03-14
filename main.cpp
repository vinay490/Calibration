#include "calibration_main.h"
#include <QApplication>
#include <QDesktopWidget>
#include <form_projector.h>
#include <form_circles_calibration.h>
#include  "form_projector_mapping.h"
#include "form_contours_roi.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);




    Calibration_Main w;
    w.showMaximized();


    //    Form_Contours_ROI myroi;
    //    myroi.showMaximized();

    //    Form_Projector_Mapping  p;
    //    p.showMaximized();

    //    Form_Circles_Calibration circle;
    //    circle.showMaximized();


    Form_projector project;
    QDesktopWidget *widget = QApplication::desktop();
    qDebug()<<"Number of screens:"<< widget->screenCount();
    for (int i = 0; i < widget->screenCount(); i++)
    {
        qDebug()<<"screen Geometry:"<< widget->screenGeometry(i);
    }
    // Get geometry of Secondary screen
    QRect rect = widget->screenGeometry(1);
    project.move(rect.width(), rect.y());
    project.showFullScreen();

    return a.exec();
}
