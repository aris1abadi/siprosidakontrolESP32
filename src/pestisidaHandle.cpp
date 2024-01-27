#include <Arduino.h>
#include <Ticker.h>
#include <mqttHandle.h>
#include <aktuatorHandle.h>
// #include <sensorFlowHandle.h>
#include <siramHandle.h>
#include <biopestHandle.h>
#include <eepromHandle.h>
#include <mySensorsHandle.h>
#include <lcdHandle.h>
#include <keypadHandle.h>

#define PESTISIDA_DEBUG

uint8_t dosisPestisida = 0;
uint8_t dosisAirPestisida = 0;
float volumeCampuranPestisida = 0.0;
uint8_t volumeAirPestisida = 0;
float kalibrasiCampuranPestisida = 1.0;

bool prosesIsiPestisida_status = false;
bool prosesIsiAirPestisida_status = false;
bool prosesSemprotPestisida_status = false;
bool pestisida_status = false;

bool selenoidLahan1_pestisida_status = false;
bool selenoidLahan2_pestisida_status = false;
bool selenoidLahan3_pestisida_status = false;
bool selenoidLahan4_pestisida_status = false;

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

extern String jadwalPestisida;

extern uint8_t jadwalPestisida1[19];
extern uint8_t jadwalPestisida2[19];
extern uint8_t jadwalPestisida3[19];
extern uint8_t jadwalPestisida4[19];
extern uint8_t jadwalPestisida5[19];

extern uint8_t hariNow;
extern uint8_t jamNow;
extern uint8_t menitNow;

extern uint8_t kalibrasiPestisida;
extern uint8_t kalibrasiAirPestisida; // 01 -20

extern bool flowRateSensor_update;
extern bool flowRateSensor_status;

uint8_t pestisida_count = 0;
unsigned long pestisida_delay = 0;
uint8_t pestisidaTimer = 0;

unsigned long kalibrasiPestisida_time = 0;
unsigned long kalibrasiPestisida_delay = 0;
bool isKalibrasi_pestisida = false;
bool isKalibrasi_air_pestisida = false;
bool runKalibrasiAir_pestisida = false;
bool isKalibrasi_air_pestisida_finish = false;

float volume_kalibrasi_air_pestisida = 3.00; // 3lter
float ref_kalibrasiAir_pestisida = 3.00;
// selenoid pada pestisida diganti ke Aktuator 3
// Ticker timerPestisida;
// Ticker timerPestisidaStop;

