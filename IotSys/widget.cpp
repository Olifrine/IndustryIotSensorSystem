#include "widget.h"
#include "ui_widget.h"
#include <uuid/uuid.h>
#include "aiot_calc.h"
#include "global.h"
#include <QWidget>
#include <QtMqtt/qmqttclient.h>
#include <QByteArray>
#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDateTime>
#include <QSslConfiguration>
#include <QCryptographicHash>
#include <QTimer>
#include <QEventLoop>
#include <QMessageBox>
#include <QFile>
#include <QFileInfo>
#include <QIODevice>
#include <QDir>
#include <QDataStream>
#include <QSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QTextCodec>
#include <QNetworkInterface>
#include <QDebug>
#include <thread>

extern int mqtt_connect();

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    settingCheck();
    AccessKeyId = ui->lineInfAcsKeyId->text();
    IotInstanceId = ui->lineInfIotInstId->text();
    ProductKey = ui->lineInfPrdKey->text();
    m_client = new QMqttClient(this);
    connect(m_client, &QMqttClient::stateChanged, this, &Widget::change_state_label);
    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished,this,&Widget::postBack);
    devStatus = 0x00;
    preStatus = 0x00;
    chgStatus = 0x00;
    localStatus = 0x00;
    devList.insert(ui->lineDevNameLocal->text().toUtf8(),0);
    devList.insert(ui->lineDevName1->text().toUtf8(),1);
    devList.insert(ui->lineDevName2->text().toUtf8(),2);
    devList.insert(ui->lineDevName3->text().toUtf8(),3);
    limitInitial();

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QDir::currentPath()+"/src/data.db");
    if (!db.open())
    {
        qDebug() << "open db failed!";
    }
    const QString sqlquery = "create table if not exists iot("
                             "id integer primary key autoincrement,"
                             "node integer not null,"
                             "temperature real not null,"
                             "humidity real not null,"
                             "lightintensity integer not null,"
                             "time text not null);";
    QSqlQuery query(db);
    query.exec(sqlquery);
    std::thread threadCheck([&](){
        QEventLoop *loopCheck = new QEventLoop();
        QTimer *timerCheckStatus = new QTimer();
        timerCheckStatus->setInterval(2000);    //节点段每2秒发送一次消息
        connect(timerCheckStatus,&QTimer::timeout,this,&Widget::post);
        timerCheckStatus->start();
        loopCheck->exec();
    });
    threadCheck.detach();
    std::thread threadtime([&](){
        QEventLoop *loop = new QEventLoop();
        QTimer *timer = new QTimer();
        timer->setInterval(1000);
        connect(timer,&QTimer::timeout,this,[&](){
            ui->labelTime->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd\tHH:mm:ss"));
        });
        timer->start();
        loop->exec();
    });
    threadtime.detach();

    on_btnTestLocal_clicked();
    connect(m_client, &QMqttClient::messageReceived, this, [this](const QByteArray &message, const QMqttTopicName &topic) {
        const QString content = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")
                                + QLatin1String(" Received Topic: ")
                                + topic.name()
                                + QLatin1String(" Message: ")
                                + message
                                + QLatin1Char('\n');
        ui->logMessage->insertPlainText(content);

        logICount += 1;
        if(logICount >= 100)
        {
            logMessageSave();
            logICount = 0;
        }
        curStatus(message);
    });
}
QString Widget::AccessKeyId = "";
QString Widget::IotInstanceId = "";
QString Widget::ProductKey = "";
int Widget::logOCount = 0;
int Widget::logICount = 0;
char Widget::clientId[150] = {0};
char Widget::username[65] = {0};
char Widget::password[65] = {0};

Widget::~Widget()
{
    QSqlQuery query(db);
    QString sql = "select 1";
    if (query.exec(sql)) {
        qDebug()<<"close";
        db.close();
    }
    else
        qDebug()<<"not open";
    logOnoffSave();
    logMessageSave();
    delete ui;
}

//测试界面使用
void Widget::change_state_label()
{
    int state = m_client->state();
    if(state == 0)
        if(Global::mqttClientCount == 0)
            ui->labelTest->setText("未连接");
        else
            ui->labelTest->setText(("已断开"));
    else if (state == 1)
    {
        const QString content = "连接服务器" + QString::number(Global::mqttClientCount)+"中";
        ui->labelTest->setText(content);
    }
    else if (state == 2)
    {
        const QString content = "已连接服务器" + QString::number(Global::mqttClientCount);
        ui->labelTest->setText(content);
        if(Global::mqttClientCount == 4)
            mqttSubscribe();
    }
    else
        ui->labelTest->setText("state="+QString::number(state)+",未知错误");
}


