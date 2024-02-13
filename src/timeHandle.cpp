
#define USE_INTERNAL_RTC

#ifndef USE_INTERNAL_RTC
#include <SPI.h>

#include <RTClib.h>
#else
#include <ESP32Time.h>
#endif

#include <NTPClient.h>
#include <WiFiUdp.h>

#include <timeHandle.h>
#include <siramHandle.h>
#include <pestisidaHandle.h>
#include <biopestHandle.h>

#include <mqttHandle.h>
#include <lcdHandle.h>
#include <oledHandle.h>
#include <eepromHandle.h>
#include "SSD1306Wire.h"  

#define USE_OLED

#ifndef USE_INTERNAL_RTC
RTC_DS3231 rtc;
#else
// ESP32Time rtc;
// ESP32Time rtc(3600);  // offset in seconds GMT+1
ESP32Time rtc(0); // GMT+7
#endif

// #define USE_NTP

uint8_t lastMenit = 0;
uint8_t lastJam = 0;
uint8_t jamNow = 0;
uint8_t menitNow = 0;
uint8_t hariNow = 0;
unsigned long time_delay = 0;
uint8_t updateSync_count = 0;
// bool siramStatus = false;

#ifndef USE_INTERNAL_RTC
DateTime now;
#endif
String timeNow = "";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 25200, 60000);

char daysOfTheWeek[7][12] = {"Minggu", "Senin", "Selasa", "Rabo", "Kamis", "Jumat", "Sabtu"};
char bulan[12][12] = {"Januari","Februari","Maret","April","Mei","Juni","Juli","Agustus","September","Oktober","Nopember","Desember"};
void ntp_init()
{
  timeClient.begin();
  Serial.println("NTP init");
}

