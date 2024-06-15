#include <Arduino.h>
#include <EEPROM.h>
#include <eepromHandle.h>
#include <pestisidaHandle.h>
#include <biopestHandle.h>
#include "define.h"

// #define EEPROM_SIZE 2048

#define EE_DURASI_SIRAM 710
#define EE_AMBANG_LENGAS 711
#define EE_USE_LENGAS 712
#define EE_DOSIS_PESTISIDA 713
#define EE_DOSIS_AIRPESTISIDA 714
#define EE_DOSIS_BIOPEST 713
#define EE_DOSIS_AIRBIOPEST 714

#define EE_KALIBRASI_PESTISIDA 715
#define EE_KALIBRASI_AIR_PESTISIDA 716

#define EE_KALIBRASI_BIOPEST 717
#define EE_KALIBRASI_AIR_BIOPEST 717

#define EE_WIFI_STATUS 719//status penyimpanan
#define EE_WIFI_SSID 720 // 32 char
#define EE_WIFI_PASS 752 // 32 char

#define EE_USER_NAME1 784 // 16 char
#define EE_USER_PASS1 800

#define EE_USER_NAME2 816 // 16 char
#define EE_USER_PASS2 832

#define EE_USER_NAME3 848 // 16 char
#define EE_USER_PASS3 864

#define EE_BROKER_STATUS 869
#define EE_BROKER_URL 870  // 30 char
#define EE_BROKER_PORT 900 // 6 char

#define EE_JADWAL_SIRAM 1000
#define EE_JADWAL_PESTISIDA 1250
#define EE_JADWAL_BIOPEST 1500

String jadwalDefault = "0,1,0,07,00,1,1,1,1,1,1,1,1,1,1,1,02,04,5;0,2,0,11,30,1,1,1,1,1,1,1,1,1,1,1,03,10,3;0,3,0,16,00,1,1,1,1,1,1,1,1,1,1,1,0,0,5;0,4,0,11,30,1,1,1,1,1,1,1,1,1,1,1,0,0,3;0,5,0,16,00,1,1,1,1,1,1,1,1,1,1,1,0,0,5;";

String jadwalSiram;
String jadwalPestisida;
String jadwalBiopest;

uint8_t durasiSiram = 2;
bool useLengas = false;
uint8_t ambangLengas;

uint8_t kalibrasiPestisida;
uint8_t kalibrasiAirPestisida;
uint8_t kalibrasiBiopest;
uint8_t kalibrasiAirBiopest;

uint8_t runMode = 0;

uint8_t jadwalSiram1[19] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t jadwalSiram2[19] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t jadwalSiram3[19] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t jadwalSiram4[19] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t jadwalSiram5[19] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

uint8_t jadwalPestisida1[19] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t jadwalPestisida2[19] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t jadwalPestisida3[19] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t jadwalPestisida4[19] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t jadwalPestisida5[19] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

uint8_t jadwalBiopest1[19] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t jadwalBiopest2[19] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t jadwalBiopest3[19] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t jadwalBiopest4[19] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t jadwalBiopest5[19] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

String userName0 = "abadinet";
String password_user0 = "001122";

/*
    format jadwal sederhana
    text 3 blok jadwal
    enable,idx,jenis,H,M,m,s,s,r,k,j,s,l1,l2,l3,l4,A,P,T;enable,idx,jenis,H,M,s,s,r,k,j,s,m,l1,l2,l3,l4,A,P,T;enable,idx,jenis,H,M,s,s,r,k,j,s,m,l1,l2,l3,l4,A,P,T

*/
void getID()
{
    String chipID = String(ESP.getEfuseMac());
    String k_id = "SP";
    k_id += chipID.substring(chipID.length() - 4, chipID.length());
    Serial.print("kontrolId: ");
    Serial.println(k_id);
    kontrolID = k_id;
    for (uint8_t i = 0; i < k_id.length(); i++)
    {
        hostName[i] = k_id.charAt(i);
    }
}

