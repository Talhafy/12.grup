hepsi: program calistir


program: nesneler
	gcc \
	./lib/main.o \
	./lib/scheduler.o \
	./lib/queue.o \
	./lib/tasks.o \
	./lib/list.o \
	./lib/fr_queue.o \
	./lib/port.o \
	./lib/heap_4.o \
	./lib/event.o \
	-o ./bin/freertos_sim.exe -lpthread


nesneler:
	gcc -I ./include \
	-I ./FreeRTOS-Kernel/include \
	-I ./FreeRTOS-Kernel/portable/ThirdParty/GCC/Posix \
	-c ./src/main.c -o ./lib/main.o

	gcc -I ./include \
	-I ./FreeRTOS-Kernel/include \
	-I ./FreeRTOS-Kernel/portable/ThirdParty/GCC/Posix \
	-c ./src/scheduler.c -o ./lib/scheduler.o

	gcc -I ./include \
	-I ./FreeRTOS-Kernel/include \
	-I ./FreeRTOS-Kernel/portable/ThirdParty/GCC/Posix \
	-c ./src/queue.c -o ./lib/queue.o

	# FreeRTOS Kernel
	gcc -I ./include \
	-I ./FreeRTOS-Kernel/include \
	-I ./FreeRTOS-Kernel/portable/ThirdParty/GCC/Posix \
	-c ./FreeRTOS-Kernel/tasks.c -o ./lib/tasks.o

	gcc -I ./include \
	-I ./FreeRTOS-Kernel/include \
	-I ./FreeRTOS-Kernel/portable/ThirdParty/GCC/Posix \
	-c ./FreeRTOS-Kernel/list.c -o ./lib/list.o

	gcc -I ./include \
	-I ./FreeRTOS-Kernel/include \
	-I ./FreeRTOS-Kernel/portable/ThirdParty/GCC/Posix \
	-c ./FreeRTOS-Kernel/queue.c -o ./lib/fr_queue.o

	# POSIX Port
	gcc -I ./include \
	-I ./FreeRTOS-Kernel/include \
	-I ./FreeRTOS-Kernel/portable/ThirdParty/GCC/Posix \
	-c ./FreeRTOS-Kernel/portable/ThirdParty/GCC/Posix/port.c \
	-o ./lib/port.o

	# Memory Management (EKLENDİ)
	gcc -I ./include \
	-I ./FreeRTOS-Kernel/include \
	-I ./FreeRTOS-Kernel/portable/ThirdParty/GCC/Posix \
	-c ./FreeRTOS-Kernel/portable/MemMang/heap_4.c \
	-o ./lib/heap_4.os

	# POSIX Event Utils (EKLENDİ)
	gcc -I ./include \
	-I ./FreeRTOS-Kernel/include \
	-I ./FreeRTOS-Kernel/portable/ThirdParty/GCC/Posix \
	-c ./FreeRTOS-Kernel/portable/ThirdParty/GCC/Posix/utils/wait_for_event.c \
	-o ./lib/event.o

calistir:
	./bin/freertos_sim.exe giris.txt
