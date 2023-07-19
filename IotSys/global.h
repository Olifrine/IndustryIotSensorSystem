#ifndef GLOBAL_H
#define GLOBAL_H
#include <QLabel>

class Global
{
public:
    Global();
    static int mqttClientCount;
    static int devIndex;
};
void mqttConnectSuccess(QLabel *label);
#endif // GLOBAL_H
