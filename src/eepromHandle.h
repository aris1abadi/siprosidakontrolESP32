#ifndef eepromHandle_h
#define eepromHandle_h

void eeprom_init();
void loadDurasiSiram();

void simpanWIFI(String ssid,String pass);
void loadWIFI();

void loadBroker();
void simpanBroker(String broker,String pass);

void simpanDurasiSiram(uint8_t dur);
uint8_t getDurasiSiram();

void simpanAmbangLengas(uint8_t val);
uint8_t getAmbangLengas();
void loadAmbangLengas();

void simpanUseLengas(uint8_t ln);
void setUseLengas(bool val);
bool getUseLengas();

uint8_t getRunMode();
void setRunMode(uint8_t val);
void loadJadwalSiram();
void simpanJadwalSiram(String jadwal);
void loadJadwalPestisida();
void simpanJadwalPestisida(String jadwal);
void loadJadwalBiopest();
void simpanJadwalBiopest(String jadwal);
String getValue(String data, char separator, int index);
void loadDefault();
void loadUseLengas();
String jadwal2lcd(uint8_t jadwal,uint8_t no_jw,uint8_t line);
//String getID();

void simpanDosisPestisida(uint8_t dosis);
void simpanDosisAirPestisida(uint8_t dosis);
void simpanDosisBiopest(uint8_t dosis);
void simpanDosisAirBiopest(uint8_t dosis);
void loadDosisPestisida();
void loadDosisAirPestisida();
void loadDosisBiopest();
void loadDosisAirBiopest();

void simpanKalibrasi_pestisida(uint8_t kal);
void simpanKalibrasi_airpestisida(uint8_t kal);
void simpanKalibrasi_biopest(uint8_t kal);
void simpanKalibrasi_airbiopest(uint8_t kal);

void setKalibrasi_pestisida(uint8_t kal);
uint8_t getKalibrasi_pestisida();
void setKalibrasi_air_pestisida(uint8_t kal);
uint8_t getKalibrasi_air_pestisida();
void setKalibrasi_biopest(uint8_t kal);
uint8_t getKalibrasi_biopest();
void setKalibrasi_air_biopest(uint8_t kal);
uint8_t getKalibrasi_air_biopest();

void loadKalibrasi();

#endif