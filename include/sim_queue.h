#ifndef QUEUE_H
#define QUEUE_H

#include "sim_task.h"
#include <stdbool.h>

typedef struct {
    Task* bas;
    Task* son;
    int boyut;
} Kuyruk;

void kuyruk_olustur(Kuyruk* k);
void kuyruk_ekle(Kuyruk* k, Task* t);
Task* kuyruk_cikar(Kuyruk* k);
bool kuyruk_bos_mu(Kuyruk* k);
void kuyruk_yaslandir_ve_temizle(Kuyruk* k, int suanki_zaman);

#endif
