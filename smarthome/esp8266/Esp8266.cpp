/**
 * @file Esp8266.cpp
 * @brief ESP8266 WiFi模块通信类的实现文件
 */
#include "Esp8266.h"
#include <unistd.h>
#include <QDebug>

/**
 * @brief ESP8266类构造函数
 * @param parent 父对象指针
 * @details 初始化串口通信参数，建立与ESP8266模块的通信，并初始化各个设备控制对象
 */
Esp8266::Esp8266(QWidget *parent)
{
    Q_UNUSED(parent)
    /* 串口对象，用于与Esp8266模块通信 */
    serialPort = new QSerialPort(this);

    // 初始化各个设备控制对象
    led = new Led(this);
    DHT11 = new dht11();
    MQ135 = new mq135();
    Beep = new beep();
    sg = new SteeringGear();
    relay = new Relay();

    // 根据平台设置不同的串口设备名
#if __arm__
    serialPort->setPortName("ttySTM2");
#else
    serialPort->setPortName("ttyUSB0");
#endif

    /* 设置串口通信参数 */
    serialPort->setBaudRate(115200);       // 波特率
    serialPort->setDataBits(QSerialPort::Data8);         // 数据位
    serialPort->setParity(QSerialPort::NoParity);        // 奇偶校验
    serialPort->setStopBits(QSerialPort::OneStop);       // 停止位
    serialPort->setFlowControl(QSerialPort::NoFlowControl); // 流控制
    
    // 尝试打开串口
    if (!serialPort->open(QIODevice::ReadWrite))
        qDebug()<<"串口无法打开！可能正在被使用！"<<endl;
    else {
        qDebug()<<"串口打开成功！"<<endl;
    }

    /* 发送模块复位命令 */
    sendCmdToEsp8266("AT+RST");
    
    /* 连接串口信号与槽，接收和处理串口数据 */
    connect(serialPort, &QSerialPort::readyRead, this, &Esp8266::serialPortReadyRead);

    /* 创建并启动数据上传定时器，设置10秒间隔 */
    datauploadtimer = new QTimer();
    datauploadtimer->start(10000);
    
    /* 连接定时器超时信号与数据上传槽函数 */
    connect(datauploadtimer, &QTimer::timeout, this, &Esp8266::uploadDate);
}

/**
 * @brief ESP8266类析构函数
 */
Esp8266::~Esp8266()
{
}

/**
 * @brief 传感器数据定时上传函数
 * @details 读取温湿度传感器和气体传感器数据，封装为JSON格式，通过MQTT协议上传到云平台
 */
void Esp8266::uploadDate()
{
    /* 获取温湿度数据 */
    QString value = DHT11->readDHT11value();
    int humidity = value.mid(0, 2).toInt();      // 提取湿度值
    int temperature = value.mid(2, 2).toInt();   // 提取温度值

    /* 获取空气质量数据(ppm) */
    float ppm = MQ135->calculateppm();
    
    // 当气体浓度超过阈值时触发报警
    if(ppm >= 10) {
        Beep->setBeepState(1);  // 打开蜂鸣器
    } else {
        Beep->setBeepState(0);  // 关闭蜂鸣器
    }

    /* 创建JSON对象并添加传感器数据 */
    QJsonObject jsonObj;
    jsonObj.insert("humidity", humidity);
    jsonObj.insert("temperature", temperature);
    jsonObj.insert("ppm", ppm);

    /* 将JSON对象转换为紧凑格式的JSON字符串 */
    QJsonDocument jsonDoc(jsonObj);
    QByteArray jsonData = jsonDoc.toJson(QJsonDocument::Compact);

    /* 将JSON字符串中的特殊字符进行转义处理 */
    QString escapedjson(jsonData);
    escapedjson.replace(",", "\\,");
    escapedjson.replace("\"", "\\\"");

    /* 通过MQTT协议发布传感器数据到指定主题 */
    sendCmdToEsp8266(
        QString("AT+MQTTPUB=0,\"/k25r9vo1EmV/esp8266/user/update\",\"%1\",0,0")
            .arg(escapedjson)
    );
}

/**
 * @brief 发送AT命令到ESP8266模块
 * @param cmd 要发送的AT命令字符串
 * @details 在命令末尾添加回车换行符，并通过串口发送到ESP8266模块
 */
void Esp8266::sendCmdToEsp8266(QString cmd)
{
    // 在命令末尾添加回车换行符
    cmd = cmd + "\r\n";
    qDebug() << cmd << endl;

    // 将命令转换为UTF-8编码并发送
    QByteArray data = cmd.toUtf8();
    serialPort->write(data);
}

/**
 * @brief 串口数据接收处理函数
 * @details 读取ESP8266模块返回的数据，并根据不同的返回内容执行相应的操作：
 *          - 处理模块复位响应
 *          - 配置WiFi模式
 *          - 连接WiFi网络
 *          - 配置SNTP服务器
 *          - 配置MQTT客户端
 *          - 连接MQTT服务器
 *          - 订阅MQTT主题
 *          - 处理接收到的MQTT消息并控制相应设备
 */
