/**
 * @file led.cpp
 * @brief LED控制类的实现文件
 */
#include "led.h"
#include <QDebug>

/**
 * @brief LED类构造函数
 * @param parent 父对象指针
 * @details 初始化LED设备文件路径，在ARM平台上设置LED触发方式
 */
Led::Led(QObject *parent)
{
    this->setParent(parent);
#if __arm__
    /* 默认是出厂系统的LED心跳的触发方式,想要控制LED，
     * 需要改变LED的触发方式，改为none，即无 */
    system("echo none > /sys/class/leds/sys-led/trigger");
#endif
    /* 开发板的LED控制接口 */
    file.setFileName("/sys/devices/platform/leds/leds/sys-led/brightness");
}

/**
 * @brief LED类析构函数
 */
Led::~Led()
{
}

/**
 * @brief 设置LED的状态
 * @param flag LED状态，true表示打开，false表示关闭
 * @details 通过向系统文件写入0或1控制LED的亮灭
 */
void Led::setLedState(bool flag)
{
    qDebug()<<flag<<endl;

    /* 如果文件不存在，则返回 */
    if (!file.exists())
        return;

    if(!file.open(QIODevice::ReadWrite))
        qDebug()<<file.errorString();

    QByteArray buf[2] = {"0", "1"};

    /* 写0或1,1~255都可以点亮LED */
    if (flag)
        file.write(buf[1]);
    else
        file.write(buf[0]);

    /* 关闭文件 */
    file.close();
}

