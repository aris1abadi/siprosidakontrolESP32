#include "I2CKeyPad.h"
#include <lcdHandle.h>
#include <eepromHandle.h>
#include <siramHandle.h>
#include <pestisidaHandle.h>
#include <biopestHandle.h>
#include <mqttHandle.h>
#include <mySensorsHandle.h>
#include <oledHandle.h>
#include <SSD1306Wire.h>

#define USE_OLED

#define STANBY 0
#define SIRAM 1
#define PESTISIDA 2
#define BIOPEST 3
#define KEY_SET_SIRAM 4
#define KEY_SET_PESTISIDA 5
#define KEY_SET_BIOPEST 6

#define SEMUA_LAHAN 0

extern bool selenoidLahan1_status;
extern bool selenoidLahan2_status;
extern bool selenoidLahan3_status;
extern bool selenoidLahan4_status;

extern bool selenoidLahan1_pestisida_status;
extern bool selenoidLahan2_pestisida_status;
extern bool selenoidLahan3_pestisida_status;
extern bool selenoidLahan4_pestisida_status;

extern bool selenoidLahan1_biopest_status;
extern bool selenoidLahan2_biopest_status;
extern bool selenoidLahan3_biopest_status;
extern bool selenoidLahan4_biopest_status;

extern uint8_t jadwalSiram1[19];
extern uint8_t jadwalSiram2[19];
extern uint8_t jadwalSiram3[19];

extern uint8_t jadwalPestisida1[19];
extern uint8_t jadwalPestisida2[19];
extern uint8_t jadwalPestisida3[19];

extern uint8_t jadwalBiopest1[19];
extern uint8_t jadwalBiopest2[19];
extern uint8_t jadwalBiopest3[19];

const uint8_t KEYPAD_ADDRESS = 0x26;

I2CKeyPad keyPad(KEYPAD_ADDRESS);

char keymap[19] = "123A456B789C*0#DNF"; // N = NoKey, F = Fail

// uint32_t start, stop;
uint32_t lastKeyPressed = 0;
uint32_t value = 0;
uint8_t repeatCount = 0;
char lastKey = 'N';

uint8_t keyMode = STANBY;

uint8_t menuCount = 0;
uint8_t subMenuCount = 0;
uint8_t keyCount = 0;
bool keyTes = false;
bool keySts = false;
String keyNumber = "";
extern String line1;
extern String line2;
uint8_t jadwalRawTemp[19];
String jadwalTemp = "0";
String jadwalTempAll = "";
String timeTemp = "";
String hariTemp = "";
String lahanTemp = "";
String durasiTemp = "";
String enableTemp = "";
String dosisAirTemp = "2";
String dosisPestTemp = "5";

uint8_t kalibrasi_value = 10;

uint8_t lcd_timeout = 3;
uint8_t lcd_timeCount = 0;
bool lcd_timeout_sts = false;

bool kalibrasi_pestisida_start = false;
bool kalibrasi_air_pestisida_start = false;
bool kalibrasi_biopest_start = false;
bool kalibrasi_air_biopest_start = false;

extern String jadwalSiram;
extern String jadwalPestisida;
extern String jadwalBiopest;
/*
    format jadwal sederhana
    text 3 blok jadwal
    enable,idx,jenis,H,M,m,s,s,r,k,j,s,l1,l2,l3,l4,A,P,T;enable,idx,jenis,H,M,s,s,r,k,j,s,m,l1,l2,l3,l4,A,P,T;enable,idx,jenis,H,M,s,s,r,k,j,s,m,l1,l2,l3,l4,A,P,T

*/

// uint8_t lahanClickCount = 0;

void keypad_init()
{
    if (keyPad.begin() == false)
    {
        Serial.println("\nERROR: keypad tidak terdeteksi.\nCek koneksi.\n");
    }
    keyPad.loadKeyMap(keymap);
    keyPad.setKeyPadMode(I2C_KEYPAD_4x4);
}

uint8_t getKalibrasi_value()
{
    return kalibrasi_value;
}