bool getPestisida_status()
{
    return pestisida_status;
}
void setPestisida_status(bool sts)
{
    pestisida_status = sts;
}
float getvolumeCampuranPestisida()
{
    return volumeCampuranPestisida;
}
void setvolumeCampuranPestisida(float vol)
{
    volumeCampuranPestisida = vol;
}
bool getPestisidaLahan1_status()
{
    return selenoidLahan1_pestisida_status;
}
bool getPestisidaLahan2_status()
{
    return selenoidLahan2_pestisida_status;
}
bool getPestisidaLahan3_status()
{
    return selenoidLahan3_pestisida_status;
}
bool getPestisidaLahan4_status()
{
    return selenoidLahan4_pestisida_status;
}
bool getIsiAirPestisida_status()
{
    return prosesIsiAirPestisida_status;
}
void setIsiAirPestisida_status(bool sts)
{
    prosesIsiAirPestisida_status = sts;
}
void setSemprotPestisida_status(bool sts)
{
    prosesSemprotPestisida_status = sts;
}
bool getSemprotPestisida_status()
{
    return prosesSemprotPestisida_status;
}
void selenoidPestisidaUpdate()
{
    // open selenoid lahan
    if (selenoidLahan1_pestisida_status)
    {
        // selenoidLahan1_open();
        kirimKeApp("pestisida", 1, "lahan", "1");
    }
    else
    {
        // selenoidLahan1_close();
        kirimKeApp("pestisida", 1, "lahan", "0");
    }

    if (selenoidLahan2_pestisida_status)
    {
        // selenoidLahan2_open();
        kirimKeApp("pestisida", 2, "lahan", "1");
    }
    else
    {
        // selenoidLahan2_close();
        kirimKeApp("pestisida", 2, "lahan", "0");
    }

    if (selenoidLahan3_pestisida_status)
    {
        // selenoidLahan3_open();
        kirimKeApp("pestisida", 3, "lahan", "1");
    }
    else
    {
        // selenoidLahan3_close();
        kirimKeApp("pestisida", 3, "lahan", "0");
    }

    if (selenoidLahan4_pestisida_status)
    {
        // selenoidLahan4_open();
        kirimKeApp("pestisida", 4, "lahan", "1");
    }
    else
    {
        // selenoidLahan4_close();
        kirimKeApp("pestisida", 4, "lahan", "0");
    }

    selenoidLahan1_status = selenoidLahan1_pestisida_status;
    selenoidLahan2_status = selenoidLahan2_pestisida_status;
    selenoidLahan3_status = selenoidLahan3_pestisida_status;
    selenoidLahan4_status = selenoidLahan4_pestisida_status;

    selenoidLahan1_update = true;
    selenoidLahan2_update = true;
    selenoidLahan3_update = true;
    selenoidLahan4_update = true;
}
void lahanPestisidaStatus_update(uint8_t lahan)
{
    switch (lahan)
    {
    case 0:
        selenoidLahan1_pestisida_status = true;
        selenoidLahan2_pestisida_status = true;
        selenoidLahan3_pestisida_status = true;
        selenoidLahan4_pestisida_status = true;
        break;
    case 1:
        selenoidLahan1_pestisida_status = true;
        break;
    case 2:
        selenoidLahan2_pestisida_status = true;
        break;
    case 3:
        selenoidLahan3_pestisida_status = true;
        break;
    case 4:
        selenoidLahan4_pestisida_status = true;
        break;
    }
}
void setDosisPestisida(uint8_t dosis)
{
    dosisPestisida = dosis;
}
uint8_t getDosisPestisida()
{
    return dosisPestisida;
}
void setDosisAirPestisida(uint8_t dosis)
{
    dosisAirPestisida = dosis;
}
float getDosisAirPestisida()
{
    float out =  float(dosisAirPestisida * kalibrasiAirPestisida / 10);   
 
    return out;
}
String getAllPestisida_status()
{
    String val = "";
    // pestisida_status,s1,s2,s3,s4,Dosis,dosisAir,kalibrasiPestisida,kalibrasiair pestisida;jadwal1;jadwal2;jadwal3,jadwal4,jadwal5;
    if (pestisida_status)
    {
        val += "1,";
    }
    else
    {
        val += "0,";
    }
    if (selenoidLahan1_pestisida_status)
    {
        val += "1,";
    }
    else
    {
        val += "0,";
    }

    if (selenoidLahan2_pestisida_status)
    {
        val += "1,";
    }
    else
    {
        val += "0,";
    }

    if (selenoidLahan3_pestisida_status)
    {
        val += "1,";
    }
    else
    {
        val += "0,";
    }

    if (selenoidLahan4_pestisida_status)
    {
        val += "1,";
    }
    else
    {
        val += "0,";
    }
    val += String(dosisPestisida);
    val += ",";
    val += String(dosisAirPestisida);
    val += ",";
    val += String(kalibrasiPestisida);
    val += ",";
    val += String(kalibrasiAirPestisida);
    val += ";";    
    val += jadwalPestisida;
    
    return val;
}
void prosesPestisida_start(uint8_t lahan)
{
    if (getPestisida_status())
    {
        Serial.println("Sedang Proses Pestisida ");
        // kirimKeApp("kontrol", 0, "alert", "Sedang Semprot Pestisida");
        lahanPestisidaStatus_update(lahan);
        if (prosesSemprotPestisida_status)
        {
            selenoidPestisidaUpdate();
        }
        kirimKeApp("kontrol", 0, "allStatus", getStatusNow());
    }
    else if (getBiopest_status())
    {
        Serial.println("Sedang Proses Biopest ");
        kirimKeApp("kontrol", 0, "alert", "Sedang Semprot Biopestisida");
    }
    else if (getSiram_status())
    {
        Serial.println("Sedang Proses Siram ");
        kirimKeApp("kontrol", 0, "alert", "Sedang Penyiraman");
    }
    else
    {
        lahanPestisidaStatus_update(lahan);
        pestisida_status = true;
        pestisida_count = 0;
        setRunMode(2);
        // isi pestisida
        kirimKeApp("kontrol", 0, "allStatus", getStatusNow());
        Serial.println("proses pestisida app start");
        //            "0123456789012345"
        lcd_clear();
        lcd_print(0, 0, "Proses pestisida");
    }
}
void prosesPestisida_stop()
{

    // aktuator_off_semua();
    setRunMode(0);
    pestisida_count = 0;
    pestisida_status = false;
    selenoidLahan1_pestisida_status = false;
    selenoidLahan2_pestisida_status = false;
    selenoidLahan3_pestisida_status = false;
    selenoidLahan4_pestisida_status = false;

    aktuator_off_semua();
    // sensorPestisida_stop();
    Serial.println("Proses pestisida stop");
    // kirimKeApp("pestisida", 0, "pestisidaStatus", getAllPestisida_status());
    kirimKeApp("kontrol", 0, "allStatus", getStatusNow());
    // d_clear();
    lcd_stanby();
}
void prosesPestisidaKey_start()
{
    if (getPestisida_status())
    {
        Serial.println("Sedang Proses Pestisida ");
        // kirimKeApp("kontrol", 0, "alert", "Sedang Semprot Pestisida");
        // lahanPestisidaStatus_update(lahan);
        // if (prosesSemprotPestisida_status)
        //{
        //    selenoidPestisidaUpdate();
        //}
        kirimKeApp("kontrol", 0, "allStatus", getStatusNow());
    }
    else if (getBiopest_status())
    {
        Serial.println("Sedang Proses Biopest ");
        kirimKeApp("kontrol", 0, "alert", "Sedang Semprot Biopestisida");
    }
    else if (getSiram_status())
    {
        Serial.println("Sedang Proses Siram ");
        kirimKeApp("kontrol", 0, "alert", "Sedang Penyiraman");
    }
    else
    {
        // lahanPestisidaStatus_update(lahan);
        pestisida_status = true;
        pestisida_count = 0;
        setRunMode(2);
        // isi pestisida
        kirimKeApp("kontrol", 0, "allStatus", getStatusNow());
        lcd_clear();
        lcd_print(0, 0, "Proses pestisida");
    }
}
void prosesBuangPestisida_start()
{
}
void prosesBuangPestisida_stop()
{
}
bool getKalibrasi_pestisida_status()
{
    return isKalibrasi_pestisida;
}
/*
kalibrasi pestisida ]
01
*/
void kalibrasiPestisida_start(uint8_t faktorKal)
{
    // setRunMode(2);
    pompaPestisida_status = true;
    pompaPestisida_update = true;
    isKalibrasi_pestisida = true;
    kalibrasiPestisida_delay = millis();
    kalibrasiPestisida_time = faktorKal * 1000; //(kal/10)*10000 ==> kal * 1000
    Serial.print("faktor kalibrasi: ");
    Serial.println(faktorKal);
    Serial.print("Kalibarsi time: ");
    Serial.println(kalibrasiPestisida_time);
}
bool getKalibrasi_air_pestisida_status()
{
    return isKalibrasi_air_pestisida;
}

