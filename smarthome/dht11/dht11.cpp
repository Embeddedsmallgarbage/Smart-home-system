/**
 * @file dht11.cpp
 * @brief DHT11温湿度传感器驱动类的实现文件
 */
#include "dht11.h"
#include <QDebug>

/**
 * @brief dht11类构造函数
 * @param parent 父对象指针
 * @details 初始化DHT11设备文件路径
 */
dht11::dht11(QObject *parent)
{
    this->setParent(parent);
    // 设置DHT11传感器的设备文件路径
    file.setFileName("/sys/class/misc/dht11/value");
}

/**
 * @brief dht11类析构函数
 */
dht11::~dht11()
{
}

/**
 * @brief 读取DHT11传感器的温湿度值
 * @return 返回温湿度值的字符串，格式为"湿度温度"，例如"3525"表示湿度35%，温度25℃
 * @details 通过读取Linux系统文件接口获取DHT11传感器数据
 */
QString dht11::readDHT11value()
{
    QString dht11_value = NULL;
    
    // 检查设备文件是否存在
    if (!file.exists())
        return dht11_value;

    // 打开并读取设备文件内容
    if(file.open(QIODevice::ReadOnly)) {
        dht11_value = QString(file.readAll());
    } else {
        qDebug()<<"error open file:"<<file.errorString();
    }
    
    // 关闭文件
    file.close();
    return dht11_value;
}