String jadwal2lcd(uint8_t jadwal, uint8_t no_jw, uint8_t line)
{
    // jadwal 1= siram,2=pestisida,3=biopest
    // output
    // 0123456789012345
    // Siram J1 18:00 +
    // 1234 MSSRKJS 30
    // pestisida
    // P1 18:00 05 20 +
    // 1234 MSSRKJS
    // biopest
    // B1 18:00 05 20 +
    // 1234 MSSRKJS

    String l1 = "";
    String l2 = "";

    if (jadwal == 1)
    {
        if (no_jw == 1)
        {
            l1 = "Siram J1 ";
            if (jadwalSiram1[3] < 10)
            {
                l1 += "0";
            }
            l1 += String(jadwalSiram1[3]); // jam
            l1 += ":";
            if (jadwalSiram1[4] < 10)
            {
                l1 += "0";
            }
            l1 += String(jadwalSiram1[4]);
            l1 += " ";
            if (jadwalSiram1[0] == 1)
            {
                l1 += "+";
            }
            else
            {
                l1 += "-";
            }
            // line2
            if (jadwalSiram1[12] == 1)
            {
                l2 = "1";
            }
            else
            {
                l2 = "-";
            }
            if (jadwalSiram1[13] == 1)
            {
                l2 += "2";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalSiram1[14] == 1)
            {
                l2 += "3";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalSiram1[15] == 1)
            {
                l2 += "4";
            }
            else
            {
                l2 += "-";
            }
            //-
            l2 += " ";
            //------
            if (jadwalSiram1[5] == 1)
            {
                l2 += "M";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalSiram1[6] == 1)
            {
                l2 += "S";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalSiram1[7] == 1)
            {
                l2 += "S";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalSiram1[8] == 1)
            {
                l2 += "R";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalSiram1[9] == 1)
            {
                l2 += "K";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalSiram1[10] == 1)
            {
                l2 += "J";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalSiram1[11] == 1)
            {
                l2 += "S";
            }
            else
            {
                l2 += "-";
            }
            //
            l2 += " ";
            if (jadwalSiram1[18] < 10)
            {
                l1 += "0";
            }
            l2 += String(jadwalSiram1[18]);
        }
        else if (no_jw == 2)
        {
            l1 = "Siram J2 ";
            if (jadwalSiram2[3] < 10)
            {
                l1 += "0";
            }
            l1 += String(jadwalSiram2[3]); // jam
            l1 += ":";
            if (jadwalSiram2[4] < 10)
            {
                l1 += "0";
            }
            l1 += String(jadwalSiram2[4]);
            l1 += " ";
            if (jadwalSiram2[0] == 1)
            {
                l1 += "+";
            }
            else
            {
                l1 += "-";
            }
            // line2
            if (jadwalSiram2[12] == 1)
            {
                l2 = "1";
            }
            else
            {
                l2 = "-";
            }
            if (jadwalSiram2[13] == 1)
            {
                l2 += "2";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalSiram2[14] == 1)
            {
                l2 += "3";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalSiram2[15] == 1)
            {
                l2 += "4";
            }
            else
            {
                l2 += "-";
            }
            //-
            l2 += " ";
            //------
            if (jadwalSiram2[5] == 1)
            {
                l2 += "M";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalSiram2[6] == 1)
            {
                l2 += "S";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalSiram2[7] == 1)
            {
                l2 += "S";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalSiram2[8] == 1)
            {
                l2 += "R";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalSiram2[9] == 1)
            {
                l2 += "K";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalSiram2[10] == 1)
            {
                l2 += "J";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalSiram2[11] == 1)
            {
                l2 += "S";
            }
            else
            {
                l2 += "-";
            }
            //
            l2 += " ";
            if (jadwalSiram2[18] < 10)
            {
                l1 += "0";
            }
            l2 += String(jadwalSiram2[18]);
        }
        else if (no_jw == 3)
        {
            l1 = "Siram J3 ";
            if (jadwalSiram3[3] < 10)
            {
                l1 += "0";
            }
            l1 += String(jadwalSiram3[3]); // jam
            l1 += ":";
            if (jadwalSiram3[4] < 10)
            {
                l1 += "0";
            }
            l1 += String(jadwalSiram3[4]);
            l1 += " ";
            if (jadwalSiram3[0] == 1)
            {
                l1 += "+";
            }
            else
            {
                l1 += "-";
            }
            // line2
            if (jadwalSiram3[12] == 1)
            {
                l2 = "1";
            }
            else
            {
                l2 = "-";
            }
            if (jadwalSiram3[13] == 1)
            {
                l2 += "2";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalSiram3[14] == 1)
            {
                l2 += "3";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalSiram3[15] == 1)
            {
                l2 += "4";
            }
            else
            {
                l2 += "-";
            }
            //-
            l2 += " ";
            //------
            if (jadwalSiram3[5] == 1)
            {
                l2 += "M";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalSiram3[6] == 1)
            {
                l2 += "S";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalSiram3[7] == 1)
            {
                l2 += "S";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalSiram3[8] == 1)
            {
                l2 += "R";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalSiram3[9] == 1)
            {
                l2 += "K";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalSiram3[10] == 1)
            {
                l2 += "J";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalSiram3[11] == 1)
            {
                l2 += "S";
            }
            else
            {
                l2 += "-";
            }
            //
            l2 += " ";
            if (jadwalSiram3[18] < 10)
            {
                l1 += "0";
            }
            l2 += String(jadwalSiram3[18]);
        }
    }
    else if (jadwal == 2)
    {
        if (no_jw == 1)
        {
            //   "0123456789012345"
            //    P1 18:00 05 20 +
            //    1234 MSSRKJS
            l1 = "P1 ";
            if (jadwalPestisida1[3] < 10)
            {
                l1 += "0";
            }
            l1 += String(jadwalPestisida1[3]); // jam
            l1 += ":";
            if (jadwalPestisida1[4] < 10)
            {
                l1 += "0";
            }
            l1 += String(jadwalPestisida1[4]);
            l1 += " ";

            if (jadwalPestisida1[16] < 10)
            {
                l1 += "0";
            }
            l1 += jadwalPestisida1[16];
            l1 += " ";

            if (jadwalPestisida1[17] < 10)
            {
                l1 += "0";
            }
            l1 += jadwalPestisida1[17];
            l1 += " ";

            if (jadwalPestisida1[0] == 1)
            {
                l1 += "+";
            }
            else
            {
                l1 += "-";
            }
            // line2
            if (jadwalPestisida1[12] == 1)
            {
                l2 = "1";
            }
            else
            {
                l2 = "-";
            }
            if (jadwalPestisida1[13] == 1)
            {
                l2 += "2";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalPestisida1[14] == 1)
            {
                l2 += "3";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalPestisida1[15] == 1)
            {
                l2 += "4";
            }
            else
            {
                l2 += "-";
            }
            //-
            l2 += " ";
            //------
            if (jadwalPestisida1[5] == 1)
            {
                l2 += "M";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalPestisida1[6] == 1)
            {
                l2 += "S";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalPestisida1[7] == 1)
            {
                l2 += "S";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalPestisida1[8] == 1)
            {
                l2 += "R";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalPestisida1[9] == 1)
            {
                l2 += "K";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalPestisida1[10] == 1)
            {
                l2 += "J";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalPestisida1[11] == 1)
            {
                l2 += "S";
            }
            else
            {
                l2 += "-";
            }
            //
            l2 += "   ";
        }
        else if (no_jw == 2)
        {
            l1 = "P2 ";
            if (jadwalPestisida2[3] < 10)
            {
                l1 += "0";
            }
            l1 += String(jadwalPestisida2[3]); // jam
            l1 += ":";
            if (jadwalPestisida2[4] < 10)
            {
                l1 += "0";
            }
            l1 += String(jadwalPestisida2[4]);
            l1 += " ";

            if (jadwalPestisida2[16] < 10)
            {
                l1 += "0";
            }
            l1 += jadwalPestisida2[16];
            l1 += " ";

            if (jadwalPestisida2[17] < 10)
            {
                l1 += "0";
            }
            l1 += jadwalPestisida2[17];
            l1 += " ";

            if (jadwalPestisida2[0] == 1)
            {
                l1 += "+";
            }
            else
            {
                l1 += "-";
            }
            if (jadwalPestisida2[12] == 1)
            {
                l2 = "1";
            }
            else
            {
                l2 = "-";
            }
            if (jadwalPestisida2[13] == 1)
            {
                l2 += "2";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalPestisida2[14] == 1)
            {
                l2 += "3";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalPestisida2[15] == 1)
            {
                l2 += "4";
            }
            else
            {
                l2 += "-";
            }
            //-
            l2 += " ";
            //------
            if (jadwalPestisida2[5] == 1)
            {
                l2 += "M";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalPestisida2[6] == 1)
            {
                l2 += "S";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalPestisida2[7] == 1)
            {
                l2 += "S";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalPestisida2[8] == 1)
            {
                l2 += "R";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalPestisida2[9] == 1)
            {
                l2 += "K";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalPestisida2[10] == 1)
            {
                l2 += "J";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalPestisida2[11] == 1)
            {
                l2 += "S";
            }
            else
            {
                l2 += "-";
            }
            //
            l2 += "   ";
        }
        else if (no_jw == 3)
        {
            l1 = "P3 ";
            if (jadwalPestisida3[3] < 10)
            {
                l1 += "0";
            }
            l1 += String(jadwalPestisida3[3]); // jam
            l1 += ":";
            if (jadwalPestisida3[4] < 10)
            {
                l1 += "0";
            }
            l1 += String(jadwalPestisida3[4]);
            l1 += " ";

            if (jadwalPestisida3[16] < 10)
            {
                l1 += "0";
            }
            l1 += jadwalPestisida3[16];
            l1 += " ";

            if (jadwalPestisida3[17] < 10)
            {
                l1 += "0";
            }
            l1 += jadwalPestisida3[17];
            l1 += " ";

            if (jadwalPestisida3[0] == 1)
            {
                l1 += "+";
            }
            else
            {
                l1 += "-";
            }

            // line2
            if (jadwalPestisida3[12] == 1)
            {
                l2 = "1";
            }
            else
            {
                l2 = "-";
            }
            if (jadwalPestisida3[13] == 1)
            {
                l2 += "2";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalPestisida3[14] == 1)
            {
                l2 += "3";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalPestisida3[15] == 1)
            {
                l2 += "4";
            }
            else
            {
                l2 += "-";
            }
            //-
            l2 += " ";
            //------
            if (jadwalPestisida3[5] == 1)
            {
                l2 += "M";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalPestisida3[6] == 1)
            {
                l2 += "S";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalPestisida3[7] == 1)
            {
                l2 += "S";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalPestisida3[8] == 1)
            {
                l2 += "R";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalPestisida3[9] == 1)
            {
                l2 += "K";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalPestisida3[10] == 1)
            {
                l2 += "J";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalPestisida3[11] == 1)
            {
                l2 += "S";
            }
            else
            {
                l2 += "-";
            }
            //
            l2 += " ";
            l2 += String(jadwalPestisida3[18]);
        }
    }
    else if (jadwal == 3)
    {
        if (no_jw == 1)
        {
            l1 = "B1 ";
            if (jadwalBiopest1[3] < 10)
            {
                l1 += "0";
            }
            l1 += String(jadwalBiopest1[3]); // jam
            l1 += ":";
            if (jadwalBiopest1[4] < 10)
            {
                l1 += "0";
            }
            l1 += String(jadwalBiopest1[4]);
            l1 += " ";

            if (jadwalBiopest1[16] < 10)
            {
                l1 += "0";
            }
            l1 += jadwalBiopest1[16];
            l1 += " ";

            if (jadwalBiopest1[17] < 10)
            {
                l1 += "0";
            }
            l1 += jadwalBiopest1[17];
            l1 += " ";

            if (jadwalBiopest1[0] == 1)
            {
                l1 += "+";
            }
            else
            {
                l1 += "-";
            }

            // line2
            if (jadwalBiopest1[12] == 1)
            {
                l2 = "1";
            }
            else
            {
                l2 = "-";
            }
            if (jadwalBiopest1[13] == 1)
            {
                l2 += "2";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalBiopest1[14] == 1)
            {
                l2 += "3";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalBiopest1[15] == 1)
            {
                l2 += "4";
            }
            else
            {
                l2 += "-";
            }
            //-
            l2 += " ";
            //------
            if (jadwalBiopest1[5] == 1)
            {
                l2 += "M";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalBiopest1[6] == 1)
            {
                l2 += "S";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalBiopest1[7] == 1)
            {
                l2 += "S";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalBiopest1[8] == 1)
            {
                l2 += "R";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalBiopest1[9] == 1)
            {
                l2 += "K";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalBiopest1[10] == 1)
            {
                l2 += "J";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalBiopest1[11] == 1)
            {
                l2 += "S";
            }
            else
            {
                l2 += "-";
            }
            //
            l2 += "   ";
        }
        else if (no_jw == 2)
        {
            l1 = "B2 ";
            if (jadwalBiopest2[3] < 10)
            {
                l1 += "0";
            }
            l1 += String(jadwalBiopest2[3]); // jam
            l1 += ":";
            if (jadwalBiopest2[4] < 10)
            {
                l1 += "0";
            }
            l1 += String(jadwalBiopest2[4]);
            l1 += " ";

            if (jadwalBiopest2[16] < 10)
            {
                l1 += "0";
            }
            l1 += jadwalBiopest2[16];
            l1 += " ";
            if (jadwalBiopest2[17] < 10)
            {
                l1 += "0";
            }
            l1 += jadwalBiopest2[17];
            l1 += " ";

            if (jadwalBiopest2[0] == 1)
            {
                l1 += "+";
            }
            else
            {
                l1 += "-";
            }

            // line2
            if (jadwalBiopest2[12] == 1)
            {
                l2 = "1";
            }
            else
            {
                l2 = "-";
            }
            if (jadwalBiopest2[13] == 1)
            {
                l2 += "2";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalBiopest2[14] == 1)
            {
                l2 += "3";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalBiopest2[15] == 1)
            {
                l2 += "4";
            }
            else
            {
                l2 += "-";
            }
            //-
            l2 += " ";
            //------
            if (jadwalBiopest2[5] == 1)
            {
                l2 += "M";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalBiopest2[6] == 1)
            {
                l2 += "S";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalBiopest2[7] == 1)
            {
                l2 += "S";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalBiopest2[8] == 1)
            {
                l2 += "R";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalBiopest2[9] == 1)
            {
                l2 += "K";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalBiopest2[10] == 1)
            {
                l2 += "J";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalBiopest2[11] == 1)
            {
                l2 += "S";
            }
            else
            {
                l2 += "-";
            }
            //
            l2 += "   ";
        }
        else if (no_jw == 3)
        {
            l1 = "B3 ";
            if (jadwalBiopest3[3] < 10)
            {
                l1 += "0";
            }
            l1 += String(jadwalBiopest3[3]); // jam
            l1 += ":";
            if (jadwalBiopest3[4] < 10)
            {
                l1 += "0";
            }
            l1 += String(jadwalBiopest3[4]);
            l1 += " ";

            if (jadwalBiopest3[16] < 10)
            {
                l1 += "0";
            }
            l1 += jadwalBiopest3[16];
            l1 += " ";
            if (jadwalBiopest3[17] < 10)
            {
                l1 += "0";
            }
            l1 += jadwalBiopest3[17];
            l1 += " ";
            if (jadwalBiopest3[0] == 1)
            {
                l1 += "+";
            }
            else
            {
                l1 += "-";
            }

            // line2
            if (jadwalBiopest3[12] == 1)
            {
                l2 = "1";
            }
            else
            {
                l2 = "-";
            }
            if (jadwalBiopest3[13] == 1)
            {
                l2 += "2";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalBiopest3[14] == 1)
            {
                l2 += "3";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalBiopest3[15] == 1)
            {
                l2 += "4";
            }
            else
            {
                l2 += "-";
            }
            //-
            l2 += " ";
            //------
            if (jadwalBiopest3[5] == 1)
            {
                l2 += "M";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalBiopest3[6] == 1)
            {
                l2 += "S";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalBiopest3[7] == 1)
            {
                l2 += "S";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalBiopest3[8] == 1)
            {
                l2 += "R";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalBiopest3[9] == 1)
            {
                l2 += "K";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalBiopest3[10] == 1)
            {
                l2 += "J";
            }
            else
            {
                l2 += "-";
            }
            if (jadwalBiopest3[11] == 1)
            {
                l2 += "S";
            }
            else
            {
                l2 += "-";
            }
            //
            l2 += "   ";
        }
    }
    if (line == 1)
    {
        return l1;
    }
    else
    {
        return l2;
    }
}
String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = {0, -1};
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++)
    {
        if (data.charAt(i) == separator || i == maxIndex)
        {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i + 1 : i;
        }
    }

    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
