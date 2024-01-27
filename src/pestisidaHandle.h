
#ifndef pestisidaHandle_h
#define pestisidaHandle_h

bool getPestisida_status();
void setPestisida_status(bool sts);
bool getPestisidaLahan1_status();
bool getPestisidaLahan2_status();
bool getPestisidaLahan3_status();
bool getPestisidaLahan4_status();
bool getIsiPestisida_status();
void setIsiPestisida_status(bool sts);
bool getIsiAirPestisida_status();
void setIsiAirPestisida_status(bool sts);

bool getSemprotPestisida_status();
void setSemprotPestisida_status(bool sts);

void prosesPestisida_start(uint8_t lahan);
void prosesPestisida_stop();
void prosesPestisidaKey_start();
void prosesBuangPestisida_start();
void prosesBuangPestisida_stop();
void setStatusLahanPestisida(uint8_t lahan, bool sts);

void setDosisPestisida(uint8_t dosis);
uint8_t getDosisPestisida();
void setDosisAirPestisida(uint8_t dosis);
float getDosisAirPestisida();

float getvolumeCampuranPestisida ();
void setvolumeCampuranPestisida(float vol);

bool getKalibrasi_pestisida_status();
bool getKalibrasi_air_pestisida_status();
void setKalibrasi_air_pestisida_status(bool sts);
bool getRunKalibrasi_air_pestisida();
bool getkalibrasi_air_pestisida_finish();

String getAllPestisida_status();
void cekJadwalPestisida();

void pestisida_loop();
void kalibrasiPestisida_start(uint8_t faktorKal);
void kalibrasiAirPestisida_start(uint8_t kal_value);
float getVolumeKalibrasiAir_pestisida();


#endif