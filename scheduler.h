#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "task.h"
#include <stdbool.h>

void scheduler_baslat();
void scheduler_gorev_kabul(Task* t);
void scheduler_calistir(int saniye);
bool scheduler_bitti_mi();

#endif