void writeString(int add, String data)
{
    int _size = data.length();
    int i;
    for (i = 0; i < _size; i++)
    {
        EEPROM.write(add + i, data[i]);
    }
    EEPROM.write(add + _size, '\0'); // Add termination null character for String Data
    EEPROM.commit();
}
String readString(int add)
{

    char data[250]; // Max 250 Bytes
    int len = 0;
    unsigned char k;
    k = EEPROM.read(add);
    while (k != '\0' && len < 251) // Read until null character
    {
        k = EEPROM.read(add + len);
        data[len] = k;
        len++;
    }
    data[len] = '\0';
    return String(data);
}
void loadDefault()
{
    loadJadwalSiram();
    loadJadwalPestisida();
    loadJadwalBiopest();
    loadDurasiSiram();
    loadAmbangLengas();
    loadUseLengas();
    loadDosisPestisida();
    loadDosisBiopest();
    loadDosisAirPestisida();
    loadDosisAirBiopest();
    loadKalibrasi();
    getID();
}
// void eeprom_init()
//{
//     EEPROM.begin(EEPROM_SIZE);
//     loadDefault();
// }
void simpanWIFI(String ssid,String pass)
{
    writeString(EE_WIFI_SSID, ssid);
    writeString(EE_WIFI_PASS, pass);
    Serial.print("Simpan Wifi SSID: ");
    Serial.print(ssid);
    Serial.print(" password: ");
    Serial.println(pass);
}


