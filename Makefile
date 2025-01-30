all: cbtc.exe

cbtc.exe:
	gcc -g -pthread -c marvelmind.c -o marvelmind.o
	gcc -g -pthread -c cbtc.c -o cbtc.o
	gcc -o cbtc.exe cbtc.o marvelmind.o -pthread

clean:
	rm -rf *.o cbtc.exe

install:
	scp -r ./* pi@192.168.1.173:/home/pi/CBTC

