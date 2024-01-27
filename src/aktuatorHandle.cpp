#define USE_SERVO_LAHAN

#define POMPA_PIN 0
#define SELENOID_INPUT_AIR_PIN 4

#define SELENOID_LAHAN1_PIN 25
#define SELENOID_LAHAN2_PIN 26
#define SELENOID_LAHAN3_PIN 27
#define SELENOID_LAHAN4_PIN 14

#define POMPA_PESTISIDA_PIN 2
#define POMPA_BIOPEST_PIN 15
#define POMPA_ADUK_PESTISIDA_PIN 32
#define POMPA_ADUK_BIOPEST_PIN 33

#define SELENOID_INLET_CAMPURAN_PIN 12
#define SELENOID_OUTLET_CAMPURAN_PIN 13

#define USE_PESTISIDA 0
#define USE_BIOPEST 180

#define NODE_AKTUATOR1 1 //( 8 sub node )pada pestisida yg dipakai 4 child(5,6,7,8)
#define NODE_AKTUATOR2 2 //(pada bsip ada 6 relay 1&2 pompa ,selenoid 3456)
#define NODE_AKTUATOR3 3 // untuk selenoid di pestisidainled outled

#define SELENOID_INLET_PESTISIDA 4
#define SELENOID_INLET_BIOPEST 1
#define SELENOID_OUTLET_PESTISIDA 3
#define SELENOID_OUTLET_BIOPEST 2

#define POMPA_PESTISIDA 5
#define POMPA_BIOPEST 6
#define POMPA_ADUK_PESTISIDA 8
#define POMPA_ADUK_BIOPEST 7

#define POMPA_SEMPROTPESTISIDA 2
#define POMPA_UTAMA 1

#define SELENOID_LAHAN1 3
#define SELENOID_LAHAN2 4
#define SELENOID_LAHAN3 5
#define SELENOID_LAHAN4 6

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

#define OFF HIGH
#define ON LOW

#define SERVO_DELAY 20 // Delay in ms servo moving(smooth move)

#include <Arduino.h>
#include <mySensorsHandle.h>

// #include <ESP32Servo.h>
// #include <Ticker.h>

// Servo servoInletCampuran;
// Servo servoOutletCampuran;

// Ticker servoTimer;

int servoInlet_pos = 0; //
int lastInlet_pos = 0;
bool servoInlet_enable = false;

long servo_delay = 0;

int servoOutlet_pos = 0; //
int lastOutlet_pos = 0;
bool servoOutlet_enable = false;

bool nextAktuator = false;
unsigned long aktuator_time = 0;
int aktuator_delay = 250; // 250 ms

bool pompaUtama_update = false;
bool pompaSemprot_update = false;
bool pompaPestisida_update = false;
bool pompaBiopest_update = false;
bool pompaAdukPestisida_update = false;
bool pompaAdukBiopest_update = false;

bool selenoidInletPestisida_update = false;
bool selenoidInletBiopest_update = false;
bool selenoidOutletPestisida_update = false;
bool selenoidOutletBiopest_update = false;

bool selenoidLahan1_update = false;
bool selenoidLahan2_update = false;
bool selenoidLahan3_update = false;
bool selenoidLahan4_update = false;

bool pompaUtama_status = false;
bool pompaSemprot_status = false;
bool pompaPestisida_status = false;
bool pompaBiopest_status = false;
bool pompaAdukPestisida_status = false;
bool pompaAdukBiopest_status = false;

bool selenoidInletPestisida_status = false;
bool selenoidInletBiopest_status = false;

bool selenoidOutletPestisida_status = false;
bool selenoidOutletBiopest_status = false;

bool selenoidLahan1_status = false;
bool selenoidLahan2_status = false;
bool selenoidLahan3_status = false;
bool selenoidLahan4_status = false;

bool flowRateSensor_update = false;
bool flowRateSensor_status = false;

void pompa_start()
{
    // digitalWrite(POMPA_PIN,ON);
    kirim_ke_device(NODE_AKTUATOR2, POMPA_UTAMA, V_STATUS, "1");
}

void pompa_stop()
{
    // digitalWrite(POMPA_PIN,OFF);
    kirim_ke_device(NODE_AKTUATOR2, POMPA_UTAMA, V_STATUS, "0");
}

