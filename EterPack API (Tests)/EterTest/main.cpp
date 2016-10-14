#include <QtGui/QApplication>
#include "test.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Test w;
    w.show();
    
    return a.exec();
}