bool getRunKalibrasi_air_pestisida(){
    return runKalibrasiAir_pestisida;
}           
void setKalibrasi_air_pestisida_status(bool sts)
{
    isKalibrasi_air_pestisida = sts;
}


bool getkalibrasi_air_pestisida_finish(){
    return isKalibrasi_air_pestisida_finish;
}

float getVolumeKalibrasiAir_pestisida()
{
    return volume_kalibrasi_air_pestisida;
}
void kalibrasiAirPestisida_start(uint8_t kal_value)
{

    
    volume_kalibrasi_air_pestisida = ref_kalibrasiAir_pestisida * kal_value / 10;
   
    Serial.print("Volume kalibrasi: ");
    Serial.println(volume_kalibrasi_air_pestisida);

    runKalibrasiAir_pestisida = true;
    isKalibrasi_air_pestisida = true; 
    isKalibrasi_air_pestisida_finish = false;  
    resetTotalLiter(); 
    flowRateSensor_status = true;
    flowRateSensor_update = true;
    // buka inled
    selenoidInletPestisida_status = true;
    selenoidInletPestisida_update = true;
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
void cekJadwalPestisida()
{
    // timer1 enable
    if ((jadwalPestisida1[0] == 1) && (jadwalPestisida1[hariNow + 7] == 1) && (jadwalPestisida1[3] == jamNow) && (jadwalPestisida1[4] == menitNow))
    {

        dosisPestisida = jadwalPestisida1[17];
        dosisAirPestisida = jadwalPestisida1[16];
        if (jadwalPestisida1[12] == 1)
        {
            selenoidLahan1_pestisida_status = true;
        }
        else
        {
            selenoidLahan1_pestisida_status = false;
        }

        if (jadwalPestisida1[13] == 1)
        {
            selenoidLahan2_pestisida_status = true;
        }
        else
        {
            selenoidLahan2_pestisida_status = false;
        }

        if (jadwalPestisida1[14] == 1)
        {
            selenoidLahan3_pestisida_status = true;
        }
        else
        {
            selenoidLahan3_pestisida_status = false;
        }

        if (jadwalPestisida1[15] == 1)
        {
            selenoidLahan4_pestisida_status = true;
        }
        else
        {
            selenoidLahan4_pestisida_status = false;
        }
        pestisida_status = true;
        pestisida_count = 0;
        setRunMode(2);
        prosesPestisidaKey_start();
    }

    // timer2 enable
    if ((jadwalPestisida2[0] == 1) && (jadwalPestisida2[hariNow + 7] == 1) && (jadwalPestisida2[3] == jamNow) && (jadwalPestisida2[4] == menitNow))
    {

        dosisPestisida = jadwalPestisida2[17];
        dosisAirPestisida = jadwalPestisida2[16];
        if (jadwalPestisida2[12] == 1)
        {
            selenoidLahan1_pestisida_status = true;
        }
        else
        {
            selenoidLahan1_pestisida_status = false;
        }

        if (jadwalPestisida2[13] == 1)
        {
            selenoidLahan2_pestisida_status = true;
        }
        else
        {
            selenoidLahan2_pestisida_status = false;
        }

        if (jadwalPestisida2[14] == 1)
        {
            selenoidLahan3_pestisida_status = true;
        }
        else
        {
            selenoidLahan3_pestisida_status = false;
        }

        if (jadwalPestisida2[15] == 1)
        {
            selenoidLahan4_pestisida_status = true;
        }
        else
        {
            selenoidLahan4_pestisida_status = false;
        }
        pestisida_status = true;
        pestisida_count = 0;
        setRunMode(2);
        prosesPestisidaKey_start();
    }

    // timer3 enable
    if ((jadwalPestisida3[0] == 1) && (jadwalPestisida3[hariNow + 7] == 1) && (jadwalPestisida3[3] == jamNow) && (jadwalPestisida3[4] == menitNow))
    {

        dosisPestisida = jadwalPestisida3[17];
        dosisAirPestisida = jadwalPestisida3[16];
        if (jadwalPestisida3[12] == 1)
        {
            selenoidLahan1_pestisida_status = true;
        }
        else
        {
            selenoidLahan1_pestisida_status = false;
        }

        if (jadwalPestisida3[13] == 1)
        {
            selenoidLahan2_pestisida_status = true;
        }
        else
        {
            selenoidLahan2_pestisida_status = false;
        }

        if (jadwalPestisida3[14] == 1)
        {
            selenoidLahan3_pestisida_status = true;
        }
        else
        {
            selenoidLahan3_pestisida_status = false;
        }

        if (jadwalPestisida3[15] == 1)
        {
            selenoidLahan4_pestisida_status = true;
        }
        else
        {
            selenoidLahan4_pestisida_status = false;
        }
        pestisida_status = true;
        pestisida_count = 0;
        setRunMode(2);
        prosesPestisidaKey_start();
    }

// timer4 enable
    if ((jadwalPestisida4[0] == 1) && (jadwalPestisida4[hariNow + 7] == 1) && (jadwalPestisida4[3] == jamNow) && (jadwalPestisida4[4] == menitNow))
    {

        dosisPestisida = jadwalPestisida4[17];
        dosisAirPestisida = jadwalPestisida4[16];
        if (jadwalPestisida4[12] == 1)
        {
            selenoidLahan1_pestisida_status = true;
        }
        else
        {
            selenoidLahan1_pestisida_status = false;
        }

        if (jadwalPestisida4[13] == 1)
        {
            selenoidLahan2_pestisida_status = true;
        }
        else
        {
            selenoidLahan2_pestisida_status = false;
        }

        if (jadwalPestisida4[14] == 1)
        {
            selenoidLahan3_pestisida_status = true;
        }
        else
        {
            selenoidLahan3_pestisida_status = false;
        }

        if (jadwalPestisida4[15] == 1)
        {
            selenoidLahan4_pestisida_status = true;
        }
        else
        {
            selenoidLahan4_pestisida_status = false;
        }
        pestisida_status = true;
        pestisida_count = 0;
        setRunMode(2);
        prosesPestisidaKey_start();
    }

    // timer5 enable
    if ((jadwalPestisida5[0] == 1) && (jadwalPestisida5[hariNow + 7] == 1) && (jadwalPestisida5[3] == jamNow) && (jadwalPestisida5[4] == menitNow))
    {

        dosisPestisida = jadwalPestisida5[17];
        dosisAirPestisida = jadwalPestisida5[16];
        if (jadwalPestisida5[12] == 1)
        {
            selenoidLahan1_pestisida_status = true;
        }
        else
        {
            selenoidLahan1_pestisida_status = false;
        }

        if (jadwalPestisida5[13] == 1)
        {
            selenoidLahan2_pestisida_status = true;
        }
        else
        {
            selenoidLahan2_pestisida_status = false;
        }

        if (jadwalPestisida5[14] == 1)
        {
            selenoidLahan3_pestisida_status = true;
        }
        else
        {
            selenoidLahan3_pestisida_status = false;
        }

        if (jadwalPestisida5[15] == 1)
        {
            selenoidLahan4_pestisida_status = true;
        }
        else
        {
            selenoidLahan4_pestisida_status = false;
        }
        pestisida_status = true;
        pestisida_count = 0;
        setRunMode(2);
        prosesPestisidaKey_start();
    }

}
void pestisida_loop()
{
    if ((millis() - pestisida_delay) > 1000)
    {
        pestisida_delay = millis();
        if (pestisida_status)
        {
            if (pestisida_count == 0)
            { // pompa pestisida on
                pompaPestisida_status = true;
                pompaPestisida_update = true;
                kalibrasiPestisida_delay = millis();
                kalibrasiPestisida_time = kalibrasiPestisida * dosisPestisida * 100; //(kal/10)*10000 ==> kal * 1000
                isKalibrasi_pestisida = false;
                pestisida_count = 1;
                //
                //            "0123456789012345"
                lcd_print(0, 1, "Isi pestisida...");
#ifdef PESTISIDA_DEBUG
                Serial.println("Pompa pestisida start");
#endif
            }
            else if (pestisida_count == 1)
            { // tunggu sampai target
            }
            else if (pestisida_count == 2)
            {
                // aktifkan flowsensor
                prosesIsiAirPestisida_status = true;
                resetTotalLiter();
                // setFlowRate_start();
                flowRateSensor_status = true;
                flowRateSensor_update = true;
                //             "0123456789012345"
                String line2 = "Isi Air(";
                line2 += String(dosisAirPestisida);
                line2 += ")    ";
                lcd_print(0, 1, line2);
#ifdef PESTISIDA_DEBUG
                Serial.print("Dosis air pestisida: ");
                Serial.println(getDosisAirPestisida());
#endif
                pestisida_count = 3;
            }
            else if (pestisida_count == 3)
            {
                // buka selenoid inled pestisida
                // pompa utama start
                selenoidInletPestisida_status = true;
                selenoidInletPestisida_update = true;
#ifdef PESTISIDA_DEBUG
                Serial.println("Buka selenoid inled");
                Serial.println("Pompa Utama start");
#endif

                pompaUtama_status = true;
                pompaUtama_update = true;
                pestisida_count = 4;
            }
            else if (pestisida_count == 4)
            {
                // tunggu sampai target
                lcd_print(12, 1, String(getTotalLiter()));
                if (!prosesIsiAirPestisida_status)
                {
                    //float totalCampuran = getTotalLiter() - kalibrasiCampuranPestisida;
                    //setvolumeCampuranPestisida(totalCampuran);
                    setvolumeCampuranPestisida(getTotalLiter());
                    Serial.print("Air pestisida: ");
                    Serial.println(getTotalLiter());
                    // setFlowRate_stop();
                    flowRateSensor_status = false;
                    flowRateSensor_update = true;
                    // pompa utama stop
                    pompaUtama_status = false;
                    pompaUtama_update = true;
                    // tutup selenoid inlet pestisida
                    selenoidInletPestisida_status = false;
                    selenoidInletPestisida_update = true;
                    pestisida_count = 5;
                    //              "0123456789012345"
                    lcd_print(0, 1, "Aduk Pestsd.....");
                }
            }
            else if (pestisida_count == 5)
            {

                // aduk pestisida
                pompaAdukBiopest_status = true;
                pompaAdukBiopest_update = true;
                pestisidaTimer = 0;
                pestisida_count = 6;
            }
            else if (pestisida_count == 6)
            {
                uint8_t p_count = (120 - pestisidaTimer);
                String cn = "";
                if (p_count < 100)
                {
                    cn += " ";
                }
                if (p_count < 10)
                {
                    cn += " ";
                }
                cn += String(p_count);
                lcd_print(13, 1, cn);
                if (++pestisidaTimer >= 120)
                {
                    // aduk 2 menit
                    // tunggu sampai target
                    pompaAdukBiopest_status = false;
                    pompaAdukBiopest_update = true;
                    pestisida_count = 7;
                }
            }
            else if (pestisida_count == 7)
            {
                // buka selenoid lahan
                selenoidLahan1_status = selenoidLahan1_pestisida_status;
                selenoidLahan2_status = selenoidLahan2_pestisida_status;
                selenoidLahan3_status = selenoidLahan3_pestisida_status;
                selenoidLahan4_status = selenoidLahan4_pestisida_status;

                selenoidLahan1_update = true;
                selenoidLahan2_update = true;
                selenoidLahan3_update = true;
                selenoidLahan4_update = true;

                pestisida_count = 8;
            }
            else if (pestisida_count == 8)
            {
                pestisida_count = 9;
            }
            else if (pestisida_count == 9)
            {
                // nyalakan pompa air untuk dorong
                pompaUtama_status = true;
                pompaUtama_update = true;
                pestisida_count = 10;
                pestisidaTimer = 0;
                //            "0123456789012345"
                lcd_print(0, 1, "Isi Air ke Pipa ");
            }
            else if (pestisida_count == 10)
            {

                uint8_t p_count = (90 - pestisidaTimer);
                String cn = "";
                if (p_count < 100)
                {
                    cn += " ";
                }
                if (p_count < 10)
                {
                    cn += " ";
                }
                cn += String(p_count);
                if (++pestisidaTimer >= 90)
                {
                    // tunggu sampai saluran penuh(dengan perkiraan timer 90 detik)
                    // pompa utama stop
                    pompaUtama_status = false;
                    pompaUtama_update = true;
                    selenoidInletPestisida_status = false;
                    selenoidInletPestisida_update = true;
                    pestisida_count = 11;
                }
            }
            else if (pestisida_count == 11)
            {
                // buka selenoid outled pestisida

                selenoidOutletPestisida_status = true;
                selenoidOutletPestisida_update = true;
                // setFlowRate_start();
                flowRateSensor_status = true;
                flowRateSensor_update = true;
                resetTotalLiter();
                prosesSemprotPestisida_status = true;
                pestisida_count = 12;
            }
            else if (pestisida_count == 12)
            {
                // pompa semprot start
                pompaSemprot_status = true;
                pompaSemprot_update = true;
                pestisida_count = 13;
                //              "0123456789012345"
                lcd_print(0, 1, "Semprot.........");
            }
            else if (pestisida_count == 13)
            {
                lcd_print(12, 1, String(getTotalLiter()));
                if (!prosesSemprotPestisida_status)
                {
                    // tunggu sampai target
                    // pompa semprot stop
                    pompaSemprot_status = false;
                    pompaSemprot_update = true;
                    // setFlowRate_stop();
                    flowRateSensor_status = false;
                    flowRateSensor_update = true;
                    pestisida_count = 14;
                }
            }
            else if (pestisida_count == 14)
            {
                // selenoid outled close
                // selenoid lahan close
                selenoidOutletPestisida_status = false;
                selenoidOutletPestisida_update = true;

                selenoidLahan1_status = false;
                selenoidLahan2_status = false;
                selenoidLahan3_status = false;
                selenoidLahan4_status = false;

                selenoidLahan1_update = true;
                selenoidLahan2_update = true;
                selenoidLahan3_update = true;
                selenoidLahan4_update = true;
                pestisida_count = 15;
            }
            else if (pestisida_count == 15)
            {
                pestisida_count = 16;
            }
            else if (pestisida_count == 16)
            {

                kirimKeApp("kontrol", 0, "allStatus", getStatusNow());
                //            "0123456789012345"
                lcd_print(0, 1, "Selesai.........");
                pestisida_count = 17;
            }
            else if (pestisida_count == 17)
            {
                pestisida_count = 18;
            }
            else if (pestisida_count == 18)
            {
                prosesPestisida_stop();
            }
        }
    }

    if (isKalibrasi_pestisida)
    { // kalibrasi pestisida
        if ((millis() - kalibrasiPestisida_delay) > kalibrasiPestisida_time)
        {
            // setRunMode(0);
            pompaPestisida_status = false;
            pompaPestisida_update = true;
            isKalibrasi_pestisida = false;
            Serial.println("Kalibrasi pestisida selesai");
        }
    }
    else if ((pestisida_status) && (pestisida_count == 1))
    {
        if ((millis() - kalibrasiPestisida_delay) > kalibrasiPestisida_time)
        {
            pompaPestisida_status = false;
            pompaPestisida_update = true;
            pestisida_count = 2;
            Serial.println("Isi pestisida selesai");
        }
    }
    else if (runKalibrasiAir_pestisida)
    {
        if (!isKalibrasi_air_pestisida)
        {
            // simpan kalibrasi?
            runKalibrasiAir_pestisida = false;
            flowRateSensor_status = false;
            flowRateSensor_update = true;

            // buka inled
            selenoidInletPestisida_status = false;
            selenoidInletPestisida_update = true;

            // pompa start
            pompaUtama_status = false;
            pompaUtama_update = true;

            isKalibrasi_air_pestisida_finish = true;
            Serial.println("kalibrasi air pestisida selesai");
        }
    }
}
