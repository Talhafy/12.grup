hepsi: program calistir

program: nesneler
	gcc ./lib/main.o ./lib/scheduler.o ./lib/queue.o -o ./bin/freertos_sim.exe

nesneler:
	gcc -I ./include -c ./src/main.c -o ./lib/main.o
	gcc -I ./include -c ./src/scheduler.c -o ./lib/scheduler.o
	gcc -I ./include -c ./src/queue.c -o ./lib/queue.o

calistir:
	./bin/freertos_sim.exe giris.txt