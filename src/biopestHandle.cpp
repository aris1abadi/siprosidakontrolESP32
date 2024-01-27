#include <Arduino.h>
#include <Ticker.h>
#include "mqttHandle.h"
#include "aktuatorHandle.h"
// #include "sensorFlowHandle.h"
#include "siramHandle.h"
#include "pestisidaHandle.h"
#include <eepromHandle.h>
#include <biopestHandle.h>
#include <mySensorsHandle.h>
#include <lcdHandle.h>

#define ADUK_BIOPEST_TIME   120   //detik
#define ISI_PIPA_TIME       90      //detik pengisian pipa semprot sebelum pestisida

uint8_t dosisBiopest = 0;
uint8_t dosisAirBiopest = 0;
float volumeCampuranBiopest = 0.0;
uint8_t volumeAirBiopest = 0;
float kalibrasiCampuranBiopest = 1.0;

bool prosesIsiBiopest_status = false;
bool prosesIsiAirBiopest_status = false;
bool prosesSemprotBiopest_status = false;
bool biopest_status = false;

bool selenoidLahan1_biopest_status = false;
bool selenoidLahan2_biopest_status = false;
bool selenoidLahan3_biopest_status = false;
bool selenoidLahan4_biopest_status = false;

extern bool pompaUtama_update;
extern bool pompaSemprot_update;
// extern bool pompaPestisida_update;
extern bool pompaBiopest_update;
// extern bool pompaAdukPestisida_update;
extern bool pompaAdukBiopest_update;

// extern bool selenoidInletPestisida_update;
extern bool selenoidInletBiopest_update;
// extern bool selenoidOutletPestisida_update;
extern bool selenoidOutletBiopest_update;

extern bool selenoidLahan1_update;
extern bool selenoidLahan2_update;
extern bool selenoidLahan3_update;
extern bool selenoidLahan4_update;

extern bool pompaUtama_status;
extern bool pompaSemprot_status;
// extern bool pompaPestisida_status;
extern bool pompaBiopest_status;
// extern bool pompaAdukPestisida_status;
extern bool pompaAdukBiopest_status;

// extern bool selenoidInletPestisida_status;
extern bool selenoidInletBiopest_status;

// extern bool selenoidOutletPestisida_status;
extern bool selenoidOutletBiopest_status;

extern bool selenoidLahan1_status;
extern bool selenoidLahan2_status;
extern bool selenoidLahan3_status;
extern bool selenoidLahan4_status;

extern uint8_t jadwalBiopest1[19];
extern uint8_t jadwalBiopest2[19];
extern uint8_t jadwalBiopest3[19];
extern uint8_t jadwalBiopest4[19];
extern uint8_t jadwalBiopest5[19];

extern String jadwalBiopest;

extern uint8_t hariNow;
extern uint8_t jamNow;
extern uint8_t menitNow;

extern uint8_t kalibrasiBiopest;
extern uint8_t kalibrasiAirBiopest;

extern bool flowRateSensor_update;
extern bool flowRateSensor_status;

unsigned long kalibrasiBiopest_time = 0;
unsigned long kalibrasiBiopest_delay = 0;
bool isKalibrasi_biopest = false;
bool isKalibrasi_air_biopest = false;
bool runKalibrasiAir_biopest = false;
bool isKalibrasi_air_biopest_finish = false;

float volume_kalibrasi_air_biopest = 3.00; // 3lter
float ref_kalibrasiAir_biopest = 3.00;

uint8_t biopest_count = 0;
unsigned long biopest_delay = 0;
uint8_t biopestTimer = 0;

// selenoid pada Biopest diganti ke Aktuator 3

Ticker timerBiopest;
// Ticker timerBiopestStop;

