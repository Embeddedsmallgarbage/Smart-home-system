#include "mq135.h"
#include <QDebug>
#include <unistd.h>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    mq135 w;
    float v1, v2;
    //w.show();
    while(1)
    {
        v1 = w.readmq135value();
        v2 = w.calculateppm();
        qDebug()<<"电压："<<v1<<" 空气质量："<<v2<<endl;
        sleep(1);
    }
    return a.exec();
}
