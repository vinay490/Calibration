#include "form_projector.h"
#include "ui_form_projector.h"

Form_projector::Form_projector(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form_projector)
{
    ui->setupUi(this);
    QPixmap  pix1(":/IMAGES/17x9.jpg");
//    QPixmap pix1(":/IMAGES/download.jpg");

    ui->label->setPixmap(pix1);
}

Form_projector::~Form_projector()
{
    delete ui;
}
