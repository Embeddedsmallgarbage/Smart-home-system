#include "mainwindow.h"
#include <unistd.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    /* 初始化界面布局 */
    InterfaceLayout();  // 调用界面布局函数，设置主窗口和各控件布局

    /* 连接MQTT服务器 */
    ConnectMQTT();  // 建立与阿里云物联网平台的MQTT连接

    /* 连接成功处理 */
    connect(client, &QMqttClient::connected, this, &MainWindow::ConnectionSucceeded);
    /* 主题接收到消息处理 */
    connect(client, &QMqttClient::messageReceived, this, &MainWindow::MessageReceived);
    /* 错误发生处理 */
    connect(client, &QMqttClient::errorChanged, this, &MainWindow::ErrorHandling);

    /* 客厅灯控制 */
    connect(livingroomlumpswitch, &QRadioButton::toggled, this, &MainWindow::LivingroomlumpControls);
    /* 客厅灯状态图片改变 */
    connect(livingroomlumpswitch, &QRadioButton::toggled, this, &MainWindow::LumpStatusImageChange);

    /* 继电器控制 */
    connect(relayswitch, &QRadioButton::toggled, this, &MainWindow::RelayControls);
    /* 继电器状态图片改变 */
    connect(relayswitch, &QRadioButton::toggled, this, &MainWindow::RelayStatusImageChange);

    /* 窗帘控制 */
    connect(curtainopen, &QPushButton::clicked, this, &MainWindow::CurtainControl);
    connect(curtainclose, &QPushButton::clicked, this, &MainWindow::CurtainControl);
    connect(curtainstop, &QPushButton::clicked, this, &MainWindow::CurtainControl);
}

MainWindow::~MainWindow()
{
}

/* 
 * 连接MQTT服务器成功处理
 * 当MQTT客户端成功连接到服务器时触发此槽函数
 */
void MainWindow::ConnectionSucceeded()
{
    qDebug()<<"连接MQTT服务器成功"<<endl;

    /* 订阅主题 */
    subscribeTopic("/k25r9vo1EmV/QtDesktop/user/get");
}

/* 
 * 主题接收到消息处理
 * @param message 接收到的MQTT消息内容
 * @param topic 消息来源的主题名称
 */
void MainWindow::MessageReceived(const QByteArray &message, const QMqttTopicName &topic)
{
    qDebug()<<"主题受到消息 "<<topic.name()<<": "<<message<<endl;

    /* 解析Json数据 */
    QJsonDocument doc = QJsonDocument::fromJson(message);
    if(doc.isObject()) {
        this->homename->setText("云启慧居");
        QJsonObject jsonObject = doc.object();

        int temperature = jsonObject["temperature"].toInt();
        int humidity = jsonObject["humidity"].toInt();
        float ppm = jsonObject["ppm"].toDouble();

        qDebug()<< "temperature: "<< temperature << "humidity: " << humidity << "ppm: " << ppm << endl;

        this->temperature->setText(QString::number(temperature) + " \u2103");
        this->thermometer->setValue(temperature);

        this->humidity->setText(QString::number(humidity) + " %RH");

        this->ppm->setText(QString::number(ppm) + " ppm");
        if(ppm >= 10) {
            this->ppm->setStyleSheet("color: red;");
        } else {
            this->ppm->setStyleSheet("color: white;");
        }
    } else {
        qDebug()<<"Invalid JSON received."<<endl;
    }
}

/* 
 * 客户端错误处理
 * @param error MQTT客户端错误代码
 */
void MainWindow::ErrorHandling(QMqttClient::ClientError error)
{
    qDebug()<<"客户端错误: "<<error<<endl;
}

/* 订阅主题 */
void MainWindow::subscribeTopic(QString topic)
{
    QMqttSubscription *subscription = client->subscribe(QMqttTopicFilter(topic));
    if (!subscription) {
        qDebug()<<"主题订阅失败: "<<topic<<endl;
        return;
    } else {
        qDebug()<<"主题订阅成功: "<<topic<<endl;
    }
}

/* 发布消息 */
void MainWindow::publicMessage(QString topic, QString mes)
{
    QMqttTopicName topicname(topic);
    QByteArray message = mes.toUtf8();
    client->publish(topicname, message);
}

