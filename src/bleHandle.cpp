
#include <BleSerial.h>
#include <mqttHandle.h>
#include <eepromHandle.h>


const int BUFFER_SIZE = 8192;
extern String kontrolID;

long dl = 0;
BleSerial SerialBT;

//uint8_t unitMACAddress[6];  // Use MAC address in BT broadcast and display
//char deviceName[20];        // The serial string that is broadcast.

String ble_buff ="";


uint8_t bleReadBuffer[BUFFER_SIZE];
uint8_t serialReadBuffer[BUFFER_SIZE];

void startBluetooth() {
  // Get unit MAC address
  //esp_read_mac(unitMACAddress, ESP_MAC_WIFI_STA);
  
  // Convert MAC address to Bluetooth MAC (add 2): https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/system.html#mac-address
  //unitMACAddress[5] += 2;                                                          
  
  //Create device name
  //sprintf(deviceName, "BleBridge-%02X%02X", unitMACAddress[4], unitMACAddress[5]); 

  //Init BLE Serial
  String bleID = "SIPROSIDA_";
  bleID += kontrolID;
  SerialBT.begin(bleID.c_str());
  SerialBT.setTimeout(10);
}

//Task for reading Serial Port
void ReadSerialTask(void *e) {
  while (true) {
    if (Serial.available()) {
      auto count = Serial.readBytes(serialReadBuffer, BUFFER_SIZE);
      SerialBT.write(serialReadBuffer, count);
    }
    delay(20);
  }
}
void kirim_ble(String bleData){
  SerialBT.print(bleData.c_str());
}

//Task for reading BLE Serial
void ReadBtTask(void *e) {
  while (true) {
    if (SerialBT.available()) {
      auto count = SerialBT.readBytes(bleReadBuffer, BUFFER_SIZE);
      Serial.write(bleReadBuffer, count);
    }
    delay(20);
  }
}


void ble_init() {
 

  //Start BLE
  startBluetooth();

  //Disable watchdog timers
  disableCore0WDT();
  disableCore1WDT();
  disableLoopWDT();
  
 
}

void ble_loop() {

if((millis() - dl) > 20){
  dl = millis();

  if (SerialBT.available()) {
    /*
      auto count = SerialBT.readBytes(bleReadBuffer, BUFFER_SIZE);      
      //Serial.write(bleReadBuffer, count);
      //Serial.println();
      //copi buff
      ble_buff = "";
      for(int i = 0;i < count ;i++){
        ble_buff += char(bleReadBuffer[i]);
      }
      ble_buff += ";\n";
      Serial.print(ble_buff);
      cekMqttMsg(getValue(ble_buff,';',0),getValue(ble_buff,';',1));
      */
   
     char b = SerialBT.read();
     if(b == '\n'){
      Serial.println(ble_buff);
      cekMqttMsg(getValue(ble_buff,';',0),getValue(ble_buff,';',1));
      ble_buff = "";
     }else{
      ble_buff += b;
     }
    }
}
  
}
