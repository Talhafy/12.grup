#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void kuyruk_olustur(Kuyruk* k) {
    k->bas = NULL;
    k->son = NULL;
    k->boyut = 0;
}

void kuyruk_ekle(Kuyruk* k, Task* t) {
    t->sonraki = NULL; // Sona eklendiği için next'i NULL
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
    k->bas = k->bas->sonraki;

    if (k->bas == NULL) {
        k->son = NULL;
    }
    k->boyut--;
    return t;
}

bool kuyruk_bos_mu(Kuyruk* k) {
    return (k->boyut == 0);
}

// Her saniye kuyruktaki elemanların bekleme süresini artırır.
// 20 saniyeyi geçenleri siler ve ekrana yazdırır.
void kuyruk_yaslandir_ve_temizle(Kuyruk* k, int suanki_zaman) {
    if (k->bas == NULL) return;

    Task* onceki = NULL;
    Task* suanki = k->bas;

    while (suanki != NULL) {
        // Bekleme süresini artır
        suanki->bekleme_suresi++;

        // 20 saniye kuralı: 20 tam saniye bekleme süresi dolduğunda (yani 21. tick'te)
        if (suanki->bekleme_suresi >= 21) { 
            // Zamanaşımı mesajı saniye zamanında raporlanır (Örnek Çıktı uyumu için 21.0000)
            printf("%.4f sn proses zamanasimi \t(id:%04d \t oncelik:%d \tkalan sure:%d sn)\n", 
                   (float)suanki_zaman, suanki->id + 1, suanki->id, suanki->oncelik, suanki->kalan_sure);
            
            // Bağlı listeden düğüm silme işlemi
            Task* silinecek = suanki;
            
            if (onceki == NULL) { // Baştaki eleman siliniyorsa
                k->bas = suanki->sonraki;
                if (k->bas == NULL) k->son = NULL;
                suanki = k->bas;
            } else { // Aradaki veya sondaki
                onceki->sonraki = suanki->sonraki;
                if (suanki->sonraki == NULL) k->son = onceki;
                suanki = onceki->sonraki;
            }
            
            k->boyut--;
            free(silinecek); // Bellekten temizle
        } else {
            // Sorun yoksa bir sonrakine geç
            onceki = suanki;
            suanki = suanki->sonraki;
        }
    }
}