void editJadwal(uint8_t jenisJadwal, uint8_t nomerJadwal, char key)
{
    if (jenisJadwal == SIRAM)
    {
        if (subMenuCount == 0)
        {
            // jam
            keyNumber += key;
            lcd_print(9, 0, keyNumber);
            // Serial.print(keyNumber);
            if (keyNumber.length() == 2)
            {
                if (keyNumber.toInt() < 24)
                {
                    // simpan sementara jam
                    timeTemp = keyNumber;
                    jadwalRawTemp[3] = keyNumber.toInt();
                    keyNumber = "";
                    subMenuCount = 1;
                    lcd_cursorBlink(12, 0);
                }
                else
                {
                    keyNumber = "";
                    lcd_cursorBlink(9, 0);
                }
            }
        }
        else if (subMenuCount == 1)
        {
            // menit
            keyNumber += key;
            lcd_print(12, 0, keyNumber);
            // Serial.print(keyNumber);
            if (keyNumber.length() == 2)
            {
                if (keyNumber.toInt() < 60)
                {
                    // simpan sementara jam
                    timeTemp += ",";
                    timeTemp += keyNumber;
                    jadwalRawTemp[4] = keyNumber.toInt();

                    keyNumber = "";
                    subMenuCount = 2;
                    lcd_cursorBlink(15, 0);
                }
                else
                {
                    keyNumber = "";
                    lcd_cursorBlink(12, 0);
                }
            }
        }
        else if (subMenuCount == 2)
        {
            if (key == '1')
            {
                lcd_print(15, 0, "+"); // enable
                lcd_cursorBlink(0, 1);

                enableTemp = "1";
                jadwalRawTemp[0] = 1;
                keyNumber = "1234";
                keyCount = 0;
                subMenuCount = 3;
            }
            else if (key == '0')
            {
                lcd_print(15, 0, "-"); // disable jadwal
                lcd_cursorBlink(0, 1);

                enableTemp = "0";
                jadwalRawTemp[0] = 0;
                keyNumber = "1234";
                keyCount = 0;
                subMenuCount = 3;
            }
        }
        else if (subMenuCount == 3)
        {
            keyTes = false;
            keySts = false;

            if (key == '1')
            {
                keyTes = true;
                keySts = true;
            }
            else if (key == '0')
            {
                keyTes = false;
                keySts = true;
            }
            if (keySts)
            {

                // cek key 0/1
                //        "0123456789012345"
                // line1 = "Siram lahan 1234";
                // line2 = "Durasi ";
                switch (keyCount)
                {
                case 0:
                    if (keyTes)
                    {
                        selenoidLahan1_status = true;
                        lcd_print(0, 1, "1");
                        lahanTemp = "1,";
                        jadwalRawTemp[12] = 1;
                    }
                    else
                    {
                        selenoidLahan1_status = false;
                        lcd_print(0, 1, "-");
                        lahanTemp = "0,";
                        jadwalRawTemp[12] = 0;
                    }

                    break;

                case 1:
                    if (keyTes)
                    {
                        selenoidLahan2_status = true;
                        lcd_print(1, 1, "2");
                        lahanTemp += "1,";
                        jadwalRawTemp[13] = 1;
                    }
                    else
                    {
                        selenoidLahan2_status = false;
                        lcd_print(1, 1, "-");
                        lahanTemp += "0,";
                        jadwalRawTemp[13] = 0;
                    }

                    break;
                case 2:
                    if (keyTes)
                    {
                        selenoidLahan3_status = true;
                        lcd_print(2, 1, "3");
                        lahanTemp += "1,";
                        jadwalRawTemp[14] = 1;
                    }
                    else
                    {
                        selenoidLahan3_status = false;
                        lcd_print(2, 1, "-");
                        lahanTemp += "0,";
                        jadwalRawTemp[14] = 0;
                    }

                    break;

                case 3:
                    if (keyTes)
                    {
                        selenoidLahan4_status = true;
                        lcd_print(3, 1, "4");
                        lahanTemp += "1";
                        jadwalRawTemp[15] = 1;
                    }
                    else
                    {
                        selenoidLahan4_status = false;
                        lcd_print(3, 1, "-");
                        lahanTemp += "0";
                        jadwalRawTemp[15] = 0;
                    }
                    break;
                }

                if (++keyCount > 3)
                {
                    subMenuCount = 4;
                    keyCount = 0;
                    keyNumber = "MSSRKJS";
                    lcd_cursorBlink(5, 1);
                }
                else
                {
                    lcd_cursorBlink((0 + keyCount), 1);
                }
            }
        }
        else if (subMenuCount == 4)
        {
            keyTes = false;
            keySts = false;

            if (key == '1')
            {
                keyTes = true;
                keySts = true;
            }
            else if (key == '0')
            {
                keyTes = false;
                keySts = true;
            }
            if (keySts)
            {

                // cek key 0/1
                //         "0123456789012345"
                // line1 = "Siram J1 14:20 +";
                // line2 = "1234 MSSRKJSM 05";

                switch (keyCount)
                {
                case 0:
                    if (keyTes)
                    {
                        // minggu
                        lcd_print(5, 1, "M");
                        hariTemp = "1,";
                        jadwalRawTemp[5] = 1;
                    }
                    else
                    {
                        lcd_print(5, 1, "-");
                        hariTemp = "0,";
                        jadwalRawTemp[5] = 0;
                    }

                    break;

                case 1:
                    if (keyTes)
                    {
                        lcd_print(6, 1, "S");
                        hariTemp += "1,";
                        jadwalRawTemp[6] = 1;
                    }
                    else
                    {
                        lcd_print(6, 1, "-");
                        hariTemp += "0,";
                        jadwalRawTemp[6] = 0;
                    }

                    break;
                case 2:
                    if (keyTes)
                    {
                        lcd_print(7, 1, "S");

                        hariTemp += "1,";
                        jadwalRawTemp[7] = 1;
                    }
                    else
                    {
                        lcd_print(7, 1, "-");
                        hariTemp += "0,";
                        jadwalRawTemp[7] = 0;
                    }

                    break;

                case 3:
                    if (keyTes)
                    {
                        lcd_print(8, 1, "R");
                        hariTemp += "1,";
                        jadwalRawTemp[8] = 1;
                    }
                    else
                    {
                        lcd_print(8, 1, "-");
                        hariTemp += "0,";
                        jadwalRawTemp[8] = 0;
                    }
                    break;

                case 4:
                    if (keyTes)
                    {
                        lcd_print(9, 1, "K");
                        hariTemp += "1,";
                        jadwalRawTemp[9] = 1;
                    }
                    else
                    {
                        lcd_print(9, 1, "-");
                        hariTemp += "0,";
                        jadwalRawTemp[9] = 0;
                    }
                    break;
                case 5:
                    if (keyTes)
                    {
                        lcd_print(10, 1, "J");
                        hariTemp += "1,";
                        jadwalRawTemp[10] = 1;
                    }
                    else
                    {
                        lcd_print(10, 1, "-");
                        hariTemp += "0,";
                        jadwalRawTemp[10] = 0;
                    }
                    break;
                case 6:
                    if (keyTes)
                    {
                        lcd_print(11, 1, "S");
                        hariTemp += "1";
                        jadwalRawTemp[11] = 1;
                    }
                    else
                    {
                        lcd_print(11, 1, "-");
                        hariTemp += "0";
                        jadwalRawTemp[11] = 0;
                    }
                    break;
                }
                if (++keyCount > 6)
                {
                    subMenuCount = 5;
                    keyCount = 0;
                    keyNumber = "";
                    lcd_cursorBlink(13, 1);
                }
                else
                {
                    lcd_cursorBlink((5 + keyCount), 1);
                }
            }
        }
        else if (subMenuCount == 5)
        {
            // durasi
            keyNumber += key;
            lcd_print(13, 1, keyNumber);
            // Serial.print(keyNumber);
            if (keyNumber.length() == 2)
            {
                if (keyNumber.toInt() < 31)
                {
                    // simpan sementara jam
                    durasiTemp = keyNumber;
                    jadwalRawTemp[18] = keyNumber.toInt();

                    keyNumber = "";
                    subMenuCount = 6;
                    // jadwal
                    jadwalTemp = enableTemp;
                    jadwalTemp += ",";
                    jadwalTemp += String(jenisJadwal);
                    jadwalTemp += ",";
                    jadwalTemp += String(nomerJadwal);
                    jadwalTemp += ",";
                    jadwalTemp += timeTemp;
                    jadwalTemp += ",";
                    jadwalTemp += hariTemp;
                    jadwalTemp += ",";
                    jadwalTemp += lahanTemp;
                    jadwalTemp += ",";
                    jadwalTemp += dosisAirTemp;
                    jadwalTemp += ",";
                    jadwalTemp += dosisPestTemp;
                    jadwalTemp += ",";
                    jadwalTemp += durasiTemp;

                    // for (uint8_t i = 0; i < 19; i++)
                    //{
                    //     Serial.print(jadwalRawTemp[i]);
                    //     Serial.print(",");
                    // }
                    // Serial.println();
                    //         "0123456789012345"
                    //  line1 = "Simpan J1 Siram?";
                    //  line1 = "Simpan J1 Pestd?";
                    //  line1 = "Simpan J1 Biops?";
                    jadwalRawTemp[1] = jenisJadwal;
                    jadwalRawTemp[2] = nomerJadwal;
                    line1 = "Simpan J";
                    if (jenisJadwal == SIRAM)
                    {
                        jadwalTempAll = "";

                        if (nomerJadwal == 1)
                        {
                            jadwalTempAll = jadwalTemp;
                            jadwalTempAll += ";";

                            jadwalTempAll += getValue(jadwalSiram, ';', 1);
                            jadwalTempAll += ";";
                            jadwalTempAll += getValue(jadwalSiram, ';', 2);
                            jadwalTempAll += ";";
                            jadwalTempAll += getValue(jadwalSiram, ';', 3);
                            jadwalTempAll += ";";
                            jadwalTempAll += getValue(jadwalSiram, ';', 4);
                            jadwalTempAll += ";";

                            line1 += "1 Siram?";
                        }
                        else if (nomerJadwal == 2)
                        {
                            jadwalTempAll = getValue(jadwalSiram, ';', 0);
                            jadwalTempAll += ";";
                            jadwalTempAll = jadwalTemp;
                            jadwalTempAll += ";";
                            jadwalTempAll += getValue(jadwalSiram, ';', 2);
                            jadwalTempAll += ";";
                            jadwalTempAll += getValue(jadwalSiram, ';', 3);
                            jadwalTempAll += ";";
                            jadwalTempAll += getValue(jadwalSiram, ';', 4);
                            jadwalTempAll += ";";

                            line1 += "2 Siram?";
                        }
                        else if (nomerJadwal == 3)
                        {
                            jadwalTempAll += getValue(jadwalSiram, ';', 0);
                            jadwalTempAll += ";";
                            jadwalTempAll += getValue(jadwalSiram, ';', 1);
                            jadwalTempAll += ";";
                            jadwalTempAll = jadwalTemp;
                            jadwalTempAll += ";";
                            jadwalTempAll += getValue(jadwalSiram, ';', 3);
                            jadwalTempAll += ";";
                            jadwalTempAll += getValue(jadwalSiram, ';', 4);
                            jadwalTempAll += ";";

                            line1 += "3 Siram?";
                        }
                        else if (nomerJadwal == 4)
                        {
                            jadwalTempAll += getValue(jadwalSiram, ';', 0);
                            jadwalTempAll += ";";
                            jadwalTempAll += getValue(jadwalSiram, ';', 1);
                            jadwalTempAll += ";";
                            jadwalTempAll += getValue(jadwalSiram, ';', 2);
                            jadwalTempAll += ";";
                            jadwalTempAll = jadwalTemp;
                            jadwalTempAll += ";";
                            jadwalTempAll += getValue(jadwalSiram, ';', 4);
                            jadwalTempAll += ";";

                            line1 += "4 Siram?";
                        }
                        else if (nomerJadwal == 5)
                        {
                            jadwalTempAll += getValue(jadwalSiram, ';', 0);
                            jadwalTempAll += ";";
                            jadwalTempAll += getValue(jadwalSiram, ';', 1);
                            jadwalTempAll += ";";
                            jadwalTempAll += getValue(jadwalSiram, ';', 2);
                            jadwalTempAll += ";";
                            jadwalTempAll += getValue(jadwalSiram, ';', 3);
                            jadwalTempAll += ";";
                            jadwalTempAll = jadwalTemp;
                            jadwalTempAll += ";";

                            line1 += "5 Siram?";
                        }

                        // Serial.print("jadwal siram def: ");
                        // Serial.println(jadwalSiram);
                        //.print("jadwal siram tem: ");
                        //.println(jadwalTempAll);
                    }
                    else if (jenisJadwal == 2)
                    {

                        if (nomerJadwal == 1)
                        {
                            line1 += "1 Pestd?";
                        }
                        else if (nomerJadwal == 2)
                        {
                            line1 += "2 Pestd?";
                        }
                        else if (nomerJadwal == 3)
                        {
                            line1 += "3 Pestd?";
                        }
                    }
                    else if (jenisJadwal == 3)
                    {
                        if (nomerJadwal == 1)
                        {
                            line1 += "1 Biops?";
                        }
                        else if (nomerJadwal == 2)
                        {
                            line1 += "2 Biops?";
                        }
                        else if (nomerJadwal == 3)
                        {
                            line1 += "3 Biops?";
                        }
                    }
                    //      "0123456789012345"
                    line2 = "#(simpan)       ";
                    // lcd_print(0, 0, line1);
                    // lcd_print(0, 1, line2);
                    lcd_print(15, 1, "?");
                    lcd_cursorBlink(15, 1);

                    // Serial.print("Jadwal: ");
                    // Serial.println(jadwalTempAll);
                }
                else
                {
                    keyNumber = "";
                    lcd_cursorBlink(13, 1);
                }
            }
        }
        else if (subMenuCount == 6)
        {
            // simpan jadwal?
            Serial.println("Simpan Jadwal?");
        }
    }
    else if ((jenisJadwal == PESTISIDA) || (jenisJadwal == BIOPEST))
    {
        // pestisida
        // 0123456789012345
        // P1 18:00 05 20 +
        // 1234 MSSRKJS
        if (subMenuCount == 0)
        {
            // jam
            keyNumber += key;
            lcd_print(3, 0, keyNumber);
            // Serial.print(keyNumber);
            if (keyNumber.length() == 2)
            {
                if (keyNumber.toInt() < 24)
                {
                    // simpan sementara jam
                    timeTemp = keyNumber;
                    jadwalRawTemp[3] = keyNumber.toInt();
                    keyNumber = "";
                    subMenuCount = 1;
                    lcd_cursorBlink(6, 0);
                }
                else
                {
                    keyNumber = "";
                    lcd_cursorBlink(3, 0);
                }
            }
        }
        else if (subMenuCount == 1)
        {
            // menit
            keyNumber += key;
            lcd_print(6, 0, keyNumber);
            // Serial.print(keyNumber);
            if (keyNumber.length() == 2)
            {
                if (keyNumber.toInt() < 60)
                {
                    // simpan sementara jam
                    timeTemp += ",";
                    timeTemp += keyNumber;
                    jadwalRawTemp[4] = keyNumber.toInt();

                    keyNumber = "";
                    subMenuCount = 2;
                    lcd_cursorBlink(9, 0);
                }
                else
                {
                    keyNumber = "";
                    lcd_cursorBlink(6, 0);
                }
            }
        }
        else if (subMenuCount == 2)
        {
            // dosis air
            keyNumber += key;
            lcd_print(9, 0, keyNumber);
            // Serial.print(keyNumber);
            if (keyNumber.length() == 2)
            {
                if (keyNumber.toInt() < 21)
                {
                    // simpan sementara jam
                    dosisAirTemp = keyNumber;
                    jadwalRawTemp[16] = keyNumber.toInt();

                    keyNumber = "";
                    subMenuCount = 3;
                    lcd_cursorBlink(12, 0);
                }
                else
                {
                    keyNumber = "";
                    lcd_cursorBlink(9, 0);
                }
            }
        }
        else if (subMenuCount == 3)
        {
            // dosis pest
            keyNumber += key;
            lcd_print(12, 0, keyNumber);
            // Serial.print(keyNumber);
            if (keyNumber.length() == 2)
            {
                if (keyNumber.toInt() < 51)
                {
                    // simpan sementara jam
                    dosisPestTemp = keyNumber;
                    jadwalRawTemp[17] = keyNumber.toInt();

                    keyNumber = "";
                    subMenuCount = 4;
                    lcd_cursorBlink(15, 0);
                }
                else
                {
                    keyNumber = "";
                    lcd_cursorBlink(12, 0);
                }
            }
        }
        else if (subMenuCount == 4)
        {
            if (key == '1')
            {
                lcd_print(15, 0, "+"); // enable
                lcd_cursorBlink(0, 1);

                enableTemp = "1";
                jadwalRawTemp[0] = 1;
                keyNumber = "1234";
                keyCount = 0;
                subMenuCount = 5;
            }
            else if (key == '0')
            {
                lcd_print(15, 0, "-"); // disable jadwal
                lcd_cursorBlink(0, 1);

                enableTemp = "0";
                jadwalRawTemp[0] = 0;
                keyNumber = "1234";
                keyCount = 0;
                subMenuCount = 5;
            }
        }
        else if (subMenuCount == 5)
        {
            keyTes = false;
            keySts = false;

            if (key == '1')
            {
                keyTes = true;
                keySts = true;
            }
            else if (key == '0')
            {
                keyTes = false;
                keySts = true;
            }
            if (keySts)
            {

                switch (keyCount)
                {
                case 0:
                    if (keyTes)
                    {
                        lcd_print(0, 1, "1");
                        lahanTemp = "1,";
                        jadwalRawTemp[12] = 1;
                    }
                    else
                    {
                        lcd_print(0, 1, "-");
                        lahanTemp = "0,";
                        jadwalRawTemp[12] = 0;
                    }

                    break;

                case 1:
                    if (keyTes)
                    {
                        lcd_print(1, 1, "2");
                        lahanTemp += "1,";
                        jadwalRawTemp[13] = 1;
                    }
                    else
                    {
                        lcd_print(1, 1, "-");
                        lahanTemp += "0,";
                        jadwalRawTemp[13] = 0;
                    }

                    break;
                case 2:
                    if (keyTes)
                    {
                        lcd_print(2, 1, "3");
                        lahanTemp += "1,";
                        jadwalRawTemp[14] = 1;
                    }
                    else
                    {
                        lcd_print(2, 1, "-");
                        lahanTemp += "0,";
                        jadwalRawTemp[14] = 0;
                    }

                    break;

                case 3:
                    if (keyTes)
                    {
                        lcd_print(3, 1, "4");
                        lahanTemp += "1";
                        jadwalRawTemp[15] = 1;
                    }
                    else
                    {
                        lcd_print(3, 1, "-");
                        lahanTemp += "0";
                        jadwalRawTemp[15] = 0;
                    }
                    break;
                }

                if (++keyCount > 3)
                {
                    subMenuCount = 6;
                    keyCount = 0;
                    keyNumber = "MSSRKJS";
                    lcd_cursorBlink(5, 1);
                }
                else
                {
                    lcd_cursorBlink((0 + keyCount), 1);
                }
            }
        }
        else if (subMenuCount == 6)
        {
            keyTes = false;
            keySts = false;

            if (key == '1')
            {
                keyTes = true;
                keySts = true;
            }
            else if (key == '0')
            {
                keyTes = false;
                keySts = true;
            }
            if (keySts)
            {

                // cek key 0/1
                //         "0123456789012345"
                // line1 = "Siram J1 14:20 +";
                // line2 = "1234 MSSRKJSM 05";

                switch (keyCount)
                {
                case 0:
                    if (keyTes)
                    {
                        // minggu
                        lcd_print(5, 1, "M");
                        hariTemp = "1,";
                        jadwalRawTemp[5] = 1;
                    }
                    else
                    {
                        lcd_print(5, 1, "-");
                        hariTemp = "0,";
                        jadwalRawTemp[5] = 0;
                    }

                    break;

                case 1:
                    if (keyTes)
                    {
                        lcd_print(6, 1, "S");
                        hariTemp += "1,";
                        jadwalRawTemp[6] = 1;
                    }
                    else
                    {
                        lcd_print(6, 1, "-");
                        hariTemp += "0,";
                        jadwalRawTemp[6] = 0;
                    }

                    break;
                case 2:
                    if (keyTes)
                    {
                        lcd_print(7, 1, "S");
                        hariTemp += "1,";
                        jadwalRawTemp[7] = 1;
                    }
                    else
                    {
                        lcd_print(7, 1, "-");
                        hariTemp += "0,";
                        jadwalRawTemp[7] = 0;
                    }

                    break;

                case 3:
                    if (keyTes)
                    {
                        lcd_print(8, 1, "R");
                        hariTemp += "1,";
                        jadwalRawTemp[8] = 1;
                    }
                    else
                    {
                        lcd_print(8, 1, "-");
                        hariTemp += "0,";
                        jadwalRawTemp[8] = 0;
                    }
                    break;

                case 4:
                    if (keyTes)
                    {
                        lcd_print(9, 1, "K");
                        hariTemp += "1,";
                        jadwalRawTemp[9] = 1;
                    }
                    else
                    {
                        lcd_print(9, 1, "-");
                        hariTemp += "0,";
                        jadwalRawTemp[9] = 0;
                    }
                    break;
                case 5:
                    if (keyTes)
                    {
                        lcd_print(10, 1, "J");
                        hariTemp += "1,";
                        jadwalRawTemp[10] = 1;
                    }
                    else
                    {
                        lcd_print(10, 1, "-");
                        hariTemp += "0,";
                        jadwalRawTemp[10] = 0;
                    }
                    break;
                case 6:
                    if (keyTes)
                    {
                        lcd_print(11, 1, "S");
                        hariTemp += "1";
                        jadwalRawTemp[11] = 1;
                    }
                    else
                    {
                        lcd_print(11, 1, "-");
                        hariTemp += "0";
                        jadwalRawTemp[11] = 0;
                    }
                    break;
                }
                if (++keyCount > 6)
                {
                    subMenuCount = 7;
                    keyCount = 0;
                    keyNumber = "";
                    lcd_print(13, 1, "?");
                    lcd_cursorBlink(13, 1);
                    Serial.println("Simpan pestisida/biopest?");
                    //

                    // jadwal
                    jadwalTemp = enableTemp;
                    jadwalTemp += ",";
                    jadwalTemp += String(jenisJadwal);
                    jadwalTemp += ",";
                    jadwalTemp += String(nomerJadwal);
                    jadwalTemp += ",";
                    jadwalTemp += timeTemp;
                    jadwalTemp += ",";
                    jadwalTemp += hariTemp;
                    jadwalTemp += ",";
                    jadwalTemp += lahanTemp;
                    jadwalTemp += ",";
                    jadwalTemp += dosisAirTemp;
                    jadwalTemp += ",";
                    jadwalTemp += dosisPestTemp;
                    jadwalTemp += ",";
                    jadwalTemp += durasiTemp;

                    // for (uint8_t i = 0; i < 19; i++)
                    //{
                    //     Serial.print(jadwalRawTemp[i]);
                    //     Serial.print(",");
                    // }
                    // Serial.println();
                    //         "0123456789012345"
                    //  line1 = "Simpan J1 Siram?";
                    //  line1 = "Simpan J1 Pestd?";
                    //  line1 = "Simpan J1 Biops?";
                    jadwalRawTemp[1] = jenisJadwal;
                    jadwalRawTemp[2] = nomerJadwal;
                    line1 = "Simpan J";
                    if (jenisJadwal == PESTISIDA)
                    {
                        jadwalTempAll = "";

                        if (nomerJadwal == 1)
                        {
                            jadwalTempAll = jadwalTemp;
                            jadwalTempAll += ";";

                            jadwalTempAll += getValue(jadwalPestisida, ';', 1);
                            jadwalTempAll += ";";
                            jadwalTempAll += getValue(jadwalPestisida, ';', 2);
                            jadwalTempAll += ";";
                            jadwalTempAll += getValue(jadwalPestisida, ';', 3);
                            jadwalTempAll += ";";
                            jadwalTempAll += getValue(jadwalPestisida, ';', 4);
                            jadwalTempAll += ";";

                            line1 += "1 Pest? ";
                        }
                        else if (nomerJadwal == 2)
                        {
                            jadwalTempAll = getValue(jadwalPestisida, ';', 0);
                            jadwalTempAll += ";";
                            jadwalTempAll = jadwalTemp;
                            jadwalTempAll += ";";
                            jadwalTempAll += getValue(jadwalPestisida, ';', 2);
                            jadwalTempAll += ";";
                            jadwalTempAll += getValue(jadwalPestisida, ';', 3);
                            jadwalTempAll += ";";
                            jadwalTempAll += getValue(jadwalPestisida, ';', 4);
                            jadwalTempAll += ";";

                            line1 += "2 Pest? ";
                        }

                        else if (nomerJadwal == 3)
                        {
                            jadwalTempAll = getValue(jadwalPestisida, ';', 0);
                            jadwalTempAll += ";";
                            jadwalTempAll += getValue(jadwalPestisida, ';', 1);
                            jadwalTempAll += ";";
                            jadwalTempAll = jadwalTemp;
                            jadwalTempAll += ";";
                            jadwalTempAll += getValue(jadwalPestisida, ';', 3);
                            jadwalTempAll += ";";
                            jadwalTempAll += getValue(jadwalPestisida, ';', 4);
                            jadwalTempAll += ";";

                            line1 += "3 Pest? ";
                        }

                        else if (nomerJadwal == 4)
                        {
                            jadwalTempAll = getValue(jadwalPestisida, ';', 0);
                            jadwalTempAll += ";";
                            jadwalTempAll += getValue(jadwalPestisida, ';', 1);
                            jadwalTempAll += ";";
                            jadwalTempAll += getValue(jadwalPestisida, ';', 2);
                            jadwalTempAll += ";";
                            jadwalTempAll = jadwalTemp;
                            jadwalTempAll += ";";
                            jadwalTempAll += getValue(jadwalPestisida, ';', 4);
                            jadwalTempAll += ";";

                            line1 += "4 Pest? ";
                        }
                        else if (nomerJadwal == 5)
                        {
                            jadwalTempAll = getValue(jadwalPestisida, ';', 0);
                            jadwalTempAll += ";";
                            jadwalTempAll += getValue(jadwalPestisida, ';', 1);
                            jadwalTempAll += ";";
                            jadwalTempAll += getValue(jadwalPestisida, ';', 2);
                            jadwalTempAll += ";";
                            jadwalTempAll += getValue(jadwalPestisida, ';', 3);
                            jadwalTempAll += ";";
                            jadwalTempAll = jadwalTemp;
                            jadwalTempAll += ";";
                            line1 += "5 Pest? ";
                        }
                    }
                    else if (jenisJadwal == 3)
                    {
                        jadwalTempAll = "";

                        if (nomerJadwal == 1)
                        {
                            jadwalTempAll = jadwalTemp;
                            jadwalTempAll += ";";
                            jadwalTempAll += getValue(jadwalBiopest, ';', 1);
                            jadwalTempAll += ";";
                            jadwalTempAll += getValue(jadwalBiopest, ';', 2);
                            jadwalTempAll += ";";
                            jadwalTempAll += getValue(jadwalBiopest, ';', 3);
                            jadwalTempAll += ";";
                            jadwalTempAll += getValue(jadwalBiopest, ';', 4);
                            jadwalTempAll += ";";

                            line1 += "1 Biops?";
                        }
                        else if (nomerJadwal == 2)
                        {
                            jadwalTempAll = getValue(jadwalBiopest, ';', 0);
                            jadwalTempAll += ";";
                            jadwalTempAll = jadwalTemp;
                            jadwalTempAll += ";";
                            jadwalTempAll += getValue(jadwalBiopest, ';', 2);
                            jadwalTempAll += ";";
                            jadwalTempAll += getValue(jadwalBiopest, ';', 3);
                            jadwalTempAll += ";";
                            jadwalTempAll += getValue(jadwalBiopest, ';', 4);
                            jadwalTempAll += ";";

                            line1 += "2 Biops? ";
                        }

                        else if (nomerJadwal == 3)
                        {
                            jadwalTempAll = getValue(jadwalBiopest, ';', 0);
                            jadwalTempAll += ";";
                            jadwalTempAll += getValue(jadwalBiopest, ';', 1);
                            jadwalTempAll += ";";
                            jadwalTempAll = jadwalTemp;
                            jadwalTempAll += ";";
                            jadwalTempAll += getValue(jadwalBiopest, ';', 3);
                            jadwalTempAll += ";";
                            jadwalTempAll += getValue(jadwalBiopest, ';', 4);
                            jadwalTempAll += ";";

                            line1 += "3 Biops? ";
                        }

                        else if (nomerJadwal == 4)
                        {
                            jadwalTempAll = getValue(jadwalBiopest, ';', 0);
                            jadwalTempAll += ";";
                            jadwalTempAll += getValue(jadwalBiopest, ';', 1);
                            jadwalTempAll += ";";
                            jadwalTempAll += getValue(jadwalBiopest, ';', 2);
                            jadwalTempAll += ";";
                            jadwalTempAll = jadwalTemp;
                            jadwalTempAll += ";";
                            jadwalTempAll += getValue(jadwalBiopest, ';', 4);
                            jadwalTempAll += ";";

                            line1 += "4 Biops? ";
                        }
                        else if (nomerJadwal == 5)
                        {
                            jadwalTempAll = getValue(jadwalBiopest, ';', 0);
                            jadwalTempAll += ";";
                            jadwalTempAll += getValue(jadwalBiopest, ';', 1);
                            jadwalTempAll += ";";
                            jadwalTempAll += getValue(jadwalBiopest, ';', 2);
                            jadwalTempAll += ";";
                            jadwalTempAll += getValue(jadwalBiopest, ';', 3);
                            jadwalTempAll += ";";
                            jadwalTempAll = jadwalTemp;
                            jadwalTempAll += ";";

                            line1 += "5 Biops? ";
                        }
                    }
                    //      "0123456789012345"
                }
                else
                {
                    lcd_cursorBlink((5 + keyCount), 1);
                }
            }
        }
        else if (subMenuCount == 7)
        {
            // simpan ?
        }
    }
}

