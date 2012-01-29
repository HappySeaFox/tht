#include <QtGui/QApplication>
#include "tht.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    THT w;
    w.show();
    
    return a.exec();
}
