#ifndef SIM_TASK_H
#define SIM_TASK_H

#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"

typedef enum {
    DURUM_HAZIR,
    DURUM_CALISIYOR,
    DURUM_ASKIDA,
    DURUM_TAMAMLANDI,
    DURUM_ZAMANASIMI
} TaskDurumu;

typedef struct Task {
    int id;
    int varis_zamani;
    int oncelik;
    int patlama_suresi;
    int kalan_sure;
    int bekleme_suresi;
    bool yenimibasladi;
    TaskDurumu durum;

    TaskHandle_t handle;      // 🔥 FreeRTOS entegrasyonu
    struct Task* sonraki;
} Task;

#endif
