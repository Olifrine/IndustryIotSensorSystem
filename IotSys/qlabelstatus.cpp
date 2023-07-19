#include "qlabelstatus.h"
#include <QString>
QLabelStatus::QLabelStatus(QWidget *parent)
{
    QString style = "color:rgb(255,255,255);\nborder-radius: 30px;\nborder:3px groove gray;\nborder-style: outset;\nbackground-color:rgb(85, 87, 83);";
    this->setStyleSheet(style);
}

QLabelStatus::~QLabelStatus()
{

}

quint8 QLabelStatus::alert = 0x00;

void QLabelStatus::statusChange(quint8 status,quint8 alertNum)
{
    QString head = "color:rgb(255,255,255);\nborder-radius: 30px;\nborder:3px groove gray;\nborder-style: outset;\nbackground-color:";
    QString color;
    if(status)
        if(alert&alertNum)
            color = "rgb(206, 74, 80);";
        else
            color = "rgb(78, 154, 6);";
    else
        color = "rgb(85, 87, 83);";
    QString style = head + color;
    this->setStyleSheet(style);
}

void QLabelStatus::statusChange(quint8 status,quint8 alertNum,quint8 local)
{
    QString head = "color:rgb(255,255,255);\nborder-radius: 30px;\nborder:3px groove gray;\nborder-style: outset;\nbackground-color:";
    QString color;
    if(status)
        if(local&alertNum)
            if(alert&alertNum)
                color = "rgb(206, 74, 80);";
            else
                color = "rgb(78, 154, 6);";
        else
            color = "rgb(251, 210, 11)";
    else
        color = "rgb(85, 87, 83);";
    QString style = head + color;
    this->setStyleSheet(style);
}


void QLabelStatus::statusAlert(quint8 status,quint8 alertNum)
{
    if(status)
        alert &= ~alertNum;
    else
        alert |= alertNum;
}
