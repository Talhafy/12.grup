#include "sim_queue.h"
#include <stdio.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"

void kuyruk_olustur(Kuyruk* k) {
    k->bas = NULL;
    k->son = NULL;
    k->boyut = 0;
}

void kuyruk_ekle(Kuyruk* k, Task* t) {
    t->sonraki = NULL;
    if (k->son == NULL) {
        k->bas = t;
        k->son = t;
    } else {
        k->son->sonraki = t;
        k->son = t;
    }
    k->boyut++;
}

Task* kuyruk_cikar(Kuyruk* k) {
    if (k->bas == NULL) return NULL;
    Task* t = k->bas;
    k->bas = t->sonraki;
    if (k->bas == NULL) k->son = NULL;
    k->boyut--;
    return t;
}

bool kuyruk_bos_mu(Kuyruk* k) {
    return k->boyut == 0;
}

void kuyruk_yaslandir_ve_temizle(Kuyruk* k, int suanki_zaman) {
    if (k->bas == NULL) return;

    Task* onceki = NULL;
    Task* suanki = k->bas;

    while (suanki != NULL) {
        suanki->bekleme_suresi++;

        // 20 sn bekleme kuralı
        if (suanki->bekleme_suresi >= 21) {
            printf("%.4f sn proses zamanasimi \t(id:%04d \t oncelik:%d \tkalan sure:%d sn)\n",
                   (float)suanki_zaman, suanki->id + 1, suanki->oncelik, suanki->kalan_sure); // Çıktı formatı düzeltildi

            Task* silinecek = suanki;

            if (onceki == NULL) {
                k->bas = suanki->sonraki;
                if (k->bas == NULL) k->son = NULL;
                suanki = k->bas;
            } else {
                onceki->sonraki = suanki->sonraki;
                if (suanki->sonraki == NULL) k->son = onceki;
                suanki = onceki->sonraki;
            }

            k->boyut--;
            
            // 🔥 FreeRTOS Task'ını da siliyoruz
            if (silinecek->handle != NULL) {
                vTaskDelete(silinecek->handle);
            }
            free(silinecek);
        } else {
            onceki = suanki;
            suanki = suanki->sonraki;
        }
    }
}
