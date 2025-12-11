#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Windows ve Linux uyumluluğu (Sleep fonksiyonu için)
#ifdef _WIN32
    #include <windows.h>
    #define sleep(x) Sleep(x * 1000)
#else
    #include <unistd.h>
#endif

#include "scheduler.h"
#include "task.h"

#define BUFFER_SIZE 1024

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Kullanim: %s <giris_dosyasi.txt>\n", argv[0]);
        return 1;
    }

    FILE* dosya = fopen(argv[1], "r");
    if (!dosya) {
        perror("Dosya acilamadi");
        return 1;
    }

    scheduler_baslat();

    // Geçici olarak görevleri tutmak için basit yapı
    struct {
        int zaman;
        int oncelik;
        int sure;
    } gelen_tasklar[200];
    int task_sayisi = 0;

    char satir[BUFFER_SIZE];
    while (fgets(satir, sizeof(satir), dosya)) {
        int z, o, s;
        // Dosya formatı: 0, 1, 2
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

    // --- SİMÜLASYON DÖNGÜSÜ ---
    while (1) {
        // 1. Bu saniyede gelen yeni görev var mı?
        for (int i = 0; i < task_sayisi; i++) {
            if (gelen_tasklar[i].zaman == global_zaman) {
                // Dinamik bellek tahsisi (Linked List için gerekli)
                Task* t = (Task*)malloc(sizeof(Task));
                t->id = next_task_id++;
                t->varis_zamani = gelen_tasklar[i].zaman;
                t->oncelik = gelen_tasklar[i].oncelik;
                t->patlama_suresi = gelen_tasklar[i].sure;
                t->kalan_sure = gelen_tasklar[i].sure;
                t->sonraki = NULL;
                
                scheduler_gorev_kabul(t);
                // "Yeni görev geldi" çıktısı istenmemiş, direkt kuyruğa alıyoruz.
            }
        }

        // 2. Scheduler Tick (Çizelgeleyiciyi çalıştır)
        scheduler_calistir(global_zaman);

        // 3. Çıkış Kontrolü
        // Hem scheduler boş olmalı hem de dosyadan gelecek görev kalmamalı
        if (scheduler_bitti_mi() && global_zaman > gelen_tasklar[task_sayisi-1].zaman + 2) {
            break;
        }

        global_zaman++;
        sleep(1); // 1 saniye bekle
    }

    return 0;
}