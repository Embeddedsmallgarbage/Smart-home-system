#include "led.h"

#include <QApplication>
#include <unistd.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Led w;
    while(1) {
       w.setLedState(1);
       sleep(1);
       w.setLedState(0);
       sleep(1);
    }
    return a.exec();
}
