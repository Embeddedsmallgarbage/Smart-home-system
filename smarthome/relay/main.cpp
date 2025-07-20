#include "relay.h"
#include <unistd.h>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Relay w;
    while (1) {
        w.setRelayState(1);
        sleep(1);
        w.setRelayState(0);
        sleep(1);
    }
    //w.show();
    return a.exec();
}
