#include <QtGui/QApplication>
#include "EterNexus.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if(argc >= 3 && argv)
    {
        s_processByArg = true;
        
        s_argv = argv;
        s_argc = argc;
    }
    
    EterNexus w;
    w.show();
    
    return a.exec();
}
