#include "dht11.h"
#include <QDebug>
#include <unistd.h>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    dht11 w;
    while(1) {
        qDebug() << w.readDHT11value()<<endl;
        sleep(1);
    }
    //w.show();
    return a.exec();
}
