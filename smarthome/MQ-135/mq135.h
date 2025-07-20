/**
 * @file mq135.h
 * @brief MQ-135气体传感器驱动类的头文件
 * @details 通过Linux系统文件接口读取MQ-135气体传感器数据并计算空气质量
 */
#ifndef MQ135_H
#define MQ135_H

#include <QObject>
#include <QFile>

/**
 * @class mq135
 * @brief MQ-135气体传感器驱动类
 * @details 负责与MQ-135气体传感器通信，获取传感器的数据并计算空气质量指标
 */
class mq135 : public QObject
{
    Q_OBJECT

private:
    float RL = 1.0;          ///< 负载电阻值，单位为KΩ
    float R0 = 34.0;         ///< 传感器在洁净空气中的电阻值，单位为KΩ
    float VC = 5.0;          ///< 供电电压，单位为V
    float A = 4.103;         ///< 浓度计算公式参数A
    float B = -2.317;        ///< 浓度计算公式参数B

    QFile rawdatafile;       ///< 文件对象，用于访问传感器原始数据文件
    QFile scalefile;         ///< 文件对象，用于访问传感器数据缩放系数文件

public:
    /**
     * @brief 构造函数
     * @param parent 父对象指针
     */
    mq135(QObject *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~mq135();

    /**
     * @brief 读取MQ-135的原始输出电压
     * @return 返回传感器的输出电压值，单位为V
     */
    float readmq135value();
    
    /**
     * @brief 计算空气中CO2等污染物的浓度
     * @return 返回计算得到的PPM浓度值
     * @details 通过传感器电阻值和公式计算空气中污染物的浓度
     */
    float calculateppm();
};
#endif // MQ135_H
