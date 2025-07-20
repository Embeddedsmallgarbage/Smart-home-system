/**
 * @file led.h
 * @brief LED控制类的头文件
 * @details 通过Linux系统文件接口控制LED的开关状态
 */
#ifndef LED_H
#define LED_H

#include <QObject>
#include <QFile>

/**
 * @class Led
 * @brief LED控制类
 * @details 负责控制开发板上LED的开关状态
 */
class Led : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief 构造函数
     * @param parent 父对象指针
     */
    Led(QObject *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~Led();

    /**
     * @brief 设置LED的状态
     * @param flag LED状态，true表示打开，false表示关闭
     */
    void setLedState(bool);

private:
    /**
     * @brief 文件对象
     * @details 用于访问LED设备控制文件
     */
    QFile file;
};
#endif // LED_H
