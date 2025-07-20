/**
 * @file Esp8266.h
 * @brief ESP8266 WiFi模块通信类的头文件
 * @details 负责与ESP8266 WiFi模块通信，处理MQTT消息，控制智能家居设备
 */
#ifndef ESP8266_H
#define ESP8266_H

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <QJsonObject>
#include <QJsonDocument>
#include "../led/led.h"
#include "../dht11/dht11.h"
#include "../MQ-135/mq135.h"
#include "../beep/beep.h"
#include "../steeringgear/steeringgear.h"
#include "../relay/relay.h"

/**
 * @class Esp8266
 * @brief ESP8266 WiFi模块通信类
 * @details 管理与ESP8266模块的通信，实现智能家居的联网功能，包括：
 *          - 配置ESP8266模块
 *          - 连接WiFi网络
 *          - 连接MQTT服务器
 *          - 上传传感器数据
 *          - 解析控制命令并执行相应动作
 */
class Esp8266 : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父对象指针
     */
    Esp8266(QWidget *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~Esp8266();

private:
    Led *led;                 ///< LED控制对象
    QSerialPort *serialPort;  ///< 串口通信对象，用于与ESP8266模块通信
    dht11 *DHT11;             ///< DHT11温湿度传感器对象
    beep *Beep;               ///< 蜂鸣器控制对象
    mq135 *MQ135;             ///< MQ-135气体传感器对象
    SteeringGear *sg;         ///< 舵机控制对象
    Relay *relay;             ///< 继电器控制对象
    QTimer *datauploadtimer;  ///< 数据上传定时器，定时上传温湿度和气体浓度数据

    /**
     * @brief 发送AT命令到ESP8266模块
     * @param cmd 要发送的AT命令字符串
     */
    void sendCmdToEsp8266(QString cmd);

    QString readData = nullptr;   ///< 从ESP8266接收到的数据缓存

private slots:
    /**
     * @brief 串口数据接收处理函数
     * @details 当串口有数据可读时，读取并处理ESP8266返回的数据
     */
    void serialPortReadyRead();
    
    /**
     * @brief 传感器数据定时上传函数
     * @details 定时读取温湿度传感器和气体传感器数据，通过MQTT上传到云端
     */
    void uploadDate();
};

#endif // ESP8266_H