void ntp_sync()
{
  timeClient.update();
  // cek update tiap 10 menit
  unsigned long unix_epoch = timeClient.getEpochTime(); // Get epoch time
  Serial.println("Sync Time");
  rtc.setTime(unix_epoch); // Set RTC time using NTP epoch time
}
void time_init()
{

  Serial.println("time init");
  // Wire.pins(D3, D4);
#ifndef USE_INTERNAL_RTC
  Wire.begin();
  rtc.begin();
  if (!rtc.lostPower())
  {
    Serial.println("RTC is NOT running, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
/*
  if (!rtc.isrunning())
  {
    Serial.println("RTC is NOT running, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  )
  */

// rtcTimer.attach(20, time_update);
#endif
}

void time_set(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year)
{
// January 21, 2014 at 3am you would call:
#ifndef USE_INTERNAL_RTC

  rtc.adjust(DateTime(year, month, dayOfMonth, hour, minute, second));
#endif
}

void getTimeDisplay()
{
}

void displayWaktu()
{
  // String wk = daysOfTheWeek[timeClient.getDay()];
  String timeDisplay1 = "";
  String timeDisplay2 = "";

#ifdef USE_INTERNAL_RTC
  timeDisplay1 = daysOfTheWeek[rtc.getDayofWeek()];
  timeDisplay1 += ",";
  timeDisplay1 += rtc.getDay();
  timeDisplay1 += " ";
  timeDisplay1 += bulan[rtc.getMonth()];
  timeDisplay1 += " ";
  timeDisplay1 += rtc.getYear();

  if (rtc.getHour(true) < 10)
  {
    timeDisplay2 += "0";
  }
  timeDisplay2 += rtc.getHour(true);
  timeDisplay2 += ":";
  if (rtc.getMinute() < 10)
  {
    timeDisplay2 += "0";
  }
  timeDisplay2 += rtc.getMinute();
  timeDisplay2 += ":";

  if (rtc.getSecond() < 10)
  {
    timeDisplay2 += "0";
  }
  timeDisplay2 += rtc.getSecond();
#else
  timeDisplay1 = daysOfTheWeek[now.dayOfTheWeek()];
  timeDisplay1 += ",";
  timeDisplay1 += now.getDay();
  timeDisplay1 += "-";
  timeDisplay1 += now.getMonth();
  timeDisplay1 += "-";
  timeDisplay1 += now.getYear();

  if (now.hour() < 10)
  {
    timeDisplay2 += "0";
  }
  timeDisplay2 += now.hour();
  timeDisplay2 += ":";
  if (now.minute() < 10)
  {
    timeDisplay2 += "0";
  }
  timeDisplay2 += now.minute();
  timeDisplay2 += ":";

  if (now.second() < 10)
  {
    timeDisplay2 += "0";
  }
  timeDisplay2 += now.second();
#endif

  // wk += ",    ";
  #ifdef USE_OLED
  oled_stanby_mode(timeDisplay1,timeDisplay2);
  #else
  lcd_print(0, 1, timeDisplay1);
  lcd_print(8, 1, timeDisplay2);
  #endif
  
}
void time_update()
{
#ifdef USE_NTP
  timeClient.update();
  if (getRunMode() == 0)
  { // mode stanby
    displayWaktu();
  }

  if (lastMenit != timeClient.getMinutes())
  {
    timeNow = timeClient.getFormattedTime();
    Serial.println(timeNow);
    kirimKeApp("kontrol", 0, "time", timeNow);

    lastMenit = timeClient.getMinutes(); // cek setiap menit saja
    jamNow = timeClient.getHours();
    hariNow = timeClient.getDay();
    menitNow = lastMenit;
    cekJadwalSiram();
    cekJadwalPestisida();
    cekJadwalBiopest();
  }
#else
#ifdef USE_INTERNAL_RTC

  if (lastMenit != rtc.getMinute())
  {

    if (getWifi_sts)
    {

      if (++updateSync_count > 10)
      {
        updateSync_count = 0;
        ntp_sync();
      }
    }
    Serial.println(rtc.getDateTime(true));
    lastMenit = rtc.getMinute(); // cek setiap menit saja
    jamNow = rtc.getHour();
    hariNow = rtc.getDayofWeek();

    menitNow = lastMenit;
  }

#else
  now = rtc.now();

  if (lastMenit != now.minute())
  {

    if (getWifi_sts)
    {
      timeClient.update();
      if (++updateSync_count > 10)
      {
        updateSync_count = 0;
        // cek update tiap 10 menit
        unsigned long unix_epoch = timeClient.getEpochTime(); // Get epoch time
        Serial.println("Sync Time");
        rtc.adjust(DateTime(unix_epoch)); // Set RTC time using NTP epoch time
      }
    }
    timeNow = String(now.year());
    timeNow += "-";
    if (now.month() < 10)
      timeNow += "0";
    timeNow += now.month();
    timeNow += "-";
    if (now.day() < 10)
      timeNow += "0";
    timeNow += now.day();
    timeNow += " ";
    if (now.hour() < 10)
    {
      timeNow += "0";
    }
    timeNow += now.hour();
    timeNow += ":";
    if (now.minute() < 10)
    {
      timeNow += "0";
    }
    timeNow += now.minute();
    timeNow += ":";
    if (now.second() < 10)
    {
      timeNow += "0";
    }
    timeNow += now.second();

    Serial.println(timeNow);
    // kirimKeApp("kontrol", 0, "time", timeNow);

    lastMenit = now.minute(); // cek setiap menit saja
    jamNow = now.hour();
    hariNow = now.dayOfTheWeek();

    menitNow = lastMenit;
    // cekJadwalSiram();
    // cekJadwalPestisida();
    // cekJadwalBiopest();
  }

#endif
  // timeClient.update();
  if (getRunMode() == 0)
  { // mode stanby
    displayWaktu();
  }

#endif
}

void time_loop()
{
  if ((millis() - time_delay) > 1000)
  {
    time_delay = millis();
    time_update();
  }
}

String getTimeNow()
{
  return timeNow;
}