void loadWIFI()
{
    uint8_t sts = EEPROM.read(EE_WIFI_STATUS);
    if(sts != 0xAA){
        EEPROM.write(EE_WIFI_STATUS,0xAA);
        EEPROM.commit();
        writeString(EE_WIFI_SSID,"siprosida");
        writeString(EE_WIFI_SSID,"balingtan123");
        ssid = "siprosida";
        pass = "balingtan123";
        Serial.println("Use default wifi");

    } else{
        ssid = readString(EE_WIFI_SSID);
        pass = readString(EE_WIFI_PASS);
        Serial.print("Wifi SSID: ");
        Serial.print(ssid);
        Serial.print(" password: ");
        Serial.println(pass);
    }
    
}

void loadBroker(){
    uint8_t sts = EEPROM.read(EE_BROKER_STATUS);
    if(sts != 0xAA){
        EEPROM.write(EE_BROKER_STATUS,0xAA);
        EEPROM.commit();
        writeString(EE_BROKER_URL,"mqtt.eclipseprojects.io");
        writeString(EE_BROKER_PORT,"1883");
        mqtt_host = "mqtt.eclipseprojects.io";
        mqtt_port = 1883;
        Serial.println("Use default broker");

    } else{
        mqtt_host = readString(EE_BROKER_URL);
        mqtt_port = readString(EE_BROKER_PORT).toInt();
        Serial.print("Broker: ");
        Serial.print(mqtt_host);
        Serial.print(" port: ");
        Serial.println(String(mqtt_port));
    }
    
}
void simpanBroker(String broker,String pass){
    writeString(EE_WIFI_SSID, broker);
    writeString(EE_WIFI_PASS, pass);
    Serial.print("Simpan broker: ");
    Serial.print(broker);
    Serial.print(" password: ");
    Serial.println(pass);
}

