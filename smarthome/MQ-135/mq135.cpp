/**
 * @file mq135.cpp
 * @brief MQ-135气体传感器驱动类的实现文件
 */
#include "mq135.h"
#include <QDebug>
#include <cmath>

/**
 * @brief MQ-135类构造函数
 * @param parent 父对象指针
 * @details 初始化MQ-135传感器设备文件路径
 */
mq135::mq135(QObject *parent)
{
    this->setParent(parent);
    // 设置原始数据文件路径
    rawdatafile.setFileName("/sys/bus/iio/devices/iio:device0/in_voltage10_raw");
    // 设置数据缩放系数文件路径
    scalefile.setFileName("/sys/bus/iio/devices/iio:device0/in_voltage_scale");
}

/**
 * @brief MQ-135类析构函数
 */
mq135::~mq135()
{
}

/**
 * @brief 读取MQ-135的原始输出电压
 * @return 返回传感器的输出电压值，单位为V
 * @details 通过读取ADC的原始值和缩放系数计算实际电压值
 */
float mq135::readmq135value()
{
    float rawdata = 0;    // ADC原始值
    float scale = 0;      // ADC缩放系数
    float vout;           // 输出电压

    QString dht11_value = NULL;
    // 检查文件是否存在
    if (!(rawdatafile.exists() && scalefile.exists()))
        return 0;

    /* 读取原始值和精度 */
    if(rawdatafile.open(QIODevice::ReadOnly)) {
        rawdata = QString(rawdatafile.readAll()).toFloat();
    } else {
        qDebug()<<"error open file:"<<rawdatafile.errorString();
    }
    rawdatafile.close();
    
    if(scalefile.open(QIODevice::ReadOnly)) {
        scale = QString(scalefile.readAll()).toFloat();
    } else {
        qDebug()<<"error open file:"<<scalefile.errorString();
    }
    scalefile.close();

    /* 计算模拟电压: 原始值 * 缩放系数 / 1000(转换为V) */
    vout = (rawdata * scale) / 1000;
    return vout;
}

/**
 * @brief 计算空气中CO2等污染物的浓度
 * @return 返回计算得到的PPM浓度值，保留一位小数
 * @details 通过传感器电阻值和公式计算空气中污染物的浓度
 *          公式: ppm = A * (Rs/R0)^B, 其中Rs为当前传感器电阻，R0为洁净空气中电阻
 */
float mq135::calculateppm()
{
    // 读取电压值
    float vrl = readmq135value();
    // 计算传感器电阻值: Rs = (Vc-Vrl)*RL/Vrl
    float rs = (VC - vrl) * RL / vrl;
    // 计算电阻比: Rs/R0
    float rs0 = rs / R0;

    // 使用公式计算PPM值: ppm = A * (Rs/R0)^B
    float ppm = A * pow(rs0, B);
    // 保留一位小数
    int temp = (int)(ppm * 10);
    ppm = (float)temp / 10;

    return ppm;
}