void Widget::on_btnTestNode1_clicked()
{
    clientId[150] = {0};
    username[65] = {0};
    password[65] = {0};
    Global::mqttClientCount = 1;
    if(m_client->state() != 0)
    {
        m_client->disconnectFromHost();
    }

    QByteArray prd_key;
    prd_key.append(ui->lineProductKey1->text().toUtf8());
    QByteArray dev_name;
    dev_name.append(ui->lineDevName1->text().toUtf8());
    QByteArray dev_srt;
    dev_srt.append(ui->lineDevSec1->text().toUtf8());
    aiotMqttSign(prd_key.data(),dev_name.data(),dev_srt.data(), clientId, username, password);

    m_client->setClientId(clientId);
    m_client->setUsername(username);
    m_client->setPassword(password);
    ui->lineClientIDCur->setText(QString::fromLocal8Bit(clientId));
    ui->lineUsernameCur->setText(QString::fromLocal8Bit(username));
    ui->linePasswdCur->setText(QString::fromLocal8Bit(password));
    m_client->setHostname(ui->lineHost1->text());
    m_client->setPort(1883);
    m_client->setKeepAlive(30);
    m_client->connectToHost();
}


void Widget::on_btnTestNode2_clicked()
{
    clientId[150] = {0};
    username[65] = {0};
    password[65] = {0};
    Global::mqttClientCount = 2;
    if(m_client->state() != 0)
    {
        m_client->disconnectFromHost();
    }

    QByteArray prd_key;
    prd_key.append(ui->lineProductKey2->text().toUtf8());
    QByteArray dev_name;
    dev_name.append(ui->lineDevName2->text().toUtf8());
    QByteArray dev_srt;
    dev_srt.append(ui->lineDevSec2->text().toUtf8());
    aiotMqttSign(prd_key.data(),dev_name.data(),dev_srt.data(), clientId, username, password);

    m_client->setClientId(clientId);
    m_client->setUsername(username);
    m_client->setPassword(password);
    ui->lineClientIDCur->setText(QString::fromLocal8Bit(clientId));
    ui->lineUsernameCur->setText(QString::fromLocal8Bit(username));
    ui->linePasswdCur->setText(QString::fromLocal8Bit(password));
    m_client->setHostname(ui->lineHost2->text());
    m_client->setPort(1883);
    m_client->setKeepAlive(30);
    m_client->connectToHost();
}


void Widget::on_btnTestNode3_clicked()
{
    clientId[150] = {0};
    username[65] = {0};
    password[65] = {0};
    Global::mqttClientCount = 3;
    if(m_client->state() != 0)
    {
        m_client->disconnectFromHost();
    }

    QByteArray prd_key;
    prd_key.append(ui->lineProductKey3->text().toUtf8());
    QByteArray dev_name;
    dev_name.append(ui->lineDevName3->text().toUtf8());
    QByteArray dev_srt;
    dev_srt.append(ui->lineDevSec3->text().toUtf8());
    aiotMqttSign(prd_key.data(),dev_name.data(),dev_srt.data(), clientId, username, password);

    m_client->setClientId(clientId);
    m_client->setUsername(username);
    m_client->setPassword(password);
    ui->lineClientIDCur->setText(QString::fromLocal8Bit(clientId));
    ui->lineUsernameCur->setText(QString::fromLocal8Bit(username));
    ui->linePasswdCur->setText(QString::fromLocal8Bit(password));
    m_client->setHostname(ui->lineHost3->text());
    m_client->setPort(1883);
    m_client->setKeepAlive(30);
    m_client->connectToHost();
}


void Widget::on_btnTestLocal_clicked()
{
    clientId[150] = {0};
    username[65] = {0};
    password[65] = {0};
    Global::mqttClientCount = 4;
    if(m_client->state() != 0)
    {
        m_client->disconnectFromHost();
    }
    QByteArray prd_key;
    prd_key.append(ui->lineProductKeyLocal->text().toUtf8());
    QByteArray dev_name;
    dev_name.append(ui->lineDevNameLocal->text().toUtf8());
    QByteArray dev_srt;
    dev_srt.append(ui->lineDevSecLocal->text().toUtf8());
    aiotMqttSign(prd_key.data(),dev_name.data(),dev_srt.data(), clientId, username, password);

    m_client->setClientId(clientId);
    m_client->setUsername(username);
    m_client->setPassword(password);
    ui->lineClientIDCur->setText(QString::fromLocal8Bit(clientId));
    ui->lineUsernameCur->setText(QString::fromLocal8Bit(username));
    ui->linePasswdCur->setText(QString::fromLocal8Bit(password));
    m_client->setHostname(ui->lineHostLocal->text());
    m_client->setPort(1883);
    m_client->setKeepAlive(30);
    m_client->connectToHost();
}


void Widget::on_btnDisconnected_clicked()
{
    Global::mqttClientCount = 0;
    if(m_client->state() != 0)
    {
        m_client->disconnectFromHost();
    }
}

