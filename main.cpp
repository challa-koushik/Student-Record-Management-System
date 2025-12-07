#include <QApplication>
#include "srmswindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    SRMSWindow w;
    w.show();

    return a.exec();
}
