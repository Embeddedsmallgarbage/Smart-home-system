/**
 * @file beep.cpp
 * @brief 蜂鸣器控制类的实现文件
 */
#include "beep.h"
#include <QDebug>

/**
 * @brief 蜂鸣器类构造函数
 * @param parent 父对象指针
 * @details 初始化蜂鸣器设备文件路径
 */
beep::beep(QObject *parent)
{
    this->setParent(parent);
    // 设置蜂鸣器控制接口文件路径
    file.setFileName("/sys/devices/platform/leds/leds/beep/brightness");
}

/**
 * @brief 蜂鸣器类析构函数
 */
beep::~beep()
{
}

/**
 * @brief 设置蜂鸣器的状态
 * @param flag 蜂鸣器状态，true表示打开，false表示关闭
 * @details 通过向系统文件写入0或1控制蜂鸣器的开关
 */
void beep::setBeepState(bool flag)
{
    qDebug()<<flag<<endl;

    /* 如果文件不存在，则返回 */
    if (!file.exists())
        return;

    if(!file.open(QIODevice::ReadWrite))
        qDebug()<<file.errorString();

    QByteArray buf[2] = {"0", "1"};

    // 写入1表示开启蜂鸣器，0表示关闭蜂鸣器
    if (flag)
        file.write(buf[1]);
    else
        file.write(buf[0]);

    /* 关闭文件 */
    file.close();
}

