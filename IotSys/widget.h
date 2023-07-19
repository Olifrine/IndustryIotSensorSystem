#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtMqtt/qmqttclient.h>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>
#include <QVector>
#include <QStringList>
#include <QMap>
#include <QSqlDatabase>
#include <QTimer>
#include "qtserver.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

using namespace std;

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);

    ~Widget();

    void post();

    void postBack(QNetworkReply* reply);

    QByteArray HMACSha1(QByteArray key, QByteArray baseString);

    void setStatus();

    void mqttSubscribe();

    void curStatus(QString message);

    void limitInitial();

    void settingCheck();

    void settingSave(bool def);

    void settingLoad();

    void logOnoffSave();

    void logMessageSave();

    QByteArray textTransform(bool text);

    void publish(int num);

    static QString AccessKeyId;
    static QString IotInstanceId;
    static QString ProductKey;
    static char clientId[150];
    static char username[65];
    static char password[65];
    static int logOCount;
    static int logICount;
//    QVector<bool> devStatus;
    quint8 devStatus;
    quint8 preStatus;
    quint8 chgStatus;
    quint8 localStatus;
    QMap<QString,int> devList;
    QMap<QString,int>::iterator indicator;
    QMap<QString,QString> setting;
    float TMin,TMax;
    float HMin,HMax;
    int LMin,LMax;
    int BMin;
    QSqlDatabase db;

private slots:
    void change_state_label();
    void on_btnTestNode1_clicked();

    void on_btnTestNode2_clicked();

    void on_btnTestNode3_clicked();

    void on_btnTestLocal_clicked();

    void on_btnDisconnected_clicked();

    void on_lineProductKeyLocal_textChanged(const QString &arg1);

    void on_btnOnOffClear_clicked();

    void on_btnMessageClear_clicked();

    void on_btnLimitFlash_clicked();

    void on_btnSave_clicked();

    void on_logOnOff_textChanged();

    void on_logMessage_textChanged();

    void on_btnReQuery_clicked();

    void on_btnSqlQuery_clicked();

    void on_cBSelectTemp_stateChanged(int arg1);

    void on_cBSelectHumid_stateChanged(int arg1);

    void on_cBSelectLight_stateChanged(int arg1);

    void on_cBSelectWhole_clicked(bool checked);

    void on_cBWhereAll_stateChanged(int arg1);

    void on_btnSqlExcelOutput_clicked();

    void on_btnISqlTxtOutput_clicked();

    void on_btnModeChange_clicked(bool checked);

signals:

private:
    Ui::Widget *ui;
    QMqttClient *m_client;
    QNetworkAccessManager *manager;
    QtServer *m_server;
};
#endif // WIDGET_H
