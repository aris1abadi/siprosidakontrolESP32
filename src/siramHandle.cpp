#include <Ticker.h>
#include <Arduino.h>
#include "aktuatorHandle.h"
#include "mqttHandle.h"
#include "pestisidaHandle.h"
#include "biopestHandle.h"
// #include "sensorFlowHandle.h"
#include <eepromHandle.h>
#include <siramHandle.h>
#include <mySensorsHandle.h>
#include <lcdHandle.h>

bool siram_status = false;
unsigned long siram_delay = 0;
int siram_count = 0;
uint8_t durasiNow = 0;

uint8_t counDownMenit = 0;
uint8_t countDownDetik = 0;

String countDownSiram = "00:00";

extern bool pompaUtama_update;
extern bool pompaSemprot_update;
extern bool pompaPestisida_update;
extern bool pompaBiopest_update;
extern bool pompaAdukPestisida_update;
extern bool pompaAdukBiopest_update;

extern bool selenoidInletPestisida_update;
extern bool selenoidInletBiopest_update;
extern bool selenoidOutletPestisida_update;
extern bool selenoidOutletBiopest_update;

extern bool selenoidLahan1_update;
extern bool selenoidLahan2_update;
extern bool selenoidLahan3_update;
extern bool selenoidLahan4_update;

extern bool pompaUtama_status;
extern bool pompaSemprot_status;
extern bool pompaPestisida_status;
extern bool pompaBiopest_status;
extern bool pompaAdukPestisida_status;
extern bool pompaAdukBiopest_status;

extern bool selenoidInletPestisida_status;
extern bool selenoidInletBiopest_status;

extern bool selenoidOutletPestisida_status;
extern bool selenoidOutletBiopest_status;

extern bool selenoidLahan1_status;
extern bool selenoidLahan2_status;
extern bool selenoidLahan3_status;
extern bool selenoidLahan4_status;

extern uint8_t jadwalSiram1[19];
extern uint8_t jadwalSiram2[19];
extern uint8_t jadwalSiram3[19];
extern uint8_t jadwalSiram4[19];
extern uint8_t jadwalSiram5[19];

extern String jadwalSiram;

extern uint8_t hariNow;
extern uint8_t jamNow;
extern uint8_t menitNow;

extern bool flowRateSensor_update;
extern bool flowRateSensor_status;

bool jadwalSiram_status = false;

void updateLahan_status(uint8_t lahan)
{
    switch (lahan)
    {
    case 0:
        // selenoidLahan1_open();
        // selenoidLahan2_open();
        // selenoidLahan3_open();
        // selenoidLahan4_open();
        selenoidLahan1_update = true;
        selenoidLahan2_update = true;
        selenoidLahan3_update = true;
        selenoidLahan4_update = true;

        selenoidLahan1_status = true;
        selenoidLahan2_status = true;
        selenoidLahan3_status = true;
        selenoidLahan4_status = true;

        break;
    case 1:
        // selenoidLahan1_open();
        selenoidLahan1_update = true;
        selenoidLahan1_status = true;

        break;
    case 2:
        // selenoidLahan2_open();
        selenoidLahan2_update = true;
        selenoidLahan2_status = true;

        break;
    case 3:
        // selenoidLahan3_open();
        selenoidLahan3_update = true;
        selenoidLahan3_status = true;

        break;
    case 4:
        // selenoidLahan4_open();
        selenoidLahan4_update = true;
        selenoidLahan4_status = true;

        break;
    }
    kirimKeApp("siram", 0, "siramStatus", getAllSiram_status());
}

bool getSiram_status()
{
    return siram_status;
}
void setSiram_status(bool sts)
{
    siram_status = sts;
}

bool getSiramLahan1_status()
{
    return selenoidLahan1_status;
}

bool getSiramLahan2_status()
{
    return selenoidLahan2_status;
}

bool getSiramLahan3_status()
{
    return selenoidLahan3_status;
}