//生成询问在线信息的url并post
void Widget::post()
{

    QString date = QDateTime::currentDateTimeUtc().toString("yyyy-MM-ddTHH:mm:ssZ");
    uuid_t uuid;
    char uuidString[36];
    uuid_generate(uuid);
    uuid_unparse(uuid, uuidString);

    QByteArray requestArray = "http://iot.cn-shanghai.aliyuncs.com/?";
    QByteArray requestHeader;
    requestHeader.append("AccessKeyId=");
    requestHeader.append(AccessKeyId.toUtf8());
    requestHeader.append("&Action=QueryDeviceByStatus&Format=JSON&IotInstanceId=");
    requestHeader.append(IotInstanceId.toUtf8());
    requestHeader.append("&ProductKey=");
    requestHeader.append(ProductKey.toUtf8());
    requestHeader.append("&RegionId=cn-shanghai&SignatureMethod=HMAC-SHA1&SignatureNonce=");
    requestHeader.append(uuidString);
    requestHeader.append("&SignatureVersion=1.0&Status=1&Timestamp=");
    requestHeader.append(date.toUtf8());
    requestHeader.append("&Version=2018-01-20");
    QByteArray requestToSign = requestHeader;
    requestToSign = requestToSign.replace("=","%3D");
    requestToSign = requestToSign.replace("&","%26");
    requestToSign = requestToSign.replace(":","%253A");
    requestToSign = "GET&%2F&" + requestToSign;
    QByteArray AccessSecret = ui->lineInfAcsScr->text().toUtf8();
    QByteArray signature = HMACSha1(AccessSecret,requestToSign);
    signature = signature.replace("=","%3D");
    signature = signature.replace("+","%2B");
    signature = signature.replace("/","%2F");
    requestHeader.append("&Signature=");
    requestHeader.append(signature);

    requestArray.append(requestHeader);

    QNetworkRequest request;

    request.setUrl(QUrl(requestArray));

    // 发送请求
    manager->get(request);

}

//解析阿里云回传的在线信息
void Widget::postBack(QNetworkReply* reply)
{
    QByteArray bytes = reply->readAll();
    // 获取响应信息
    QJsonParseError jsonError;
    QJsonDocument doucment = QJsonDocument::fromJson(bytes, &jsonError);
    if (jsonError.error != QJsonParseError::NoError) {
        qDebug() << QStringLiteral("解析Json失败");
        return;
    }

    // 解析Json
    if (doucment.isObject())
    {
        QJsonObject reqInf = doucment.object();
        if (reqInf.contains("Data"))
        {
            QJsonObject reqData = reqInf["Data"].toObject();
            QJsonArray simpleDeviceInfo = reqData["SimpleDeviceInfo"].toArray();
            QStringList devOnlineList;
            std::for_each(simpleDeviceInfo.begin(),simpleDeviceInfo.end(),[&devOnlineList](QJsonValue const &nodeItem){
                QJsonObject node = nodeItem.toObject();
                devOnlineList.append(node["DeviceName"].toString());
            });

            devStatus = 0x00;
            foreach (const QString &devOlName, devOnlineList) {
                if((indicator = devList.find(devOlName)) != devList.end())
                    devStatus = devStatus|(0x01<<indicator.value());
            }
            setStatus();
        }
    }
    return;
}

//HMACSha1加密算法
QByteArray Widget::HMACSha1(QByteArray key, QByteArray baseString)
{
    int blockSize = 64; // HMAC-SHA-1 block size, defined in SHA-1 standard
    if (key.length() > blockSize) { // if key is longer than block size (64), reduce key length with SHA-1 compression
        key = QCryptographicHash::hash(key, QCryptographicHash::Sha1);
    }

    QByteArray innerPadding(blockSize, char(0x36)); // initialize inner padding with char "6"
    QByteArray outerPadding(blockSize, char(0x5c)); // initialize outer padding with char "quot;

    for (int i = 0; i < key.length(); i++) {
        innerPadding[i] = innerPadding[i] ^ key.at(i); // XOR operation between every byte in key and innerpadding, of key length
        outerPadding[i] = outerPadding[i] ^ key.at(i); // XOR operation between every byte in key and outerpadding, of key length
    }

    QByteArray total = outerPadding;
    QByteArray part = innerPadding;
    part.append(baseString);
    total.append(QCryptographicHash::hash(part, QCryptographicHash::Sha1));
    QByteArray hashed = QCryptographicHash::hash(total, QCryptographicHash::Sha1);
    return hashed.toBase64();
}

void Widget::on_lineProductKeyLocal_textChanged(const QString &arg1)
{
    ui->lineProductKey1->setText(ui->lineProductKeyLocal->text());
    ui->lineProductKey2->setText(ui->lineProductKeyLocal->text());
    ui->lineProductKey3->setText(ui->lineProductKeyLocal->text());
}

