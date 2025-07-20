#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtMqtt/qmqttclient.h>
#include <QtMqtt/qmqtttopicfilter.h>
#include <QtMqtt/qmqttsubscription.h>
#include <QtMqtt/qmqttmessage.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLayout>
#include <QLabel>
#include <QFont>
#include <QRadioButton>
#include <QPushButton>
#include <QPixmap>
#include <QProgressBar>
#include <QDebug>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QMqttClient *client;                 // mqtt客户端

    /* 主窗口相关变量 */
    QWidget *mainwidget;                 // 主窗口
    QWidget *widget[4];
    QHBoxLayout *hboxlayout[4];
    QVBoxLayout *mainvboxlayout;         // 主窗口采用垂直布局
    QLabel *homename;                    // 智能家居系统名称

    /* 客厅灯相关变量 */
    QWidget *lumpwidget;                 // 客厅灯窗口部件
    QVBoxLayout *lumpvboxlayout;         // 客厅灯垂直布局
    QLabel *lumpstatusimage;             // 客厅灯状态图片
    QRadioButton *livingroomlumpswitch;  // 客厅灯开关

    /* 温度相关变量 */
    QWidget *tempwidget[2];              // 温度窗口部件
    QVBoxLayout *tempvboxlayout;
    QHBoxLayout *temphboxlayout;
    QLabel *temperature;                 // 温度数字
    QLabel *tempname;                    // 温度名字
    QProgressBar *thermometer;           // 温度计

    /* 湿度相关变量 */
    QWidget *humiditywidget;
    QLabel *humidityname;
    QLabel *humidity;
    QVBoxLayout *humidityvboxlayout;

    /* ppm相关变量 */
    QWidget *ppmwidget;
    QLabel *ppmname;
    QLabel *ppm;
    QVBoxLayout *ppmvboxlayout;

    /* 继电器相关变量 */
    QWidget *relaywidget;
    QVBoxLayout *relayvboxlayout;         // 继电器垂直布局
    QLabel *relaystatusimage;             // 继电器状态图片
    QRadioButton *relayswitch;            // 继电器开关

    /* 窗帘相关变量 */
    QWidget *curtainwidget;
    QLabel *curtainname;
    QWidget *controlwidget;
    QPushButton *curtainopen;
    QPushButton *curtainstop;
    QPushButton *curtainclose;
    QHBoxLayout *controlhboxlayout;
    QVBoxLayout *curtainvboxlayout;

    void ConnectMQTT();                             // 连接阿里云物联网平台
    void subscribeTopic(QString topic);             // 订阅主题
    void publicMessage(QString topic, QString mes); // 发布消息

    void InterfaceLayout();                         // 界面布局
    void LumpInterfaceLayout();                     // 客厅灯部分的界面布局
    void TmeperatureInterfaceLayout();              // 温度部分的界面布局
    void HumidityInterfaceLayout();                 // 湿度部分的界面布局
    void PpmInterfaceLayout();                      // 空气质量部分的界面布局
    void CurtainInterfaceLayout();                  // 窗帘部分的界面布局
    void RelayInterfaceLayout();                    // 继电器(加湿器)部分界面布局

private slots:
    void ConnectionSucceeded();                         // 连接阿里云物联网平台成功后处理
    void MessageReceived(const QByteArray &message, const QMqttTopicName &topic);
    void ErrorHandling(QMqttClient::ClientError error); // mqtt客户端错误处理

    void LivingroomlumpControls(bool checked);          // 客厅灯控制
    void LumpStatusImageChange(bool checked);           // 客厅灯状态图片改变

    void RelayControls(bool checked);          // 继电器控制
    void RelayStatusImageChange(bool checked);          // 继电器状态图片改变

    void CurtainControl();                              // 窗帘控制
};
#endif // MAINWINDOW_H
