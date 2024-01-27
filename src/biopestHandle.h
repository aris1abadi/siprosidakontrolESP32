#ifndef biopestHandle_h
#define biopestHandle_h

bool getBiopest_status();
void setBiopest_status(bool sts);
bool getBiopestLahan1_status();
bool getBiopestLahan2_status();
bool getBiopestLahan3_status();
bool getBiopestLahan4_status();
bool getIsiBiopest_status();
void setIsiBiopest_status(bool sts);
bool getIsiAirBiopest_status();
void setIsiAirBiopest_status(bool sts);

bool getSemprotBiopest_status();
void setSemprotBiopest_status(bool sts);

void prosesBiopest_start(uint8_t lahan);
void prosesBiopestKey_start();
void prosesBiopest_stop();
void prosesBuangBiopest_start();
void prosesBuangBiopest_stop();
void setStatusLahanBiopest(uint8_t lahan, bool sts);

void setDosisBiopest(uint8_t dosis);
uint8_t getDosisBiopest();
void setDosisAirBiopest(uint8_t dosis);
float getDosisAirBiopest();

float getvolumeCampuranBiopest();
void setvolumeCampuranBiopest(float vol);

bool getKalibrasi_biopest_status();
bool getKalibrasi_air_biopest_status();
void setKalibrasi_air_biopest_status(bool sts);
bool getRunKalibrasi_air_biopest();
bool getkalibrasi_air_biopest_finish();

String getAllBiopest_status();
void cekJadwalBiopest();

void biopest_loop();
void kalibrasiBiopest_start(uint8_t faktorKal);
void kalibrasiAirBiopest_start(uint8_t kal_value);
float getVolumeKalibrasiAir_biopest();

#endif