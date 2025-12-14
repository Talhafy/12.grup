#ifndef QUEUE_H
#define QUEUE_H

#include "task.h"
#include <stdbool.h>

// Bağlı Liste Tabanlı Kuyruk
typedef struct {
    Task* bas;   // Head
    Task* son;   // Tail
    int boyut;   // Eleman sayısı
} Kuyruk;

void kuyruk_olustur(Kuyruk* k);
void kuyruk_ekle(Kuyruk* k, Task* t);
Task* kuyruk_cikar(Kuyruk* k);
bool kuyruk_bos_mu(Kuyruk* k);

// ÖZEL FONKSİYON: Bekleyenleri yaşlandır ve 20 sn dolanı sil
void kuyruk_yaslandir_ve_temizle(Kuyruk* k, int suanki_zaman);

#endif