void selenoidLahan1_open()
{
    // digitalWrite(SELENOID_LAHAN1,ON);
    kirim_ke_device(NODE_AKTUATOR2, SELENOID_LAHAN1, V_STATUS, "1");
}
void selenoidLahan1_close()
{
    // digitalWrite(SELENOID_LAHAN1,OFF);
    kirim_ke_device(NODE_AKTUATOR2, SELENOID_LAHAN1, V_STATUS, "0");
}

void selenoidLahan2_open()
{
    // digitalWrite(SELENOID_LAHAN2,ON);
    kirim_ke_device(NODE_AKTUATOR2, SELENOID_LAHAN2, V_STATUS, "1");
}
void selenoidLahan2_close()
{
    // digitalWrite(SELENOID_LAHAN2,OFF);
    kirim_ke_device(NODE_AKTUATOR2, SELENOID_LAHAN2, V_STATUS, "0");
}

void selenoidLahan3_open()
{
    // digitalWrite(SELENOID_LAHAN3,ON);
    kirim_ke_device(NODE_AKTUATOR2, SELENOID_LAHAN3, V_STATUS, "1");
}
void selenoidLahan3_close()
{
    // digitalWrite(SELENOID_LAHAN3,OFF);
    kirim_ke_device(NODE_AKTUATOR2, SELENOID_LAHAN3, V_STATUS, "0");
}

void selenoidLahan4_open()
{
    // digitalWrite(SELENOID_LAHAN4,ON);
    kirim_ke_device(NODE_AKTUATOR2, SELENOID_LAHAN4, V_STATUS, "1");
}
void selenoidLahan4_close()
{
    // digitalWrite(SELENOID_LAHAN4,OFF);
    kirim_ke_device(NODE_AKTUATOR2, SELENOID_LAHAN4, V_STATUS, "0");
}

void pompaPestisida_start()
{
    // digitalWrite(POMPA_PESTISIDA,ON);
    kirim_ke_device(NODE_AKTUATOR1, POMPA_PESTISIDA, V_STATUS, "1");
}

void pompaPestisida_stop()
{
    // digitalWrite(POMPA_PESTISIDA,OFF);
    kirim_ke_device(NODE_AKTUATOR1, POMPA_PESTISIDA, V_STATUS, "0");
}

void pompaBiopest_start()
{
    // digitalWrite(POMPA_BIOPEST,ON);
    kirim_ke_device(NODE_AKTUATOR1, POMPA_BIOPEST, V_STATUS, "1");
}

void pompaBiopest_stop()
{
    // digitalWrite(POMPA_BIOPEST,OFF);
    kirim_ke_device(NODE_AKTUATOR1, POMPA_BIOPEST, V_STATUS, "0");
}

void pompaAdukPestisida_start()
{
    // digitalWrite(POMPA_ADUK_PESTISIDA,ON);
    kirim_ke_device(NODE_AKTUATOR1, POMPA_ADUK_PESTISIDA, V_STATUS, "1");
}

void pompaAdukPestisida_stop()
{
    // digitalWrite(POMPA_ADUK_PESTISIDA,OFF);
    kirim_ke_device(NODE_AKTUATOR1, POMPA_ADUK_PESTISIDA, V_STATUS, "0");
}

void pompaAdukBiopest_start()
{
    // digitalWrite(POMPA_ADUK_BIOPEST,ON);
    kirim_ke_device(NODE_AKTUATOR1, POMPA_ADUK_BIOPEST, V_STATUS, "1");
}

void pompaAdukBiopest_stop()
{
    // digitalWrite(POMPA_ADUK_BIOPEST,OFF);
    kirim_ke_device(NODE_AKTUATOR1, POMPA_ADUK_BIOPEST, V_STATUS, "0");
}

void pompaSemprot_start()
{
    // device_ON_OFF(NODE_AKTUATOR2, POMPA_SEMPROTPESTISIDA, 1);
    kirim_ke_device(NODE_AKTUATOR2, POMPA_SEMPROTPESTISIDA, V_STATUS, "1");
}

void pompaSemprot_stop()
{
    kirim_ke_device(NODE_AKTUATOR2, POMPA_SEMPROTPESTISIDA, V_STATUS, "0");
}

