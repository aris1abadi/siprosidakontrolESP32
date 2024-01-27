#ifndef AKTUATORHANDLE_H
#define AKTUATORHANDLE_H

void aktuator_init();
void aktuator_loop();

void pompa_start();
void pompa_stop();

void selenoidLahan1_open();
void selenoidLahan1_close();

void selenoidLahan2_open();
void selenoidLahan2_close();

void selenoidLahan3_open();
void selenoidLahan3_close();

void selenoidLahan4_open();
void selenoidLahan4_close();

void pompaPestisida_start();
void pompaPestisida_stop();

void pompaBiopest_start();
void pompaBiopest_stop();

void pompaSemprot_start();
void pompaSemprot_stop();

void selenoidInletPestisida_open();
void selenoidInletPestisida_close();

void selenoidOutletPestisida_open();
void selenoidOutletPestisida_close();

void selenoidInletBiopest_open();
void selenoidInletBiopest_close();

void selenoidOutletBiopest_open();
void selenoidOutletBiopest_close();

void selenoidInletUtama_open();
void selenoidInletUtama_close();

void pompaAdukPestisida_start();
void pompaAdukPestisida_stop();

void pompaAdukBiopest_start();
void pompaAdukBiopest_stop();


void aktuator_off_semua();
void aktuator_loop();


#endif