#include <Wire.h>               // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h"  
#include <oledHandle.h>
#include "oledFont.h"

SSD1306Wire display(0x3c, SDA, SCL);

void oled_init(){
  display.init();

  display.flipScreenVertically();
  display.setFont(Roboto_Bold_12);
  display.setTextAlignment(TEXT_ALIGN_LEFT);  
  //                       "01234567890123456"
  display.drawString(0, 0, "    SIPROSIDA    ");
  display.display();
  //oled_update("Siprosida","test 1234567","baris 3","baris 4");

}

void oled_stanby_mode(String hari,String jam){
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT); 
  display.setFont(ArialMT_Plain_16);
  oled_header("    SIPROSIDA    ");
  display.setFont(ArialMT_Plain_10);  
  display.drawString(0, 16, hari.c_str());
  display.setFont(ArialMT_Plain_24);   
  display.drawString(16, 28, jam.c_str());  
  display.setFont(ArialMT_Plain_10);  
  //                        plain 10 >> 21 karakter
  display.drawString(0, 50, " ");
  display.display();

}

void oled_update(String header,String body1,String body2,uint8_t footer){
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT); 
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, header.c_str());
  display.setFont(ArialMT_Plain_10);  
  display.drawString(0, 16, body1.c_str());
  display.setFont(ArialMT_Plain_24);   
  display.drawString(16, 28, body2.c_str());  
  display.setFont(ArialMT_Plain_10);  
  //                        plain 10 >> 21 karakter
  if(footer == 1)
  display.drawString(4, 50, "* (start)");
  display.drawString(68, 50, "# (stop)");
  display.display();
}

void oled_update_display(){
  display.display();
}

void oled_kotak(int16_t x, int16_t y, int16_t width, int16_t height){
  display.drawRect( x,  y,  width,  height);
}
void oled_header(String hd){
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0,hd.c_str());
}


void oled_print(uint8_t x,uint8_t y,String content){  
  
  display.drawString(x, y, content.c_str());
  display.display();
}
void oled_font(const uint8_t *fontData){
  display.setFont(fontData);
}

void oled_clear(){
  display.clear();
}
void oled_loop(){

}