bool getSiramLahan4_status()
{
    return selenoidLahan4_status;
}
String getAllSiram_status()
{
    // siram_status,s1,s2,s3,s4,durasiSiram,ambangLengas;jadwal1;jadwal2;jadwal3,jadwal4,jadwal5;
    String sts = "";
    if (siram_status)
    {
        sts += "1,";
    }
    else
    {
        sts += "0,";
    }
    if (selenoidLahan1_status)
    {
        sts += "1,";
    }
    else
    {
        sts += "0,";
    }

    if (selenoidLahan2_status)
    {
        sts += "1,";
    }
    else
    {
        sts += "0,";
    }

    if (selenoidLahan3_status)
    {
        sts += "1,";
    }
    else
    {
        sts += "0,";
    }

    if (selenoidLahan4_status)
    {
        sts += "1,";
    }
    else
    {
        sts += "0,";
    }
    sts += String(getDurasiSiram());
    sts += ",";
    sts += String(getAmbangLengas());
    sts += ";";
    sts += jadwalSiram;

    return sts;
}
void siram_start(uint8_t lahan, uint8_t dur)
{
    if (getPestisida_status())
    {
        Serial.println("Sedang Proses pestisida ");
        kirimKeApp("kontrol", 0, "alert", "Pestisida run");
    }
    else if (getBiopest_status())
    {
        Serial.println("Sedang Proses Biopest ");
        kirimKeApp("kontrol", 0, "alert", "Biopestisida run");
    }
    else if (getSiram_status())
    {
        Serial.println("Sedang Proses Siram ");
        kirimKeApp("kontrol", 0, "alert", "Siram run");
        jadwalSiram_status = false;
        updateLahan_status(lahan);
        kirimKeApp("kontrol", 0, "allStatus", getStatusNow());
    }
    else
    {
        // aktuator_off_semua();
        siram_status = true;
        jadwalSiram_status = false;
        siram_count = 0;
        durasiNow = dur;
        counDownMenit = durasiNow - 1;
        countDownDetik = 59;
        setRunMode(1);
        resetTotalLiter();
        updateLahan_status(lahan);
        kirimKeApp("kontrol", 0, "allStatus", getStatusNow());
        lcd_prosesSiram();
        // nyalakan pompa
        flowRateSensor_status = true;
        flowRateSensor_update = true;
        pompaUtama_status = true;
        pompaUtama_update = true;
        // pompa_start();
    }
}
void siram_stop(uint8_t lahan)
{
    siram_count = 0;

    switch (lahan)
    {
    case 0:
        /* code */
        // aktuator_off_semua();
        ////sensorAir_stop();
        setRunMode(0);
        // setFlowRate_stop();
        flowRateSensor_status = false;
        flowRateSensor_update = true;
        selenoidLahan1_update = true;
        selenoidLahan2_update = true;
        selenoidLahan3_update = true;
        selenoidLahan4_update = true;
        pompaUtama_update = true;
        siram_status = false;
        pompaUtama_status = false;
        selenoidLahan1_status = false;
        selenoidLahan2_status = false;
        selenoidLahan3_status = false;
        selenoidLahan4_status = false;
        lcd_stanby();
        break;
    case 1:
        selenoidLahan1_close();
        selenoidLahan1_status = false;
        selenoidLahan1_update = true;
        if (!selenoidLahan2_status && !selenoidLahan3_status && !selenoidLahan4_status)
        {
            // aktuator_off_semua();
            ////sensorAir_stop();
            pompaUtama_update = true;
            siram_status = false;
            pompaUtama_status = false;
            setRunMode(0);
            // setFlowRate_stop();
            flowRateSensor_status = false;
            flowRateSensor_update = true;
            lahan = 0;
            lcd_stanby();
        }
        break;
    case 2:
        selenoidLahan2_close();
        selenoidLahan2_status = false;
        selenoidLahan2_update = true;
        if (!selenoidLahan1_status && !selenoidLahan3_status && !selenoidLahan4_status)
        {
            // aktuator_off_semua();
            // sensorAir_stop();
            pompaUtama_update = true;
            siram_status = false;
            pompaUtama_status = false;
            setRunMode(0);
            // setFlowRate_stop();
            flowRateSensor_status = false;
            flowRateSensor_update = true;
            lahan = 0;
            lcd_stanby();
        }
        break;
    case 3:
        selenoidLahan3_close();
        selenoidLahan3_status = false;
        selenoidLahan3_update = true;
        if (!selenoidLahan2_status && !selenoidLahan1_status && !selenoidLahan4_status)
        {
            // aktuator_off_semua();
            // sensorAir_stop();
            pompaUtama_update = true;
            siram_status = false;
            pompaUtama_status = false;
            setRunMode(0);
            // setFlowRate_stop();
            flowRateSensor_status = false;
            flowRateSensor_update = true;
            lahan = 0;
            lcd_stanby();
        }
        break;
    case 4:
        selenoidLahan4_close();
        selenoidLahan4_status = false;
        selenoidLahan4_update = true;
        if (!selenoidLahan2_status && !selenoidLahan3_status && !selenoidLahan1_status)
        {
            // aktuator_off_semua();
            // sensorAir_stop();
            pompaUtama_update = true;
            siram_status = false;
            pompaUtama_status = false;
            setRunMode(0);
            // setFlowRate_stop();
            flowRateSensor_status = false;
            flowRateSensor_update = true;
            lahan = 0;
            lcd_stanby();
        }
        break;

    default:
        break;
    }
    // kirimKeApp("siram", 0, "siramStatus", getAllSiram_status());
    kirimKeApp("kontrol", 0, "allStatus", getStatusNow());
}
/*
    format jadwal sederhana
    text 3 blok jadwal
    0     ,1  ,2    ,3,4,5
    enable,idx,jenis,H,M,m,s,s,r,k,j,s,l1,l2,l3,l4,A,P,T;enable,idx,jenis,H,M,s,s,r,k,j,s,m,l1,l2,l3,l4,A,P,T;enable,idx,jenis,H,M,s,s,r,k,j,s,m,l1,l2,l3,l4,A,P,T

*/
void updateCountDown()
{

    countDownSiram = "";
    if ((countDownDetik != 0) && (counDownMenit != 0))
    {
        if (--countDownDetik == 0)
        {
            if (counDownMenit != 0)
            {
                counDownMenit -= 1;
                countDownDetik = 59;
            }
        }
        if (counDownMenit < 10)
        {
            countDownSiram += "0";
        }
        countDownSiram += String(counDownMenit);
        countDownSiram += ":";

        if (countDownDetik < 10)
        {
            countDownSiram += "0";
        }
        countDownSiram += String(countDownDetik);
        countDownSiram += " - ";
        countDownSiram += String(getTotalLiter());
        countDownSiram += " liter";
        lcd_print(0, 1, countDownSiram);
        kirimKeApp("siram", 0, "siramCount", countDownSiram);
    }
}