bool getBiopest_status()
{
    return biopest_status;
}
void setBiopest_status(bool sts)
{
    biopest_status = sts;
}
float getvolumeCampuranBiopest()
{
    return volumeCampuranBiopest;
}
void setvolumeCampuranBiopest(float vol)
{
    volumeCampuranBiopest = vol;
}
bool getBiopestLahan1_status()
{
    return selenoidLahan1_biopest_status;
}
bool getBiopestLahan2_status()
{
    return selenoidLahan2_biopest_status;
}
bool getBiopestLahan3_status()
{
    return selenoidLahan3_biopest_status;
}
bool getBiopestLahan4_status()
{
    return selenoidLahan4_biopest_status;
}
bool getIsiAirBiopest_status()
{
    return prosesIsiAirBiopest_status;
}
void setIsiAirBiopest_status(bool sts)
{
    prosesIsiAirBiopest_status = sts;
}
void setSemprotBiopest_status(bool sts)
{
    prosesSemprotBiopest_status = sts;
}
bool getSemprotBiopest_status()
{
    return prosesSemprotBiopest_status;
}
void selenoidBiopestUpdate()
{
    // open selenoid lahan
    if (selenoidLahan1_biopest_status)
    {
        // selenoidLahan1_open();
        kirimKeApp("biopest", 1, "lahan", "1");
    }
    else
    {
        // selenoidLahan1_close();
        kirimKeApp("biopest", 1, "lahan", "0");
    }

    if (selenoidLahan2_biopest_status)
    {
        // selenoidLahan2_open();
        kirimKeApp("biopest", 2, "lahan", "1");
    }
    else
    {
        // selenoidLahan2_close();
        kirimKeApp("biopest", 2, "lahan", "0");
    }

    if (selenoidLahan3_biopest_status)
    {
        // selenoidLahan3_open();
        kirimKeApp("biopest", 3, "lahan", "1");
    }
    else
    {
        // selenoidLahan3_close();
        kirimKeApp("biopest", 3, "lahan", "0");
    }

    if (selenoidLahan4_biopest_status)
    {
        // selenoidLahan4_open();
        kirimKeApp("biopest", 4, "lahan", "1");
    }
    else
    {
        // selenoidLahan4_close();
        kirimKeApp("biopest", 4, "lahan", "0");
    }

    selenoidLahan1_status = selenoidLahan1_biopest_status;
    selenoidLahan2_status = selenoidLahan2_biopest_status;
    selenoidLahan3_status = selenoidLahan3_biopest_status;
    selenoidLahan4_status = selenoidLahan4_biopest_status;

    selenoidLahan1_update = true;
    selenoidLahan2_update = true;
    selenoidLahan3_update = true;
    selenoidLahan4_update = true;
}
void lahanBiopestStatus_update(uint8_t lahan)
{
    switch (lahan)
    {
    case 0:
        selenoidLahan1_biopest_status = true;
        selenoidLahan2_biopest_status = true;
        selenoidLahan3_biopest_status = true;
        selenoidLahan4_biopest_status = true;
        break;
    case 1:
        selenoidLahan1_biopest_status = true;
        break;
    case 2:
        selenoidLahan2_biopest_status = true;
        break;
    case 3:
        selenoidLahan3_biopest_status = true;
        break;
    case 4:
        selenoidLahan4_biopest_status = true;
        break;
    }
}
void setDosisBiopest(uint8_t dosis)
{
    dosisBiopest = dosis;
}
uint8_t getDosisBiopest()
{
    return dosisBiopest;
}
void setDosisAirBiopest(uint8_t dosis)
{
    dosisAirBiopest = dosis;
}
float getDosisAirBiopest()
{
    float out = 0.0;
    if (kalibrasiAirBiopest <= 10)
    {
        out = float(dosisAirBiopest) - float(kalibrasiAirBiopest / 10);
    }
    else
    {
        out = float(dosisAirBiopest) + float((kalibrasiAirBiopest - 10) / 10);
    }

    return out;
}
String getAllBiopest_status()
{
    String val = "";
    if (biopest_status)
    {
        val += "1,";
    }
    else
    {
        val += "0,";
    }
    if (selenoidLahan1_biopest_status)
    {
        val += "1,";
    }
    else
    {
        val += "0,";
    }

    if (selenoidLahan2_biopest_status)
    {
        val += "1,";
    }
    else
    {
        val += "0,";
    }

    if (selenoidLahan3_biopest_status)
    {
        val += "1,";
    }
    else
    {
        val += "0,";
    }

    if (selenoidLahan4_biopest_status)
    {
        val += "1,";
    }
    else
    {
        val += "0,";
    }
    val += String(dosisBiopest);
    val += ",";
    val += String(dosisAirBiopest);
    val += ",";
    val += String(kalibrasiBiopest);
    val += ",";
    val += String(kalibrasiAirBiopest);
    val += ";";
    val += jadwalBiopest;
   
    return val;
}
void prosesBiopest_start(uint8_t lahan)
{
    if (getPestisida_status())
    {
        Serial.println("Sedang Proses Pestisida ");
        kirimKeApp("kontrol", 0, "alert", "Sedang Semprot Pestisida");
    }
    else if (getBiopest_status())
    {
        Serial.println("Sedang Proses Biopest ");
        // kirimKeApp("kontrol", 0, "alert", "Sedang Semprot Biopestisida");
        lahanBiopestStatus_update(lahan);
        if (prosesSemprotBiopest_status)
        {
            selenoidBiopestUpdate();
        }
        kirimKeApp("kontrol", 0, "allStatus", getStatusNow());
    }
    else if (getSiram_status())
    {
        Serial.println("Sedang Proses Siram ");
        kirimKeApp("kontrol", 0, "alert", "Sedang Penyiraman");
    }
    else
    {
        lahanBiopestStatus_update(lahan);
        biopest_status = true;
        biopest_count = 0;
        setRunMode(3);
        // 
        kirimKeApp("kontrol", 0, "allStatus", getStatusNow());
        Serial.println("proses biopest start");
        lcd_clear();
        //            "01234567899012345"
        lcd_print(0, 0, "Proses Biopest...");
    }
}