void loadDurasiSiram()
{
    uint8_t dr = EEPROM.read(EE_DURASI_SIRAM);
    if (dr > 60)
    {
        dr = 5;
        EEPROM.write(EE_DURASI_SIRAM, dr);
        EEPROM.commit();
        Serial.println("load default durasi siram 5 menit");
    }
    else
    {
        Serial.print("Durasi Siram ");
        Serial.print(dr);
        Serial.println(" menit");
    }
    durasiSiram = dr;
}
uint8_t getDurasiSiram()
{
    return durasiSiram;
}
void simpanDurasiSiram(uint8_t dur)
{
    durasiSiram = dur;
    EEPROM.write(EE_DURASI_SIRAM, dur);
    EEPROM.commit();
    Serial.print("Simpan durasi siram ");
    Serial.print(dur);
    Serial.println(" menit");
}
void simpanAmbangLengas(uint8_t val)
{
    EEPROM.write(EE_AMBANG_LENGAS, val);
    EEPROM.commit();
    ambangLengas = val;
}
void loadAmbangLengas()
{
    uint8_t al = EEPROM.read(EE_AMBANG_LENGAS);
    if (al > 100)
    {
        ambangLengas = 60;
        EEPROM.write(EE_AMBANG_LENGAS, ambangLengas);
        EEPROM.commit();
    }
    else
    {
        ambangLengas = al;
    }
}
uint8_t getAmbangLengas()
{
    return ambangLengas;
}
void simpanUseLengas(uint8_t ln)
{
    if (ln == 1)
    {
        useLengas = true;
    }
    else
    {
        useLengas = false;
    }
    EEPROM.write(EE_USE_LENGAS, ln);
    EEPROM.commit();
}
uint8_t getRunMode()
{
    return runMode;
}
void setRunMode(uint8_t val)
{
    runMode = val;
}
void loadUseLengas()
{
    uint8_t ul = EEPROM.read(EE_USE_LENGAS);
    if (ul > 1)
    {
        useLengas = false;
        EEPROM.write(EE_USE_LENGAS, 0);
        EEPROM.commit();
    }
    else
    {
        if (ul == 1)
        {
            useLengas = true;
        }
        else
        {
            useLengas = false;
        }
    }
}
void setUseLengas(bool val)
{
    useLengas = val;
}
bool getUseLengas()
{
    return useLengas;
}
void decodeJadwal(String jw, uint8_t out1[19], uint8_t out2[19], uint8_t out3[19], uint8_t out4[19], uint8_t out5[19])
{
    String jadwal123 = getValue(jw, ';', 0);
    for (int i = 0; i < 19; i++)
    {
        out1[i] = (getValue(jadwal123, ',', i)).toInt();
    }
    jadwal123 = getValue(jw, ';', 1);
    for (int i = 0; i < 19; i++)
    {
        out2[i] = (getValue(jadwal123, ',', i)).toInt();
    }
    jadwal123 = getValue(jw, ';', 2);
    for (int i = 0; i < 19; i++)
    {
        out3[i] = (getValue(jadwal123, ',', i)).toInt();
    }
    jadwal123 = getValue(jw, ';', 3);
    for (int i = 0; i < 19; i++)
    {
        out4[i] = (getValue(jadwal123, ',', i)).toInt();
    }
    jadwal123 = getValue(jw, ';', 4);
    for (int i = 0; i < 19; i++)
    {
        out5[i] = (getValue(jadwal123, ',', i)).toInt();
    }
    // tes jadwal1
}
void loadJadwalSiram()
{
    uint8_t e_code = EEPROM.read(EE_JADWAL_SIRAM);
    if (e_code == 0xAA)
    {
        String jw = readString(EE_JADWAL_SIRAM + 1);
        // String jw = jadwalDefault;
        Serial.println("------Jadwal Siram -------");
        Serial.println(jw);
        Serial.println("---------------------------");
        // Serial.print("Jadwal Siram len: ");
        // Serial.println(jw.length());
        jadwalSiram = jw;
        decodeJadwal(jw, jadwalSiram1, jadwalSiram2, jadwalSiram3, jadwalSiram4, jadwalSiram5);
        // cek jadwal
        // String jm = String(jadwalSiram1[3]);
        // jm += ':';
        // jm += String(jadwalSiram1[4]);

        // Serial.print("jam jadwal1: ");
        // Serial.println(jm);
    }
    else
    {
        Serial.println("Jadwal Siram masih kosong,Isi default jadwal");
        jadwalSiram = jadwalDefault;
        simpanJadwalSiram(jadwalDefault);
    }
}
void simpanJadwalSiram(String jadwal)
{
    EEPROM.write(EE_JADWAL_SIRAM, 0xAA); // kode jadwal telah diisi
    EEPROM.commit();
    writeString(EE_JADWAL_SIRAM + 1, jadwal);
    //
    jadwalSiram = jadwal;
    decodeJadwal(jadwal, jadwalSiram1, jadwalSiram2, jadwalSiram3, jadwalSiram4, jadwalSiram5);
}
void loadJadwalPestisida()
{
    uint8_t e_code = EEPROM.read(EE_JADWAL_PESTISIDA);
    if (e_code == 0xAA)
    {
        String jw = readString(EE_JADWAL_PESTISIDA + 1);
        Serial.println("------Jadwal Pestisida -------");
        Serial.println(jw);
        Serial.println("---------------------------");
        jadwalPestisida = jw;
        decodeJadwal(jw, jadwalPestisida1, jadwalPestisida2, jadwalPestisida3, jadwalPestisida4, jadwalPestisida5);
        // cek jadwal
        // String jm = String(jadwalSiram1[3]);
        // jm += ':';
        // jm += String(jadwalSiram1[4]);

        // Serial.print("jam jadwal1: ");
        // Serial.println(jm);
    }
    else
    {
        Serial.println("Jadwal Pestisida masih kosong,simpan default jadwal");
        simpanJadwalPestisida(jadwalDefault);
    }
}
void simpanJadwalPestisida(String jadwal)
{
    EEPROM.write(EE_JADWAL_PESTISIDA, 0xAA); // kode jadwal telah diisi
    EEPROM.commit();
    writeString(EE_JADWAL_PESTISIDA + 1, jadwal);
    //
    jadwalPestisida = jadwal;
    decodeJadwal(jadwal, jadwalPestisida1, jadwalPestisida2, jadwalPestisida3, jadwalPestisida4, jadwalPestisida5);
}
void loadJadwalBiopest()
{
    uint8_t e_code = EEPROM.read(EE_JADWAL_BIOPEST);
    if (e_code == 0xAA)
    {
        String jw = readString(EE_JADWAL_BIOPEST + 1);
        Serial.println("------Jadwal Biopestisida -------");
        Serial.println(jw);
        Serial.println("---------------------------");
        jadwalBiopest = jw;
        decodeJadwal(jw, jadwalBiopest1, jadwalBiopest2, jadwalBiopest3, jadwalBiopest4, jadwalBiopest5);
        // cek jadwal
        // String jm = String(jadwalSiram1[3]);
        // jm += ':';
        // jm += String(jadwalSiram1[4]);

        // Serial.print("jam jadwal1: ");
        // Serial.println(jm);
    }
    else
    {
        Serial.println("Jadwal Biopest masih kosong,simpan default jadwal");
        simpanJadwalBiopest(jadwalDefault);
    }
}
void simpanJadwalBiopest(String jadwal)
{
    EEPROM.write(EE_JADWAL_BIOPEST, 0xAA); // kode jadwal telah diisi
    EEPROM.commit();
    writeString(EE_JADWAL_BIOPEST + 1, jadwal);
    //
    jadwalBiopest = jadwal;
    decodeJadwal(jadwal, jadwalBiopest1, jadwalBiopest2, jadwalBiopest3, jadwalBiopest4, jadwalBiopest5);
}
void simpanDosisPestisida(uint8_t dosis)
{
    if ((dosis > 0) && (dosis < 51))
    {
        EEPROM.write(EE_DOSIS_PESTISIDA, dosis);
        EEPROM.commit();
        setDosisPestisida(dosis);
        Serial.println("Dosis pestisida Disimpan");
    }
    else
    {
        Serial.println("Dosis pestisida diluar 1-100");
    }
}
void simpanDosisAirPestisida(uint8_t dosis)
{
    if ((dosis > 0) && (dosis < 21))
    {
        EEPROM.write(EE_DOSIS_AIRPESTISIDA, dosis);
        EEPROM.commit();
        setDosisAirPestisida(dosis);
        Serial.println("Dosis Air pestisida Disimpan");
    }
    else
    {
        Serial.println("Dosis Air pestisida diluar 1-20");
    }
}
void simpanDosisBiopest(uint8_t dosis)
{
    if ((dosis > 0) && (dosis < 51))
    {
        EEPROM.write(EE_DOSIS_BIOPEST, dosis);
        EEPROM.commit();
        setDosisBiopest(dosis);
        Serial.println("Dosis Biopest Disimpan");
    }
    else
    {
        Serial.println("Dosis Biopest diluar 1-100");
    }
}
void simpanDosisAirBiopest(uint8_t dosis)
{
    if ((dosis > 0) && (dosis < 21))
    {
        EEPROM.write(EE_DOSIS_AIRBIOPEST, dosis);
        EEPROM.commit();
        setDosisAirBiopest(dosis);
        Serial.println("Dosis Air Biopest Disimpan");
    }
    else
    {
        Serial.println("Dosis Air Biopest diluar 1-20");
    }
}
void loadDosisPestisida()
{
    uint8_t d = EEPROM.read(EE_DOSIS_PESTISIDA);
    if (d < 51)
    {
        // load
        setDosisPestisida(d);
    }
    else
    {
        // default dosis 5
        setDosisPestisida(5);
        Serial.println("Load default dosis pestisida 5 ml");
    }
}
void loadDosisAirPestisida()
{
    uint8_t d = EEPROM.read(EE_DOSIS_AIRPESTISIDA);
    if (d < 21)
    {
        // load
        setDosisAirPestisida(d);
    }
    else
    {
        // default dosis 5
        setDosisAirPestisida(5);
        Serial.println("Load default dosis pestisida 5 L");
    }
}
void loadDosisBiopest()
{
    uint8_t d = EEPROM.read(EE_DOSIS_BIOPEST);
    if (d < 51)
    {
        // load
        setDosisBiopest(d);
    }
    else
    {
        // default dosis 5
        setDosisBiopest(5);
        Serial.println("Load default dosis biopestisida 5 ml");
    }
}
void loadDosisAirBiopest()
{
    uint8_t d = EEPROM.read(EE_DOSIS_AIRBIOPEST);
    if (d < 21)
    {
        // load
        setDosisAirBiopest(d);
    }
    else
    {
        // default dosis 5
        setDosisAirBiopest(5);
        Serial.println("Load default dosis Biopest 5 L");
    }
}