void Widget::setStatus()
{
/*    for(int i = 0;i<4;i++)
    {
        qDebug()<<devList->at(i)<<devStatus[i];
    }*/
    if(ui->btnModeChange->isChecked())  //remote模式
    {
        ui->labelStatus1->statusChange(devStatus&0x02,0x02);
        ui->labelStatus2->statusChange(devStatus&0x04,0x04);
        ui->labelStatus3->statusChange(devStatus&0x08,0x08);
        ui->labelStatusLocal->statusChange(devStatus&0x01,0x01);
    }
    else    //local模式
    {
        ui->labelStatus1->statusChange(devStatus&0x02,0x02,localStatus&0x02);
        ui->labelStatus2->statusChange(devStatus&0x04,0x04,localStatus&0x04);
        ui->labelStatus3->statusChange(devStatus&0x08,0x08,localStatus&0x08);
        ui->labelStatusLocal->statusChange(devStatus&0x01,0x01);
        localStatus = 0x00;
    }
    if(chgStatus = preStatus^devStatus)
    {
        if(chgStatus&0x01)
            if(devStatus&0x01)
            {
                const QString content = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss:")
                                        + ui->labelStatusLocal->text()
                                        + "->上线";
                ui->labelCurOnOff->setText(content);
                ui->logOnOff->insertPlainText(content+QLatin1Char('\n'));
            }
            else
            {
                const QString content = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss:")
                                        + ui->labelStatusLocal->text()
                                        + "->下线";
                ui->labelCurOnOff->setText(content);
                ui->logOnOff->insertPlainText(content+QLatin1Char('\n'));
            }

        if(chgStatus&0x02)
            if(devStatus&0x02)
            {
                const QString content = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss:")
                                        + ui->labelStatus1->text()
                                        + "->上线";
                ui->labelCurOnOff->setText(content);
                ui->logOnOff->insertPlainText(content+QLatin1Char('\n'));
            }
            else
            {
                const QString content = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss:")
                                        + ui->labelStatus1->text()
                                        + "->下线";
                ui->labelCurOnOff->setText(content);
                ui->logOnOff->insertPlainText(content+QLatin1Char('\n'));
            }

        if(chgStatus&0x04)
            if(devStatus&0x04)
            {
                const QString content = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss:")
                                        + ui->labelStatus2->text()
                                        + "->上线";
                ui->labelCurOnOff->setText(content);
                ui->logOnOff->insertPlainText(content+QLatin1Char('\n'));
            }
            else
            {
                const QString content = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss:")
                                        + ui->labelStatus2->text()
                                        + "->下线";
                ui->labelCurOnOff->setText(content);
                ui->logOnOff->insertPlainText(content+QLatin1Char('\n'));
            }
        if(chgStatus&0x08)
            if(devStatus&0x08)
            {
                const QString content = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss:")
                                        + ui->labelStatus3->text()
                                        + "->上线";
                ui->labelCurOnOff->setText(content);
                ui->logOnOff->insertPlainText(content+QLatin1Char('\n'));
            }
            else
            {
                const QString content = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss:")
                                        + ui->labelStatus3->text()
                                        + "->下线";
                ui->labelCurOnOff->setText(content);
                ui->logOnOff->insertPlainText(content+QLatin1Char('\n'));
            }
        logOCount += 1;
        if(logOCount >= 100)
        {
            logOnoffSave();
            logOCount = 0;
        }
    }
    preStatus = devStatus;
}

//mqtt订阅
void Widget::mqttSubscribe()
{
    auto subscription = m_client->subscribe(ui->lineInfTopic->text());
    if (!subscription) {
        QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Could not subscribe. Is there a valid connection?"));
        return;
    }
}

//清空上下线日志区
void Widget::on_btnOnOffClear_clicked()
{
    ui->logOnOff->clear();
}

//清空消息日志区
void Widget::on_btnMessageClear_clicked()
{
    ui->logMessage->clear();
}

