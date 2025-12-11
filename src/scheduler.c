#include "scheduler.h"
#include "queue.h"
#include <stdio.h>
#include <stdlib.h>

// Global Kuyruklar
Kuyruk q_rt; // Priority 0 (Real Time)
Kuyruk q_1;  // Priority 1 (High)
Kuyruk q_2;  // Priority 2 (Medium)
Kuyruk q_3;  // Priority 3 (Low)

Task* calisan_gorev = NULL;

void scheduler_baslat() {
    kuyruk_olustur(&q_rt);
    kuyruk_olustur(&q_1);
    kuyruk_olustur(&q_2);
    kuyruk_olustur(&q_3);
    printf("Scheduler baslatildi...\n");
}

void scheduler_gorev_kabul(Task* t) {
    t->durum = DURUM_HAZIR;
    t->bekleme_suresi = 0;
    
    // Görevleri uygun kuyruğa yerleştir
    switch (t->oncelik) {
        case 0: kuyruk_ekle(&q_rt, t); break;
        case 1: kuyruk_ekle(&q_1, t); break;
        case 2: kuyruk_ekle(&q_2, t); break;
        default: 
            t->oncelik = 3; 
            kuyruk_ekle(&q_3, t); 
            break;
    }
}

// Yardımcı Fonksiyon: En yüksek öncelikli kuyruktan görev çek
Task* sonraki_gorevi_sec() {
    if (!kuyruk_bos_mu(&q_rt)) return kuyruk_cikar(&q_rt);
    if (!kuyruk_bos_mu(&q_1))  return kuyruk_cikar(&q_1);
    if (!kuyruk_bos_mu(&q_2))  return kuyruk_cikar(&q_2);
    if (!kuyruk_bos_mu(&q_3))  return kuyruk_cikar(&q_3);
    return NULL;
}

void scheduler_calistir(int saniye) {
    // 1. ADIM: ZAMANAŞIMI KONTROLÜ (Aging)
    // Bekleyen görevlerin süresini artır, 20 sn olanı sil
    kuyruk_yaslandir_ve_temizle(&q_rt, saniye);
    kuyruk_yaslandir_ve_temizle(&q_1, saniye);
    kuyruk_yaslandir_ve_temizle(&q_2, saniye);
    kuyruk_yaslandir_ve_temizle(&q_3, saniye);

    // 2. ADIM: PREEMPTION (Kesme)
    // Eğer çalışan görev Kullanıcı Görevi ise ve RT kuyruğuna görev geldiyse kes.
    if (calisan_gorev != NULL && calisan_gorev->oncelik > 0) {
        if (!kuyruk_bos_mu(&q_rt)) {
            printf("%.4f sn task%d askida \t(id:%04d \t oncelik:%d \tkalan sure:%d sn)\n",
                   (float)saniye, calisan_gorev->id + 1, calisan_gorev->id, calisan_gorev->oncelik, calisan_gorev->kalan_sure);
            
            calisan_gorev->durum = DURUM_ASKIDA;
            calisan_gorev->bekleme_suresi = 0;
            
            // Kaldığı yere (kendi önceliğinin sonuna) geri dön
            switch(calisan_gorev->oncelik) {
                case 1: kuyruk_ekle(&q_1, calisan_gorev); break;
                case 2: kuyruk_ekle(&q_2, calisan_gorev); break;
                case 3: kuyruk_ekle(&q_3, calisan_gorev); break;
            }
            calisan_gorev = NULL;
        }
    }

    // 3. ADIM: GÖREV SEÇİMİ (CPU Boşsa)
    if (calisan_gorev == NULL) {
        calisan_gorev = sonraki_gorevi_sec();
        
        if (calisan_gorev != NULL) {
            calisan_gorev->durum = DURUM_CALISIYOR;
            calisan_gorev->bekleme_suresi = 0; // Çalışan beklemez

            // Görev ilk kez mi başlıyor?
            if (calisan_gorev->patlama_suresi == calisan_gorev->kalan_sure) {
                 printf("%.4f sn task%d basladi \t(id:%04d \t oncelik:%d \tkalan sure:%d sn)\n", 
                    (float)saniye, calisan_gorev->id + 1, calisan_gorev->id, calisan_gorev->oncelik, calisan_gorev->kalan_sure);
            }
        }
    }

    // 4. ADIM: YÜRÜTME
    if (calisan_gorev != NULL) {
        // Eğer görev yeni başlamadıysa "yürütülüyor" mesajı ver
        if (calisan_gorev->patlama_suresi != calisan_gorev->kalan_sure) {
             printf("%.4f sn task%d yurutuluyor \t(id:%04d \t oncelik:%d \tkalan sure:%d sn)\n", 
                (float)saniye, calisan_gorev->id + 1, calisan_gorev->id, calisan_gorev->oncelik, calisan_gorev->kalan_sure);
        }

        // 1 saniye çalıştır
        calisan_gorev->kalan_sure--;

        // Durum Kontrolü
        if (calisan_gorev->kalan_sure <= 0) {
            // GÖREV BİTTİ
            calisan_gorev->durum = DURUM_TAMAMLANDI;
            printf("%.4f sn task%d sonlandi \t(id:%04d \t oncelik:%d \tkalan sure:0 sn)\n", 
                (float)(saniye + 1), calisan_gorev->id + 1, calisan_gorev->id, calisan_gorev->oncelik);
            free(calisan_gorev); // Temizle
            calisan_gorev = NULL;
        }
        else if (calisan_gorev->oncelik > 0) {
            // KULLANICI GÖREVİ QUANTUM DOLDU (1 sn)
            // Öncelik düşür ve askıya al
            int eski_oncelik = calisan_gorev->oncelik;
            int yeni_oncelik = eski_oncelik;
            
            if (yeni_oncelik < 3) yeni_oncelik++; // Priority 3'ten daha düşemez

            printf("%.4f sn task%d askida \t(id:%04d \t oncelik:%d \tkalan sure:%d sn)\n",
                (float)(saniye + 1), calisan_gorev->id + 1, calisan_gorev->id, yeni_oncelik, calisan_gorev->kalan_sure); // Burada bir sonraki önceliği mi yoksa anlık önceliği mi göstereceği tartışmalıdır, örnek çıktıya göre genelde o anki durumu basar. Logunuzda "task1 askıda ... öncelik:2" yazıyor, demek ki yeni önceliği basıyor.

            calisan_gorev->oncelik = yeni_oncelik; // Önceliği güncelle
            calisan_gorev->durum = DURUM_ASKIDA;
            calisan_gorev->bekleme_suresi = 0;
            
            // Yeni önceliğine göre kuyruğa at
            switch (yeni_oncelik) {
                case 1: kuyruk_ekle(&q_1, calisan_gorev); break; // Teorik olarak buraya gelmez
                case 2: kuyruk_ekle(&q_2, calisan_gorev); break;
                case 3: kuyruk_ekle(&q_3, calisan_gorev); break;
            }
            calisan_gorev = NULL;
        }
        // RT görevleri (Priority 0) preempt edilmediği sürece bitene kadar çalışır.
    }
}

bool scheduler_bitti_mi() {
    return (calisan_gorev == NULL && 
            kuyruk_bos_mu(&q_rt) && 
            kuyruk_bos_mu(&q_1) && 
            kuyruk_bos_mu(&q_2) && 
            kuyruk_bos_mu(&q_3));
}