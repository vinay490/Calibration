#ifndef FORM_PROJECTOR_H
#define FORM_PROJECTOR_H

#include <QWidget>
#include <QPixmap>
#include <QTimer>
namespace Ui {
class Form_projector;
}

class Form_projector : public QWidget
{
    Q_OBJECT

public:
    explicit Form_projector(QWidget *parent = 0);
    ~Form_projector();

private:
    Ui::Form_projector *ui;
    QTimer t1;

};

#endif // FORM_PROJECTOR_H
