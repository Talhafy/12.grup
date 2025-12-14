#ifndef TASK_H
#define TASK_H

#include <stdbool.h>

// Görev Durumları
typedef enum {
    DURUM_HAZIR,
    DURUM_CALISIYOR,
    DURUM_ASKIDA,
    DURUM_TAMAMLANDI,
    DURUM_ZAMANASIMI
} TaskDurumu;

// Görev Yapısı (Linked List Node)
typedef struct Task {
    int id;                 // Görev ID (0, 1, 2...)
    int varis_zamani;       // Dosyadan okunan geliş zamanı
    int oncelik;            // 0: RT, 1-3: Kullanıcı
    int patlama_suresi;     // Toplam çalışması gereken süre
    int kalan_sure;         // Geriye kalan süre
    int bekleme_suresi;     // YENI: Kuyrukta bekleme süresi (20 sn kuralı)
    TaskDurumu durum;       // Anlık durum
    bool yenimibasladi;
    struct Task* sonraki;   // Sonraki görev (Linked List için)
} Task;

#endif