void selenoidInletPestisida_open()
{

    // servoInletCampuran.write(0);
    servoInlet_pos = 0;
    servoInlet_enable = true;
    // device_ON_OFF(NODE_AKTUATOR3, SELENOID_INLET_PESTISIDA, 1);
    kirim_ke_device(NODE_AKTUATOR3, SELENOID_INLET_PESTISIDA, V_STATUS, "1");
}

void selenoidInletPestisida_close()
{
    // servoInletCampuran.write(90);
    servoInlet_pos = 90;
    servoInlet_enable = true;
    kirim_ke_device(NODE_AKTUATOR3, SELENOID_INLET_PESTISIDA, V_STATUS, "0");
}

void selenoidOutletPestisida_open()
{
    // servoOutletCampuran.write(90);
    servoOutlet_pos = 90;
    servoOutlet_enable = true;
    kirim_ke_device(NODE_AKTUATOR3, SELENOID_OUTLET_PESTISIDA, V_STATUS, "1");
}

void selenoidOutletPestisida_close()
{
    // servoOutletCampuran.write(90);
    servoOutlet_pos = 90;
    servoOutlet_enable = true;
    kirim_ke_device(NODE_AKTUATOR3, SELENOID_OUTLET_PESTISIDA, V_STATUS, "0");
}

void selenoidInletBiopest_open()
{

    // servoInletCampuran.write(180);
    servoInlet_pos = 180;
    servoInlet_enable = true;
    kirim_ke_device(NODE_AKTUATOR3, SELENOID_INLET_BIOPEST, V_STATUS, "1");
}

void selenoidInletBiopest_close()
{

    // servoInletCampuran.write(180);
    servoInlet_pos = 180;
    servoInlet_enable = true;
    kirim_ke_device(NODE_AKTUATOR3, SELENOID_INLET_BIOPEST, V_STATUS, "0");
}

void selenoidOutletBiopest_open()
{

    // servoOutletCampuran.write(180);
    servoOutlet_pos = 180;
    servoOutlet_enable = true;
    kirim_ke_device(NODE_AKTUATOR3, SELENOID_OUTLET_BIOPEST, V_STATUS, "1");
}

void selenoidOutletBiopest_close()
{

    // servoOutletCampuran.write(180);
    servoOutlet_pos = 180;
    servoOutlet_enable = true;
    kirim_ke_device(NODE_AKTUATOR3, SELENOID_OUTLET_BIOPEST, V_STATUS, "0");
}

void selenoidInletUtama_open()
{
}

void selenoidInletUtama_close()
{
}

void servoInlet_loop()
{
    if (servoInlet_pos == lastInlet_pos)
    {
        servoInlet_enable = false;
    }
    else if (servoInlet_pos > lastInlet_pos)
    {
        // servoInletCampuran.write(lastInlet_pos);
        lastInlet_pos += 1;
        // last_pos = servo_pos;
    }
    else if (servoInlet_pos < lastInlet_pos)
    {
        // servoInletCampuran.write(lastInlet_pos);
        lastInlet_pos -= 1;
    }
}

void servoOutlet_loop()
{
    if (servoOutlet_pos == lastOutlet_pos)
    {
        servoOutlet_enable = false;
    }
    else if (servoOutlet_pos > lastOutlet_pos)
    {
        // servoOutletCampuran.write(lastOutlet_pos);
        lastOutlet_pos += 1;
        // last_pos = servo_pos;
    }
    else if (servoOutlet_pos < lastOutlet_pos)
    {
        // servoOutletCampuran.write(lastOutlet_pos);
        lastOutlet_pos -= 1;
    }
}

