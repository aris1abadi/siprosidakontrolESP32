/*
Base koneksi ke aktuator dan sensor dengan MySensors
*/

// Enable and select radio type attached
#define MY_RADIO_RF24
// #define MY_RADIO_NRF5_ESB
// #define MY_RADIO_RFM69
// #define MY_RADIO_RFM95
#define MY_RF24_CHANNEL 120

// Set LOW transmit power level as default, if you have an amplified NRF-module and
// power your radio separately with a good regulator you can turn up PA level.
// #define MY_RF24_PA_LEVEL RF24_PA_LOW

// Enable serial gateway
#define MY_GATEWAY_SERIAL

// Define a lower baud rate for Arduinos running on 8 MHz (Arduino Pro Mini 3.3V & SenseBender)
#if F_CPU == 8000000L
#define MY_BAUD_RATE 38400
#endif

// Enable inclusion mode
#define MY_INCLUSION_MODE_FEATURE
// Enable Inclusion mode button on gateway
// #define MY_INCLUSION_BUTTON_FEATURE

// Inverses behavior of inclusion button (if using external pullup)
// #define MY_INCLUSION_BUTTON_EXTERNAL_PULLUP

// Set inclusion mode duration (in seconds)
#define MY_INCLUSION_MODE_DURATION 60
// Digital pin used for inclusion mode button
// #define MY_INCLUSION_MODE_BUTTON_PIN  3

// Set blinking period
#define MY_DEFAULT_LED_BLINK_PERIOD 300

//ESP32 CE pada GPIO4
#define MY_RF24_CE_PIN 4 

// Inverses the behavior of leds
// #define MY_WITH_LEDS_BLINKING_INVERSE

// Flash leds on rx/tx/err
// Uncomment to override default HW configurations
// #define MY_DEFAULT_ERR_LED_PIN 4  // Error led pin
// #define MY_DEFAULT_RX_LED_PIN  6  // Receive led pin
// #define MY_DEFAULT_TX_LED_PIN  5  // the PCB, on board LED

#define SLEEP_PIN GPIO_NUM_15
//#define USE_OLED

//#include "define.h"
#include <MySensors.h>
#include <mqttHandle.h>
#include <timeHandle.h>
#include <eepromHandle.h>
#include <aktuatorHandle.h>
#include <siramHandle.h>
#include <pestisidaHandle.h>
#include <biopestHandle.h>
#include <otaHandle.h>
#include <bleHandle.h>
#ifdef USE_OLED
#include <oledHandle.h>
#else
#include <lcdHandle.h>
#endif
#include <keypadHandle.h>



unsigned long sleep_timer = millis();
int sleep_bounc =0;

void sleep_loop(){
    //bounc cek gpio2
    if((millis() - sleep_timer) > 50){
        sleep_timer = millis();
        int pin_sleep_status = digitalRead(SLEEP_PIN);
        if(pin_sleep_status == 0){
            if(++sleep_bounc > 10){
                //masuk sleep mode
                esp_deep_sleep_start();
            }
        }else{
            sleep_bounc = 0;
        }
    }
    

}

void before()
{
    //getID();
    esp_sleep_enable_ext0_wakeup(SLEEP_PIN,1); //1 = High, 0 = Low
    time_init();
    mqtt_init();
    
    #ifdef USE_OLED
    oled_init();
    #else
    lcd_init();
    #endif
    keypad_init();
    
   
}

void setup()
{
   loadDefault();   
   aktuator_off_semua();
   //ble_init();
   //Serial.println(getID());  

}

void presentation()
{
    // Present locally attached sensors
}

void loop()
{
    time_loop();
    aktuator_loop();
    siram_loop();
    pestisida_loop();
    biopest_loop();
    mqtt_loop();
    ota_loop();
    keypad_loop();
    //ble_loop();
    sleep_loop();
}
