#ifndef QLABELSTATUS_H
#define QLABELSTATUS_H

#include <QObject>
#include <QWidget>
#include <QLabel>
class QLabelStatus : public QLabel
{
    Q_OBJECT
public:
    explicit QLabelStatus(QWidget *parent = nullptr);
    ~QLabelStatus();
    void statusAlert(quint8 status,quint8 alertNum);
    void statusChange(quint8 status,quint8 alertNum);
    void statusChange(quint8 status,quint8 alertNum,quint8 local);
public slots:

private:
    static quint8 alert;
signals:

};

#endif // QLABELSTATUS_H
