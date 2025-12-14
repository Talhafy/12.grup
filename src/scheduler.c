#include "scheduler.h"
#include "queue.h"
#include <stdio.h>
#include <stdlib.h>

// Global Kuyruklar
Kuyruk q_rt; // Priority 0 (Real Time)
Kuyruk q_1;  // Priority 1 (High)
Kuyruk q_2;  // Priority 2 (Medium)
Kuyruk q_3;  // Priority 3 (Low, P4, P5, ...)

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
    t->yenimibasladi= false;
    
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
    kuyruk_yaslandir_ve_temizle(&q_rt, saniye);
    kuyruk_yaslandir_ve_temizle(&q_1, saniye);
    kuyruk_yaslandir_ve_temizle(&q_2, saniye);
    kuyruk_yaslandir_ve_temizle(&q_3, saniye);

    // 2. ADIM: PREEMPTION (Kesme)
    if (calisan_gorev != NULL && calisan_gorev->oncelik > 0) {
        if (!kuyruk_bos_mu(&q_rt)) {
            // Askıya alma (Anlık Durum Değişikliği) -> saniye zamanında raporlanır
            printf("%.4f sn proses askida \t(id:%04d \t oncelik:%d \tkalan sure:%d sn)\n",
                   (float)saniye, calisan_gorev->id + 1, calisan_gorev->id, calisan_gorev->oncelik, calisan_gorev->kalan_sure);
            
            calisan_gorev->durum = DURUM_ASKIDA;
            
            // Kaldığı yere (kendi önceliğinin sonuna) geri dön
            if (calisan_gorev->oncelik == 1) kuyruk_ekle(&q_1, calisan_gorev); 
            else if (calisan_gorev->oncelik == 2) kuyruk_ekle(&q_2, calisan_gorev);
            else kuyruk_ekle(&q_3, calisan_gorev); 
            
            calisan_gorev = NULL;
        }
    }

    // 3. ADIM: GÖREV SEÇİMİ (CPU Boşsa)
    if (calisan_gorev == NULL) {
        calisan_gorev = sonraki_gorevi_sec();


        
        if (calisan_gorev != NULL) {
            
            // Görev ilk kez VEYA askıdan sonra başlıyor: Her zaman "başladı" mesajı basılır.
             printf("%.4f sn proses basladi \t(id:%04d \t oncelik:%d \tkalan sure:%d sn)\n", 
                (float)saniye, calisan_gorev->id + 1, calisan_gorev->id, calisan_gorev->oncelik, calisan_gorev->kalan_sure);
            
            calisan_gorev->yenimibasladi = true;
            calisan_gorev->durum = DURUM_CALISIYOR;
            calisan_gorev->bekleme_suresi = 0; // Çalışmaya başladığı an bekleme süresi sıfırlanır
        }
    }

    // 4. ADIM: YÜRÜTME (Bu saniye içinde çalışır)
    if (calisan_gorev != NULL) {
        
        // Yürütülüyor: Sadece görev kalan süresi patlama süresinden 1'den fazla eksikse basılır.
        // Bu, görevin ilk kuantumunda Yürütülüyor basılmasını engeller.
        // Task 4 (Süre 3) için: Başladı t=2 (kalan 3). Yürütülüyor t=3 (kalan 2) ve t=4 (kalan 1)
        
        // Kural: Kalan süre, patlama süresinden en az 2 azaldıysa (yani 2. kuantum bittiyse) Yürütülüyor bas.
        // Hayır, bu da hatalı. Örnek çıktıya tam uyum için kural:
        // Yürütülüyor mesajı, görevin patlama süresi 1 azaldıktan SONRA basılır.
        
        if (calisan_gorev->kalan_sure < calisan_gorev->patlama_suresi  && calisan_gorev->yenimibasladi== false) { 
             printf("%.4f sn proses yurutuluyor \t(id:%04d \t oncelik:%d \tkalan sure:%d sn)\n", 
                (float)saniye, calisan_gorev->id + 1, calisan_gorev->id, calisan_gorev->oncelik, calisan_gorev->kalan_sure);
        }

        // 1 saniye çalıştır
        calisan_gorev->kalan_sure--;
        calisan_gorev->yenimibasladi = false;

        // 4a. Durum Kontrolü (1 saniye çalıştıktan sonraki durum)
        
        // GÖREV BİTTİ (İşlem Sonucu) -> saniye + 1 zamanında raporlanır
        if (calisan_gorev->kalan_sure <= 0) {
            calisan_gorev->durum = DURUM_TAMAMLANDI;
            printf("%.4f sn proses sonlandi \t(id:%04d \t oncelik:%d \tkalan sure:0 sn)\n", 
                (float)saniye + 1, calisan_gorev->id + 1, calisan_gorev->id, calisan_gorev->oncelik);
            free(calisan_gorev); // Temizle
            calisan_gorev = NULL;
        }
        // KULLANICI GÖREVİ QUANTUM DOLDU (1 sn)
        else if (calisan_gorev->oncelik > 0) {
            // Öncelik düşür ve askıya al
            int eski_oncelik = calisan_gorev->oncelik;
            int yeni_oncelik = eski_oncelik + 1; // P3'ten sonra da artırılır (P4, P5, ...)

            // Askıya alma (İşlem Sonucu) -> saniye + 1 zamanında raporlanır
            printf("%.4f sn proses askida \t(id:%04d \t oncelik:%d \tkalan sure:%d sn)\n",
                (float)saniye + 1, calisan_gorev->id + 1, calisan_gorev->id, yeni_oncelik, calisan_gorev->kalan_sure);

            calisan_gorev->oncelik = yeni_oncelik; // Görev nesnesindeki önceliği güncelle
            calisan_gorev->durum = DURUM_ASKIDA;
            
            // Kuyruğa atarken her zaman P3 kuyruğuna (en düşük) at:
            if (yeni_oncelik == 1) kuyruk_ekle(&q_1, calisan_gorev);
            else if (yeni_oncelik == 2) kuyruk_ekle(&q_2, calisan_gorev);
            else kuyruk_ekle(&q_3, calisan_gorev); 
            
            calisan_gorev = NULL;
        }
    }
}

bool scheduler_bitti_mi() {
    return (calisan_gorev == NULL && 
            kuyruk_bos_mu(&q_rt) && 
            kuyruk_bos_mu(&q_1) && 
            kuyruk_bos_mu(&q_2) && 
            kuyruk_bos_mu(&q_3));
}