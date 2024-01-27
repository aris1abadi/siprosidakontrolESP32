// total memori device 840 byte
// total memori sensor 67x20 1340 byte
/*
    uint8_t nodeId;
    uint8_t sensorId;
    uint8_t ambangAtas = 80;
    uint8_t ambangBawah = 70;   //yg dipake
    uint8_t sensorUse = 1;      //yg dipake
    uint8_t value = 0;          //on/off
    uint8_t status = 0;         //device status
    uint8_t timer[24] ={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // TIMER FORMAT Jam,Menit,Durasi,hari(bentuk bit)
    uint8_t lastUpdate[6];//format yy,bb,tg,jm,mn,dt

struct sensorDev{
    uint8_t nodeId;
    uint8_t sensorId;
    uint8_t type;
    uint8_t devSts;//device status
    uint8_t interval;
    uint8_t batt;
    uint8_t value;
    unsigned char param[30];
    unsigned char dataSave[30];

    format pesan dari node
    >> sensor id sebagai trpe sensor

    msg child id
    0 | aktuator/relay
    1 | temp/suhu
    2 | humidity
    3 | CO2
    4 | CO
    5 | CH4
    6 | gas1
    7 | gas2
    8| gas3
    9| gas4
    10| gas5
    11| gas6
    12| pH tanah
    13| lengas
    14| genangan_atas
    15| genangan_bawah
    16| curah hujan
    17| angin
    18|
*/

// uint8_t deviceCount = 0;
// uint8_t sensorCount = 0;
// uint8_t lengasCount = 0;

#include <Arduino.h>
#include <Ticker.h>
#include <eepromHandle.h>
#include <mqttHandle.h>
#include <siramHandle.h>
#include <pestisidaHandle.h>
#include <biopestHandle.h>
#include <mySensorsHandle.h>

#define ON 1
#define OFF 0

// #define DEBUG_MYSENSOR

#define NODE_AKTUATOR1 1
#define NODE_AKTUATOR2 2
#define NODE_AKTUATOR3 3

#define NODE_DISPLAY_OUTDOOR 30

#define NODE_LENGAS1 51
#define NODE_LENGAS2 52
#define NODE_LENGAS3 53
#define NODE_LENGAS4 54
#define NODE_DHT 55
#define NODE_WATERFLOW 56
#define NODE_MQ4 102
#define NODE_MQ135 103
#define NODE_NPK 104
#define NODE_HUJAN 105
#define NODE_SHT 106
#define NODE_MQ4_MQ135 107
#define NODE_MHZ19C 108
#define NODE_MHZ19B 109
#define NODE_MULTI 110

// chlid id (model node node)
#define POMPA 0
#define TEMP 1
#define HUM 2
#define CO2 3
#define CO 4
#define CH4 5
#define GAS1 6
#define GAS2 7
#define GAS3 8
#define GAS4 9
#define GAS5 10
#define GAS6 11
#define PH_TANAH 12
#define LENGAS 13
#define GENANGAN 14
#define CURAH_HUJAN 15
#define ANGIN 16
#define N_SENSOR 17
#define P_SENSOR 18
#define K_SENSOR 19
#define TEMP_SENSOR 20
#define MOISTURE_SENSOR 21
#define EC_SENSOR 22
#define PH_SENSOR 23
#define CO2_MHZ19B 24
#define CO2_MHZ19C 25
// flow sensor
#define FLOWRATE_SENSOR 1
#define VOLUME_SENSOR 2

#define V_STATUS 2
#define V_VAR1 24
#define V_VAR2 25
#define V_VAR3 26
#define V_VAR4 27
#define V_TEMP 0
#define V_HUM 1
#define V_CURRENT 39
#define V_LEVEL 37

Ticker MySensorTimer;

String mqttMsgInput = "";
bool mqttMsgReady = false;

String webMsg = "";

// hitung air
unsigned long flowMilliLitres;
unsigned int totalMilliLitres;
float flowLitres = 0;
float totalLitres = 0;

int pestisidaCount = 0;
int lastPestisidaCount = 0;

