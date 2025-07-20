/**
 * @file steeringgear.h
 * @brief 舵机控制类的头文件
 * @details 通过Linux系统PWM接口控制舵机的旋转
 */
#ifndef STEERINGGEAR_H
#define STEERINGGEAR_H

#include <QObject>
#include <QFile>

/**
 * @class SteeringGear
 * @brief 舵机控制类
 * @details 负责控制开发板上舵机的旋转方向和停止动作
 */
class SteeringGear : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父对象指针
     */
    SteeringGear(QObject *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~SteeringGear();

    /**
     * @brief 控制舵机顺时针旋转
     * @details 通过设置适当的PWM占空比使舵机顺时针旋转
     */
    void SGturnClockwise();    // 顺时针转
    
    /**
     * @brief 控制舵机逆时针旋转
     * @details 通过设置适当的PWM占空比使舵机逆时针旋转
     */
    void SGcounterclockwise(); // 逆时针转
    
    /**
     * @brief 停止舵机旋转
     * @details 通过禁用PWM信号使舵机停止旋转
     */
    void SGstop();             // 停转

private:
    QFile tim1pwmexport;
    QFile pwm2period;
    QFile pwm2duty_cycle;
    QFile pwm2enable;
};
#endif // STEERINGGEAR_H