void aktuator_loop()
{
    if ((millis() - servo_delay) > SERVO_DELAY)
    {
        servo_delay = millis();

        if (servoInlet_enable)
        {
            servoInlet_loop();
        }

        if (servoOutlet_enable)
        {
            servoOutlet_loop();
        }
    }

    if ((millis() - aktuator_time) > 250)
    {
        aktuator_time = millis();
        if (flowRateSensor_update)
        {
            flowRateSensor_update = false;
            if (flowRateSensor_status)
            {
                setFlowRate_start();
            }
            else
            {
                setFlowRate_stop();
            }
            return;
        }
        if (pompaUtama_update)
        {
            pompaUtama_update = false;
            if (pompaUtama_status)
            {
                pompa_start();
            }
            else
            {
                pompa_stop();
            }
            return;
        }
        if (pompaSemprot_update)
        {
            pompaSemprot_update = false;
            if (pompaSemprot_status)
            {
                pompaSemprot_start();
            }
            else
            {
                pompaSemprot_stop();
            }
            return;
        }
        if (pompaPestisida_update)
        {
            pompaPestisida_update = false;
            if (pompaPestisida_status)
            {
                pompaPestisida_start();
            }
            else
            {
                pompaPestisida_stop();
            }
            return;
        }
        if (pompaBiopest_update)
        {
            pompaBiopest_update = false;
            if (pompaBiopest_status)
            {
                pompaBiopest_start();
            }
            else
            {
                pompaBiopest_stop();
            }
            return;
        }
        if (pompaAdukPestisida_update)
        {
            pompaAdukPestisida_update = false;
            if (pompaAdukPestisida_status)
            {
                pompaAdukPestisida_start();
            }
            else
            {
                pompaAdukPestisida_stop();
            }
            return;
        }
        if (pompaAdukBiopest_update)
        {
            pompaAdukBiopest_update = false;
            if (pompaAdukBiopest_status)
            {
                pompaAdukBiopest_start();
            }
            else
            {
                pompaAdukBiopest_stop();
            }
            return;
        }
        if (selenoidLahan1_update)
        {
            selenoidLahan1_update = false;
            if (selenoidLahan1_status)
            {
                selenoidLahan1_open();
            }
            else
            {
                selenoidLahan1_close();
            }
            return;
        }
        if (selenoidLahan2_update)
        {
            selenoidLahan2_update = false;
            if (selenoidLahan2_status)
            {
                selenoidLahan2_open();
            }
            else
            {
                selenoidLahan2_close();
            }
            return;
        }
        if (selenoidLahan3_update)
        {
            selenoidLahan3_update = false;
            if (selenoidLahan3_status)
            {
                selenoidLahan3_open();
            }
            else
            {
                selenoidLahan3_close();
            }
            return;
        }
        if (selenoidLahan4_update)
        {
            selenoidLahan4_update = false;
            if (selenoidLahan4_status)
            {
                selenoidLahan4_open();
            }
            else
            {
                selenoidLahan4_close();
            }
            return;
        }
        if (selenoidInletPestisida_update)
        {
            selenoidInletPestisida_update = false;
            if (selenoidInletPestisida_status)
            {
                selenoidInletPestisida_open();
            }
            else
            {
                selenoidInletPestisida_close();
            }
            return;
        }
        if (selenoidInletBiopest_update)
        {
            selenoidInletBiopest_update = false;
            if (selenoidInletBiopest_status)
            {
                selenoidInletBiopest_open();
            }
            else
            {
                selenoidInletBiopest_close();
            }
            return;
        }
        if (selenoidOutletPestisida_update)
        {
            selenoidOutletPestisida_update = false;
            if (selenoidOutletPestisida_status)
            {
                selenoidOutletPestisida_open();
            }
            else
            {
                selenoidOutletPestisida_close();
            }
            return;
        }
        if (selenoidOutletBiopest_update)
        {
            selenoidOutletBiopest_update = false;
            if (selenoidOutletBiopest_status)
            {
                selenoidOutletBiopest_open();
            }
            else
            {
                selenoidOutletBiopest_close();
            }
            return;
        }
        /*

        if (pompaUtama_update)
        {
            pompaUtama_update = false;
            if (pompaUtama_status)
            {
                pompa_start();
            }
            else
            {
                pompa_stop();
            }
        }
        else if (pompaSemprot_update)
        {
            pompaSemprot_update = false;
            if (pompaSemprot_status)
            {
                pompaSemprot_start();
            }
            else
            {
                pompaSemprot_stop();
            }
        }
        else if (pompaPestisida_update)
        {
            pompaPestisida_update = false;
            if (pompaPestisida_status)
            {
                pompaPestisida_start();
            }
            else
            {
                pompaPestisida_stop();
            }
        }
        else if (pompaBiopest_update)
        {
            pompaBiopest_update = false;
            if (pompaBiopest_status)
            {
                pompaBiopest_start();
            }
            else
            {
                pompaBiopest_stop();
            }
        }
        else if (pompaAdukPestisida_update)
        {
            pompaAdukPestisida_update = false;
            if (pompaAdukPestisida_status)
            {
                pompaAdukPestisida_start();
            }
            else
            {
                pompaAdukPestisida_stop();
            }
        }
        else if (pompaAdukBiopest_update)
        {
            pompaAdukBiopest_update = false;
            if (pompaAdukBiopest_status)
            {
                pompaAdukBiopest_start();
            }
            else
            {
                pompaAdukBiopest_stop();
            }
        }
        else if (selenoidLahan1_update)
        {
            selenoidLahan1_update = false;
            if (selenoidLahan1_status)
            {
                selenoidLahan1_open();
            }
            else
            {
                selenoidLahan1_close();
            }
        }
        else if (selenoidLahan2_update)
        {
            selenoidLahan2_update = false;
            if (selenoidLahan2_status)
            {
                selenoidLahan2_open();
            }
            else
            {
                selenoidLahan2_close();
            }
        }
        else if (selenoidLahan3_update)
        {
            selenoidLahan3_update = false;
            if (selenoidLahan3_status)
            {
                selenoidLahan3_open();
            }
            else
            {
                selenoidLahan3_close();
            }
        }
        else if (selenoidLahan4_update)
        {
            selenoidLahan4_update = false;
            if (selenoidLahan4_status)
            {
                selenoidLahan4_open();
            }
            else
            {
                selenoidLahan4_close();
            }
        }
        else if (selenoidInletPestisida_update)
        {
            selenoidInletPestisida_update = false;
            if (selenoidInletPestisida_status)
            {
                selenoidInletPestisida_open();
            }
            else
            {
                selenoidInletPestisida_close();
            }
        }
        else if (selenoidInletBiopest_update)
        {
            selenoidInletBiopest_update = false;
            if (selenoidInletBiopest_status)
            {
                selenoidInletBiopest_open();
            }
            else
            {
                selenoidInletBiopest_close();
            }
        }
        else if (selenoidOutletPestisida_update)
        {
            selenoidOutletPestisida_update = false;
            if (selenoidOutletPestisida_status)
            {
                selenoidOutletPestisida_open();
            }
            else
            {
                selenoidOutletPestisida_close();
            }
        }
        else if (selenoidOutletBiopest_update)
        {
            selenoidOutletBiopest_update = false;
            if (selenoidOutletBiopest_status)
            {
                selenoidOutletBiopest_open();
            }
            else
            {
                selenoidOutletBiopest_close();
            }
        }


    }


        if (pompaUtama_update)
        {
            pompaUtama_update = false;
            if (pompaUtama_status)
            {
                pompa_start();
            }
            else
            {
                pompa_stop();
            }
        }
        else if (pompaSemprot_update)
        {
            pompaSemprot_update = false;
            if (pompaSemprot_status)
            {
                pompaSemprot_start();
            }
            else
            {
                pompaSemprot_stop();
            }
        }
        else if (pompaPestisida_update)
        {
            pompaPestisida_update = false;
            if (pompaPestisida_status)
            {
                pompaPestisida_start();
            }
            else
            {
                pompaPestisida_stop();
            }
        }
        else if (pompaBiopest_update)
        {
            pompaBiopest_update = false;
            if (pompaBiopest_status)
            {
                pompaBiopest_start();
            }
            else
            {
                pompaBiopest_stop();
            }
        }
        else if (pompaAdukPestisida_update)
        {
            pompaAdukPestisida_update = false;
            if (pompaAdukPestisida_status)
            {
                pompaAdukPestisida_start();
            }
            else
            {
                pompaAdukPestisida_stop();
            }
        }
        else if (pompaAdukBiopest_update)
        {
            pompaAdukBiopest_update = false;
            if (pompaAdukBiopest_status)
            {
                pompaAdukBiopest_start();
            }
            else
            {
                pompaAdukBiopest_stop();
            }
        }
        else if (selenoidLahan1_update)
        {
            selenoidLahan1_update = false;
            if (selenoidLahan1_status)
            {
                selenoidLahan1_open();
            }
            else
            {
                selenoidLahan1_close();
            }
        }
        else if (selenoidLahan2_update)
        {
            selenoidLahan2_update = false;
            if (selenoidLahan2_status)
            {
                selenoidLahan2_open();
            }
            else
            {
                selenoidLahan2_close();
            }
        }
        else if (selenoidLahan3_update)
        {
            selenoidLahan3_update = false;
            if (selenoidLahan3_status)
            {
                selenoidLahan3_open();
            }
            else
            {
                selenoidLahan3_close();
            }
        }
        else if (selenoidLahan4_update)
        {
            selenoidLahan4_update = false;
            if (selenoidLahan4_status)
            {
                selenoidLahan4_open();
            }
            else
            {
                selenoidLahan4_close();
            }
        }
        else if (selenoidInletPestisida_update)
        {
            selenoidInletPestisida_update = false;
            if (selenoidInletPestisida_status)
            {
                selenoidInletPestisida_open();
            }
            else
            {
                selenoidInletPestisida_close();
            }
        }
        else if (selenoidInletBiopest_update)
        {
            selenoidInletBiopest_update = false;
            if (selenoidInletBiopest_status)
            {
                selenoidInletBiopest_open();
            }
            else
            {
                selenoidInletBiopest_close();
            }
        }
        else if (selenoidOutletPestisida_update)
        {
            selenoidOutletPestisida_update = false;
            if (selenoidOutletPestisida_status)
            {
                selenoidOutletPestisida_open();
            }
            else
            {
                selenoidOutletPestisida_close();
            }
        }
        else if (selenoidOutletBiopest_update)
        {
            selenoidOutletBiopest_update = false;
            if (selenoidOutletBiopest_status)
            {
                selenoidOutletBiopest_open();
            }
            else
            {
                selenoidOutletBiopest_close();
            }
        }

        if (pompaUtama_update)
        {
            pompaUtama_update = false;
            if (pompaUtama_status)
            {
                pompa_start();
            }
            else
            {
                pompa_stop();
            }
        }
        else if (pompaSemprot_update)
        {
            pompaSemprot_update = false;
            if (pompaSemprot_status)
            {
                pompaSemprot_start();
            }
            else
            {
                pompaSemprot_stop();
            }
        }
        else if (pompaPestisida_update)
        {
            pompaPestisida_update = false;
            if (pompaPestisida_status)
            {
                pompaPestisida_start();
            }
            else
            {
                pompaPestisida_stop();
            }
        }
        else if (pompaBiopest_update)
        {
            pompaBiopest_update = false;
            if (pompaBiopest_status)
            {
                pompaBiopest_start();
            }
            else
            {
                pompaBiopest_stop();
            }
        }
        else if (pompaAdukPestisida_update)
        {
            pompaAdukPestisida_update = false;
            if (pompaAdukPestisida_status)
            {
                pompaAdukPestisida_start();
            }
            else
            {
                pompaAdukPestisida_stop();
            }
        }
        else if (pompaAdukBiopest_update)
        {
            pompaAdukBiopest_update = false;
            if (pompaAdukBiopest_status)
            {
                pompaAdukBiopest_start();
            }
            else
            {
                pompaAdukBiopest_stop();
            }
        }
        else if (selenoidLahan1_update)
        {
            selenoidLahan1_update = false;
            if (selenoidLahan1_status)
            {
                selenoidLahan1_open();
            }
            else
            {
                selenoidLahan1_close();
            }
        }
        else if (selenoidLahan2_update)
        {
            selenoidLahan2_update = false;
            if (selenoidLahan2_status)
            {
                selenoidLahan2_open();
            }
            else
            {
                selenoidLahan2_close();
            }
        }
        else if (selenoidLahan3_update)
        {
            selenoidLahan3_update = false;
            if (selenoidLahan3_status)
            {
                selenoidLahan3_open();
            }
            else
            {
                selenoidLahan3_close();
            }
        }
        else if (selenoidLahan4_update)
        {
            selenoidLahan4_update = false;
            if (selenoidLahan4_status)
            {
                selenoidLahan4_open();
            }
            else
            {
                selenoidLahan4_close();
            }
        }
        else if (selenoidInletPestisida_update)
        {
            selenoidInletPestisida_update = false;
            if (selenoidInletPestisida_status)
            {
                selenoidInletPestisida_open();
            }
            else
            {
                selenoidInletPestisida_close();
            }
        }
        else if (selenoidInletBiopest_update)
        {
            selenoidInletBiopest_update = false;
            if (selenoidInletBiopest_status)
            {
                selenoidInletBiopest_open();
            }
            else
            {
                selenoidInletBiopest_close();
            }
        }
        else if (selenoidOutletPestisida_update)
        {
            selenoidOutletPestisida_update = false;
            if (selenoidOutletPestisida_status)
            {
                selenoidOutletPestisida_open();
            }
            else
            {
                selenoidOutletPestisida_close();
            }
        }
        else if (selenoidOutletBiopest_update)
        {
            selenoidOutletBiopest_update = false;
            if (selenoidOutletBiopest_status)
            {
                selenoidOutletBiopest_open();
            }
            else
            {
                selenoidOutletBiopest_close();
            }
        }

    */
    }
}

