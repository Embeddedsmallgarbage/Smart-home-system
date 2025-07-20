#include "steeringgear.h"
#include <unistd.h>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SteeringGear w;
    while(1)
    {
        w.SGturnClockwise();
        sleep(1);
        w.SGstop();
        sleep(1);
        w.SGcounterclockwise();
        sleep(1);
    }
    return a.exec();
}