void cekKey(char key)
{
    keyMode = getRunMode();
    //           "012345678901234567890"
    String ftr = "*(start)       #(stop)";
    String txt1 = "";
    switch (key)
    {

    case 'A':
        if (keyMode == STANBY)
        {

#ifdef USE_OLED
            txt1 = "Lahan ";
            if (selenoidLahan1_status)
            {
                txt1 += "1";
            }
            else
            {
                txt1 += " ";
            }

            if (selenoidLahan2_status)
            {
                txt1 += "2";
            }
            else
            {
                txt1 += " ";
            }

            if (selenoidLahan3_status)
            {
                txt1 += "3";
            }
            else
            {
                txt1 += " ";
            }

            if (selenoidLahan4_status)
            {
                txt1 += "4 > ";
            }
            else
            {
                txt1 += "  > ";
            }
            if (getDurasiSiram() < 10)
            {
                txt1 += " ";
            }

            txt1 += String(getDurasiSiram());
            txt1 += " menit";

            oled_update("   Penyiraman  ", txt1, "00:00  0Ltr", ftr);

#else
            String txt1 = "Siram 1234 > ";
            txt1 += String(getDurasiSiram());
            lcd_clear();
            lcd_print(0, 0, txt1);
            lcd_print(0, 1, "*(start) #(stop)");
#endif

            setRunMode(KEY_SET_SIRAM);
            keyMode = KEY_SET_SIRAM;
            menuCount = 0;
            subMenuCount = 0;
            // keyMode = SIRAM;
            keyNumber = "1234";
            // set default semua lahan
            selenoidLahan1_status = true;
            selenoidLahan2_status = true;
            selenoidLahan3_status = true;
            selenoidLahan4_status = true;
        }
        break;

    case 'B':
        if (keyMode == STANBY)
        {
//               "0123456789012345"
#ifdef USE_OLED
            //     "012345678901234567890"
            txt1 = "Lahan 1234 ";
            txt1 += String(getDosisPestisida());
            txt1 += "mL  ";
            txt1 += String(getDosisAirPestisida());
            txt1 += "Ltr ";
            oled_update("   Pestisida  ", txt1, "     ", ftr);
#else
            lcd_print(0, 0, " Mode Pestisida ");
            lcd_print(0, 1, "*(start) #(stop)");
#endif
            setRunMode(KEY_SET_PESTISIDA);
            keyMode = KEY_SET_PESTISIDA;

            selenoidLahan1_pestisida_status = true;
            selenoidLahan2_pestisida_status = true;
            selenoidLahan3_pestisida_status = true;
            selenoidLahan4_pestisida_status = true;

            menuCount = 0;
            keyNumber = "";
        }
        break;

    case 'C':
        if (keyMode == STANBY)
        {
#ifdef USE_OLED
            //     "012345678901234567890"
            txt1 = "Lahan 1234 ";
            txt1 += String(getDosisBiopest());
            txt1 += "mL  ";
            txt1 += String(getDosisAirBiopest());
            txt1 += "Ltr ";
            //          "012345678901234567890"
            oled_update("  Biopestisida  ", txt1, "     ", ftr);
#else
            lcd_print(0, 0, "  Mode Biopest  ");
            lcd_print(0, 1, "*(start) #(stop)");
#endif
            setRunMode(KEY_SET_BIOPEST);
            keyMode = KEY_SET_BIOPEST;
            menuCount = 0;
            keyNumber = "";
        }
        break;
    case 'D':

        if (keyMode == KEY_SET_SIRAM)
        {
            if (menuCount == 0)
            {
                uint8_t dur = getDurasiSiram();
                //      "0123456789012345"

#ifdef USE_OLED

                line1 = "Lahan 1 2 3 4 ";
                line2 = "Durasi: ";
                line2 += String(getDurasiSiram());
                line2 += " menit";
                oled_clear();
                oled_header("    Penyiraman  ");
                oled_font(ArialMT_Plain_16);
                // oled_print(0,24,line1);
                oled_print(0, 18, line1);
                oled_print(0, 36, line2);
                // oled_kotak(0,15,32,18);
                oled_kotak(48, 17, 14, 18);
                oled_update_display();

#else
                line1 = "Siram lahan 1234";
                line2 = "Durasi ";
                if (dur < 10)
                {
                    line2 += "0";
                }
                line2 += dur;
                line2 += " mnt";

                lcd_clear();
                lcd_print(0, 0, line1);
                lcd_print(0, 1, line2);
                lcd_cursorBlink(12, 0);
#endif
                menuCount = 1;
                subMenuCount = 0;
                keyCount = 0;
            }
            else if (menuCount == 1)
            {
                //      "0123456789012345"
                line1 = "Use lengas -    ";
                lcd_clear();

                lcd_print(0, 0, line1);
                lcd_cursorBlink(11, 0);
                keyNumber = "";
                menuCount = 2;
            }
            else if (menuCount == 2)
            {
                // set jadwal1
                // format
                // 0123456789012345
                // Siram J1 18:00 +
                // 1234-MSSRKJS-30

                line1 = jadwal2lcd(1, 1, 1);
                line2 = jadwal2lcd(1, 1, 2);
                lcd_clear();
                lcd_print(0, 0, line1);
                lcd_print(0, 1, line2);
                lcd_cursorBlink(9, 0);
                keyNumber = "";
                keyCount = 0;
                menuCount = 3;
            }
            else if (menuCount == 3)
            {

                line1 = jadwal2lcd(1, 2, 1);
                line2 = jadwal2lcd(1, 2, 2);
                lcd_clear();
                lcd_print(0, 0, line1);
                lcd_print(0, 1, line2);
                lcd_cursorBlink(10, 0);
                menuCount = 4;
                keyNumber = "";
            }
            else if (menuCount == 4)
            {

                line1 = jadwal2lcd(1, 3, 1);
                line2 = jadwal2lcd(1, 3, 2);
                lcd_clear();
                lcd_print(0, 0, line1);
                lcd_print(0, 1, line2);
                lcd_cursorBlink(10, 0);
                menuCount = 5;
            }
            else if (menuCount == 5)
            {
                lcd_stanby();
                menuCount = 0;
                subMenuCount = 0;
                setRunMode(0);
            }
        }
        else if (keyMode == KEY_SET_PESTISIDA)
        {
            if (menuCount == 0)
            {
                //      "0123456789012345"
                line1 = "Semprot lh  1234";
                line2 = "Air "; // 02  pest 20 ";
                if (getDosisAirPestisida() < 10)
                {
                    line2 += "0";
                }
                uint8_t val = getDosisAirPestisida();
                line2 += String(val);
                line2 += ",pest ";
                if (getDosisPestisida() < 10)
                {
                    line2 += "0";
                }
                line2 += String(getDosisPestisida());
                line2 += "  ";
                lcd_print(0, 0, line1);
                lcd_print(0, 1, line2);
                lcd_cursorBlink(12, 0);
                menuCount = 1;
                subMenuCount = 0;
                keyCount = 0;
                keyNumber = "";
            }
            else if (menuCount == 1)
            {
                // jadwal 1
                line1 = jadwal2lcd(2, 1, 1);
                line2 = jadwal2lcd(2, 1, 2);
                lcd_clear();
                lcd_print(0, 0, line1);
                lcd_print(0, 1, line2);
                lcd_cursorBlink(3, 0);
                menuCount = 2;
                keyNumber = "";
                subMenuCount = 0;
                keyCount = 0;
            }
            else if (menuCount == 2)
            {
                // jadwal 2
                line1 = jadwal2lcd(2, 2, 1);
                line2 = jadwal2lcd(2, 2, 2);
                lcd_clear();
                lcd_print(0, 0, line1);
                lcd_print(0, 1, line2);
                lcd_cursorBlink(3, 0);
                menuCount = 3;
                keyNumber = "";
                subMenuCount = 0;
                keyCount = 0;
            }
            else if (menuCount == 3)
            {
                // jadwal 3
                line1 = jadwal2lcd(2, 3, 1);
                line2 = jadwal2lcd(2, 3, 2);
                lcd_clear();
                lcd_print(0, 0, line1);
                lcd_print(0, 1, line2);
                lcd_cursorBlink(3, 0);
                menuCount = 4;
                keyNumber = "";
                subMenuCount = 0;
                keyCount = 0;
            }
            else if (menuCount == 4)
            {
                // set jadwal 3
                //       "0123456789012345"
                //       "10 *(start)     "
                line1 = "Kal pestisida   ";
                line2 = "";

                if (getKalibrasi_pestisida() < 10)
                {
                    line2 += "0";
                }
                line2 += String(getKalibrasi_pestisida());
                //     "0123456789012345"
                line2 += " *(start) 10mL";
                lcd_clear();
                lcd_print(0, 0, line1);
                lcd_print(0, 1, line2);
                lcd_cursorBlink(0, 1);

                menuCount = 5;
                keyNumber = "";
                subMenuCount = 0;
                keyCount = 0;
            }
            else if (menuCount == 5)
            {
                // kalibrasi pestisida
                //      "0123456789012345"
                line1 = "Kal Airpestisida";
                line2 = "";

                if (getKalibrasi_air_pestisida() < 10)
                {
                    line2 += "0";
                }
                line2 += String(getKalibrasi_air_pestisida());
                //       "0123456789012345"
                line2 += " *(start)3Ltr";
                lcd_clear();
                lcd_print(0, 0, line1);
                lcd_print(0, 1, line2);
                lcd_cursorBlink(0, 1);

                menuCount = 6;
                keyNumber = "";
                subMenuCount = 0;
                keyCount = 0;
            }
            else if (menuCount == 6)
            {
                lcd_stanby();
                menuCount = 0;
                subMenuCount = 0;
                setRunMode(0);
            }
        }
        else if (keyMode == KEY_SET_BIOPEST)
        {
            if (menuCount == 0)
            {
                //      "0123456789012345"
                line1 = "Semprot Bio 1234";
                line2 = "Air "; // 02  pest 20 ";
                if (getDosisAirBiopest() < 10)
                {
                    line2 += "0";
                }
                uint8_t val = getDosisAirBiopest();
                line2 += String(val);
                line2 += ",biop ";
                if (getDosisBiopest() < 10)
                {
                    line2 += "0";
                }
                line2 += String(getDosisBiopest());
                line2 += "  ";
                lcd_print(0, 0, line1);
                lcd_print(0, 1, line2);
                lcd_cursorBlink(12, 0);
                menuCount = 1;
                subMenuCount = 0;
                keyCount = 0;
                keyNumber = "";
            }
            else if (menuCount == 1)
            {
                // jadwal 1
                line1 = jadwal2lcd(3, 1, 1);
                line2 = jadwal2lcd(3, 1, 2);
                lcd_clear();
                lcd_print(0, 0, line1);
                lcd_print(0, 1, line2);
                lcd_cursorBlink(3, 0);
                menuCount = 2;
                keyNumber = "";
                subMenuCount = 0;
                keyCount = 0;
            }
            else if (menuCount == 2)
            {
                // jadwal 2
                line1 = jadwal2lcd(3, 2, 1);
                line2 = jadwal2lcd(3, 2, 2);
                lcd_clear();
                lcd_print(0, 0, line1);
                lcd_print(0, 1, line2);
                lcd_cursorBlink(3, 0);
                menuCount = 3;
                keyNumber = "";
                subMenuCount = 0;
                keyCount = 0;
            }
            else if (menuCount == 3)
            {
                // jadwal 3
                line1 = jadwal2lcd(3, 3, 1);
                line2 = jadwal2lcd(3, 3, 2);
                lcd_clear();
                lcd_print(0, 0, line1);
                lcd_print(0, 1, line2);
                lcd_cursorBlink(3, 0);
                menuCount = 4;
                keyNumber = "";
                subMenuCount = 0;
                keyCount = 0;
            }
            else if (menuCount == 4)
            {
                // set jadwal 3
                //       "0123456789012345"
                //       "10 *(start)     "
                line1 = "Kal Biopest     ";
                line2 = "";

                if (getKalibrasi_biopest() < 10)
                {
                    line2 += "0";
                }
                line2 += String(getKalibrasi_biopest());
                //     "0123456789012345"
                line2 += " *(start) 10mL";
                lcd_clear();
                lcd_print(0, 0, line1);
                lcd_print(0, 1, line2);
                lcd_cursorBlink(0, 1);

                menuCount = 5;
                keyNumber = "";
                subMenuCount = 0;
                keyCount = 0;
            }
            else if (menuCount == 5)
            {
                // kalibrasi pestisida
                //      "0123456789012345"
                line1 = "Kal AirBiopest  ";
                line2 = "";

                if (getKalibrasi_air_biopest() < 10)
                {
                    line2 += "0";
                }
                line2 += String(getKalibrasi_air_biopest());
                //       "0123456789012345"
                line2 += " *(start)3Ltr";
                lcd_clear();
                lcd_print(0, 0, line1);
                lcd_print(0, 1, line2);
                lcd_cursorBlink(0, 1);

                menuCount = 6;
                keyNumber = "";
                subMenuCount = 0;
                keyCount = 0;
            }
            else if (menuCount == 6)
            {
                lcd_stanby();
                menuCount = 0;
                subMenuCount = 0;
                setRunMode(0);
            }
        }
        break;

    case '*':
        if (keyMode == KEY_SET_SIRAM)
        {
            if ((!getSiram_status()) && (!getPestisida_status()) && (!getBiopest_status()))
            {
                siramKey_start();
                menuCount = 0;
                //              "0123456789012345"
            }
            // siram_start(SEMUA_LAHAN, getDurasiSiram());
        }
        else if (keyMode == KEY_SET_PESTISIDA)
        {
            if ((menuCount == 0) || (menuCount == 1 && subMenuCount == 3))
            {
                if ((!getSiram_status()) && (!getPestisida_status()) && (!getBiopest_status()))
                {

                    menuCount = 0;
                    prosesPestisidaKey_start();
                    Serial.println("proses pestisida key start");
                }
            }
            else if (menuCount == 5)
            {
                if (subMenuCount == 0)
                {
                    kalibrasiPestisida_start(getKalibrasi_pestisida());
                    kalibrasi_pestisida_start = true;
                    Serial.println("kalibrasi pestisida key start");
                    //      "0123456789012345"
                    line2 = "Tungggu.........";
                    lcd_print(0, 1, line2);
                }
                if (subMenuCount == 1)
                {
                    kalibrasiPestisida_start(kalibrasi_value);
                    kalibrasi_pestisida_start = true;
                    Serial.println("kalibrasi pestisida key start");
                    line2 = "Tungggu.........";
                    lcd_print(0, 1, line2);
                }
            }
            else if (menuCount == 6)
            {
                if (subMenuCount == 0)
                {
                    kalibrasiAirPestisida_start(getKalibrasi_air_pestisida());
                    kalibrasi_air_pestisida_start = true;
                    Serial.println("kalibrasi air pestisida key start");
                    //      "0123456789012345"
                    line2 = "Tungggu.........";
                    lcd_print(0, 1, line2);
                }
                else if (subMenuCount == 1)
                {
                    kalibrasiAirPestisida_start(kalibrasi_value);
                    kalibrasi_air_pestisida_start = true;
                    Serial.println("kalibrasi air pestisida key start");
                    //      "0123456789012345"
                    line2 = "Tungggu.........";
                    lcd_print(0, 1, line2);
                }
            }
        }
        else if (keyMode == KEY_SET_BIOPEST)
        {
            if ((menuCount == 0) || (menuCount == 1 && subMenuCount == 3))
            {
                if ((!getSiram_status()) && (!getPestisida_status()) && (!getBiopest_status()))
                {

                    menuCount = 0;
                    prosesBiopestKey_start();
                    Serial.println("proses biopest key start");
                }
            }
            else if (menuCount == 5)
            {
                if (subMenuCount == 0)
                {
                    kalibrasiBiopest_start(getKalibrasi_biopest());
                    kalibrasi_biopest_start = true;
                    Serial.println("kalibrasi biopest key start");
                    //      "0123456789012345"
                    line2 = "Tungggu.........";
                    lcd_print(0, 1, line2);
                }
                if (subMenuCount == 1)
                {
                    kalibrasiBiopest_start(kalibrasi_value);
                    kalibrasi_biopest_start = true;
                    Serial.println("kalibrasi biopest key start");
                    line2 = "Tungggu.........";
                    lcd_print(0, 1, line2);
                }
            }
            else if (menuCount == 6)
            {
                if (subMenuCount == 0)
                {
                    kalibrasiAirBiopest_start(getKalibrasi_air_biopest());
                    kalibrasi_air_biopest_start = true;
                    Serial.println("kalibrasi air biopest key start");
                    //      "0123456789012345"
                    line2 = "Tungggu.........";
                    lcd_print(0, 1, line2);
                }
                else if (subMenuCount == 1)
                {
                    kalibrasiAirBiopest_start(kalibrasi_value);
                    kalibrasi_air_biopest_start = true;
                    Serial.println("kalibrasi air biopest key start");
                    //      "0123456789012345"
                    line2 = "Tungggu.........";
                    lcd_print(0, 1, line2);
                }
            }
        }
        break;

    case '#':
        // stop runing
        if (keyMode == KEY_SET_SIRAM)
        {

            if ((menuCount == 3) || (menuCount == 4) || (menuCount == 5))
            {
                if (subMenuCount == 7)
                {
                    simpanJadwalSiram(jadwalTempAll);
                    jadwalSiram = jadwalTempAll;
                    for (uint8_t i = 0; i < 19; i++)
                    {
                        if (menuCount == 3)
                        {
                            jadwalSiram1[i] = jadwalRawTemp[i];
                        }
                        else if (menuCount == 4)
                        {
                            jadwalSiram2[i] = jadwalRawTemp[i];
                        }
                        else if (menuCount == 5)
                        {
                            jadwalSiram3[i] = jadwalRawTemp[i];
                        }
                    }

                    kirimKeApp("siram", 0, "jadwalSiram", jadwalTempAll);
                    //            "0123456789012345"
                    lcd_print(0, 0, "  Jadwal Siram  ");
                    lcd_print(0, 1, "----Disimpan----");
                    lcd_timeout = 30; // 3 detik(dicek setiap 100ms)
                    lcd_timeout_sts = true;
                }
            }
            else
            {
                setRunMode(STANBY);
#ifdef USE_OLED
                oled_stanby_mode("---", "00:00");
#else
                lcd_stanby();
#endif
            }
        }
        else if (keyMode == KEY_SET_PESTISIDA)
        {

            if (menuCount == 2)
            {
                if (subMenuCount == 7)
                {

                    simpanJadwalPestisida(jadwalTempAll);
                    jadwalPestisida = jadwalTempAll;
                    for (uint8_t i = 0; i < 19; i++)
                    {

                        jadwalPestisida1[i] = jadwalRawTemp[i];
                    }

                    kirimKeApp("pestisida", 0, "jadwalPestisida", jadwalTempAll);
                    //              "0123456789012345"
                    lcd_print(0, 0, "Jadwal pestisida");
                    lcd_print(0, 1, "----Disimpan----");
                    lcd_timeout = 30; // 3 detik(dicek setiap 100ms)
                    lcd_timeout_sts = true;
                }
                else
                {
                    lcd_stanby();
                    menuCount = 0;
                    subMenuCount = 0;
                    setRunMode(0);
                }
            }
            else if (menuCount == 3)
            {
                if (subMenuCount == 7)
                {

                    simpanJadwalPestisida(jadwalTempAll);
                    jadwalPestisida = jadwalTempAll;
                    for (uint8_t i = 0; i < 19; i++)
                    {

                        jadwalPestisida2[i] = jadwalRawTemp[i];
                    }

                    kirimKeApp("pestisida", 0, "jadwalPestisida", jadwalTempAll);
                    //              "0123456789012345"
                    lcd_print(0, 0, "Jadwal pestisida");
                    lcd_print(0, 1, "----Disimpan----");
                    lcd_timeout = 30; // 3 detik(dicek setiap 100ms)
                    lcd_timeout_sts = true;
                }
                else
                {
                    lcd_stanby();
                    menuCount = 0;
                    subMenuCount = 0;
                    setRunMode(0);
                }
            }
            else if (menuCount == 4)
            {
                if (subMenuCount == 7)
                {

                    simpanJadwalPestisida(jadwalTempAll);
                    jadwalPestisida = jadwalTempAll;
                    for (uint8_t i = 0; i < 19; i++)
                    {
                        jadwalPestisida3[i] = jadwalRawTemp[i];
                    }

                    kirimKeApp("pestisida", 0, "jadwalPestisida", jadwalTempAll);
                    //              "0123456789012345"
                    lcd_print(0, 0, "Jadwal pestisida");
                    lcd_print(0, 1, "----Disimpan----");
                    lcd_timeout = 30; // 3 detik(dicek setiap 100ms)
                    lcd_timeout_sts = true;
                }
                else
                {
                    lcd_stanby();
                    menuCount = 0;
                    subMenuCount = 0;
                    setRunMode(0);
                }
            }
            else if (menuCount == 5)
            {
                if (subMenuCount == 2)
                {
                    simpanKalibrasi_pestisida(kalibrasi_value);
                    Serial.println("Simpan kalibrasi value");
                    //             "0123456789012345"
                    lcd_clear();
                    lcd_print(0, 0, "Simpan kalibrasi");
                    lcd_print(6, 1, String(kalibrasi_value));

                    lcd_timeout = 30;
                    lcd_timeout_sts = true;
                }
                else
                {
                    lcd_stanby();
                    menuCount = 0;
                    subMenuCount = 0;
                    setRunMode(0);
                }
            }
            else if (menuCount == 6)
            {
                if (subMenuCount == 2)
                {
                    simpanKalibrasi_airpestisida(kalibrasi_value);
                    Serial.println("Simpan kalibrasi air value");
                    //             "0123456789012345"
                    lcd_clear();
                    lcd_print(0, 0, "Simpan kalibrasi");
                    lcd_print(6, 1, String(kalibrasi_value));

                    lcd_timeout = 30;
                    lcd_timeout_sts = true;
                }
                else
                {
                    lcd_stanby();
                    menuCount = 0;
                    subMenuCount = 0;
                    setRunMode(0);
                }
            }
            else
            {
                setRunMode(STANBY);
#ifdef USE_OLED
                oled_stanby_mode("---", "00:00");
#else
                lcd_stanby();
#endif
            }
        }
        else if (keyMode == KEY_SET_BIOPEST)
        {
            if (menuCount == 2)
            {
                if (subMenuCount == 7)
                {

                    simpanJadwalBiopest(jadwalTempAll);
                    jadwalBiopest = jadwalTempAll;
                    for (uint8_t i = 0; i < 19; i++)
                    {

                        jadwalBiopest1[i] = jadwalRawTemp[i];
                    }

                    kirimKeApp("biopest", 0, "jadwalBiopest", jadwalTempAll);
                    //              "0123456789012345"
                    lcd_print(0, 0, "Jadwal Biopest  ");
                    lcd_print(0, 1, "----Disimpan----");
                    lcd_timeout = 30; // 3 detik(dicek setiap 100ms)
                    lcd_timeout_sts = true;
                }
                else
                {
                    lcd_stanby();
                    menuCount = 0;
                    subMenuCount = 0;
                    setRunMode(0);
                }
            }
            else if (menuCount == 3)
            {
                if (subMenuCount == 7)
                {

                    simpanJadwalBiopest(jadwalTempAll);
                    jadwalBiopest = jadwalTempAll;
                    for (uint8_t i = 0; i < 19; i++)
                    {

                        jadwalBiopest2[i] = jadwalRawTemp[i];
                    }

                    kirimKeApp("biopest", 0, "jadwalBiopest", jadwalTempAll);
                    //              "0123456789012345"
                    lcd_print(0, 0, "Jadwal Biopest  ");
                    lcd_print(0, 1, "----Disimpan----");
                    lcd_timeout = 30; // 3 detik(dicek setiap 100ms)
                    lcd_timeout_sts = true;
                }
                else
                {
                    lcd_stanby();
                    menuCount = 0;
                    subMenuCount = 0;
                    setRunMode(0);
                }
            }
            else if (menuCount == 4)
            {
                if (subMenuCount == 7)
                {

                    simpanJadwalBiopest(jadwalTempAll);
                    jadwalBiopest = jadwalTempAll;
                    for (uint8_t i = 0; i < 19; i++)
                    {
                        jadwalBiopest3[i] = jadwalRawTemp[i];
                    }

                    kirimKeApp("biopest", 0, "jadwalBiopest", jadwalTempAll);
                    //              "0123456789012345"
                    lcd_print(0, 0, "Jadwal Biopest  ");
                    lcd_print(0, 1, "----Disimpan----");
                    lcd_timeout = 30; // 3 detik(dicek setiap 100ms)
                    lcd_timeout_sts = true;
                }
                else
                {
                    lcd_stanby();
                    menuCount = 0;
                    subMenuCount = 0;
                    setRunMode(0);
                }
            }
            else if (menuCount == 5)
            {
                if (subMenuCount == 2)
                {
                    simpanKalibrasi_biopest(kalibrasi_value);
                    Serial.println("Simpan kalibrasi value");
                    //             "0123456789012345"
                    lcd_clear();
                    lcd_print(0, 0, "Simpan kalibrasi");
                    lcd_print(6, 1, String(kalibrasi_value));

                    lcd_timeout = 30;
                    lcd_timeout_sts = true;
                }
                else
                {
                    lcd_stanby();
                    menuCount = 0;
                    subMenuCount = 0;
                    setRunMode(0);
                }
            }
            else if (menuCount == 6)
            {
                if (subMenuCount == 2)
                {
                    simpanKalibrasi_airbiopest(kalibrasi_value);
                    Serial.println("Simpan kalibrasi air value");
                    //             "0123456789012345"
                    lcd_clear();
                    lcd_print(0, 0, "Simpan kalibrasi");
                    lcd_print(6, 1, String(kalibrasi_value));

                    lcd_timeout = 30;
                    lcd_timeout_sts = true;
                }
                else
                {
                    lcd_stanby();
                    menuCount = 0;
                    subMenuCount = 0;
                    setRunMode(0);
                }
            }
            else
            {
                setRunMode(STANBY);
#ifdef USE_OLED
                oled_stanby_mode("---", "00:00");
#else
                lcd_stanby();
#endif
            }
        }
        else if (keyMode == SIRAM)
        {
            if (getSiram_status())
            {
                siram_stop(SEMUA_LAHAN);
            }
        }
        else if (keyMode == PESTISIDA)
        {
            if (getPestisida_status())
            {
                prosesPestisida_stop();
                Serial.println("Proses pestisida stop by key");
            }
        }
        else if (keyMode == BIOPEST)
        {
            if (getBiopest_status())
            {
                prosesBiopest_stop();
                Serial.println("Proses biopest stop by key");
            }
        }

        break;
    //
    case '0' ... '9':
        keyTes = false;
        keySts = false;

        if (keyMode == KEY_SET_SIRAM)
        {
            if (menuCount == 1)
            {

                /*
                if (subMenuCount == 0)
                {
                    if (key == '1')
                    {
                        keyTes = true;
                        keySts = true;
                    }
                    else if (key == '0')
                    {
                        keyTes = false;
                        keySts = true;
                    }
                    if (keySts)
                    {

                        // cek key 0/1
                        //        "0123456789012345"
                        // line1 = "Siram lahan 1234";
                        // line2 = "Durasi ";
                        switch (keyCount)
                        {
                        case 0:
                            if (keyTes)
                            {
                                selenoidLahan1_status = true;
                            }
                            else
                            {
                                selenoidLahan1_status = false;
                                lcd_print(12, 0, "-");
                            }

                            break;

                        case 1:
                            if (keyTes)
                            {
                                selenoidLahan2_status = true;
                            }
                            else
                            {
                                selenoidLahan2_status = false;
                                lcd_print(13, 0, "-");
                            }

                            break;
                        case 2:
                            if (keyTes)
                            {
                                selenoidLahan3_status = true;
                            }
                            else
                            {
                                selenoidLahan3_status = false;
                                lcd_print(14, 0, "-");
                            }

                            break;

                        case 3:
                            if (keyTes)
                            {
                                selenoidLahan4_status = true;
                            }
                            else
                            {
                                selenoidLahan4_status = false;
                                lcd_print(15, 0, "-");
                            }
                            break;
                        }

                        if (++keyCount > 3)
                        {
                            subMenuCount = 1;
                            keyCount = 0;
                            keyNumber = "";
                            lcd_cursorBlink(7, 1);
                        }
                        else
                        {
                            lcd_cursorBlink((12 + keyCount), 0);
                        }
                    }
                }

                */
                if (subMenuCount == 0)
                {

                    //         "0123456789012345"
                    // line1 = "Semprot lh  1234";
                    // line
                    keyTes = false;
                    keySts = false;
                    switch (keyCount)
                    {
                    case 0:
                        if (key == '1')
                        {
                            selenoidLahan1_status = true;
                            lcd_print(12, 0, "1");
                            keySts = true;
                        }
                        else if (key == '0')
                        {
                            selenoidLahan1_status = false;
                            lcd_print(12, 0, "-");
                            keySts = true;
                        }

                        break;

                    case 1:
                        if (key == '2')
                        {
                            selenoidLahan2_status = true;
                            lcd_print(13, 0, "2");
                            keySts = true;
                        }
                        else if (key == '0')
                        {
                            selenoidLahan2_status = false;
                            lcd_print(13, 0, "-");
                            keySts = true;
                        }

                        break;
                    case 2:
                        if (key == '3')
                        {
                            selenoidLahan3_status = true;
                            lcd_print(14, 0, "3");
                            keySts = true;
                        }
                        else if (key == '0')
                        {
                            selenoidLahan3_status = false;
                            lcd_print(14, 0, "-");
                            keySts = true;
                        }

                        break;

                    case 3:
                        if (key == '4')
                        {
                            selenoidLahan4_status = true;
                            lcd_print(15, 0, "4");
                            keySts = true;
                        }
                        else if (key == '0')
                        {
                            selenoidLahan4_status = false;
                            lcd_print(15, 0, "-");
                            keySts = true;
                        }
                        break;
                    }
                    if (keySts)
                    {
                        // line1 = "Lahan 1 2 3 4 ";
                        line1 = "Lahan ";
                        if (selenoidLahan1_status)
                        {
                            line1 += "1 ";
                        }
                        else
                        {
                            line1 += "  ";
                        }

                        if (selenoidLahan2_status)
                        {
                            line1 += "2 ";
                        }
                        else
                        {
                            line1 += "  ";
                        }

                        if (selenoidLahan3_status)
                        {
                            line1 += "3 ";
                        }
                        else
                        {
                            line1 += "  ";
                        }

                        if (selenoidLahan4_status)
                        {
                            line1 += "4 ";
                        }
                        else
                        {
                            line1 += "  ";
                        }

                        if (++keyCount > 3)
                        {
                            subMenuCount = 1;
                            keyCount = 0;
                            keyNumber = "";
                            lcd_cursorBlink(7, 1);

                            line2 = "Durasi: ";
                            if (getDurasiSiram() < 10)
                            {
                                line2 += " ";
                            }
                            line2 += String(getDurasiSiram());
                            line2 += " menit";
                            oled_clear();
                            oled_header("    Penyiraman  ");
                            oled_font(ArialMT_Plain_16);
                            // oled_print(0,24,line1);
                            oled_print(0, 18, line1);
                            oled_print(0, 36, line2);
                            oled_kotak(50, 35, 64, 18);
                            oled_update_display();
                        }
                        else
                        {
                            lcd_cursorBlink((12 + keyCount), 0);
                            line2 = "Durasi: ";
                            if (getDurasiSiram() < 10)
                            {
                                line2 += " ";
                            }
                            line2 += String(getDurasiSiram());
                            line2 += " menit";
                            oled_clear();
                            oled_header("    Penyiraman  ");
                            oled_font(ArialMT_Plain_16);
                            // oled_print(0,24,line1);
                            oled_print(0, 18, line1);
                            oled_print(0, 36, line2);
                            // oled_kotak(0,15,32,18);
                            if (keyCount == 0)
                            {
                                oled_kotak(48, 17, 14, 18);
                            }
                            else if (keyCount == 1)
                            {
                                oled_kotak(62, 17, 14, 18);
                            }
                            else if (keyCount == 2)
                            {
                                oled_kotak(76, 17, 14, 18);
                            }
                            else if (keyCount == 3)
                            {
                                oled_kotak(90, 17, 14, 18);
                            }
                            oled_update_display();
                        }
                    }
                }

                else if (subMenuCount == 1)
                {
                    keyNumber += key;
                    lcd_print(7, 1, keyNumber);
                    keyCount++;
                    lcd_cursorBlink((keyCount + 7), 1);

                    line2 = "Durasi: ";
                    line2 += keyNumber;
                    line2 += " menit";
                    oled_clear();
                    oled_header("    Penyiraman  ");
                    oled_font(ArialMT_Plain_16);
                    // oled_print(0,24,line1);
                    oled_print(0, 18, line1);
                    oled_print(0, 36, line2);
                    oled_kotak(50, 35, 64, 18);
                    oled_update_display();
                    // Serial.print(keyNumber);
                    if (keyNumber.length() == 2)
                    {
                        if (keyNumber.toInt() < 31)
                        {
                            // simpan sementara durasi
                            simpanDurasiSiram(keyNumber.toInt());
                            keyNumber = "";
                            menuCount = 0;
                            keyCount = 0;

                            txt1 = "Lahan ";
                            if (selenoidLahan1_status)
                            {
                                txt1 += "1";
                            }
                            else
                            {
                                txt1 += " ";
                            }

                            if (selenoidLahan2_status)
                            {
                                txt1 += "2";
                            }
                            else
                            {
                                txt1 += " ";
                            }

                            if (selenoidLahan3_status)
                            {
                                txt1 += "3";
                            }
                            else
                            {
                                txt1 += " ";
                            }

                            if (selenoidLahan4_status)
                            {
                                txt1 += "4 > ";
                            }
                            else
                            {
                                txt1 += "  > ";
                            }
                            if (getDurasiSiram() < 10)
                            {
                                txt1 += " ";
                            }

                            txt1 += String(getDurasiSiram());
                            txt1 += " menit";

                            oled_update("   Penyiraman  ", txt1, "00:00  0Ltr", ftr);

                            // setRunMode(SIRAM);
                            menuCount = 0;
                            keyMode = KEY_SET_SIRAM;
                        }
                        else
                        {
                            keyNumber = "";
                            keyCount = 0;
                            lcd_cursorBlink(7, 1);
                        }
                    }
                }
            }
            else if (menuCount == 2)
            {
                // set use lengas
                if (key == '1')
                {
                }
                else if (key == '0')
                {
                }
            }
            else if (menuCount == 3)
            {
                editJadwal(SIRAM, 1, key);
            }
            else if (menuCount == 4)
            {
                editJadwal(SIRAM, 2, key);
            }
            else if (menuCount == 5)
            {
                editJadwal(SIRAM, 3, key);
            }
        }
        else if (keyMode == KEY_SET_PESTISIDA)
        {
            if (menuCount == 1)
            {

                if (subMenuCount == 0)
                {

                    //         "0123456789012345"
                    // line1 = "Semprot lh  1234";
                    // line
                    keyTes = false;
                    keySts = false;
                    switch (keyCount)
                    {
                    case 0:
                        if (key == '1')
                        {
                            selenoidLahan1_pestisida_status = true;
                            lcd_print(12, 0, "1");
                            keySts = true;
                        }
                        else if (key == '0')
                        {
                            selenoidLahan1_pestisida_status = false;
                            lcd_print(12, 0, "-");
                            keySts = true;
                        }

                        break;

                    case 1:
                        if (key == '2')
                        {
                            selenoidLahan2_pestisida_status = true;
                            lcd_print(13, 0, "2");
                            keySts = true;
                        }
                        else if (key == '0')
                        {
                            selenoidLahan2_pestisida_status = false;
                            lcd_print(13, 0, "-");
                            keySts = true;
                        }

                        break;
                    case 2:
                        if (key == '3')
                        {
                            selenoidLahan3_pestisida_status = true;
                            lcd_print(14, 0, "3");
                            keySts = true;
                        }
                        else if (key == '0')
                        {
                            selenoidLahan3_pestisida_status = false;
                            lcd_print(14, 0, "-");
                            keySts = true;
                        }

                        break;

                    case 3:
                        if (key == '4')
                        {
                            selenoidLahan4_pestisida_status = true;
                            lcd_print(15, 0, "4");
                            keySts = true;
                        }
                        else if (key == '0')
                        {
                            selenoidLahan4_pestisida_status = true;
                            lcd_print(15, 0, "-");
                            keySts = true;
                        }
                        break;
                    }
                    if (keySts)
                    {

                        if (++keyCount > 3)
                        {
                            subMenuCount = 1;
                            keyCount = 0;
                            keyNumber = "";
                            lcd_cursorBlink(4, 1);
                        }
                        else
                        {
                            lcd_cursorBlink((12 + keyCount), 0);
                        }
                    }
                }
                else if (subMenuCount == 1)
                {
                    // set dosis air
                    //         "0123456789012345"
                    // line1 = "Semprot lh  1234";
                    // line2 = "Air 02,pest 20*?";
                    keyNumber += key;
                    lcd_print(4, 1, keyNumber);
                    keyCount++;
                    lcd_cursorBlink((keyCount + 4), 1);
                    // Serial.print(keyNumber);
                    if (keyNumber.length() == 2)
                    {
                        if (keyNumber.toInt() < 21)
                        {
                            setDosisAirPestisida(keyNumber.toInt());
                            simpanDosisAirPestisida(keyNumber.toInt());
                            keyNumber = "";
                            subMenuCount = 2;
                            keyCount = 0;
                            lcd_cursorBlink(12, 1);
                        }
                        else
                        {
                            keyNumber = "";
                            keyCount = 0;
                            lcd_cursorBlink(4, 1);
                        }
                    }
                }
                else if (subMenuCount == 2)
                {
                    // set dosis pestisida
                    //         "0123456789012345"
                    // line1 = "Semprot lh  1234";
                    // line2 = "Air 02,pest 20*?";
                    keyNumber += key;
                    lcd_print(12, 1, keyNumber);
                    keyCount++;
                    lcd_cursorBlink((keyCount + 12), 1);
                    // Serial.print(keyNumber);
                    if (keyNumber.length() == 2)
                    {
                        if (keyNumber.toInt() < 51)
                        {
                            setDosisPestisida(keyNumber.toInt());
                            simpanDosisPestisida(keyNumber.toInt());
                            keyNumber = "";
                            subMenuCount = 3;
                            keyCount = 0;
                            lcd_print(14, 1, "*?");
                            lcd_cursorBlink(15, 1);
                        }
                        else
                        {
                            keyNumber = "";
                            keyCount = 0;
                            lcd_cursorBlink(12, 1);
                        }
                    }
                }
                else if (subMenuCount == 3)
                {
                    // mulai semprot?
                }
            }
            else if (menuCount == 2)
            {
                editJadwal(PESTISIDA, 1, key);
            }
            else if (menuCount == 3)
            {
                editJadwal(PESTISIDA, 2, key);
            }
            else if (menuCount == 4)
            {
                editJadwal(PESTISIDA, 3, key);
                keyNumber = "";
                menuCount = 6;
                subMenuCount = 0;
            }
            else if (menuCount == 5)
            {
                // kalibrasi pestisida
                if (subMenuCount == 0)
                {
                    keyNumber += key;
                    lcd_print(0, 1, keyNumber);
                    keyCount++;
                    lcd_cursorBlink(keyCount, 1);
                    // Serial.print(keyNumber);
                    if (keyNumber.length() == 2)
                    {
                        if (keyNumber.toInt() < 21)
                        {
                            kalibrasi_value = keyNumber.toInt();
                            subMenuCount = 1;
                            //           "0123456789012345"
                            lcd_print(2, 1, " *(start),# ^");
                            lcd_cursorBlink(3, 1);
                        }
                        else
                        {
                            keyNumber = "";
                            keyCount = 0;
                            lcd_cursorBlink(0, 1);
                        }
                    }
                }
                else if (subMenuCount == 1)
                {
                    // mulai kalibrasi?
                }
            }
            else if (menuCount == 6)
            {
                // kalibrasi air pestisida
                // kalibrasi pestisida
                if (subMenuCount == 0)
                {
                    keyNumber += key;
                    lcd_print(0, 1, keyNumber);
                    keyCount++;
                    lcd_cursorBlink(keyCount, 1);
                    // Serial.print(keyNumber);
                    if (keyNumber.length() == 2)
                    {
                        if (keyNumber.toInt() < 21)
                        {
                            kalibrasi_value = keyNumber.toInt();
                            subMenuCount = 1;
                            //           "0123456789012345"
                            lcd_print(2, 1, " *(start)3Ltr");
                            lcd_cursorBlink(3, 1);
                            Serial.print("keyNumber: ");
                            Serial.println(kalibrasi_value);
                        }
                        else
                        {
                            keyNumber = "";
                            keyCount = 0;
                            lcd_cursorBlink(0, 1);
                        }
                    }
                }
                else if (subMenuCount == 1)
                {
                    // mulai kalibrasi?
                }
            }
        }
        else if (keyMode == KEY_SET_BIOPEST)
        {
            if (menuCount == 1)
            {
                if (subMenuCount == 0)
                {

                    //         "0123456789012345"
                    // line1 = "Semprot lh  1234";
                    // line
                    keyTes = false;
                    keySts = false;
                    switch (keyCount)
                    {
                    case 0:
                        if (key == '1')
                        {
                            selenoidLahan1_biopest_status = true;
                            lcd_print(12, 0, "1");
                            keySts = true;
                        }
                        else if (key == '0')
                        {
                            selenoidLahan1_biopest_status = false;
                            lcd_print(12, 0, "-");
                            keySts = true;
                        }

                        break;

                    case 1:
                        if (key == '2')
                        {
                            selenoidLahan2_biopest_status = true;
                            lcd_print(13, 0, "2");
                            keySts = true;
                        }
                        else if (key == '0')
                        {
                            selenoidLahan2_biopest_status = false;
                            lcd_print(13, 0, "-");
                            keySts = true;
                        }

                        break;
                    case 2:
                        if (key == '3')
                        {
                            selenoidLahan3_biopest_status = true;
                            lcd_print(14, 0, "3");
                            keySts = true;
                        }
                        else if (key == '0')
                        {
                            selenoidLahan3_biopest_status = false;
                            lcd_print(14, 0, "-");
                            keySts = true;
                        }

                        break;

                    case 3:
                        if (key == '4')
                        {
                            selenoidLahan4_biopest_status = true;
                            lcd_print(15, 0, "4");
                            keySts = true;
                        }
                        else if (key == '0')
                        {
                            selenoidLahan4_biopest_status = true;
                            lcd_print(15, 0, "-");
                            keySts = true;
                        }
                        break;
                    }
                    if (keySts)
                    {

                        if (++keyCount > 3)
                        {
                            subMenuCount = 1;
                            keyCount = 0;
                            keyNumber = "";
                            lcd_cursorBlink(4, 1);
                        }
                        else
                        {
                            lcd_cursorBlink((12 + keyCount), 0);
                        }
                    }
                }
                else if (subMenuCount == 1)
                {
                    // set dosis air
                    //         "0123456789012345"
                    // line1 = "Semprot lh  1234";
                    // line2 = "Air 02,pest 20*?";
                    keyNumber += key;
                    lcd_print(4, 1, keyNumber);
                    keyCount++;
                    lcd_cursorBlink((keyCount + 4), 1);
                    // Serial.print(keyNumber);
                    if (keyNumber.length() == 2)
                    {
                        if (keyNumber.toInt() < 21)
                        {
                            setDosisAirBiopest(keyNumber.toInt());
                            simpanDosisAirBiopest(keyNumber.toInt());
                            keyNumber = "";
                            subMenuCount = 2;
                            keyCount = 0;
                            lcd_cursorBlink(12, 1);
                        }
                        else
                        {
                            keyNumber = "";
                            keyCount = 0;
                            lcd_cursorBlink(4, 1);
                        }
                    }
                }
                else if (subMenuCount == 2)
                {
                    // set dosis pestisida
                    //         "0123456789012345"
                    // line1 = "Semprot lh  1234";
                    // line2 = "Air 02,pest 20*?";
                    keyNumber += key;
                    lcd_print(12, 1, keyNumber);
                    keyCount++;
                    lcd_cursorBlink((keyCount + 12), 1);
                    // Serial.print(keyNumber);
                    if (keyNumber.length() == 2)
                    {
                        if (keyNumber.toInt() < 51)
                        {
                            setDosisBiopest(keyNumber.toInt());
                            simpanDosisBiopest(keyNumber.toInt());
                            keyNumber = "";
                            subMenuCount = 3;
                            keyCount = 0;
                            lcd_print(14, 1, " ?");
                            lcd_cursorBlink(15, 1);
                        }
                        else
                        {
                            keyNumber = "";
                            keyCount = 0;
                            lcd_cursorBlink(12, 1);
                        }
                    }
                }
                else if (subMenuCount == 3)
                {
                    //
                }
            }
            else if (menuCount == 2)
            {
                editJadwal(BIOPEST, 1, key);
            }
            else if (menuCount == 3)
            {
                editJadwal(BIOPEST, 2, key);
            }
            else if (menuCount == 4)
            {
                editJadwal(BIOPEST, 3, key);
                keyNumber = "";
                menuCount = 6;
                subMenuCount = 0;
            }
            else if (menuCount == 5)
            {
                // kalibrasi pestisida
                if (subMenuCount == 0)
                {
                    keyNumber += key;
                    lcd_print(0, 1, keyNumber);
                    keyCount++;
                    lcd_cursorBlink(keyCount, 1);
                    // Serial.print(keyNumber);
                    if (keyNumber.length() == 2)
                    {
                        if (keyNumber.toInt() < 21)
                        {
                            kalibrasi_value = keyNumber.toInt();
                            subMenuCount = 1;
                            //           "0123456789012345"
                            lcd_print(2, 1, " *(start),# ^");
                            lcd_cursorBlink(3, 1);
                        }
                        else
                        {
                            keyNumber = "";
                            keyCount = 0;
                            lcd_cursorBlink(0, 1);
                        }
                    }
                }
                else if (subMenuCount == 1)
                {
                    // mulai kalibrasi?
                }
            }
            else if (menuCount == 6)
            {
                // kalibrasi air pestisida
                // kalibrasi pestisida
                if (subMenuCount == 0)
                {
                    keyNumber += key;
                    lcd_print(0, 1, keyNumber);
                    keyCount++;
                    lcd_cursorBlink(keyCount, 1);
                    // Serial.print(keyNumber);
                    if (keyNumber.length() == 2)
                    {
                        if (keyNumber.toInt() < 21)
                        {
                            kalibrasi_value = keyNumber.toInt();
                            subMenuCount = 1;
                            //           "0123456789012345"
                            lcd_print(2, 1, " *(start)3Ltr");
                            lcd_cursorBlink(3, 1);
                        }
                        else
                        {
                            keyNumber = "";
                            keyCount = 0;
                            lcd_cursorBlink(0, 1);
                        }
                    }
                }
                else if (subMenuCount == 1)
                {
                    // mulai kalibrasi?
                }
            }
        }

        break;
    }
}

