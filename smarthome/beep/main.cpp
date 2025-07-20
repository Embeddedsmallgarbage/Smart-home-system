#include "beep.h"
#include <unistd.h>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    beep w;
    while (1) {
        w.setBeepState(1);
        sleep(1);
        w.setBeepState(0);
        sleep(1);
    }
    //w.show();
    return a.exec();
}
