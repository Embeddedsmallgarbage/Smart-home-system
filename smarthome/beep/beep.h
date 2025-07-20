/**
 * @file beep.h
 * @brief 蜂鸣器控制类的头文件
 * @details 通过Linux系统文件接口控制蜂鸣器的开关状态
 */
#ifndef BEEP_H
#define BEEP_H

#include <QObject>
#include <QFile>

/**
 * @class beep
 * @brief 蜂鸣器控制类
 * @details 负责控制开发板上蜂鸣器的开关状态
 */
class beep : public QObject
{
    Q_OBJECT

private:
    /**
     * @brief 文件对象
     * @details 用于访问蜂鸣器设备控制文件
     */
    QFile file;

public:
    /**
     * @brief 构造函数
     * @param parent 父对象指针
     */
    beep(QObject *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~beep();

    /**
     * @brief 设置蜂鸣器的状态
     * @param flag 蜂鸣器状态，true表示打开，false表示关闭
     */
    void setBeepState(bool);
};
#endif // BEEP_H
