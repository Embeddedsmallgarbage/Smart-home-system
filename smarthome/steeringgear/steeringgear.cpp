/**
 * @file steeringgear.cpp
 * @brief 舵机控制类的实现文件
 */
#include "steeringgear.h"
#include <QDebug>

/**
 * @brief 舵机类构造函数
 * @param parent 父对象指针
 * @details 初始化舵机相关的PWM控制文件路径
 */
SteeringGear::SteeringGear(QObject *parent)
{
    this->setParent(parent);
    // PWM通道导出文件
    tim1pwmexport.setFileName("/sys/class/pwm/pwmchip4/export");
    // PWM周期设置文件
    pwm2period.setFileName("/sys/class/pwm/pwmchip4/pwm2/period");
    // PWM占空比设置文件
    pwm2duty_cycle.setFileName("/sys/class/pwm/pwmchip4/pwm2/duty_cycle");
    // PWM使能控制文件
    pwm2enable.setFileName("/sys/class/pwm/pwmchip4/pwm2/enable");
}

/**
 * @brief 控制舵机顺时针旋转
 * @details 通过设置PWM信号参数使舵机顺时针旋转
 *          1. 导出PWM通道2
 *          2. 设置PWM周期为20ms(20,000,000ns)
 *          3. 设置PWM占空比为0.5ms(500,000ns)，对应舵机顺时针旋转
 *          4. 使能PWM输出
 */
void SteeringGear::SGturnClockwise()
{
    if (!tim1pwmexport.exists()) {
        qDebug() << "/sys/class/pwm/pwmchip4/export: 文件不存在" << endl;
        return;
    }

    /* 打开TIM1CH3通道 */
    if(!tim1pwmexport.open(QIODevice::WriteOnly))
        qDebug()<<tim1pwmexport.errorString();
    tim1pwmexport.write("2");
    tim1pwmexport.close();
    
    /* 设置周期为20ms */
    if(!pwm2period.open(QIODevice::WriteOnly))
        qDebug()<<pwm2period.errorString();
    pwm2period.write("20000000");
    pwm2period.close();
    
    /* 设置PWM占空比,0.5ms */
    if(!pwm2duty_cycle.open(QIODevice::WriteOnly))
        qDebug()<<pwm2duty_cycle.errorString();
    pwm2duty_cycle.write("500000");
    pwm2duty_cycle.close();
    
    /* 使能TIM1CH3通道 */
    if(!pwm2enable.open(QIODevice::WriteOnly))
        qDebug()<<pwm2enable.errorString();
    pwm2enable.write("1");
    pwm2enable.close();
}

/**
 * @brief 控制舵机逆时针旋转
 * @details 通过设置PWM信号参数使舵机逆时针旋转
 *          1. 导出PWM通道2
 *          2. 设置PWM周期为20ms(20,000,000ns)
 *          3. 设置PWM占空比为2.5ms(2,500,000ns)，对应舵机逆时针旋转
 *          4. 使能PWM输出
 */
void SteeringGear::SGcounterclockwise()
{
    if (!tim1pwmexport.exists()) {
        qDebug() << "/sys/class/pwm/pwmchip4/export: 文件不存在" << endl;
        return;
    }

    /* 打开TIM1CH3通道 */
    if(!tim1pwmexport.open(QIODevice::WriteOnly))
        qDebug()<<tim1pwmexport.errorString();
    tim1pwmexport.write("2");
    tim1pwmexport.close();
    
    /* 设置周期为20ms */
    if(!pwm2period.open(QIODevice::WriteOnly))
        qDebug()<<pwm2period.errorString();
    pwm2period.write("20000000");
    pwm2period.close();
    
    /* 设置PWM占空比,2.5ms */
    if(!pwm2duty_cycle.open(QIODevice::WriteOnly))
        qDebug()<<pwm2duty_cycle.errorString();
    pwm2duty_cycle.write("2500000");
    pwm2duty_cycle.close();
    
    /* 使能TIM1CH3通道 */
    if(!pwm2enable.open(QIODevice::WriteOnly))
        qDebug()<<pwm2enable.errorString();
    pwm2enable.write("1");
    pwm2enable.close();
}

/**
 * @brief 停止舵机旋转
 * @details 通过禁用PWM输出使舵机停止旋转
 */
void SteeringGear::SGstop()
{
    if (!pwm2enable.exists()) {
        qDebug() << "/sys/class/pwm/pwmchip4/pwm2/enable: 文件不存在" << endl;
        return;
    }

    // 禁用PWM输出，写入0表示禁用
    if(!pwm2enable.open(QIODevice::WriteOnly))
        qDebug()<<pwm2enable.errorString();
    pwm2enable.write("0");
    pwm2enable.close();
}

/**
 * @brief 舵机类析构函数
 */
SteeringGear::~SteeringGear()
{
}

