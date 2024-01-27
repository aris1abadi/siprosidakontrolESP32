#include <LCD_I2C.h>

bool lcd_update = false;

String line1 = "                ";
String line2 = "                ";

LCD_I2C lcd(0x27,16,2); // Default address of most PCF8574 modules, change according

void lcdUpdate(){
    lcd_update = true;    

}



void lcd_init(){
    lcd.begin();
    lcd.backlight();
    //         0123456789012345
    lcd.print("   SIPROSIDA    "); // You can make spaces using well... spaces
    lcd.setCursor(0, 1); // Or setting the cursor in the desired position.
    lcd.print("------BSIP-----");
}

void lcd_cursorBlink(uint8_t x,uint8_t y){
    lcd.setCursor(x, y);
    lcd.blink();
}

void lcd_stanby(){
    lcd.clear();
    lcd.print("   SIPROSIDA    "); // You can make spaces using well... spaces
    lcd.setCursor(0, 1); // Or setting the cursor in the desired position.
    lcd.print("----------------");
}

void lcd_prosesSiram(){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Proses Siram ...");
    lcd.setCursor(0, 1);
    lcd.print("00:00     00 ltr");
}

void lcd_noBlink(){
    
}

void lcd_print(uint8_t x,uint8_t y,String val){
    lcd.setCursor(x,y);
    lcd.print(val);
}

void lcd_clear(){
    lcd.clear();

}

void lcd_loop(){
    if(lcd_update){
        lcd_print(0,0,line1);
        lcd_print(0,0,line2);
    }
}