void prosesBiopestKey_start()
{
    if (getPestisida_status())
    {
        Serial.println("Sedang Proses Pestisida ");
        kirimKeApp("kontrol", 0, "alert", "Sedang Semprot Pestisida");
    }
    else if (getBiopest_status())
    {
        Serial.println("Sedang Proses Biopest ");
        // kirimKeApp("kontrol", 0, "alert", "Sedang Semprot Biopestisida");

        kirimKeApp("kontrol", 0, "allStatus", getStatusNow());
    }
    else if (getSiram_status())
    {
        Serial.println("Sedang Proses Siram ");
        kirimKeApp("kontrol", 0, "alert", "Sedang Penyiraman");
    }
    else
    {
        // lahanBiopestStatus_update(lahan);
        biopest_status = true;
        biopest_count = 0;
        setRunMode(3);
        // isi pestisida
        kirimKeApp("kontrol", 0, "allStatus", getStatusNow());
        Serial.println("proses biopest start");
        lcd_clear();
        //            "01234567899012345"
        lcd_print(0, 0, "Proses Biopest...");
    }
}
void prosesBiopest_stop()
{

    // aktuator_off_semua();
    setRunMode(0);
    biopest_count = 0;
    biopest_status = false;
    selenoidLahan1_biopest_status = false;
    selenoidLahan2_biopest_status = false;
    selenoidLahan3_biopest_status = false;
    selenoidLahan4_biopest_status = false;

    aktuator_off_semua();
    // sensorPestisida_stop();
    Serial.println("Proses biopest stop");
    // kirimKeApp("pestisida", 0, "pestisidaStatus", getAllbiopest_status());
    kirimKeApp("kontrol", 0, "allStatus", getStatusNow());
    lcd_stanby();
}
void prosesBuangBiopest_start()
{
}
void prosesBuangBiopest_stop()
{
}

bool getKalibrasi_biopest_status()
{
    return isKalibrasi_biopest;
}
void kalibrasiBiopest_start(uint8_t faktorKal)
{
    setRunMode(3);
    pompaBiopest_status = true;
    pompaBiopest_update = true;
    isKalibrasi_biopest = true;
    kalibrasiBiopest_delay = millis();
    kalibrasiBiopest_time = faktorKal * 1000; //(kal/10)*10000 ==> kal * 1000
    Serial.print("faktor kalibrasi: ");
    Serial.println(faktorKal);
    Serial.print("Kalibarsi time: ");
    Serial.println(kalibrasiBiopest_time);
}

