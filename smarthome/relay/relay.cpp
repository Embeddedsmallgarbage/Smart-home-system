/**
 * @file relay.cpp
 * @brief 继电器控制类的实现文件
 */
#include "relay.h"
#include <QDebug>

/**
 * @brief 继电器类构造函数
 * @param parent 父对象指针
 * @details 初始化继电器设备文件路径
 */
Relay::Relay(QObject *parent)
{
    this->setParent(parent);
    file.setFileName("/sys/class/leds/relay/brightness");
}

/**
 * @brief 继电器类析构函数
 */
Relay::~Relay()
{
}

/**
 * @brief 设置继电器的状态
 * @param flag 继电器状态，true表示打开，false表示关闭
 * @details 通过向系统文件写入0或1控制继电器的开闭
 */
void Relay::setRelayState(bool flag)
{
    /* 如果文件不存在，则返回 */
    if (!file.exists())
        return;

    if(!file.open(QIODevice::ReadWrite))
        qDebug()<<file.errorString();

    QByteArray buf[2] = {"0", "1"};

    /* 写0或1,1表示打开继电器，0表示关闭继电器 */
    if (flag)
        file.write(buf[1]);
    else
        file.write(buf[0]);

    /* 关闭文件 */
    file.close();
}
