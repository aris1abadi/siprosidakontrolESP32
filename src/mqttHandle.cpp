/*
set jam
D000:SJM*th*bl*tg*jm*mn*dt*

penggantian format pesan

topic subscript bsip-in/+/+/+
bsip-in/2002/type(siram/pompa/selenoid/sensor)/nomer device/cmd

>>siram semua
bsip-in/2002/siram/0/cmd --payload 1/0

>> siram lahan 1
bsip-in/2002/siram/1/cmd --payload 1/0

>>set selsnoid valve
bsip-in/2002/selenoid/1/set --payload 1-180

format pesan
*/

#define USE_PESTISIDA 0
#define USE_BIOPEST 180


#include <Arduino.h>
// #include <ESP8266WiFi.h>
#include <WiFi.h>
#ifdef USE_LOCAL_OTA
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ElegantOTA.h>
#endif
#include <Ticker.h>
#include <AsyncMqttClient.h>
#include <mqttHandle.h>
#include <siramHandle.h>
#include <pestisidaHandle.h>
#include <biopestHandle.h>
#include <aktuatorHandle.h>
#include <eepromHandle.h>
#include <mySensorsHandle.h>
#include <otaHandle.h>
#include <timeHandle.h>

// #define MQTT_DEBUG
#define RECONECT_WIFI_TIME 10 // detik
#define RECONECT_MQTT_TIME 10

extern String kontrolID;
extern const char *firmwareVersion;
extern String ssid;
extern String pass;
extern String mqtt_host;
extern int mqtt_port;


//  Set your Static IP address
#ifdef USE_LOCAL_OTA
AsyncWebServer server(80);
IPAddress local_IP(192, 168, 0, 51);
//  Set your Gateway IP address
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

IPAddress primaryDNS(8, 8, 8, 8);   // optional
IPAddress secondaryDNS(8, 8, 4, 4); // optional
#endif

AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;
Ticker wifiReconnectTimer;
Ticker rebootTimer;

// WiFiEventHandler wifiConnectHandler;
// WiFiEventHandler wifiDisconnectHandler;

// 

// variable esternal

bool wifi_status = false;
bool mqtt_status = false;

extern String jadwalSiram;
extern String jadwalPestisida;
extern String jadwalBiopest;

extern float volumeCampuranPestisida;
extern uint8_t volumeAirPestisida;
unsigned long mqtt_delay = 0;
unsigned long heartbeat_time = 10000;
unsigned int heartBeat_val = 0;

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

#ifdef USE_LOCAL_OTA
unsigned long ota_progress_millis = 0;


void onOTAStart() {
  // Log when OTA has started
  Serial.println("OTA update started!");
  // <Add your own code here>
}

void onOTAProgress(size_t current, size_t final) {
  // Log every 1 second
  if (millis() - ota_progress_millis > 1000) {
    ota_progress_millis = millis();
    Serial.printf("OTA Progress Current: %u bytes, Final: %u bytes\n", current, final);
  }
}

void onOTAEnd(bool success) {
  // Log when OTA has finished
  if (success) {
    Serial.println("OTA update finished successfully!");
  } else {
    Serial.println("There was an error during OTA update!");
  }
  // <Add your own code here>
}


void localOta_init(){
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Hai,saya SIPROSIDA");
  });

  ElegantOTA.begin(&server);    // Start ElegantOTA
  // ElegantOTA callbacks
  ElegantOTA.onStart(onOTAStart);
  ElegantOTA.onProgress(onOTAProgress);
  ElegantOTA.onEnd(onOTAEnd);

  server.begin();
  Serial.print("OTA pada IP: ");
  Serial.println(WiFi.localIP());
}
#endif

void sendHeatBeat()
{
  ++heartBeat_val;
  kirimKeApp("kontrol", 0, "heartBeat", String(heartBeat_val));
}
void mqtt_loop()
{
  if ((millis() - mqtt_delay) > heartbeat_time)
  {
    mqtt_delay = millis();
    if (mqtt_status)
    {
      sendHeatBeat();
    }
  }
  #ifdef USE_LOCAL_OTA
  ElegantOTA.loop();
  #endif
}

