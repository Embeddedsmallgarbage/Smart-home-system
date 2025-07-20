/**
 * @file dht11.h
 * @brief DHT11温湿度传感器驱动类的头文件
 * @details 通过读取Linux系统文件接口获取DHT11传感器的温湿度数据
 */
#ifndef DHT11_H
#define DHT11_H

#include <QObject>
#include <QFile>

/**
 * @class dht11
 * @brief DHT11温湿度传感器驱动类
 * @details 负责与DHT11温湿度传感器通信，获取传感器的温湿度数据
 */
class dht11 : public QObject
{
    Q_OBJECT

private:
    QFile file;    ///< 文件对象，用于访问DHT11设备文件

public:
    /**
     * @brief 构造函数
     * @param parent 父对象指针
     */
    dht11(QObject *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~dht11();

    /**
     * @brief 读取DHT11传感器的温湿度值
     * @return 返回温湿度值的字符串，格式为"湿度温度"，例如"3525"表示湿度35%，温度25℃
     */
    QString readDHT11value();
};
#endif // DHT11_H
