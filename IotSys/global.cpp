#include "global.h"

Global::Global()
{

}
int Global::mqttClientCount = 0;
int Global::devIndex = 0;

void mqttConnectSuccess(QLabel *label)
{
    label->setText("连接成功");
}
