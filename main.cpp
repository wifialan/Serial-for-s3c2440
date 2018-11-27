#include "serialport.h"
#include <QApplication>
//#include "serialwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    SerialWidget w;
    serialport w;
    w.show();

    return a.exec();
}