void cekJadwalSiram()
{
    // cek tiap menit

    // timer1 enable
    if ((jadwalSiram1[0] == 1) && (jadwalSiram1[hariNow + 7] == 1) && (jadwalSiram1[3] == jamNow) && (jadwalSiram1[4] == menitNow))
    {
        // setRunMode(1);

        if (jadwalSiram1[12] == 1)
        {
            selenoidLahan1_status = true;
        }
        else
        {
            selenoidLahan1_status = false;
        }

        if (jadwalSiram1[13] == 1)
        {
            selenoidLahan2_status = true;
        }
        else
        {
            selenoidLahan2_status = false;
        }
        if (jadwalSiram1[14] == 1)
        {
            selenoidLahan3_status = true;
        }
        else
        {
            selenoidLahan3_status = false;
        }
        if (jadwalSiram1[15] == 1)
        {
            selenoidLahan4_status = true;
        }
        else
        {
            selenoidLahan4_status = false;
        }
        jadwalSiram_status = true;
        durasiNow = jadwalSiram1[18];
        siramJadwal_start();
    }

    // timer2 enable
    // pilihan hari aktif
    if ((jadwalSiram2[0] == 1) && (jadwalSiram2[hariNow + 7] == 1) && (jadwalSiram2[3] == jamNow) && (jadwalSiram2[4] == menitNow))
    {
        if (jadwalSiram2[12] == 1)
        {
            selenoidLahan1_status = true;
        }
        else
        {
            selenoidLahan1_status = false;
        }

        if (jadwalSiram2[13] == 1)
        {
            selenoidLahan2_status = true;
        }
        else
        {
            selenoidLahan2_status = false;
        }
        if (jadwalSiram2[14] == 1)
        {
            selenoidLahan3_status = true;
        }
        else
        {
            selenoidLahan3_status = false;
        }
        if (jadwalSiram2[15] == 1)
        {
            selenoidLahan4_status = true;
        }
        else
        {
            selenoidLahan4_status = false;
        }
        // jadwalSiram_status = true;
        durasiNow = jadwalSiram2[18];
        siramJadwal_start();
    }

    // timer3 enable
    // pilihan hari aktif
    if ((jadwalSiram3[0] == 1) && (jadwalSiram3[hariNow + 7] == 1) && (jadwalSiram3[3] == jamNow) && (jadwalSiram3[4] == menitNow))
    {
        if (jadwalSiram3[12] == 1)
        {
            selenoidLahan1_status = true;
        }
        else
        {
            selenoidLahan1_status = false;
        }

        if (jadwalSiram3[13] == 1)
        {
            selenoidLahan2_status = true;
        }
        else
        {
            selenoidLahan2_status = false;
        }
        if (jadwalSiram3[14] == 1)
        {
            selenoidLahan3_status = true;
        }
        else
        {
            selenoidLahan3_status = false;
        }
        if (jadwalSiram3[15] == 1)
        {
            selenoidLahan4_status = true;
        }
        else
        {
            selenoidLahan4_status = false;
        }
        jadwalSiram_status = true;
        durasiNow = jadwalSiram3[18];
        siramJadwal_start();
    }

    // timer4 enable
    // pilihan hari aktif
    if ((jadwalSiram4[0] == 1) && (jadwalSiram4[hariNow + 7] == 1) && (jadwalSiram4[3] == jamNow) && (jadwalSiram4[4] == menitNow))
    {
        if (jadwalSiram4[12] == 1)
        {
            selenoidLahan1_status = true;
        }
        else
        {
            selenoidLahan1_status = false;
        }

        if (jadwalSiram4[13] == 1)
        {
            selenoidLahan2_status = true;
        }
        else
        {
            selenoidLahan2_status = false;
        }
        if (jadwalSiram4[14] == 1)
        {
            selenoidLahan3_status = true;
        }
        else
        {
            selenoidLahan3_status = false;
        }
        if (jadwalSiram4[15] == 1)
        {
            selenoidLahan4_status = true;
        }
        else
        {
            selenoidLahan4_status = false;
        }
        jadwalSiram_status = true;
        durasiNow = jadwalSiram4[18];
        siramJadwal_start();
    }

    // timer5 enable
    // pilihan hari aktif
    if ((jadwalSiram5[0] == 1) && (jadwalSiram5[hariNow + 7] == 1) && (jadwalSiram5[3] == jamNow) && (jadwalSiram5[4] == menitNow))
    {
        if (jadwalSiram5[12] == 1)
        {
            selenoidLahan1_status = true;
        }
        else
        {
            selenoidLahan1_status = false;
        }

        if (jadwalSiram5[13] == 1)
        {
            selenoidLahan2_status = true;
        }
        else
        {
            selenoidLahan2_status = false;
        }
        if (jadwalSiram5[14] == 1)
        {
            selenoidLahan3_status = true;
        }
        else
        {
            selenoidLahan3_status = false;
        }
        if (jadwalSiram5[15] == 1)
        {
            selenoidLahan4_status = true;
        }
        else
        {
            selenoidLahan4_status = false;
        }
        jadwalSiram_status = true;
        durasiNow = jadwalSiram5[18];
        siramJadwal_start();
    }
}
void siramKey_start()
{
    if (!getSiram_status() && !getPestisida_status() && !getBiopest_status())
    {
        siram_status = true;
        siram_count = 0;
        durasiNow = getDurasiSiram();
        counDownMenit = durasiNow - 1;
        countDownDetik = 59;
        updateCountDown();
        setRunMode(1);
        kirimKeApp("kontrol", 0, "allStatus", getStatusNow());
        resetTotalLiter();
        lcd_prosesSiram();
        flowRateSensor_status = true;
        flowRateSensor_update = true;
        pompaUtama_status = true;
        pompaUtama_update = true;

        selenoidLahan1_update = true;
        selenoidLahan2_update = true;
        selenoidLahan3_update = true;
        selenoidLahan4_update = true;
    }
}
void siramJadwal_start()
{
    if (!getSiram_status() && !getPestisida_status() && !getBiopest_status())
    {
        siram_status = true;
        siram_count = 0;
        // durasiNow = 250;
        counDownMenit = durasiNow - 1;
        countDownDetik = 59;
        setRunMode(1);
        kirimKeApp("kontrol", 0, "allStatus", getStatusNow());
        resetTotalLiter();
        lcd_prosesSiram();
        flowRateSensor_status = true;
        flowRateSensor_update = true;
        pompaUtama_status = true;
        pompaUtama_update = true;

        selenoidLahan1_update = true;
        selenoidLahan2_update = true;
        selenoidLahan3_update = true;
        selenoidLahan4_update = true;
    }
}
void siram_loop()
{
    if ((millis() - siram_delay) > 1000)
    {
        siram_delay = millis();
        if (siram_status)
        {
            updateCountDown();
            if (++siram_count >= (durasiNow * 60))
            {
                siram_count = 0;
                siram_stop(0);
            }
        }
    }
}