//当前状态显示和数据库插入字段
void Widget::curStatus(QString message)
{
    float t,h;
    int l,b;
    QStringList state= message.split("|");
    if(state.at(0) == "1")
    {
        ui->labelN1T->setText(state.at(1)+"°C");
        ui->labelN1H->setText(state.at(2)+"%");
        ui->labelN1L->setText(state.at(3)+"lx");
        ui->labelN1B->setText(state.at(4)+"%");
        t = state.at(1).toFloat();
        h = state.at(2).toFloat();
        l = state.at(3).toInt();
        b = state.at(4).toInt();
        if(t >= TMin && t <= TMax && h >= HMin && h <= HMax && l >= LMin && l <= LMax && b >= BMin)
            ui->labelStatus1->statusAlert(0x01,0x02);
        else
            ui->labelStatus1->statusAlert(0x00,0x02);
        localStatus |= 0x02;
    }
    else if(state.at(0) == "2")
    {
        ui->labelN2T->setText(state.at(1)+"°C");
        ui->labelN2H->setText(state.at(2)+"%");
        ui->labelN2L->setText(state.at(3)+"lx");
        ui->labelN2B->setText(state.at(4)+"%");
        t = state.at(1).toFloat();
        h = state.at(2).toFloat();
        l = state.at(3).toInt();
        b = state.at(4).toInt();
        if(t >= TMin && t <= TMax && h >= HMin && h <= HMax && l >= LMin && l <= LMax && b >= BMin)
            ui->labelStatus2->statusAlert(0x01,0x04);
        else
            ui->labelStatus2->statusAlert(0x00,0x04);
        localStatus |= 0x04;
    }
    else if(state.at(0) == "3")
    {
        ui->labelN3T->setText(state.at(1)+"°C");
        ui->labelN3H->setText(state.at(2)+"%");
        ui->labelN3L->setText(state.at(3)+"lx");
        ui->labelN3B->setText(state.at(4)+"%");
        t = state.at(1).toFloat();
        h = state.at(2).toFloat();
        l = state.at(3).toInt();
        b = state.at(4).toInt();
        if(t >= TMin && t <= TMax && h >= HMin && h <= HMax && l >= LMin && l <= LMax && b >= BMin)
            ui->labelStatus3->statusAlert(0x01,0x08);
        else
            ui->labelStatus3->statusAlert(0x00,0x08);
        localStatus |= 0x08;
    }
    else
    {
        return;
    }
    QSqlQuery query(db);
    state.removeAt(4);
    const QString sqlinsert = "insert into iot(node,temperature,humidity,lightintensity,time) values("
                              + state.join(",")
                              + ",\""
                              + QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")
                              + "\");";
    query.exec(sqlinsert);//记得判断失败处理
}

//阈值设定
void Widget::limitInitial()
{
    if(ui->cBTMin->isChecked())
    {
        TMin = 0.0;
        ui->lineThresholdTempMin->setReadOnly(true);
    }
    else
    {
        TMin = ui->lineThresholdTempMin->text().toFloat();
        ui->lineThresholdTempMin->setReadOnly(false);
    }
    if(ui->cBTMax->isChecked())
    {
        TMax = 150.0;
        ui->lineThresholdTempMax->setReadOnly(true);
    }
    else
    {
        TMax = ui->lineThresholdTempMax->text().toFloat();
        ui->lineThresholdTempMax->setReadOnly(false);
    }

    if(ui->cBHMin->isChecked())
    {
        HMin = 0.0;
        ui->lineThresholdHumidityMin->setReadOnly(true);
    }
    else
    {
        HMin = ui->lineThresholdHumidityMin->text().toFloat();
        ui->lineThresholdHumidityMin->setReadOnly(false);
    }
    if(ui->cBHMax->isChecked())
    {
        HMax = 100.0;
        ui->lineThresholdHumidityMax->setReadOnly(true);
    }
    else
    {
        HMax = ui->lineThresholdHumidityMax->text().toFloat();
        ui->lineThresholdHumidityMax->setReadOnly(false);
    }

    if(ui->cBLMin->isChecked())
    {
        LMin = 0;
        ui->lineThresholdLightMin->setReadOnly(true);
    }
    else
    {
        LMin = ui->lineThresholdLightMin->text().toInt();
        ui->lineThresholdLightMin->setReadOnly(false);
    }
    if(ui->cBLMax->isChecked())
    {
        LMax = 2000;
        ui->lineThresholdLightMax->setReadOnly(true);
    }
    else
    {
        LMax = ui->lineThresholdLightMax->text().toInt();
        ui->lineThresholdLightMax->setReadOnly(false);
    }

    if(ui->CBBMin->isChecked())
    {
        BMin = 0;
        ui->lineThresholdBatteryMin->setReadOnly(true);
    }
    else
    {
        BMin = ui->lineThresholdBatteryMin->text().toInt();
        ui->lineThresholdBatteryMin->setReadOnly(false);
    }
    settingSave(false);
}

//刷新阈值设定
void Widget::on_btnLimitFlash_clicked()
{
    limitInitial();
}

//检查开机设置
void Widget::settingCheck()
{
    QString fullpath = QDir::currentPath()+"/src";
    QDir dir(fullpath);
    if(!dir.exists())
    {
        dir.mkdir(fullpath);
    }

    fullpath += "/setting";
    QFileInfo fileInfo(fullpath);
    if(fileInfo.isFile())
    {
        settingLoad();
    }
    else
    {
        settingSave(true);
    }

    fullpath = QDir::currentPath()+"/src/messagelog";
    QDir dir2(fullpath);
    if(!dir2.exists())
    {
        dir2.mkdir(fullpath);
    }

    fullpath = QDir::currentPath()+"/src/statuslog";
    QDir dir3(fullpath);
    if(!dir3.exists())
    {
        dir3.mkdir(fullpath);
    }

    fullpath = QDir::currentPath()+"/output";
    QDir dir4(fullpath);
    if(!dir4.exists())
    {
        dir4.mkdir(fullpath);
    }

}

