#include "scheduler.h"
#include "sim_queue.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include <stdlib.h>

// Global Kuyruklar
static Kuyruk q_rt, q1, q2, q3;
static Task* calisan_gorev = NULL;

// Sembolik FreeRTOS Task Fonksiyonu
static void FreeRTOSTask(void* pv) {
    // Bu task sadece var olmak için var. 
    // Tüm mantık scheduler_calistir içinde yönetiliyor.
    // Böylece senkronizasyon %100 korunuyor.
    while(1) {
        vTaskSuspend(NULL);
    }
}

void scheduler_baslat() {
    kuyruk_olustur(&q_rt);
    kuyruk_olustur(&q1);
    kuyruk_olustur(&q2);
    kuyruk_olustur(&q3);
    printf("Scheduler baslatildi...\n");
}

void scheduler_gorev_kabul(Task* t) {
    t->bekleme_suresi = 0;
    t->yenimibasladi = false;
    t->durum = DURUM_HAZIR;

    // FreeRTOS Task Oluşturma (Sembolik)
    xTaskCreate(FreeRTOSTask, "PROC", 2048, t, tskIDLE_PRIORITY + 1, &t->handle);
    vTaskSuspend(t->handle); // Hemen durdur, kontrol bizde

    if (t->oncelik == 0) kuyruk_ekle(&q_rt, t);
    else if (t->oncelik == 1) kuyruk_ekle(&q1, t);
    else if (t->oncelik == 2) kuyruk_ekle(&q2, t);
    else {
        t->oncelik = 3;
        kuyruk_ekle(&q3, t);
    }
}

static Task* sec() {
    if (!kuyruk_bos_mu(&q_rt)) return kuyruk_cikar(&q_rt);
    if (!kuyruk_bos_mu(&q1)) return kuyruk_cikar(&q1);
    if (!kuyruk_bos_mu(&q2)) return kuyruk_cikar(&q2);
    if (!kuyruk_bos_mu(&q3)) return kuyruk_cikar(&q3);
    return NULL;
}

void scheduler_calistir(int zaman) {
    // 1. YAŞLANDIRMA
    kuyruk_yaslandir_ve_temizle(&q_rt, zaman);
    kuyruk_yaslandir_ve_temizle(&q1, zaman);
    kuyruk_yaslandir_ve_temizle(&q2, zaman);
    kuyruk_yaslandir_ve_temizle(&q3, zaman);

    // 2. PREEMPTION (KESME)
    if (calisan_gorev != NULL && calisan_gorev->oncelik > 0) {
        if (!kuyruk_bos_mu(&q_rt)) {
            printf("%.4f sn proses askida \t(id:%04d \t oncelik:%d \tkalan sure:%d sn)\n",
                   (float)zaman, calisan_gorev->id + 1, calisan_gorev->oncelik, calisan_gorev->kalan_sure);
            
            calisan_gorev->durum = DURUM_ASKIDA;
            
            if (calisan_gorev->oncelik == 1) kuyruk_ekle(&q1, calisan_gorev);
            else if (calisan_gorev->oncelik == 2) kuyruk_ekle(&q2, calisan_gorev);
            else kuyruk_ekle(&q3, calisan_gorev);
            
            calisan_gorev = NULL;
        }
    }

    // 3. SEÇİM
    if (!calisan_gorev) {
        calisan_gorev = sec();
        if (calisan_gorev) {
            printf("%.4f sn proses basladi \t(id:%04d \t oncelik:%d \tkalan sure:%d sn)\n",
                   (float)zaman, calisan_gorev->id + 1, calisan_gorev->oncelik, calisan_gorev->kalan_sure);
            calisan_gorev->yenimibasladi = true;
            calisan_gorev->durum = DURUM_CALISIYOR;
            calisan_gorev->bekleme_suresi = 0;
        }
    }

    // 4. YÜRÜTME
    if (calisan_gorev) {
        if (calisan_gorev->kalan_sure < calisan_gorev->patlama_suresi && calisan_gorev->yenimibasladi == false) {
             printf("%.4f sn proses yurutuluyor \t(id:%04d \t oncelik:%d \tkalan sure:%d sn)\n", 
                 (float)zaman, calisan_gorev->id + 1, calisan_gorev->oncelik, calisan_gorev->kalan_sure);
        }

        // 🔥 İŞLEM BURADA YAPILIYOR (Task içinde değil, senkronizasyon için)
        calisan_gorev->kalan_sure--;
        calisan_gorev->yenimibasladi = false;

        // BİTİŞ KONTROLÜ
        if (calisan_gorev->kalan_sure <= 0) {
            calisan_gorev->durum = DURUM_TAMAMLANDI;
            printf("%.4f sn proses sonlandi \t(id:%04d \t oncelik:%d \tkalan sure:0 sn)\n",
                   (float)zaman + 1, calisan_gorev->id + 1, calisan_gorev->oncelik);
            
            vTaskDelete(calisan_gorev->handle);
            free(calisan_gorev);
            calisan_gorev = NULL;
        }
        // QUANTUM KONTROLÜ
        else if (calisan_gorev->oncelik > 0) {
            int eski_oncelik = calisan_gorev->oncelik;
            int yeni_oncelik = eski_oncelik + 1;

            printf("%.4f sn proses askida \t(id:%04d \t oncelik:%d \tkalan sure:%d sn)\n",
                (float)zaman + 1, calisan_gorev->id + 1, yeni_oncelik, calisan_gorev->kalan_sure);

            calisan_gorev->oncelik = yeni_oncelik;
            calisan_gorev->durum = DURUM_ASKIDA;
            
            if (yeni_oncelik == 1) kuyruk_ekle(&q1, calisan_gorev);
            else if (yeni_oncelik == 2) kuyruk_ekle(&q2, calisan_gorev);
            else kuyruk_ekle(&q3, calisan_gorev);
            
            calisan_gorev = NULL;
        }
    }
}

bool scheduler_bitti_mi() {
    return !calisan_gorev &&
           kuyruk_bos_mu(&q_rt) &&
           kuyruk_bos_mu(&q1) &&
           kuyruk_bos_mu(&q2) &&
           kuyruk_bos_mu(&q3);
}
