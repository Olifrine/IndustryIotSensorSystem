#ifndef AIOT_CALC_H
#define AIOT_CALC_H
int aiotMqttSign(const char *productKey, const char *deviceName, const char *deviceSecret,
                  char clientId[150], char username[64], char password[65]);
#endif // AIOT_CALC_H
