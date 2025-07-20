/**
 * @file relay.h
 * @brief 继电器控制类的头文件
 * @details 通过Linux系统文件接口控制继电器的开关状态
 */
#ifndef RELAY_H
#define RELAY_H

#include <QObject>
#include <QFile>

/**
 * @class Relay
 * @brief 继电器控制类
 * @details 负责控制开发板上继电器的开关状态
 */
class Relay : public QObject
{
    Q_OBJECT

private:
    /**
     * @brief 文件对象
     * @details 用于访问继电器设备控制文件
     */
    QFile file;

public:
    /**
     * @brief 构造函数
     * @param parent 父对象指针
     */
    Relay(QObject *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~Relay();

    /**
     * @brief 设置继电器的状态
     * @param flag 继电器状态，true表示打开，false表示关闭
     */
    void setRelayState(bool);
};
#endif // RELAY_H