void aktuator_init()
{

    // pinMode(POMPA_PIN,OUTPUT);
    // pinMode(SELENOID_INPUT_AIR,OUTPUT);

    // pinMode(SELENOID_LAHAN1,OUTPUT);
    // pinMode(SELENOID_LAHAN2,OUTPUT);
    // pinMode(SELENOID_LAHAN3,OUTPUT);
    // pinMode(SELENOID_LAHAN4,OUTPUT);

    // pinMode(POMPA_PESTISIDA,OUTPUT);
    // pinMode(POMPA_BIOPEST,OUTPUT);
    // pinMode(POMPA_ADUK_PESTISIDA,OUTPUT);
    // pinMode(POMPA_ADUK_PESTISIDA,OUTPUT);

    // pinMode(SELENOID_INLET_CAMPURAN,OUTPUT);
    // pinMode(SELENOID_OUTLET_CAMPURAN,OUTPUT);

    /*
        ESP32PWM::allocateTimer(3);
        servoInletCampuran.setPeriodHertz(50);// Standard 50hz ser
        servoOutletCampuran.setPeriodHertz(50);

        servoInletCampuran.attach(SELENOID_INLET_CAMPURAN, 1000, 2000);
        servoOutletCampuran.attach(SELENOID_OUTLET_CAMPURAN, 1000, 2000);

        servoInlet_pos = 90;
        servoOutlet_pos = 90;

        servoInlet_enable = true;
        servoOutlet_enable = true;
        */
    Serial.println("aktuator Init");
}