bool getKalibrasi_air_biopest_status()
{
    return isKalibrasi_air_biopest;
}

bool getRunKalibrasi_air_biopest(){
    return runKalibrasiAir_biopest;
}           
void setKalibrasi_air_biopest_status(bool sts)
{
    isKalibrasi_air_biopest = sts;
}


bool getkalibrasi_air_biopest_finish(){
    return isKalibrasi_air_biopest_finish;
}

float getVolumeKalibrasiAir_biopest()
{
    return volume_kalibrasi_air_biopest;
}

void kalibrasiAirBiopest_start(uint8_t kal_value)
{

    //volumeKalibrasiAir_pestisida();
    if (kal_value <= 10)
    {
        volume_kalibrasi_air_biopest = ref_kalibrasiAir_biopest - float(kal_value / 10);
    }
    else
    {
        volume_kalibrasi_air_biopest = ref_kalibrasiAir_biopest + float((kal_value - 10) / 10);
    }

    Serial.print("Volume kalibrasi: ");
    Serial.println(volume_kalibrasi_air_biopest);

    runKalibrasiAir_biopest = true;
    isKalibrasi_air_biopest = true; 
    isKalibrasi_air_biopest_finish = false;   
    flowRateSensor_status = true;
    flowRateSensor_update = true;
    // buka inled
    selenoidInletBiopest_status = true;
    selenoidInletBiopest_update = true;
    // pompa start
    pompaUtama_status = true;
    pompaUtama_update = true;
}
/*
    format jadwal sederhana
    text 3 blok jadwal
    0     ,1  ,2    ,3,4,5
    enable,idx,jenis,H,M,m,s,s,r,k,j,s,l1,l2,l3,l4,A,P,T;enable,idx,jenis,H,M,s,s,r,k,j,s,m,l1,l2,l3,l4,A,P,T;enable,idx,jenis,H,M,s,s,r,k,j,s,m,l1,l2,l3,l4,A,P,T

*/
void cekJadwalBiopest()
{

    // timer1 enable
    if ((jadwalBiopest1[0] == 1) && (jadwalBiopest1[hariNow + 7] == 1) && (jadwalBiopest1[3] == jamNow) && (jadwalBiopest1[4] == menitNow))
    {

        dosisBiopest = jadwalBiopest1[17];
        dosisAirBiopest = jadwalBiopest1[16];
        if (jadwalBiopest1[12] == 1)
        {
            selenoidLahan1_biopest_status = true;
        }
        else
        {
            selenoidLahan1_biopest_status = false;
        }

        if (jadwalBiopest1[13] == 1)
        {
            selenoidLahan2_biopest_status = true;
        }
        else
        {
            selenoidLahan2_biopest_status = false;
        }

        if (jadwalBiopest1[14] == 1)
        {
            selenoidLahan3_biopest_status = true;
        }
        else
        {
            selenoidLahan3_biopest_status = false;
        }

        if (jadwalBiopest1[15] == 1)
        {
            selenoidLahan4_biopest_status = true;
        }
        else
        {
            selenoidLahan4_biopest_status = false;
        }
        biopest_status = true;
        biopest_count = 0;
        prosesBiopestKey_start();
    }

    // timer2 enable
    if ((jadwalBiopest2[0] == 1) && (jadwalBiopest2[hariNow + 7] == 1) && (jadwalBiopest2[3] == jamNow) && (jadwalBiopest2[4] == menitNow))
    {

        dosisBiopest = jadwalBiopest2[17];
        dosisAirBiopest = jadwalBiopest2[16];
        if (jadwalBiopest2[12] == 1)
        {
            selenoidLahan1_biopest_status = true;
        }
        else
        {
            selenoidLahan1_biopest_status = false;
        }

        if (jadwalBiopest2[13] == 1)
        {
            selenoidLahan2_biopest_status = true;
        }
        else
        {
            selenoidLahan2_biopest_status = false;
        }

        if (jadwalBiopest2[14] == 1)
        {
            selenoidLahan3_biopest_status = true;
        }
        else
        {
            selenoidLahan3_biopest_status = false;
        }

        if (jadwalBiopest2[15] == 1)
        {
            selenoidLahan4_biopest_status = true;
        }
        else
        {
            selenoidLahan4_biopest_status = false;
        }
        biopest_status = true;
        biopest_count = 0;
        setRunMode(3);
        prosesBiopestKey_start();
    }

    // timer3 enable
    if ((jadwalBiopest3[0] == 1) && (jadwalBiopest3[hariNow + 7] == 1) && (jadwalBiopest3[3] == jamNow) && (jadwalBiopest3[4] == menitNow))
    {

        dosisBiopest = jadwalBiopest3[17];
        dosisAirBiopest = jadwalBiopest3[16];
        if (jadwalBiopest3[12] == 1)
        {
            selenoidLahan1_biopest_status = true;
        }
        else
        {
            selenoidLahan1_biopest_status = false;
        }

        if (jadwalBiopest3[13] == 1)
        {
            selenoidLahan2_biopest_status = true;
        }
        else
        {
            selenoidLahan2_biopest_status = false;
        }

        if (jadwalBiopest3[14] == 1)
        {
            selenoidLahan3_biopest_status = true;
        }
        else
        {
            selenoidLahan3_biopest_status = false;
        }

        if (jadwalBiopest3[15] == 1)
        {
            selenoidLahan4_biopest_status = true;
        }
        else
        {
            selenoidLahan4_biopest_status = false;
        }
        biopest_status = true;
        biopest_count = 0;
        setRunMode(3);
        prosesBiopestKey_start();
    }

// timer4 enable
    if ((jadwalBiopest4[0] == 1) && (jadwalBiopest4[hariNow + 7] == 1) && (jadwalBiopest4[3] == jamNow) && (jadwalBiopest4[4] == menitNow))
    {

        dosisBiopest = jadwalBiopest4[17];
        dosisAirBiopest = jadwalBiopest4[16];
        if (jadwalBiopest4[12] == 1)
        {
            selenoidLahan1_biopest_status = true;
        }
        else
        {
            selenoidLahan1_biopest_status = false;
        }

        if (jadwalBiopest4[13] == 1)
        {
            selenoidLahan2_biopest_status = true;
        }
        else
        {
            selenoidLahan2_biopest_status = false;
        }

        if (jadwalBiopest4[14] == 1)
        {
            selenoidLahan3_biopest_status = true;
        }
        else
        {
            selenoidLahan3_biopest_status = false;
        }

        if (jadwalBiopest4[15] == 1)
        {
            selenoidLahan4_biopest_status = true;
        }
        else
        {
            selenoidLahan4_biopest_status = false;
        }
        biopest_status = true;
        biopest_count = 0;
        setRunMode(3);
        prosesBiopestKey_start();
    }

// timer5 enable
    if ((jadwalBiopest5[0] == 1) && (jadwalBiopest5[hariNow + 7] == 1) && (jadwalBiopest5[3] == jamNow) && (jadwalBiopest5[4] == menitNow))
    {

        dosisBiopest = jadwalBiopest5[17];
        dosisAirBiopest = jadwalBiopest5[16];
        if (jadwalBiopest5[12] == 1)
        {
            selenoidLahan1_biopest_status = true;
        }
        else
        {
            selenoidLahan1_biopest_status = false;
        }

        if (jadwalBiopest5[13] == 1)
        {
            selenoidLahan2_biopest_status = true;
        }
        else
        {
            selenoidLahan2_biopest_status = false;
        }

        if (jadwalBiopest5[14] == 1)
        {
            selenoidLahan3_biopest_status = true;
        }
        else
        {
            selenoidLahan3_biopest_status = false;
        }

        if (jadwalBiopest5[15] == 1)
        {
            selenoidLahan4_biopest_status = true;
        }
        else
        {
            selenoidLahan4_biopest_status = false;
        }
        biopest_status = true;
        biopest_count = 0;
        setRunMode(3);
        prosesBiopestKey_start();
    }

    
}
void biopest_loop()
{
    if ((millis() - biopest_delay) > 1000)
    {
        biopest_delay = millis();
        if (biopest_status)
        {

            if (biopest_count == 0)
            { // pompa pestisida on
                pompaBiopest_status = true;
                pompaBiopest_update = true;                
                kalibrasiBiopest_delay = millis();
                kalibrasiBiopest_time = kalibrasiBiopest * dosisBiopest * 100; //(kal/10)*10000 ==> kal * 1000

                biopestTimer = 0;
                biopest_count = 1;
                Serial.println("Pompa biopest start");
                //             "0123456789012345"
                String line2 = "Isi Biopest ";
                if (dosisBiopest < 10)
                {
                    line2 += " ";
                }
                line2 += String(dosisBiopest);
                line2 += "mL";
                lcd_print(0, 1, line2);
            }
            else if (biopest_count == 1)
            { // tunggu sampai target
            }
            else if (biopest_count == 2)
            {
                // aktifkan flowsensor
                prosesIsiAirBiopest_status = true;
                resetTotalLiter();
                // setFlowRate_start();
                flowRateSensor_status = true;
                flowRateSensor_update = true;
                Serial.print("Dosis air pestisida: ");
                Serial.println(getDosisAirPestisida());
                biopest_count = 3;

                //             "0123456789012345"
                String line2 = "Isi Air(";
                line2 += String(dosisAirBiopest);
                line2 += ")    ";
                lcd_print(0, 1, line2);
            }
            else if (biopest_count == 3)
            {
                // buka selenoid inled pestisida
                // pompa utama start
                selenoidInletBiopest_status = true;
                selenoidInletBiopest_update = true;

                pompaUtama_status = true;
                pompaUtama_update = true;
                biopest_count = 4;
            }
            else if (biopest_count == 4)
            {
                // tunggu sampai target
                lcd_print(12, 1, String(getTotalLiter()));
                if (!prosesIsiAirBiopest_status)
                {
                    float totalCampuran = getTotalLiter() - kalibrasiCampuranBiopest;
                    setvolumeCampuranPestisida(totalCampuran);
                    // setFlowRate_stop();
                    flowRateSensor_status = false;
                    flowRateSensor_update = true;
                    // pompa utama stop
                    pompaUtama_status = false;
                    pompaUtama_update = true;
                    // tutup selenoid inlet pestisida
                    selenoidInletBiopest_status = false;
                    selenoidInletBiopest_update = true;
                    biopest_count = 5;
                }
            }
            else if (biopest_count == 5)
            {

                // aduk pestisida
                pompaAdukBiopest_status = true;
                pompaAdukBiopest_update = true;
                biopestTimer = 0;
                biopest_count = 6;
                //            "0123456789012345"
                lcd_print(0, 1, "Aduk Biopest....");
            }
            else if (biopest_count == 6)
            {

                uint8_t b_count = (ADUK_BIOPEST_TIME - biopestTimer);
                String cn = "";
                if (b_count < 100)
                {
                    cn += " ";
                }
                if (b_count < 10)
                {
                    cn += " ";
                }
                cn += String(b_count);

                if (++biopestTimer >= ADUK_BIOPEST_TIME)
                {
                    // aduk 2 menit
                    // tunggu sampai target
                    pompaAdukBiopest_status = false;
                    pompaAdukBiopest_update = true;
                    biopest_count = 7;
                }
            }
            else if (biopest_count == 7)
            {
                // buka selenoid lahan
                selenoidLahan1_status = selenoidLahan1_biopest_status;
                selenoidLahan2_status = selenoidLahan2_biopest_status;
                selenoidLahan3_status = selenoidLahan3_biopest_status;
                selenoidLahan4_status = selenoidLahan4_biopest_status;

                selenoidLahan1_update = true;
                selenoidLahan2_update = true;
                selenoidLahan3_update = true;
                selenoidLahan4_update = true;

                biopest_count = 8;
            }
            else if (biopest_count == 8)
            {
                biopest_count = 9;
            }
            else if (biopest_count == 9)
            {
                // nyalakan pompa air untuk dorong
                pompaUtama_status = true;
                pompaUtama_update = true;
                biopest_count = 10;
                biopestTimer = 0;
                //            "0123456789012345"
                lcd_print(0, 1, "IsiAirPipa....");
            }
            else if (biopest_count == 10)
            {
                
                if (++biopestTimer >= ISI_PIPA_TIME)
                {
                    // tunggu sampai saluran penuh(dengan perkiraan timer)
                    // pompa utama stop
                    pompaUtama_status = false;
                    pompaUtama_update = true;
                    selenoidInletBiopest_status = false;
                    selenoidInletBiopest_update = true;
                    biopest_count = 11;
                }
            }
            else if (biopest_count == 11)
            {
                // buka selenoid outled pestisida

                selenoidOutletBiopest_status = true;
                selenoidOutletBiopest_update = true;
                // setFlowRate_start();
                flowRateSensor_status = true;
                flowRateSensor_update = true;
                resetTotalLiter();
                prosesSemprotBiopest_status = true;
                biopest_count = 12;
            }
            else if (biopest_count == 12)
            {
                // pompa semprot start
                pompaSemprot_status = true;
                pompaSemprot_update = true;
                biopest_count = 13;
                //            "0123456789012345"
                lcd_print(0, 1, "Semprot Lahan...");
            }
            else if (biopest_count == 13)
            {
                if (!prosesSemprotBiopest_status)
                {
                    // tunggu sampai target
                    // pompa semprot stop
                    pompaSemprot_status = false;
                    pompaSemprot_update = true;
                    // setFlowRate_stop();
                    flowRateSensor_status = false;
                    flowRateSensor_update = true;
                    biopest_count = 14;
                }
            }
            else if (biopest_count == 14)
            {
                // selenoid outled close
                // selenoid lahan close
                selenoidOutletBiopest_status = false;
                selenoidOutletBiopest_update = true;

                selenoidLahan1_status = false;
                selenoidLahan2_status = false;
                selenoidLahan3_status = false;
                selenoidLahan4_status = false;

                selenoidLahan1_update = true;
                selenoidLahan2_update = true;
                selenoidLahan3_update = true;
                selenoidLahan4_update = true;
                biopest_count = 15;
            }
            else if (biopest_count == 15)
            {
                biopest_count = 16;
            }
            else if (biopest_count == 16)
            {
                //            "0123456789012345"
                lcd_print(0, 1, "Selesai.........");
                biopest_count = 0;
                biopest_status = false;
                aktuator_off_semua();
                setRunMode(0);
                kirimKeApp("kontrol", 0, "allStatus", getStatusNow());
            }
        }
    }

    if (isKalibrasi_biopest)
    { // kalibrasi pestisida
        if ((millis() - kalibrasiBiopest_delay) > kalibrasiBiopest_time)
        {
            kalibrasiBiopest_delay = millis();
            setRunMode(0);
            pompaBiopest_status = false;
            pompaBiopest_update = true;
            isKalibrasi_biopest = false;
            Serial.println("Kalibrasi biopest selesai");
        }
    }
    else if ((biopest_status) && (biopest_count == 1))
    {
        if ((millis() - kalibrasiBiopest_delay) > kalibrasiBiopest_time)
        {
            kalibrasiBiopest_delay = millis();
            pompaBiopest_status = false;
            pompaBiopest_update = true;
            biopest_count = 2;
            Serial.println("Isi biopest selesai");
        }
    }
    else if (runKalibrasiAir_biopest)
    {
        if (!isKalibrasi_air_biopest)
        {
            // simpan kalibrasi?
            runKalibrasiAir_biopest = false;
            flowRateSensor_status = false;
            flowRateSensor_update = true;

            // buka inled
            selenoidInletBiopest_status = false;
            selenoidInletBiopest_update = true;

            // pompa start
            pompaUtama_status = false;
            pompaUtama_update = true;

            isKalibrasi_air_biopest_finish = true;
            Serial.println("kalibrasi air biopest selesai");
        }
    }
}