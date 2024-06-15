#ifndef mqttHandle_h
#define mqttHandle_h

void mqtt_init();
bool getWifi_sts();
void kirimKeApp(String type, uint8_t nomer, String cmd, String msg);
String getStatusNow();
void mqtt_loop();
void cekMqttMsg(String mqTopic, String mqMsg);
#endif