//保存当前设置
void Widget::settingSave(bool def)
{
    setting.clear();
    setting.insert("n1pk",ui->lineProductKey1->text());
    setting.insert("n1dn",ui->lineDevName1->text());
    setting.insert("n1ds",ui->lineDevSec1->text());
    setting.insert("n1host",ui->lineHost1->text());

    setting.insert("n2pk",ui->lineProductKey2->text());
    setting.insert("n2dn",ui->lineDevName2->text());
    setting.insert("n2ds",ui->lineDevSec2->text());
    setting.insert("n2host",ui->lineHost2->text());

    setting.insert("n3pk",ui->lineProductKey3->text());
    setting.insert("n3dn",ui->lineDevName3->text());
    setting.insert("n3ds",ui->lineDevSec3->text());
    setting.insert("n3host",ui->lineHost3->text());

    setting.insert("n4pk",ui->lineProductKeyLocal->text());
    setting.insert("n4dn",ui->lineDevNameLocal->text());
    setting.insert("n4ds",ui->lineDevSecLocal->text());
    setting.insert("n4host",ui->lineHostLocal->text());

    setting.insert("acski",ui->lineInfAcsKeyId->text());
    setting.insert("acsscr",ui->lineInfAcsScr->text());
    setting.insert("iotinsid",ui->lineInfIotInstId->text());
    setting.insert("prdkey",ui->lineInfPrdKey->text());
    setting.insert("topic",ui->lineInfTopic->text());

    setting.insert("tmin",ui->lineThresholdTempMin->text());
    setting.insert("tmax",ui->lineThresholdTempMax->text());
    setting.insert("hmin",ui->lineThresholdHumidityMin->text());
    setting.insert("hmax",ui->lineThresholdHumidityMax->text());
    setting.insert("lmin",ui->lineThresholdLightMin->text());
    setting.insert("lmax",ui->lineThresholdLightMax->text());
    setting.insert("bmin",ui->lineThresholdBatteryMin->text());

    setting.insert("etmin",QString::number(ui->cBTMin->isChecked()));
    setting.insert("etmax",QString::number(ui->cBTMax->isChecked()));
    setting.insert("ehmin",QString::number(ui->cBHMin->isChecked()));
    setting.insert("ehmax",QString::number(ui->cBHMax->isChecked()));
    setting.insert("elmin",QString::number(ui->cBLMin->isChecked()));
    setting.insert("elmax",QString::number(ui->cBLMax->isChecked()));
    setting.insert("ebmin",QString::number(ui->CBBMin->isChecked()));

    QString filepath = QDir::currentPath() + "/src/setting";
    QFile file(filepath);
    file.open(QIODevice::WriteOnly | QIODevice::Truncate) ;
    QDataStream out (&file);
    out << setting;
    file.close();

    if(def)
    {
        QString filepath = QDir::currentPath() + "/src/settingdef";
        QFile file(filepath);
        file.open(QIODevice::WriteOnly | QIODevice::Truncate) ;
        QDataStream out (&file);
        out << setting;
        file.close();
    }
}

//加载当前设置
void Widget::settingLoad()
{
    QString filepath = QDir::currentPath() + "/src/setting";
    QFile file(filepath);
    file.open(QIODevice::ReadOnly);
    QDataStream in(&file);
    setting.clear();
    in >> setting;
    file.close();
    ui->lineProductKey1->setText(setting.value("n1pk"));
    ui->lineDevName1->setText(setting.value("n1dn"));
    ui->lineDevSec1->setText(setting.value("n1ds"));
    ui->lineHost1->setText(setting.value("n1host"));

    ui->lineProductKey2->setText(setting.value("n2pk"));
    ui->lineDevName2->setText(setting.value("n2dn"));
    ui->lineDevSec2->setText(setting.value("n2ds"));
    ui->lineHost2->setText(setting.value("n2host"));

    ui->lineProductKey3->setText(setting.value("n3pk"));
    ui->lineDevName3->setText(setting.value("n3dn"));
    ui->lineDevSec3->setText(setting.value("n3ds"));
    ui->lineHost3->setText(setting.value("n3host"));

    ui->lineProductKeyLocal->setText(setting.value("n4pk"));
    ui->lineDevNameLocal->setText(setting.value("n4dn"));
    ui->lineDevSecLocal->setText(setting.value("n4ds"));
    ui->lineHostLocal->setText(setting.value("n4host"));

    ui->lineInfAcsKeyId->setText(setting.value("acski"));
    ui->lineInfAcsScr->setText(setting.value("acsscr"));
    ui->lineInfIotInstId->setText(setting.value("iotinsid"));
    ui->lineInfPrdKey->setText(setting.value("prdkey"));
    ui->lineInfTopic->setText(setting.value("topic"));

    ui->lineThresholdTempMin->setText(setting.value("tmin"));
    ui->lineThresholdTempMax->setText(setting.value("tmax"));
    ui->lineThresholdHumidityMin->setText(setting.value("hmin"));
    ui->lineThresholdHumidityMax->setText(setting.value("hmax"));
    ui->lineThresholdLightMin->setText(setting.value("lmin"));
    ui->lineThresholdLightMax->setText(setting.value("lmax"));
    ui->lineThresholdBatteryMin->setText(setting.value("bmin"));

    QVariant temp;
    temp = setting.value("etmin");
    ui->cBTMin->setChecked(temp.toBool());
    temp = setting.value("etmax");
    ui->cBTMax->setChecked(temp.toBool());
    temp = setting.value("ehmin");
    ui->cBHMin->setChecked(temp.toBool());
    temp = setting.value("ehmax");
    ui->cBHMax->setChecked(temp.toBool());
    temp = setting.value("elmin");
    ui->cBLMin->setChecked(temp.toBool());
    temp = setting.value("elmax");
    ui->cBLMax->setChecked(temp.toBool());
    temp = setting.value("ebmin");
    ui->CBBMin->setChecked(temp.toBool());
}