void keypad_loop()
{
    uint32_t now = millis();

    if (now - lastKeyPressed >= 100)
    {
        lastKeyPressed = now;

        if (keyPad.isPressed())
        {
            char ch = keyPad.getChar(); // note we want the translated char
            if (lastKey != ch)
            {
                lastKey = ch;
                repeatCount = 0;
                // Serial.println(ch);
                cekKey(ch);
            }
            else
            {
                if (++repeatCount > 5)
                {
                    repeatCount = 0;
                    lastKey = 'N';
                }
            }
        }

        if (lcd_timeout_sts)
        {
            if (++lcd_timeCount >= lcd_timeout)
            {
                lcd_timeout_sts = false;
                lcd_timeCount = 0;
                setRunMode(STANBY);
                lcd_stanby();
            }
        }
        if (kalibrasi_pestisida_start)
        {
            if (!getKalibrasi_pestisida_status())
            {
                kalibrasi_pestisida_start = false;
                //      "0123456789012345"
                lcd_print(0, 1, "                ");
                line2 = "Simpan ";

                if (kalibrasi_value < 10)
                {
                    line2 += " ";
                }
                line2 += String(kalibrasi_value);
                line2 += " ?";
                lcd_print(0, 1, line2);
                lcd_cursorBlink(11, 1);
                subMenuCount = 2;
            }
        }
        else if (kalibrasi_air_pestisida_start)
        {
            if (getkalibrasi_air_pestisida_finish())
            {
                kalibrasi_air_pestisida_start = false;
                //      "0123456789012345"
                line2 = "Simpan ";
                if (kalibrasi_value < 10)
                {
                    line2 += "0";
                }
                line2 += String(kalibrasi_value);
                line2 += " ?     ";
                lcd_print(0, 1, line2);
                lcd_cursorBlink(10, 1);
                subMenuCount = 2;
            }
            else
            {

                lcd_print(12, 1, String(getTotalLiter()));
            }
        }

        // biopest
        else if (kalibrasi_biopest_start)
        {
            if (!getKalibrasi_biopest_status())
            {
                kalibrasi_biopest_start = false;
                //      "0123456789012345"
                lcd_print(0, 1, "                ");
                line2 = "Simpan ";

                if (kalibrasi_value < 10)
                {
                    line2 += " ";
                }
                line2 += String(kalibrasi_value);
                line2 += " ?";
                lcd_print(0, 1, line2);
                lcd_cursorBlink(11, 1);
                subMenuCount = 2;
            }
        }
        else if (kalibrasi_air_biopest_start)
        {
            if (getkalibrasi_air_biopest_finish())
            {
                kalibrasi_air_biopest_start = false;
                //      "0123456789012345"
                line2 = "Simpan ";
                if (kalibrasi_value < 10)
                {
                    line2 += "0";
                }
                line2 += String(kalibrasi_value);
                line2 += " ?     ";
                lcd_print(0, 1, line2);
                lcd_cursorBlink(10, 1);
                subMenuCount = 2;
            }
            else
            {

                lcd_print(12, 1, String(getTotalLiter()));
            }
        }
    }
}