SHELL=bash

DIR_Inc=../../common

all: make

make: 
	g++ -Wall -o snif.out -I$(DIR_Inc) serial.cpp $(DIR_Inc)/uart_init.cpp $(DIR_Inc)/stdin_init.cpp 

debug: 
	g++ -g -o snif.out -I$(DIR_Inc) RS485_snif.cpp $(DIR_Inc)/uart_config.c $(DIR_Inc)/stdin_config.c

run0:
	sudo ./snif.out /dev/ttyUSB0 57600 10

run1:
	sudo ./snif.out /dev/ttyUSB1 57600 10

clean:
	rm *.out