/* 
 * 客厅灯控制
 * @param checked 开关状态，true表示开启，false表示关闭
 */
void MainWindow::LivingroomlumpControls(bool checked)
{
    if (checked) {
        publicMessage("/k25r9vo1EmV/QtDesktop/user/update", "{\"livingroomlump\":\"open\"}");
    } else {
        publicMessage("/k25r9vo1EmV/QtDesktop/user/update", "{\"livingroomlump\":\"close\"}");
    }
}

/* 
 * 客厅灯状态图片改变
 * 根据开关状态切换灯泡图片
 * @param checked 开关状态，true表示开启，false表示关闭
 */
void MainWindow::LumpStatusImageChange(bool checked)
{
    QPixmap pixmap = checked ? QPixmap(":/images/lump_open") : QPixmap(":/images/lump_close");
    lumpstatusimage->setPixmap(pixmap.scaled(lumpstatusimage->size(), Qt::KeepAspectRatio));
}

/* 
 * 继电器控制
 * 控制加湿器的开关状态
 * @param checked 开关状态，true表示开启，false表示关闭
 */
void MainWindow::RelayControls(bool checked)
{
    if (checked) {
        publicMessage("/k25r9vo1EmV/QtDesktop/user/update", "{\"relay\":\"open\"}");
    } else {
        publicMessage("/k25r9vo1EmV/QtDesktop/user/update", "{\"relay\":\"close\"}");
    }
}

/* 
 * 继电器状态图片改变
 * 根据继电器状态切换加湿器图片
 * @param checked 开关状态，true表示开启，false表示关闭
 */
void MainWindow::RelayStatusImageChange(bool checked)
{
    QPixmap pixmap = checked ? QPixmap(":/images/humidifier_on") : QPixmap(":/images/humidifier_off");
    relaystatusimage->setPixmap(pixmap.scaled(relaystatusimage->size(), Qt::KeepAspectRatio));
}

/* 
 * 窗帘控制
 * 根据按钮信号发送不同的窗帘控制指令
 */
void MainWindow::CurtainControl()
{
    /* 获取发射信号的对象 */
    QPushButton *button = qobject_cast<QPushButton*>(sender());

    if(button == curtainopen) {
        publicMessage("/k25r9vo1EmV/QtDesktop/user/update", "{\"curtain\":\"open\"}");
    } else if(button == curtainstop) {
        publicMessage("/k25r9vo1EmV/QtDesktop/user/update", "{\"curtain\":\"stop\"}");
    } else if(button == curtainclose) {
        publicMessage("/k25r9vo1EmV/QtDesktop/user/update", "{\"curtain\":\"close\"}");
    }
}

/* 连接MQTT */
void MainWindow::ConnectMQTT()
{
    client = new QMqttClient();

    QString hostname = "iot-06z00jdbr4qssk5.mqtt.iothub.aliyuncs.com";
    QString clientId = "k25r9vo1EmV.QtDesktop|securemode=2,signmethod=hmacsha256,timestamp=1737095727187|";
    QString username = "QtDesktop&k25r9vo1EmV";
    QString password = "0c116e011356fe008b6dbfb23481337238bd252d17eddf5a3f6c07d3d69ff61b";

    client->setHostname(hostname);
    client->setPort(1883);
    client->setClientId(clientId);
    client->setUsername(username);
    client->setPassword(password);

    /* 连接MQTT服务器 */
    client->connectToHost();
}

/* 
 * 客厅灯部分的界面布局
 * 创建客厅灯开关控件和状态显示图片，并设置垂直布局
 */
void MainWindow::LumpInterfaceLayout()
{
    /* 客厅灯开关实例化 */
    livingroomlumpswitch = new QRadioButton();
    livingroomlumpswitch->setFixedSize(100, 50);
    livingroomlumpswitch->setText("客厅灯");
    livingroomlumpswitch->setChecked(false);

    QPixmap lumpclosepixmap(":/images/lump_close");
    lumpstatusimage = new QLabel();
    lumpstatusimage->setFixedSize(100, 50);
    /* 缩放图片以适应QLabel大小，同时保持图片的宽高比 */
    lumpstatusimage->setPixmap(lumpclosepixmap.scaled(lumpstatusimage->size(), Qt::KeepAspectRatio));

    lumpvboxlayout = new QVBoxLayout();
    lumpvboxlayout->addWidget(livingroomlumpswitch);
    lumpvboxlayout->addWidget(lumpstatusimage);

    lumpwidget = new QWidget();
    lumpwidget->setFixedSize(120, 120);
    lumpwidget->setObjectName("lumpwidget");
    lumpwidget->setLayout(lumpvboxlayout);
}