void setKalibrasi_pestisida(uint8_t kal)
{
    kalibrasiPestisida = kal;
}

uint8_t getKalibrasi_pestisida()
{
    return kalibrasiPestisida;
}

void setKalibrasi_air_pestisida(uint8_t kal)
{
    kalibrasiAirPestisida = kal;
}

uint8_t getKalibrasi_air_pestisida()
{
    return kalibrasiAirPestisida;
}

void setKalibrasi_biopest(uint8_t kal)
{
    kalibrasiBiopest = kal;
}

uint8_t getKalibrasi_biopest()
{
    return kalibrasiBiopest;
}

void setKalibrasi_air_biopest(uint8_t kal)
{
    kalibrasiAirBiopest = kal;
}

uint8_t getKalibrasi_air_biopest()
{
    return kalibrasiAirBiopest;
}

void simpanKalibrasi_pestisida(uint8_t kal)
{
    kalibrasiPestisida = kal;
    EEPROM.write(EE_KALIBRASI_PESTISIDA, kal);
    EEPROM.commit();

    Serial.print("Simpan kalibrasi pestisida: ");
    Serial.println(kal);
}
void simpanKalibrasi_airpestisida(uint8_t kal)
{
    kalibrasiAirPestisida = kal;
    EEPROM.write(EE_KALIBRASI_AIR_PESTISIDA, kal);
    EEPROM.commit();

    Serial.print("Simpan kalibrasi campuran air pestisida: ");
    Serial.println(kal);
}
void simpanKalibrasi_biopest(uint8_t kal)
{
    kalibrasiBiopest = kal;
    EEPROM.write(EE_KALIBRASI_BIOPEST, kal);
    EEPROM.commit();

    Serial.print("Simpan kalibrasi biopest: ");
    Serial.println(kal);
}
void simpanKalibrasi_airbiopest(uint8_t kal)
{
    kalibrasiAirBiopest = kal;
    EEPROM.write(EE_KALIBRASI_AIR_BIOPEST, kal);
    EEPROM.commit();

    Serial.print("Simpan kalibrasi air biopest: ");
    Serial.println(kal);
}
void loadKalibrasi()
{
    uint8_t kalibrasi = 0;

    // kalibrasi pestisida
    kalibrasi = EEPROM.read(EE_KALIBRASI_PESTISIDA);
    if (kalibrasi > 21)
    {
        // isi default kalibrasi
        kalibrasiPestisida = 10;
        EEPROM.write(EE_KALIBRASI_PESTISIDA, 10);
        EEPROM.commit();
        Serial.println("default kalibrasi pestisida 10");
    }
    else
    {
        kalibrasiPestisida = kalibrasi;
        Serial.print("Kalibrasi pestisida: ");
        Serial.println(kalibrasiPestisida);
    }

    // kalibrasi air pestisida
    kalibrasi = EEPROM.read(EE_KALIBRASI_AIR_PESTISIDA);
    if (kalibrasi > 21)
    {
        // isi default kalibrasi
        kalibrasiAirPestisida = 10;
        EEPROM.write(EE_KALIBRASI_AIR_PESTISIDA, 10);
        EEPROM.commit();
        Serial.println("default kalibrasi air pestisida 10");
    }
    else
    {
        kalibrasiAirPestisida = kalibrasi;
        Serial.print("Kalibrasi air pestisida: ");
        Serial.println(kalibrasiAirPestisida);
    }

    // kalibrasi biopest
    kalibrasi = EEPROM.read(EE_KALIBRASI_BIOPEST);
    if (kalibrasi > 21)
    {
        // isi default kalibrasi
        kalibrasiBiopest = 10;
        EEPROM.write(EE_KALIBRASI_BIOPEST, 10);
        EEPROM.commit();
        Serial.println("default kalibrasi biopest 10");
    }
    else
    {
        kalibrasiBiopest = kalibrasi;
        Serial.print("Kalibrasi biopest: ");
        Serial.println(kalibrasiBiopest);
    }

    // kalibrasi biopest
    kalibrasi = EEPROM.read(EE_KALIBRASI_AIR_BIOPEST);
    if (kalibrasi > 21)
    {
        // isi default kalibrasi
        kalibrasiAirBiopest = 10;
        EEPROM.write(EE_KALIBRASI_AIR_BIOPEST, 10);
        EEPROM.commit();
        Serial.println("default kalibrasi air biopest 10");
    }
    else
    {
        kalibrasiAirBiopest = kalibrasi;
        Serial.print("Kalibrasi air biopest: ");
        Serial.println(kalibrasiAirBiopest);
    }
}
