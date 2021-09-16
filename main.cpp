#include <QApplication>
#include "main_refrf.h"


int main(int argc, char *argv[])
{

    QApplication a(argc, argv);

    a.setApplicationVersion(APP_VERSION);
    Main_TD w;

    w.show();

    return a.exec();
}
