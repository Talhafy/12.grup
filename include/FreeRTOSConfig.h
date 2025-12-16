#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/* -------------------------------------------------
 * KERNEL SETTINGS
 * -------------------------------------------------*/
#define configUSE_PREEMPTION            1
#define configUSE_IDLE_HOOK             0
#define configUSE_TICK_HOOK             0
#define configCPU_CLOCK_HZ              ( ( unsigned long ) 100000000 )
#define configTICK_RATE_HZ              ( ( TickType_t ) 1000 )
#define configMAX_PRIORITIES            5
#define configMINIMAL_STACK_SIZE        128
#define configTOTAL_HEAP_SIZE           ( ( size_t ) ( 1024 * 1024 ) )
#define configMAX_TASK_NAME_LEN         16
#define configUSE_16_BIT_TICKS          0
#define configIDLE_SHOULD_YIELD         1

/* -------------------------------------------------
 * MEMORY
 * -------------------------------------------------*/
#define configSUPPORT_DYNAMIC_ALLOCATION 1
#define configSUPPORT_STATIC_ALLOCATION  0

/* -------------------------------------------------
 * QUEUE & SEMAPHORE (🔥 ASIL KRİTİK KISIM)
 * -------------------------------------------------*/
#define configUSE_QUEUE_SETS            1
#define configUSE_MUTEXES               1
#define configUSE_COUNTING_SEMAPHORES   1
#define configQUEUE_REGISTRY_SIZE       10

/* -------------------------------------------------
 * API FUNCTIONS (QUEUE HANDLE BURADA AÇILIR)
 * -------------------------------------------------*/
#define INCLUDE_vTaskPrioritySet        1
#define INCLUDE_uxTaskPriorityGet       1
#define INCLUDE_vTaskDelete             1
#define INCLUDE_vTaskDelay              1
#define INCLUDE_vTaskSuspend            1
#define INCLUDE_xQueueGetMutexHolder    1
#define INCLUDE_xTaskGetSchedulerState  1

/* -------------------------------------------------
 * ASSERT
 * -------------------------------------------------*/
#define configASSERT( x ) if( ( x ) == 0 ) { taskDISABLE_INTERRUPTS(); for( ;; ); }

#endif /* FREERTOS_CONFIG_H */