/* 
 * 继电器(加湿器)部分的界面布局
 * 创建加湿器开关控件和状态显示图片，并设置垂直布局
 */
void MainWindow::RelayInterfaceLayout()
{
    /* 客厅灯开关实例化 */
    relayswitch = new QRadioButton();
    relayswitch->setFixedSize(100, 50);
    relayswitch->setText("加湿器");
    relayswitch->setChecked(false);

    QPixmap relayclosepixmap(":/images/humidifier_off");
    relaystatusimage = new QLabel();
    relaystatusimage->setFixedSize(100, 50);
    /* 缩放图片以适应QLabel大小，同时保持图片的宽高比 */
    relaystatusimage->setPixmap(relayclosepixmap.scaled(relaystatusimage->size(), Qt::KeepAspectRatio));

    relayvboxlayout = new QVBoxLayout();
    relayvboxlayout->addWidget(relayswitch);
    relayvboxlayout->addWidget(relaystatusimage);

    relaywidget = new QWidget();
    relaywidget->setFixedSize(120, 120);
    relaywidget->setObjectName("relaywidget");
    relaywidget->setLayout(relayvboxlayout);
}

/* 
 * 温度部分界面布局
 * 创建温度显示标签和温度计进度条，并设置混合布局
 */
void MainWindow::TmeperatureInterfaceLayout()
{
    tempname = new QLabel();
    tempname->setText("室内温度");
    tempname->setObjectName("tempname");
    tempname->setAlignment(Qt::AlignCenter);

    temperature = new QLabel();
    temperature->setText("-- \u2103");
    temperature->setObjectName("temperature");
    temperature->setAlignment(Qt::AlignCenter);

    thermometer = new QProgressBar();
    thermometer->setRange(0, 100);
    thermometer->setOrientation(Qt::Vertical);
    thermometer->setValue(0);
    thermometer->setFormat("%p\u2103");
    thermometer->setFixedWidth(30);

    tempvboxlayout = new QVBoxLayout();
    tempvboxlayout->addWidget(tempname);
    tempvboxlayout->addWidget(temperature);

    tempwidget[0] = new QWidget();
    tempwidget[0]->setLayout(tempvboxlayout);

    temphboxlayout = new QHBoxLayout();
    temphboxlayout->addWidget(thermometer);
    temphboxlayout->addWidget(tempwidget[0]);

    tempwidget[1] = new QWidget();
    tempwidget[1]->setFixedSize(170, 120);
    tempwidget[1]->setObjectName("tempwidget");
    tempwidget[1]->setLayout(temphboxlayout);
}

/* 
 * 湿度部分布局
 * 创建湿度显示标签和名称标签，并设置垂直布局
 */
void MainWindow::HumidityInterfaceLayout()
{
    humidityname = new QLabel();
    humidityname->setText("相对湿度");
    humidityname->setObjectName("humidityname");
    humidityname->setAlignment(Qt::AlignCenter);

    humidity = new QLabel();
    humidity->setText("-- %RH");
    humidity->setObjectName("humidity");
    humidity->setAlignment(Qt::AlignCenter);

    humidityvboxlayout = new QVBoxLayout();
    humidityvboxlayout->addWidget(humidityname);
    humidityvboxlayout->addWidget(humidity);

    humiditywidget = new QWidget();
    humiditywidget->setFixedSize(120, 120);
    humiditywidget->setObjectName("humiditywidget");
    humiditywidget->setLayout(humidityvboxlayout);
}

