#ifndef mySensorsHandle_h
#define mySensorsHandle_h

//void handleInp(MyMessage msg, String textMode);
void cekMySensors(String msg);
void kirim_ke_device(uint8_t node, uint8_t childnode, uint8_t type, String payload);
void setFlowRate_start();
void setFlowRate_stop();
void resetTotalLiter();
float getTotalLiter();

#endif