String getStatusNow()
{
  String out = "-";

  switch (getRunMode())
  {
  case 0:
    out = "0,0,0,0,0,";
    break;
  case 1: // siram
    out = getAllSiram_status();
    break;
  case 2:
    out = getAllPestisida_status();
    break;
  case 3:
    out = getAllBiopest_status();
    break;
  }
  String rm = String(getRunMode());
  rm += ',';
  rm += out.substring(2, out.length());

  return rm;
}
void rebootNow()
{
  ESP.restart();
}
void cekMqttMsg(String mqTopic, String mqMsg)
{
  //           0123456
  // Serial.println(mqTopic);
  // format pesan
  // topic  bsip-in/2002/type/nomerId/cmd
  //
  String devType = getValue(mqTopic, '/', 2);
  int nomerId = (getValue(mqTopic, '/', 3)).toInt();
  String mqCmd = getValue(mqTopic, '/', 4);

  if (devType == "kontrol")
  {
    if (mqCmd == "cerah")
    {
      String msgCmd = "cerah=";
      msgCmd += mqMsg;
      Serial.print("SetDisplay brigtnes : ");
      Serial.println(mqMsg);
      // kirimKeNode(NODE_DISPLAY_OUTDOOR,mqMsg);
      // updateDisplayOutdoor(msgCmd);
    }
    else if (mqCmd == "setIntervalLog")
    {
      int val = mqMsg.toInt();
      if (val <= 60)
      {
        // simpanIntervalLog(val);
        Serial.print("Update Intervallog: ");
        Serial.print(val);
        Serial.println(" menit");
      }
      else
      {
        Serial.println("Interval terlalu lama( >60 menit");
      }
      /*
     float temp;
    float humidity;
    float curah_hujan;
    int co2Value;
    int ch4Value;
    int n2oValue;
    int nValue ;
    int pValue ;
    int kValue ;
    int lengas1 ;
    int lengas2 ;
    float phValue ;
    float ecValue ;

     */
    }
    else if (mqCmd == "getTempLog")
    {
      String dt = mqMsg.substring(0, (mqMsg.length() - 3));
      // long tmMq = atol(dt.c_str());
      //  String logData = loadDataLog(1, tmMq);
      //  kirimKeApp(devType, nomerId, "tempLog", logData);
      //  Serial.println(logData);
    }
    else if (mqCmd == "getHumLog")
    {
    }
    else if (mqCmd == "getCurahHujanLog")
    {
    }
    else if (mqCmd == "getCo2Log")
    {
    }
    else if (mqCmd == "getCh4Log")
    {
    }
    else if (mqCmd == "getN2oLog")
    {
    }
    else if (mqCmd == "getnValueLog")
    {
    }
    else if (mqCmd == "getpValueLog")
    {
    }
    else if (mqCmd == "getkValueLog")
    {
    }
    else if (mqCmd == "getLengas1Log")
    {
    }
    else if (mqCmd == "getLengas2Log")
    {
    }
    else if (mqCmd == "getphValueLog")
    {
    }
    else if (mqCmd == "getecValueLog")
    {
    }
    else if (mqCmd == "getIP")
    {
      kirimKeApp("kontrol", 1, "ip", WiFi.localIP().toString());
    }
    else if (mqCmd == "getJadwalSiram")
    {
      kirimKeApp("siram", 1, "jadwal", jadwalSiram);
    }
    else if (mqCmd == "getJadwalPestisida")
    {
      kirimKeApp("pestisida", 1, "jadwal", jadwalPestisida);
    }
    else if (mqCmd == "getJadwalBiopest")
    {
      kirimKeApp("biopest", 1, "jadwal", jadwalBiopest);
    }
    else if (mqCmd == "setKalibrasiAir")
    {
      Serial.print("set kalibrasi Air: ");
      Serial.println(mqMsg);

      uint8_t val = mqMsg.toInt();
      if (val > 50)
      {
        Serial.println("angka terlalu besar");
      }
      else
      {
        // kalibrasiAir = float(val / 10);
        // simpanKalibrasiAir(val);
      }
    }
    else if (mqCmd == "getAllStatus")
    {
      // runMode,s1,s2,s3,s4,p1,p2,p3,p4,b1,b2,b3,b4,
      kirimKeApp("kontrol", 0, "allStatus", getStatusNow());
    }
    else if (mqCmd == "reboot")
    {
      kirimKeApp("kontrol", 0, "rebootKontroller", "0");
      Serial.println("Reboot kontroller");
      rebootTimer.once_ms(3000, rebootNow);
    }
    else if (mqCmd == "getTime")
    {
      kirimKeApp("kontrol", 0, "time", getTimeNow());
    }
    else if (mqCmd == "login")
    {
      // format login password,username,clientID,

      // format respon status loginstatus(0/1),-/username,clientID(jika login dari perangkat baru),
      Serial.print("login password: ");
      Serial.println(mqMsg);
    }
    else if (mqCmd == "savePass")
    {
      Serial.print("save password: ");
      Serial.println(mqMsg);
    }
  }
  else if (devType == "pompa")
  {
    if (mqCmd == "utama")
    {
      if (mqMsg.toInt() == 1)
      {
        pompa_start();
      }
      else
      {
        pompa_stop();
      }
    }
    else if (mqCmd == "semprot")
    {
      if (mqMsg.toInt() == 1)
      {
        pompaSemprot_start();
      }
      else
      {
        pompaSemprot_stop();
      }
    }
    else if (mqCmd == "pestisida")
    {
      if (mqMsg.toInt() == 1)
      {
        pompaPestisida_start();
      }
      else
      {
        pompaPestisida_stop();
      }
    }
    else if (mqCmd == "adukPestisida")
    {
      if (mqMsg.toInt() == 1)
      {
        pompaAdukPestisida_start();
      }
      else
      {
        pompaAdukPestisida_stop();
      }
    }
    else if (mqCmd == "biopest")
    {
      if (mqMsg.toInt() == 1)
      {
        pompaBiopest_start();
      }
      else
      {
        pompaBiopest_stop();
      }
    }
    else if (mqCmd == "adukBiopest")
    {
      if (mqMsg.toInt() == 1)
      {
        pompaAdukBiopest_start();
      }
      else
      {
        pompaAdukBiopest_stop();
      }
    }
  }
  else if (devType == "selenoid")
  {
    if (mqCmd == "lahan")
    {
      switch (nomerId)
      {
      case 0:
        if (mqMsg.toInt() == 1)
        {
          selenoidLahan1_status = true;
          selenoidLahan2_status = true;
          selenoidLahan3_status = true;
          selenoidLahan4_status = true;

          selenoidLahan1_update = true;
          selenoidLahan2_update = true;
          selenoidLahan3_update = true;
          selenoidLahan4_update = true;
        }
        else
        {
          selenoidLahan1_status = false;
          selenoidLahan2_status = false;
          selenoidLahan3_status = false;
          selenoidLahan4_status = false;

          selenoidLahan1_update = true;
          selenoidLahan2_update = true;
          selenoidLahan3_update = true;
          selenoidLahan4_update = true;
        }
        break;

      case 1:
        if (mqMsg.toInt() == 1)
        {
          selenoidLahan1_status = true;
          selenoidLahan1_update = true;
        }
        else
        {
          selenoidLahan1_status = false;
          selenoidLahan1_update = true;
        }
        break;

      case 2:
        if (mqMsg.toInt() == 1)
        {
          selenoidLahan2_status = true;
          selenoidLahan2_update = true;
        }
        else
        {
          selenoidLahan2_status = false;
          selenoidLahan2_update = true;
        }
        break;

      case 3:
        if (mqMsg.toInt() == 1)
        {
          selenoidLahan3_status = true;
          selenoidLahan3_update = true;
        }
        else
        {
          selenoidLahan3_status = false;
          selenoidLahan3_update = true;
        }
        break;

      case 4:
        if (mqMsg.toInt() == 1)
        {
          selenoidLahan4_status = true;
          selenoidLahan4_update = true;
        }
        else
        {
          selenoidLahan4_status = false;
          selenoidLahan4_update = true;
        }
        break;
      }
    }
    else if (mqCmd == "inletPestisida")
    {
      if (mqMsg.toInt() == 1)
      {
        selenoidInletPestisida_status = true;
        selenoidInletPestisida_update = true;
      }
      else
      {
        selenoidInletPestisida_status = false;
        selenoidInletPestisida_update = true;
      }
    }
    else if (mqCmd == "outletPestisida")
    {
      if (mqMsg.toInt() == 1)
      {
        selenoidOutletPestisida_status = true;
        selenoidOutletPestisida_update = true;
      }
      else
      {
        selenoidOutletPestisida_status = false;
        selenoidOutletPestisida_update = true;
      }
    }
    else if (mqCmd == "inletBiopest")
    {
      if (mqMsg.toInt() == 1)
      {
        selenoidInletBiopest_status = true;
        selenoidInletBiopest_update = true;
      }
      else
      {
        selenoidInletBiopest_status = false;
        selenoidInletBiopest_update = true;
      }
    }
    else if (mqCmd == "outletBiopest")
    {
      if (mqMsg.toInt() == 1)
      {
        selenoidOutletBiopest_status = true;
        selenoidOutletBiopest_update = true;
      }
      else
      {
        selenoidOutletBiopest_status = false;
        selenoidOutletBiopest_update = true;
      }
    }
    else if (mqCmd == "utama")
    {
      if (mqMsg.toInt() == 1)
      {
        selenoidInletUtama_open();
      }
      else
      {
        selenoidInletUtama_close();
      }
    }
  }
  else if (devType == "lengas")
  {
    if (mqCmd == "setInterval")
    {
      uint8_t nd = (nomerId + 50);
      kirim_ke_device(nd, 13, 24, mqMsg); // V_VAR1
    }
    else if (mqCmd == "set100")
    {
      uint8_t nd = (nomerId + 50);
      kirim_ke_device(nd, 13, 27, mqMsg); // V_VAR4
    }
    else if (mqCmd == "set0")
    {
      uint8_t nd = (nomerId + 50);
      kirim_ke_device(nd, 13, 26, mqMsg); // V_VAR3
    }
  }
  else if (devType == "siram")
  {
    if (mqCmd == "cmd")
    {
      if (mqMsg.toInt() == 1)
      {
        Serial.println("Proses siram Start");
        siram_start(nomerId, getDurasiSiram());
      }
      else
      {

        Serial.println("Proses siram Stop");
        siram_stop(nomerId);
      }
    }
    else if (mqCmd == "setJadwal")
    {
      Serial.println("jadwal diterima");
      simpanJadwalSiram(mqMsg);
      // simpanJadwal(1, mqMsg); // 1 id untuk siram
      kirimKeApp("siram", 0, "jadwalSiram", jadwalSiram);
    }
    else if (mqCmd == "getJadwal")
    {
      kirimKeApp("siram", 0, "jadwalSiram", jadwalSiram);
      // Serial.print("Jadwal Siram: ");
      // Serial.println(jadwalSiram);
    }
    else if (mqCmd == "useLengas")
    {
      simpanUseLengas(mqMsg.toInt());
      if (mqMsg.toInt() == 1)
      {
        setUseLengas(true);
      }
      else
      {
        setUseLengas(false);
      }

      // simpanUseLengas(useLengas);
    }
    else if (mqCmd == "setAmbang")
    {
      uint8_t val = mqMsg.toInt();
      if (val > 100)
      {
        Serial.println("Ambang  lengas 10-100%");
      }
      else
      {
        simpanAmbangLengas(val);
        kirimKeApp("siram", 0, "ambangLengas", String(getAmbangLengas()));
        Serial.print("setAmbang lengas: ");
        Serial.println(val);
      }
    }
    else if (mqCmd == "getAmbang")
    {
      kirimKeApp("siram", 0, "ambangLengas", String(getAmbangLengas()));
    }
    else if (mqCmd == "setDurasi")
    {
      simpanDurasiSiram(mqMsg.toInt());
      kirimKeApp("siram", 0, "durasiSiram", mqMsg);

      Serial.print("setDurasi siram: ");
      Serial.println(mqMsg.toInt());
    }
    else if (mqCmd == "getDurasi")
    {
      kirimKeApp("siram", 0, "durasiSiram", String(getDurasiSiram()));
    }
    else if (mqCmd == "getAllStatus")
    {
      // get status siram dan selenoid
      // format status siram,s1,s2,s3,s4 >>> 1,1,0,0,0

      kirimKeApp("siram", 0, "siramStatus", getAllSiram_status());
    }
  }
  else if (devType == "pestisida")
  {
    if (mqCmd == "cmd")
    {
      if (mqMsg.toInt() == 1)
      {

        // mulai proses pestisida
        prosesPestisida_start(nomerId);
        Serial.println("Pestisida start");
      }
      else
      {

        prosesPestisida_stop();
        Serial.println("Pestisida stop");
      }
    }
    else if (mqCmd == "setJadwal")
    {
      Serial.println("jadwal pestidsida diterima");
      simpanJadwalPestisida(mqMsg);
      // simpanJadwal(1, mqMsg); // 1 id untuk siram
      kirimKeApp("pestisida", 0, "jadwalPestisida", jadwalPestisida);
    }
    else if (mqCmd == "getJadwal")
    {
      kirimKeApp("pestisida", 0, "jadwalPestisida", jadwalPestisida);
      // Serial.print("kirim jadwal pestisida: ");
      // Serial.println(jadwalPestisida);
    }
    else if (mqCmd == "setDosisPestisida")
    {
      setDosisPestisida(mqMsg.toInt());
      kirimKeApp("pestisida", 0, "dosisPestisida", mqMsg);
      simpanDosisPestisida(mqMsg.toInt());
      Serial.print("dosis pestisida: ");
      Serial.print(mqMsg);
    }
    else if (mqCmd == "setDosisAirPestisida")
    {
      setDosisAirPestisida(mqMsg.toInt());
      kirimKeApp("pestisida", 0, "dosisAirPestisida", mqMsg);
      simpanDosisAirPestisida(mqMsg.toInt());
      Serial.print("dosis Air pestisida: ");
      Serial.print(mqMsg);
    }
    else if (mqCmd == "getStatus")
    {
      kirimKeApp("pestisida", 0, "pestisidaStatus", getAllPestisida_status());
    }
    else if (mqCmd == "buangPestisida")
    {
      if (mqMsg.toInt() == 1)
      {
        prosesBuangPestisida_start();
        Serial.println("buang pestisida start");
      }
      else
      {

        prosesBuangPestisida_stop();
        Serial.println("buang pestisida stop");
      }
    }
    else if (mqCmd == "kalibrasi")
    {
      kalibrasiPestisida_start(mqMsg.toInt());
    }
    else if (mqCmd == "simpanKalibrasi")
    {
      simpanKalibrasi_pestisida(mqMsg.toInt());
      kirimKeApp("pestisida", 0, "kalibrasi", mqMsg);
    }
    else if (mqCmd == "kalibrasiAir")
    {
      kalibrasiAirPestisida_start(mqMsg.toInt());
    }
    else if (mqCmd == "simpanKalibrasiAir")
    {
      simpanKalibrasi_airpestisida(mqMsg.toInt());
      kirimKeApp("pestisida", 0, "kalibrasiAir", mqMsg);
    }
    else if (mqCmd == "getKalibrasi")
    {
      String kal = String(getKalibrasi_pestisida());
      kal += ",";
      kal += String(getKalibrasi_air_pestisida());
      kal += ",";
      kirimKeApp("pestisida", 0, "kalibrasiValue", kal);
    }
  }
  else if (devType == "biopest")
  {
    if (mqCmd == "cmd")
    {
      if (mqMsg.toInt() == 1)
      {
        prosesBiopest_start(nomerId);
        Serial.println("biopest start");
      }
      else
      {

        prosesBiopest_stop();
      }
    }
    else if (mqCmd == "setJadwal")
    {
      Serial.println("jadwal Biopest diterima");
      simpanJadwalBiopest(mqMsg);
      // simpanJadwal(1, mqMsg); // 1 id untuk siram
      kirimKeApp("biopest", 0, "jadwalBiopest", jadwalBiopest);
    }
    else if (mqCmd == "getJadwal")
    {
      kirimKeApp("biopest", 0, "jadwalBiopest", jadwalBiopest);
    }
    else if (mqCmd == "setDosisBiopest")
    {
      setDosisBiopest(mqMsg.toInt());
      kirimKeApp("biopest", 0, "dosisBiopest", mqMsg);
      simpanDosisBiopest(mqMsg.toInt());
      Serial.print("Simpan dosis Biopest: ");
      Serial.println(mqMsg);
    }
    else if (mqCmd == "setDosisAirBiopest")
    {
      setDosisAirBiopest(mqMsg.toInt());
      kirimKeApp("biopest", 0, "dosisAirBiopest", mqMsg);
      simpanDosisAirBiopest(mqMsg.toInt());
      Serial.print("Simpan dosis Air Biopest: ");
      Serial.println(mqMsg);
    }
    else if (mqCmd == "getStatus")
    {
      kirimKeApp("biopest", 0, "biopestStatus", getAllBiopest_status());
    }
    else if (mqCmd == "buangBiopest")
    {
      if (mqMsg.toInt() == 1)
      {
        prosesBuangBiopest_start();
        Serial.println("buang biopest start");
      }
      else
      {

        prosesBuangBiopest_stop();
        Serial.println("buang biopest stop");
      }
    }
    else if (mqCmd == "kalibrasi")
    {
      kalibrasiBiopest_start(mqMsg.toInt());
    }
    else if (mqCmd == "simpanKalibrasi")
    {
      simpanKalibrasi_biopest(mqMsg.toInt());
      kirimKeApp("biopest", 0, "kalibrasi", mqMsg);
    }
    else if (mqCmd == "kalibrasiAir")
    {
      kalibrasiAirBiopest_start(mqMsg.toInt());
    }
    else if (mqCmd == "simpanKalibrasiAir")
    {
      simpanKalibrasi_airbiopest(mqMsg.toInt());
      kirimKeApp("biopest", 0, "kalibrasiAir", mqMsg);
    }
    else if (mqCmd == "getKalibrasi")
    {
      String kal = String(getKalibrasi_biopest());
      kal += ",";
      kal += String(getKalibrasi_air_biopest());
      kal += ",";
      kirimKeApp("biopest", 0, "kalibrasiValue", kal);
    }
  }
}
void connectToMqtt()
{
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}
void connectToWifi()
{
  Serial.println("Connecting to Wi-Fi...");
  // if (!WiFi.config(local_IP, gateway, subnet)) {
  //   Serial.println("STA Failed to configure");
  // }
  // Configures static IP address
  #ifdef USE_LOCAL_OTA
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS))
  {
    Serial.println("STA Failed to configure");
  }
  #endif

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), pass.c_str());
}
/*
void onWifiConnect(const WiFiEventStationModeGotIP &event)
{
  Serial.println("Connected to Wi-Fi.");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  connectToMqtt();
  wifi_status = true;
}
void onWifiDisconnect(const WiFiEventStationModeDisconnected &event)
{
  Serial.println("Disconnected from Wi-Fi.");
  wifi_status = false;
  mqtt_status = false;
  mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
  wifiReconnectTimer.once(RECONECT_WIFI_TIME, connectToWifi);
  // led_mode = 0;
}
*/
bool getWifi_sts()
{
  return wifi_status;
}
void onMqttConnect(bool sessionPresent)
{
  mqtt_status = true;
  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);
  String subTxt = "bsip-in/";
  subTxt += kontrolID;
  subTxt += "/#";
  uint16_t packetIdSub = mqttClient.subscribe(subTxt.c_str(), 2);
  Serial.print("Subscribing at QoS 2, packetId: ");
  Serial.println(packetIdSub);
  subTxt = "bsip-out/";
  subTxt += kontrolID;
  subTxt += "/kontrol/0/status";
  String k_id = "KontrolID ";
  k_id += kontrolID;
  k_id += " firmware Version ";
  k_id += firmwareVersion;
  mqttClient.publish(subTxt.c_str(), 0, true, k_id.c_str());
  Serial.println("Publishing at QoS 0");
  // ota online init
  ota_init();
  #ifdef USE_LOCAL_OTA
  localOta_init();
  #endif
  ntp_init();
  ntp_sync();
}
void kirimKeApp(String type, uint8_t nomer, String cmd, String msg)
{
  String topic_id = "bsip-out/";
  topic_id += kontrolID;
  topic_id += "/";
  topic_id += type;
  topic_id += "/";
  topic_id += String(nomer);
  topic_id += "/";
  topic_id += cmd;
  mqttClient.publish(topic_id.c_str(), 0, true, msg.c_str());
}
void onMqttDisconnect(AsyncMqttClientDisconnectReason reason)
{
  Serial.println("Disconnected from MQTT.");
  mqtt_status = false;

  if (WiFi.isConnected())
  {
    mqttReconnectTimer.once(RECONECT_MQTT_TIME, connectToMqtt);
  }
}
void onMqttSubscribe(uint16_t packetId, uint8_t qos)
{
  Serial.println("Subscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
  Serial.print("  qos: ");
  Serial.println(qos);
}
void onMqttUnsubscribe(uint16_t packetId)
{
  Serial.println("Unsubscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}
void onMqttMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
{
  String mqttMsg = "";
  for (unsigned int i = 0; i < len; i++)
  {
    mqttMsg += payload[i];
  }
  String mqttTopic = String(topic);

  cekMqttMsg(mqttTopic, mqttMsg);
#ifdef MQTT_DEBUG
  Serial.print("topic: ");
  Serial.println(mqttTopic);
  Serial.print("Rcv msg: ");
  Serial.println(mqttMsg);
#endif
}
void onMqttPublish(uint16_t packetId)
{
  Serial.println("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void WiFiEvent(WiFiEvent_t event)
{
  // Serial.printf("[WiFi-event] event: %d\n", event);
  switch (event)
  {
  case SYSTEM_EVENT_STA_GOT_IP:
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    wifi_status = true;
    connectToMqtt();
    break;
  case SYSTEM_EVENT_STA_DISCONNECTED:
    Serial.println("WiFi lost connection");
    wifi_status = false;
    mqtt_status = false;
    mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
    wifiReconnectTimer.once(RECONECT_WIFI_TIME, connectToWifi);
    break;
  }
}

void mqtt_init()
{
  Serial.println("network init");
  // WiFi.onEvent(WiFiEvent);
  // wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
  // wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);

  WiFi.onEvent(WiFiEvent);

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(mqtt_host.c_str(), mqtt_port);
  connectToWifi();
}