void Widget::on_btnSave_clicked()
{
    settingSave(false);
}

//保存上下线日志
void Widget::logOnoffSave()
{
    QString filepath = (QDir::currentPath() + "/src/statuslog/" + QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss") + "-status.log").toUtf8();
    QFile file(filepath);
    file.open(QIODevice::WriteOnly | QIODevice::Truncate) ;
    QDataStream out (&file);
    out << ui->logOnOff->toPlainText().toUtf8().data();
    ui->logOnOff->clear();
    file.close();
}

//保存消息日志
void Widget::logMessageSave()
{
    QString filepath = QDir::currentPath() + "/src/messagelog/" + QDateTime::currentDateTime().toString("yyyy-MM-dd-HH-mm-ss") + "-message.log";
    QFile file(filepath);
    file.open(QIODevice::WriteOnly | QIODevice::Truncate) ;
    QDataStream out (&file);
    out << ui->logMessage->toPlainText().toUtf8().data();
    ui->logMessage->clear();
    file.close();
}

void Widget::on_logOnOff_textChanged()
{
    ui->logOnOff->moveCursor(QTextCursor::End);
}


void Widget::on_logMessage_textChanged()
{
    ui->logMessage->moveCursor(QTextCursor::End);
}


void Widget::on_btnReQuery_clicked()
{
    ui->tabWidget->setCurrentIndex(1);
}


void Widget::on_btnSqlQuery_clicked()
{
    QSqlQueryModel *sqlmodel;
    sqlmodel = new QSqlQueryModel(this);

    QString order = " order by ";
    if(ui->rBTime->isChecked())
        order += "id";
    else if(ui->rBTemp->isChecked())
        order += "temperature";
    else if(ui->rBHumid->isChecked())
        order += "humidity";
    else if(ui->rBLight->isChecked())
        order += "lightintensity";
    if(ui->cBOrderReverse->isChecked())
        order += " DESC";
    else
        order += " ASC";

    if(!ui->cBWhereAll->isChecked())
    {
        QString where = " where";

        QStringList whereTemp;
        if(ui->cBWhereN1->isChecked())
            whereTemp.append("node = 1");
        if(ui->cBWhereN2->isChecked())
            whereTemp.append("node = 2");
        if(ui->cBWhereN3->isChecked())
            whereTemp.append("node = 3");
        if(!whereTemp.isEmpty())
        {
            where = where + " (" + whereTemp.join(" or ") + ")";
            whereTemp.clear();
        }

        if(!ui->cBTMinSql->isChecked())
            whereTemp.append("temperature >= " + QString::number(ui->sBTMin->value()));
        if(!ui->cBTMaxSql->isChecked())
            whereTemp.append("temperature <= " + QString::number(ui->sBTMax->value()));
        if(!ui->cBHMinSql->isChecked())
            whereTemp.append("humidity >= " + QString::number(ui->sBHMin->value()));
        if(!ui->cBHMaxSql->isChecked())
            whereTemp.append("humidity <= " + QString::number(ui->sBHMax->value()));
        if(!ui->cBLMinSql->isChecked())
            whereTemp.append("lightintensity >= " + QString::number(ui->sBLMin->value()));
        if(!ui->cBLMaxSql->isChecked())
            whereTemp.append("lightintensity <= " + QString::number(ui->sBLMax->value()));
        if(!ui->cBTimeMinSql->isChecked())
            whereTemp.append("time >= \"" + ui->dateTimeMin->dateTime().toString("yyyy-MM-dd hh:mm:ss") + "\"");
        if(!ui->cBTimeMaxSql->isChecked())
            whereTemp.append("time <= \"" + ui->dateTimeMax->dateTime().toString("yyyy-MM-dd hh:mm:ss") + "\"");

        if(!whereTemp.isEmpty())
        {
            if(where.length()!=6)
                where += " and";
            where = where + " " + whereTemp.join(" and ");
            whereTemp.clear();
        }

        if(where.length()!=6)
            order = where + order;
    }

    quint8 head = 0x00;
    QString sqlquery = "select ";
    int count = 1;
    if(ui->cBSelectWhole->isChecked())
    {
        sqlquery += "node,temperature,humidity,lightintensity,time from iot";
        sqlquery += order;

        sqlmodel->setQuery(sqlquery,db);
        sqlmodel->setHeaderData(0,Qt::Horizontal,"节点");
        sqlmodel->setHeaderData(1,Qt::Horizontal,"温度(°C)");
        sqlmodel->setHeaderData(2,Qt::Horizontal,"湿度(%)");
        sqlmodel->setHeaderData(3,Qt::Horizontal,"光照(lx)");
        sqlmodel->setHeaderData(4,Qt::Horizontal,"时间");
        count = 4;
    }
    else
    {
        sqlquery += "node";
        if(ui->cBSelectTemp->isChecked())
        {
            sqlquery += ",temperature";
            head |= 0x01;
        }
        if(ui->cBSelectHumid->isChecked())
        {
            sqlquery += ",humidity";
            head |= 0x02;
        }
        if(ui->cBSelectLight->isChecked())
        {
            sqlquery += ",lightintensity";
            head |= 0x04;
        }
        sqlquery += ",time from iot";
        sqlquery += order;

        sqlmodel->setQuery(sqlquery,db);
        sqlmodel->setHeaderData(0,Qt::Horizontal,"节点");
        if(head&0x01)
            sqlmodel->setHeaderData(count++,Qt::Horizontal,"温度(°C)");
        if(head&0x02)
            sqlmodel->setHeaderData(count++,Qt::Horizontal,"湿度(%)");
        if(head&0x04)
            sqlmodel->setHeaderData(count++,Qt::Horizontal,"光照(lx)");
        sqlmodel->setHeaderData(count,Qt::Horizontal,"时间");
    }

    ui->tableView->setModel(sqlmodel);
    ui->tableView->setColumnWidth(count,250);
    ui->lineSqlInfo->setText(sqlquery);
    ui->tabWidget->setCurrentIndex(2);
}

void Widget::on_cBSelectTemp_stateChanged(int arg1)
{
    if(arg1)
        ui->cBSelectWhole->setChecked(false);
}


void Widget::on_cBSelectHumid_stateChanged(int arg1)
{
    if(arg1)
        ui->cBSelectWhole->setChecked(false);
}


void Widget::on_cBSelectLight_stateChanged(int arg1)
{
    if(arg1)
        ui->cBSelectWhole->setChecked(false);
}

void Widget::on_cBSelectWhole_clicked(bool checked)
{
    if(checked)
    {
        ui->cBSelectTemp->setChecked(false);
        ui->cBSelectHumid->setChecked(false);
        ui->cBSelectLight->setChecked(false);
    }
    else
    {
        ui->cBSelectTemp->setChecked(true);
        ui->cBSelectHumid->setChecked(true);
        ui->cBSelectLight->setChecked(true);
    }
}


void Widget::on_cBWhereAll_stateChanged(int arg1)
{
    if(arg1)
        ui->cBWhereAll->setStyleSheet("color::rgb(138, 255, 133);");
    else
        ui->cBWhereAll->setStyleSheet("color:rgb(85, 87, 83);");
}

QByteArray Widget::textTransform(bool text)
{
    QAbstractItemModel *model;
    model = ui->tableView->model();
    int rows = model->rowCount();
    int columns = model->columnCount();
    QByteArray seperator = text?"\t|":", ";

    QByteArray textout = "";
    for (int column = 0; column < columns; column++) {
        textout += model->headerData(column,Qt::Horizontal).toString().toUtf8().data();
        textout += seperator;
    }
    textout += "\n";
    for (int row = 0; row < rows; row++) {
        for (int column = 0; column < columns; column++) {
            textout += model->data(model->index(row,column)).toString().toUtf8().data();
            textout += seperator;
        }
        textout += "\n";
    }
    return textout;
}

void Widget::on_btnSqlExcelOutput_clicked()
{
    QString fullpath = QDir::currentPath()+"/output/"+QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss")+".csv";
    QFile csvFile(fullpath);
    if(csvFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {

        QTextStream out(&csvFile);
        QTextCodec *codec = QTextCodec::codecForName("GBK");
        out.setCodec(codec);
        out << QString::fromUtf8(textTransform(false));
        csvFile.close();
        QMessageBox::information(this,"保存成功", "已保存到"+fullpath);
    }
}


void Widget::on_btnISqlTxtOutput_clicked()
{
    QString fullpath = QDir::currentPath()+"/output/"+QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss")+".txt";
    QFile txtFile(fullpath);
    if(txtFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {

        QTextStream out(&txtFile);
        out << QString::fromUtf8(textTransform(true));
        QMessageBox::information(this,"保存成功", "已保存到"+fullpath);
        txtFile.close();
    }
}


void Widget::on_btnModeChange_clicked(bool checked)
{
    if(checked)
        ui->btnModeChange->setText("Remote");
    else
        ui->btnModeChange->setText("Local");

}

