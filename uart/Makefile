SHELL=bash

DIR_Inc=../common

all: make

make: 
	g++ -Wall -o snif.out -I../common serial.cpp ../common/uart_init.cpp ../common/stdin_init.cpp 

debug: 
	g++ -g -o snif.out -I$(DIR_Inc) RS485_snif.cpp $(DIR_Inc)/uart_config.c $(DIR_Inc)/stdin_config.c

run0:
	sudo ./snif.out /dev/ttyUSB0 57600 10

clean:
	rm *.out
