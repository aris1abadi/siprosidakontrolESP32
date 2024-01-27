#include <Arduino.h>
#include "NuSerial.hpp"
#include "NimBLEDevice.h"

extern String kontrolID;
extern unsigned int heartBeat_val;
String ble_buff = "";
bool rx_ready = false;
int tx_count = 0;

unsigned long ble_time = millis();

void ble_init()
{
  String ble_name = "Siprosida ";
  ble_name += kontrolID;
  Serial.println("*****************************");
  Serial.println("         Siprosida           ");
  Serial.println("*****************************");
  Serial.println("--Initializing--");

  // Initialize BLE stack and Nordic UART service
  NimBLEDevice::init(ble_name.c_str());
  NuSerial.begin(115200);

  // Initialization complete
  Serial.println("--Ready--");
}

void ble_loop()
{

  if (NuSerial.isConnected())
  {

    // int serialMonitorChar = Serial.read();
    // if ((serialMonitorChar == 'E') || (serialMonitorChar == 'e'))
    //{
    // Open the serial monitor in Arduino IDE
    // Type "E" or "e" and press ENTER to drop the BLE connection
    //  Serial.println("--Terminating connection from server side--");
    //  NuSerial.end();
    //}
    // else
    //{
    // int processedCount = 0;
    // int availableCount = NuSerial.available();
    // if (availableCount)
    //   Serial.printf("--Available %d bytes for processing--\n", availableCount);
    if ((NuSerial.available()) && (!rx_ready))
    {
      char bleChar = NuSerial.read();
      if (bleChar == '\r')
      {
        rx_ready = true;
        tx_count = 0;
        Serial.println(ble_buff);
        }
      else
      {
       
          ble_buff += bleChar;
       
      }
    }
  
    if(rx_ready){
      if ((millis() - ble_time) > 30)
        {
          ble_time = millis();

          if (NuSerial.write(ble_buff.charAt(tx_count)) < 1){
            Serial.println("ERROR: NuSerial.write() failed");
          }
          if(++tx_count >= ble_buff.length()){
            tx_count = 0;
            rx_ready = false;
            ble_buff ="";
          }
          
        }
      
    }
  }

  // if (processedCount != availableCount)
  //  Serial.printf("\nERROR: %d bytes were available, but %d bytes were processed.\n", availableCount, processedCount);
  // else if (processedCount)
  //{
  // Serial.printf("\n--Stream of %d bytes processed--\n", processedCount);
  // }
  //}
}

/*

lolin32
BatteryLevelCharacteristic.setValue(&level, 1);
  BatteryLevelCharacteristic.notify();
*/
