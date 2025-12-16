#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

#include "scheduler.h"
#include "sim_task.h"

#define BUFFER_SIZE 1024

// Simülasyonu yönetecek Ana Task
static void SimulationTask(void *pvParameters)
{
    char *dosya_adi = (char *)pvParameters;

    FILE *dosya = fopen(dosya_adi, "r");
    if (!dosya) {
        perror("Dosya acilamadi");
        vTaskDelete(NULL);
    }

    scheduler_baslat();

    // Stack taşmasını önlemek için static yaptık
    static struct {
        int zaman;
        int oncelik;
        int sure;
    } gelen_tasklar[200];

    static char satir[BUFFER_SIZE];
    int task_sayisi = 0;

    while (fgets(satir, sizeof(satir), dosya)) {
        int z, o, s;
        if (sscanf(satir, "%d , %d , %d", &z, &o, &s) == 3) {
            gelen_tasklar[task_sayisi].zaman = z;
            gelen_tasklar[task_sayisi].oncelik = o;
            gelen_tasklar[task_sayisi].sure = s;
            task_sayisi++;
        }
    }
    fclose(dosya);

    int global_zaman = 0;
    int next_task_id = 0;

    while (1) {
        // 1. Yeni Görevler
        for (int i = 0; i < task_sayisi; i++) {
            if (gelen_tasklar[i].zaman == global_zaman) {
                Task *t = (Task *)malloc(sizeof(Task));
                if (t) {
                    t->id = next_task_id++;
                    t->varis_zamani = gelen_tasklar[i].zaman;
                    t->oncelik = gelen_tasklar[i].oncelik;
                    t->patlama_suresi = gelen_tasklar[i].sure;
                    t->kalan_sure = gelen_tasklar[i].sure;
                    t->sonraki = NULL;

                    scheduler_gorev_kabul(t);
                }
            }
        }

        // 2. Scheduler Çalıştır
        scheduler_calistir(global_zaman);

        // 3. Çıkış Kontrolü
        if (scheduler_bitti_mi() && global_zaman > gelen_tasklar[task_sayisi - 1].zaman + 2) {
            break;
        }

        global_zaman++;
        // 1 saniye bekle
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    printf("Simulasyon bitti.\n");
    vTaskDelete(NULL);
}

int main(int argc, char *argv[])
{
    // Tamponlamayı kapat (anlık çıktı için)
    setvbuf(stdout, NULL, _IONBF, 0);

    if (argc != 2) {
        printf("Kullanim: %s <giris_dosyasi.txt>\n", argv[0]);
        return 1;
    }

    xTaskCreate(
        SimulationTask,
        "SimTask",
        8192,  // Yüksek Stack Size
        argv[1],
        tskIDLE_PRIORITY + 2,
        NULL
    );

    vTaskStartScheduler();
    return 0;
}