void Esp8266::serialPortReadyRead()
{
    /* 从接收缓冲区读取数据 */
    QByteArray buf = serialPort->readAll();

    // 将接收到的数据转换为字符串并追加到缓存中
    QString temp = QString(buf);
    readData.append(temp);
    qDebug()<<temp<<endl;

    // 处理ESP8266模块复位成功的响应
    if  (readData.contains("ready")) {
        // 设置为Station模式(STA)
        sendCmdToEsp8266("AT+CWMODE=1");
        readData.clear();
    }

    // 处理设置WiFi模式成功的响应
    if (readData.contains("OK") && readData.contains("AT+CWMODE")) {
        qDebug()<<"设置STA模式成功"<<endl;
        // 连接到指定的WiFi网络
        sendCmdToEsp8266("AT+CWJAP=\"Preference\",\"9624641314\"");
        qDebug()<<"开始连接WIFI"<<endl;
        readData.clear();
    }

    // 处理WiFi连接成功的响应
    if  (readData.contains("WIFI GOT IP") && readData.contains("OK")) {
        qDebug()<<"连接WIFI成功"<<endl;
        sleep(1);
        // 设置时区和SNTP服务器
        sendCmdToEsp8266("AT+CIPSNTPCFG=1,8,\"ntp1.aliyun.com\"");
        qDebug()<<"开始设置时区和SNTP服务器:北京时间,阿里云服务器请等待"<<endl;
        readData.clear();
    }

    // 处理设置SNTP成功的响应
    if (readData.contains("OK") && readData.contains("AT+CIPSNTPCFG")) {
        qDebug()<<"设置时区和SNTP服务器成功"<<endl;
        sleep(1);
        // 配置MQTT用户属性
        sendCmdToEsp8266("AT+MQTTUSERCFG=0,1,\"NULL\",\"esp8266&k25r9vo1EmV\",\"ec9b0ed647b36cd271dc34fdac6171f1b559ed2cfa053a8520dab8df5aaa0ed1\",0,0,\"\"");
        qDebug()<<"开始设置MQTT用户属性"<<endl;
        readData.clear();
    }

    // 处理设置MQTT用户属性成功的响应
    if (readData.contains("OK") && readData.contains("AT+MQTTUSERCFG")) {
        qDebug()<<"MQTT用户属性设置成功"<<endl;
        sleep(1);
        // 设置MQTT客户端ID
        sendCmdToEsp8266("AT+MQTTCLIENTID=0,\"k25r9vo1EmV.esp8266|securemode=2\\,signmethod=hmacsha256\\,timestamp=1735386764701|\"");
        qDebug()<<"开始设置MQTT客户端ID"<<endl;
        readData.clear();
    }

    // 处理设置MQTT客户端ID成功的响应
    if (readData.contains("OK") && readData.contains("AT+MQTTCLIENTID")) {
        qDebug()<<"MQTT客户端ID设置成功"<<endl;
        sleep(1);
        // 连接MQTT Broker
        sendCmdToEsp8266("AT+MQTTCONN=0,\"iot-06z00jdbr4qssk5.mqtt.iothub.aliyuncs.com\",1883,1");
        qDebug()<<"开始连接MQTT Broker"<<endl;
        readData.clear();
    }

    // 处理MQTT连接成功的响应
    if (readData.contains("OK") && readData.contains("AT+MQTTCONN")) {
        qDebug()<<"MQTT Broker连接成功"<<endl;
        sleep(1);
        // 订阅MQTT主题
        sendCmdToEsp8266("AT+MQTTSUB=0,\"/k25r9vo1EmV/esp8266/user/get\",0");
        qDebug()<<"订阅主题"<<endl;
        readData.clear();
    }

    // 处理MQTT主题订阅成功的响应
    if (readData.contains("OK") && readData.contains("AT+MQTTSUB")) {
        qDebug()<<"主题订阅成功"<<endl;
        readData.clear();
    }

    // 处理客厅灯控制的MQTT消息
    if (readData.contains("livingroomlump") && readData.contains("MQTTSUBRECV")) {
        if (readData.contains("open")) {
            led->setLedState(true);  // 打开LED灯
            qDebug()<<"客厅灯打开"<<endl;
            readData.clear();
        } else if (readData.contains("close")) {
            led->setLedState(false); // 关闭LED灯
            qDebug()<<"客厅灯关闭"<<endl;
            readData.clear();
        }
    }

    // 处理窗帘控制的MQTT消息
    if (readData.contains("curtain") && readData.contains("MQTTSUBRECV")) {
        if (readData.contains("open")) {
            sg->SGturnClockwise();   // 窗帘顺时针转动(打开)
            qDebug()<<"窗帘打开"<<endl;
            readData.clear();
        } else if(readData.contains("close")) {
            sg->SGcounterclockwise(); // 窗帘逆时针转动(关闭)
            qDebug()<<"窗帘关闭"<<endl;
            readData.clear();
        } else if(readData.contains("stop")) {
            sg->SGstop();            // 窗帘停止转动
            qDebug()<<"窗帘停止"<<endl;
            readData.clear();
        }
    }

    // 处理继电器控制的MQTT消息
    if (readData.contains("relay") && readData.contains("MQTTSUBRECV")) {
        if (readData.contains("open")) {
            relay->setRelayState(true);  // 打开继电器
            qDebug()<<"继电器打开"<<endl;
            readData.clear();
        } else if (readData.contains("close")) {
            relay->setRelayState(false); // 关闭继电器
            qDebug()<<"继电器关闭"<<endl;
            readData.clear();
        }
    }
}