void aktuator_off_semua()
{
    /*
    pompa_stop();
    pompaPestisida_stop();
    pompaBiopest_stop();
    pompaAdukPestisida_stop();
    pompaAdukBiopest_stop();
    selenoidInletUtama_close();
    //selenoidInletCampuran_close();
    //selenoidOutletCampuran_close();
    selenoidLahan1_close();
    selenoidLahan2_close();
    selenoidLahan3_close();
    selenoidLahan4_close();
    */
    // setFlowRate_stop();

    selenoidLahan1_status = false;
    selenoidLahan2_status = false;
    selenoidLahan3_status = false;
    selenoidLahan4_status = false;

    selenoidLahan1_update = true;
    selenoidLahan2_update = true;
    selenoidLahan3_update = true;
    selenoidLahan4_update = true;

    selenoidInletPestisida_status = false;
    selenoidInletBiopest_status = false;
    selenoidOutletPestisida_status = false;
    selenoidOutletBiopest_status = false;

    selenoidInletPestisida_update = true;
    selenoidInletBiopest_update = true;
    selenoidOutletPestisida_update = true;
    selenoidOutletBiopest_update = true;

    pompaPestisida_status = false;
    pompaBiopest_status = false;
    pompaPestisida_update = true;
    pompaBiopest_update = true;

    pompaAdukPestisida_status = false;
    pompaAdukBiopest_status = false;
    pompaAdukPestisida_update = true;
    pompaAdukBiopest_update = true;

    pompaSemprot_status = false;
    pompaSemprot_update = true;

    pompaUtama_status = false;
    pompaUtama_update = true;

    flowRateSensor_status = false;
    flowRateSensor_update = true;
}