float kalibrasiAir = 1.0; // di jadikan float pada pemakaian

void setFlowRate_start()
{
    kirim_ke_device(56, 1, V_VAR3, "1");
}

void setFlowRate_stop()
{
    kirim_ke_device(56, 1, V_VAR3, "0");
}

void resetTotalLiter()
{
    totalLitres = 0;
    totalMilliLitres = 0;
}
float getTotalLiter()
{
    return totalLitres;
}

void kirim_ke_device(uint8_t node, uint8_t childnode, uint8_t type, String payload)
{
    String strOut = "";
    String resp = "";
    int isi = payload.toInt();

    strOut = String(node);
    strOut += ";";
    strOut += String(childnode);
    strOut += ";1;1;";
    strOut += String(type);
    strOut += ";";
    strOut += String(isi);
    strOut += "\n";

    mqttMsgInput = strOut;
    mqttMsgReady = true;
    Serial.println(strOut);
}

void device_ON_OFF(uint8_t nomerNode, uint8_t nomerPompa, uint8_t on_off)

{
    String strOut = String(nomerNode);
    String resp = "";

    strOut += ';';
    strOut += String(nomerPompa);
    strOut += ";1;1;2;";
    if (on_off == ON)
    {
        strOut += "1\n";
        resp = "1";
    }
    else
    {
        strOut += "0\n";
        resp = "0";
    }
    mqttMsgInput = strOut;
    mqttMsgReady = true;
    Serial.println(strOut);
    // kirimKeApp(String type,uint8_t nomer,String cmd,String msg)
    // kirimKeApp("aktuator", nomerPompa, "status", resp);
}
void setSensorInterval(uint8_t n_id, uint8_t sensorType, uint8_t dtk)
{
    String msgOut = String(n_id);
    msgOut += ';';
    msgOut += String(sensorType);
    msgOut += ";1;0;24;";
    msgOut += String(dtk);
    msgOut += '\n';

    mqttMsgInput = msgOut;
    mqttMsgReady = true;

    Serial.print("Kirim ke sensor >> ");
    Serial.println(msgOut);
}
void cekMySensors(String msg)
{

    uint8_t nodeId = (getValue(msg, ';', 0)).toInt();
    uint8_t childId = (getValue(msg, ';', 1)).toInt();
    uint8_t type_msg = (getValue(msg, ';', 4)).toInt();
    String pl = getValue(msg, ';', 5);
    String payload = "";
    if (pl.length() > 0)
    {
        for (uint8_t i = 0; i < pl.length(); i++)
        {
            if (pl.charAt(i) == '\r' || pl.charAt(i) == '\n')
            {
                break;
            }
            else
            {
                payload += pl.charAt(i);
            }
        }
    }

#ifdef DEBUG_MYSENSOR
    Serial.print(msg);
    Serial.print("nodeId: ");
    Serial.print(nodeId);
    Serial.print(" chidId: ");
    Serial.print(childId);
    Serial.print(" Type: ");
    Serial.print(type_msg);
    Serial.print(" payload:");
    Serial.println(payload);
#endif

    switch (nodeId)
    {
    case NODE_AKTUATOR1:
        if (type_msg == V_STATUS)
        {
#ifdef DEBUG_MYSENSOR
            Serial.print("response aktuator 1 sensorID: ");
            Serial.println(childId);
#endif
            kirimKeApp("aktuator1", childId, "status", payload);
        }
        break;
    case NODE_AKTUATOR2:
        if (type_msg == V_STATUS)
        {
#ifdef DEBUG_MYSENSOR
            Serial.print("response aktuator 2 sensorID: ");
            Serial.println(childId);
#endif
            kirimKeApp("aktuator2", childId, "status", payload);
        }
        break;
    case NODE_AKTUATOR3:
        if (type_msg == V_STATUS)
        {
#ifdef DEBUG_MYSENSOR
            Serial.print("response aktuator 1 sensorID: ");
            Serial.println(childId);
#endif
            kirimKeApp("aktuator3", childId, "status", payload);
        }
        break;
    case NODE_LENGAS1:
        if (type_msg == V_LEVEL)
        {

            // lengas1.value = payload.toInt();
            // lengas1.status = 1;
            Serial.print("Lengas 1 : ");
            Serial.print(payload);
            Serial.println("%");
            kirimKeApp("lengas", 1, "value", payload);
            if (getUseLengas())
            {
                uint8_t val = payload.toInt();
                if (val < (getAmbangLengas() - 2))
                {
                    // pompa on
                    siram_start(1, getDurasiSiram());
                    // set interval lengas ke lengas1
                    kirim_ke_device(NODE_LENGAS1, 1, V_VAR1, "5");
                }
                else if (val > getAmbangLengas())
                {

                    siram_stop(0);

                    kirim_ke_device(NODE_LENGAS2, 1, V_VAR1, "60");
                }
            }
        }
        else if (type_msg == V_CURRENT)
        {
            kirimKeApp("lengas", 1, "rawvalue", payload);
        }

        break;
    case NODE_LENGAS2:
        if (type_msg == V_LEVEL)
        {
            // lengas2.value = payload.toInt();
            // lengas2.status = 1;
            Serial.print("Lengas 2 : ");
            Serial.print(payload);
            Serial.println("%");
            kirimKeApp("lengas", 2, "value", payload);
            if (getUseLengas())
            {
                uint8_t val = payload.toInt();
                if (val < (getAmbangLengas() - 2))
                {
                    // pompa on
                    siram_start(2, getDurasiSiram());
                    // set interval lengas ke lengas2
                    kirim_ke_device(NODE_LENGAS1, 2, V_VAR1, "5");
                }
                else if (val > getAmbangLengas())
                {

                    siram_stop(0);

                    kirim_ke_device(NODE_LENGAS2, 1, V_VAR1, "60");
                }
            }
        }
        else if (type_msg == V_CURRENT)
        {
            kirimKeApp("lengas", 2, "rawvalue", payload);
        }

        break;
    case NODE_LENGAS3:
        if (type_msg == V_LEVEL)
        {

            // lengas1.value = payload.toInt();
            // lengas1.status = 1;
            Serial.print("Lengas 3 : ");
            Serial.print(payload);
            Serial.println("%");
            kirimKeApp("lengas", 3, "value", payload);
            if (getUseLengas())
            {
                uint8_t val = payload.toInt();
                if (val < (getAmbangLengas() - 2))
                {
                    // pompa on
                    siram_start(3, getDurasiSiram());
                    // set interval lengas ke lengas1
                    kirim_ke_device(NODE_LENGAS3, 1, V_VAR1, "5");
                }
                else if (val > getAmbangLengas())
                {

                    siram_stop(0);

                    kirim_ke_device(NODE_LENGAS3, 1, V_VAR1, "60");
                }
            }
        }
        else if (type_msg == V_CURRENT)
        {
            kirimKeApp("lengas", 3, "rawvalue", payload);
        }

        break;
    case NODE_LENGAS4:
        if (type_msg == V_LEVEL)
        {

            // lengas1.value = payload.toInt();
            // lengas1.status = 1;
            Serial.print("Lengas 4 : ");
            Serial.print(payload.toInt());
            Serial.println("%");
            kirimKeApp("lengas", 4, "value", payload);

            if (getUseLengas())
            {
                uint8_t val = payload.toInt();
                if (val < (getAmbangLengas() - 2))
                {
                    // pompa on
                    siram_start(1, getDurasiSiram());
                    // set interval lengas ke lengas1
                    kirim_ke_device(NODE_LENGAS4, 1, V_VAR1, "5");
                }
                else if (val > getAmbangLengas())
                {

                    siram_stop(0);

                    kirim_ke_device(NODE_LENGAS4, 1, V_VAR1, "60");
                }
            }
        }
        else if (type_msg == V_CURRENT)
        {
            kirimKeApp("lengas", 4, "rawvalue", payload);
        }

        break;
    case NODE_DHT:

        if (childId == TEMP)
        {

            kirimKeApp("sensorDHT", 1, "temp", payload);
            // kirimKeDisplay("dhtTemp", String(sensorDHT.value1));
        }
        else if (childId == HUM)
        {

            kirimKeApp("sensorDHT", 1, "hum", payload);
            // kirimKeDisplay("dhtHum", String(sensorDHT.value2));
        }

        break;
    case NODE_MQ4: // ch4

        if (childId == CH4)
        {

            kirimKeApp("sensorMQ4", 1, "ch4", payload);
            // kirimKeDisplay("mq4Ch4", String(sensorMQ4.value1));
        }

        break;
    case NODE_MQ135:
        if (childId == CO2)
        {

            kirimKeApp("sensorMQ135", 1, "co2", payload);
            // kirimKeDisplay("mq135Co2", String(sensorMQ135.value1));
        }
        break;
    case NODE_NPK:

        if (childId == N_SENSOR)
        {

            kirimKeApp("sensorNPK", 1, "nValue", payload);
            // kirimKeDisplay("npkN", String(sensorNPK.value1));
        }
        else if (childId == P_SENSOR)
        {

            kirimKeApp("sensorNPK", 1, "pValue", payload);
            // kirimKeDisplay("npk_p", String(sensorNPK.value2));
        }
        else if (childId == K_SENSOR)
        {

            kirimKeApp("sensorNPK", 1, "kValue", payload);
            // kirimKeDisplay("npkK", String(sensorNPK.value3));
        }
        else if (childId == TEMP_SENSOR)
        {

            kirimKeApp("sensorNPK", 1, "tempValue", payload);
            // kirimKeDisplay("npkTemp", String(sensorNPK.value4));
        }
        else if (childId == MOISTURE_SENSOR)
        {

            kirimKeApp("sensorNPK", 1, "moistureValue", payload);
            // kirimKeDisplay("npkMoisture", String(sensorNPK.value5));
        }
        else if (childId == EC_SENSOR)
        {

            kirimKeApp("sensorNPK", 1, "ecValue", payload);
            // kirimKeDisplay("npkEc", String(sensorNPK.value6));
        }
        else if (childId == PH_SENSOR)
        {

            kirimKeApp("sensorNPK", 1, "phValue", payload);
            // kirimKeDisplay("npkPh", String(sensorNPK.value7));
        }

        break;
    case NODE_MQ4_MQ135:
        if (childId == CO2)
        {

            kirimKeApp("sensorMulti1", 1, "co2", payload);
            // kirimKeDisplay("multi1Co2", String(sensorMQ135.value1));
        }
        else if (childId == CH4)
        {

            kirimKeApp("sensorMulti1", 1, "ch4", payload);
            // kirimKeDisplay("multi1Ch4", String(sensorMQ4.value1));
        }
        else if (childId == TEMP)
        {

            kirimKeApp("sensorMulti1", 1, "temp", payload);
            // kirimKeDisplay("multi1Temp", String(sensorDHT.value1));
        }
        else if (childId == HUM)
        {

            kirimKeApp("sensorDHT", 1, "hum", payload);
            // kirimKeDisplay("multi1Hum", String(sensorDHT.value1));
        }
        break;
    case NODE_MHZ19B:
        if (childId == CO2_MHZ19B)
        {

            kirimKeApp("sensorMHZ19B", 1, "co2", payload);
            // kirimKeDisplay("mhz19b", String(sensorMHZ19B.value1));
        }
        break;
    case NODE_MHZ19C:
        if (childId == CO2_MHZ19C)
        {

            kirimKeApp("sensorMulti2", 1, "co2", payload);
            // kirimKeDisplay("multi2Co2", String(sensorMHZ19C.value1));
        }
        else if (childId == TEMP)
        {
            // tambahan sensor SHT30

            kirimKeApp("sensorMulti2", 1, "temp", payload);
            // kirimKeDisplay("multi2Temp", String(sensorMHZ19C.value2));
        }
        else if (childId == HUM)
        {
            // tambahan sensor SHT30

            kirimKeApp("sensorMulti2", 1, "hum", payload);
            // kirimKeDisplay("multi2Hum", String(sensorMHZ19C.value3));
        }
        break;
    case NODE_SHT:
        if (childId == TEMP)
        {

            kirimKeApp("sensorSHT", 1, "temp", payload);
            // kirimKeDisplay("shtTemp", String(sensorSHT.value1));
        }
        else if (childId == HUM)
        {

            kirimKeApp("sensorSHT", 1, "hum", payload);
            // kirimKeDisplay("shtHum", String(sensorSHT.value2));
        }
        break;
    case NODE_HUJAN:
        if (childId == CURAH_HUJAN)
        {

            kirimKeApp("sensorHujan", 1, "curah_hujan", payload);
            // kirimKeDisplay("hujan", String(sensorHUJAN.value1));
        }
        break;
    case NODE_MULTI: // mq135,mq4,sht30,curah hujan
        if (childId == TEMP)
        {

            kirimKeApp("sensorMulti3", 1, "temp", payload);
            // kirimKeDisplay("multi3Temp", String(sensorSHT.value3));
        }
        else if (childId == HUM)
        {

            kirimKeApp("sensorMulti3", 1, "hum", payload);
            // kirimKeDisplay("multi3Hum", String(sensorSHT.value4));
        }
        else if (childId == CO2)
        {

            kirimKeApp("sensorMulti3", 1, "co2", payload);
            // kirimKeDisplay("co2Multi3", String(sensorMQ135.value3));
        }
        else if (childId == CH4)
        {

            kirimKeApp("sensorMulti3", 1, "ch4", payload);
            // kirimKeDisplay("ch4Multi3", String(sensorMQ4.value4));
        }
        else if (childId == CURAH_HUJAN)
        {

            kirimKeApp("sensorMulti3", 1, "curah_hujan", payload);
            // kirimKeDisplay("hujanMulti3", String(sensorHUJAN.value2));
        }
        break;

    case NODE_WATERFLOW:
        if (childId == FLOWRATE_SENSOR)
        {

            // kirimKeApp("sensorFlow", 1, "flowRate", nodeValue);
            float flowRate = payload.toInt();
            flowMilliLitres = (flowRate / 60) * 1000;
            flowLitres = (flowRate / 60);

            // Add the millilitres passed in this second to the cumulative total
            totalMilliLitres += flowMilliLitres;
            totalLitres += flowLitres;
            Serial.print("Total Air: ");
            Serial.println(totalLitres);

            if (getIsiAirPestisida_status())
            {
                if (totalLitres >= getDosisAirPestisida())
                {

                    setIsiAirPestisida_status(false);
                    Serial.println("Isi air pestisida selesai");
                }
            }
            else if (getSemprotPestisida_status())
            {
                if (totalLitres > getvolumeCampuranPestisida())
                {

                    setSemprotPestisida_status(false);
                    Serial.println("Semprot pestisida selesai");
                }
            }
            else if (getIsiAirBiopest_status())
            {
                if (totalLitres >= getDosisAirBiopest())
                {

                    setIsiAirBiopest_status(false);
                    Serial.println("Isi air biopest selesai");
                }
            }
            else if (getSemprotBiopest_status())
            {
                if (totalLitres > getvolumeCampuranBiopest())
                {
                    setSemprotBiopest_status(false);
                    Serial.println("Semprot biopest selesai");
                }
            }else if(getKalibrasi_air_pestisida_status()){
                if (totalLitres > getVolumeKalibrasiAir_pestisida()){
                    setKalibrasi_air_pestisida_status(false);
                    Serial.println("kalibrasi air pestisida selesai");
                }
                
            }else if(getKalibrasi_air_biopest_status()){
                if (totalLitres > getVolumeKalibrasiAir_biopest()){
                    setKalibrasi_air_biopest_status(false);
                    Serial.println("kalibrasi air biopest selesai");
                }
                
            }

            //kirimKeApp("sensorFlow", 1, "volumeRate", String(totalLitres));
        }
        else if (childId == VOLUME_SENSOR)
        {

           // kirimKeApp("sensorFlow", 1, "volumeRate", payload);
        }
        break;
    }
}
