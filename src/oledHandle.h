#ifndef OLEDHANDLE_H
#define OLEDHANDLE_H

void oled_init();
void oled_print(uint8_t x,uint8_t y,String content);
void oled_font(const uint8_t *fontData);
void oled_clear();
void oled_stanby_mode(String hari,String jam);
void oled_header(String hd);
void oled_update();
void oled_update_display();
void oled_kotak(int16_t x, int16_t y, int16_t width, int16_t height);
void oled_update(String header,String body1,String body2,String footer);
#endif
