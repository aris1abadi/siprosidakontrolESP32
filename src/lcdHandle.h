#ifndef lcdHandle_h
#define lcdHandle_h

    void lcd_init();
    void lcd_print(uint8_t x,uint8_t y,String val);
    void lcd_clear();
    void lcd_cursorBlink(uint8_t x,uint8_t y);
    void lcd_stanby();
    void lcd_prosesSiram();
    void lcdUpdate();
    void lcd_loop();

#endif