/* ppm部分界面布局 */
void MainWindow::PpmInterfaceLayout()
{
    ppmname = new QLabel();
    ppmname->setText("空气质量");
    ppmname->setObjectName("ppmname");
    ppmname->setAlignment(Qt::AlignCenter);

    ppm = new QLabel();
    ppm->setText("-- ppm");
    ppm->setObjectName("ppm");
    ppm->setAlignment(Qt::AlignCenter);

    ppmvboxlayout = new QVBoxLayout();
    ppmvboxlayout->addWidget(ppmname);
    ppmvboxlayout->addWidget(ppm);

    ppmwidget = new QWidget();
    ppmwidget->setFixedSize(120, 120);
    ppmwidget->setObjectName("ppmwidget");
    ppmwidget->setLayout(ppmvboxlayout);
}

/* 窗帘部分的界面布局 */
void MainWindow::CurtainInterfaceLayout()
{
    curtainname = new QLabel();
    curtainname->setText("窗帘");
    curtainname->setObjectName("curtainname");
    curtainname->setAlignment(Qt::AlignCenter);

    curtainopen = new QPushButton("开启");
    curtainstop = new QPushButton("停止");
    curtainclose = new QPushButton("关闭");

    controlhboxlayout = new QHBoxLayout();
    controlhboxlayout->addWidget(curtainopen);
    controlhboxlayout->addWidget(curtainstop);
    controlhboxlayout->addWidget(curtainclose);

    controlwidget = new QWidget();
    controlwidget->setLayout(controlhboxlayout);

    curtainvboxlayout = new QVBoxLayout();
    curtainvboxlayout->addWidget(curtainname);
    curtainvboxlayout->addWidget(controlwidget);

    curtainwidget = new QWidget();
    curtainwidget->setFixedSize(250, 100);
    curtainwidget->setObjectName("curtainwidget");
    curtainwidget->setLayout(curtainvboxlayout);
}

/* 界面布局 */
void MainWindow::InterfaceLayout()
{
    /* 主界面背景 */
    this->setGeometry(0, 0, 1024, 600);
    this->setMinimumWidth(1024);
    this->setMinimumHeight(600);
    this->setObjectName("smarthome");

    homename = new QLabel();
    homename->setObjectName("homename");
    homename->setText("初始化中……");
    homename->setFixedSize(170, 65);
    homename->setAlignment(Qt::AlignCenter);

    /* 各个窗口部件界面函数 */
    LumpInterfaceLayout();
    RelayInterfaceLayout();
    TmeperatureInterfaceLayout();
    HumidityInterfaceLayout();
    PpmInterfaceLayout();
    CurtainInterfaceLayout();

    hboxlayout[0] = new QHBoxLayout();
    hboxlayout[0]->addWidget(homename, 0, Qt::AlignLeft);

    hboxlayout[1] = new QHBoxLayout();
    hboxlayout[1]->addWidget(lumpwidget, 0, Qt::AlignLeft);
    hboxlayout[1]->addWidget(relaywidget, 0, Qt::AlignLeft);
    hboxlayout[1]->addWidget(curtainwidget, 0, Qt::AlignLeft);

    hboxlayout[2] = new QHBoxLayout();
    hboxlayout[2]->addWidget(tempwidget[1], 0, Qt::AlignLeft);
    hboxlayout[2]->addWidget(humiditywidget, 0, Qt::AlignLeft);
    hboxlayout[2]->addWidget(ppmwidget, 0, Qt::AlignLeft);


    widget[0] = new QWidget();
    widget[0]->setLayout(hboxlayout[0]);

    widget[1] = new QWidget();
    widget[1]->setLayout(hboxlayout[1]);

    widget[2] = new QWidget();
    widget[2]->setLayout(hboxlayout[2]);

    widget[3] = new QWidget();

    mainvboxlayout = new QVBoxLayout();

    mainwidget = new QWidget(this);
    mainwidget->setObjectName(tr("mainwidget"));

    /*
     * 这里的0是stretch，表示控件在布局中分配的伸缩因子
     */
    mainvboxlayout->addWidget(widget[0], 0, Qt::AlignTop);
    mainvboxlayout->addWidget(widget[1], 0, Qt::AlignTop);
    mainvboxlayout->addWidget(widget[2], 0, Qt::AlignTop);
    mainvboxlayout->addWidget(widget[3], 0, Qt::AlignTop);
    mainwidget->setLayout(mainvboxlayout);
    setCentralWidget(mainwidget);
}

