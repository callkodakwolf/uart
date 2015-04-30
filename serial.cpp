#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <stdlib.h>
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <errno.h>   /* Error number definitions */
#include <sys/types.h>
#include <sys/time.h>
#include <stdint.h>

#include "uart_init.h"
#include "stdin_init.h"

#define PARSE 1	

#define BUF_SIZE 14400 

int main(int argc, char** argv)
{
	int serial, tty_in;
	struct termios option_old;
	struct termios option_old_ttyin;
	struct termios option_ttyin;

	int errno_saved;
	speed_t sp = atoi(argv[2]);
	long RW_timeout = 1;

	unsigned char serialRX[BUF_SIZE];
	unsigned char serialTX[BUF_SIZE];
	unsigned char stdin_buf[BUF_SIZE];

	const char Req_10[] = { 0x84, 0x10, 0x79, 0x00, 0x00, 0x09,0x00, 0x00, 0x00,0x6e,0xfb,'\0'};
	
	int ret_val = uart_nonblock_init(&serial, argv[1],&sp,&option_old, &RW_timeout);
	if( ret_val != 0){
		printf(" error init serial");
		return -1;
	}

	ret_val = stdin_nonblock_init(&tty_in, &option_old_ttyin);
	if( ret_val != 0){
		printf(" error init tty_in");
		return -1;
	}
		

	bool done = false;
	fd_set inputs;
	struct timeval timeout, tm;
	int state = 0;
	int last_state = 0;
	int start_byte = 0x00;
	uint8_t previous_byte = 0;
	int end_byte = 0x01;
	bool tilde = false;

	while(!done){
		FD_ZERO(&inputs);
		FD_SET(serial, &inputs);
		FD_SET(tty_in, &inputs);
		timeout.tv_sec = 0;
		timeout.tv_usec = 5000000;

		int result= select(FD_SETSIZE, &inputs, NULL, NULL, &timeout);	
		if( result < 0){
			perror("select() error");
			done = true;
		}else if ( result == 0){
			//printf(" 5s timeout ");
		}else{
			if( FD_ISSET( serial, &inputs)){
				int byte_count = read(serial,serialRX, BUF_SIZE);
				if( byte_count == 0){
					printf(" unknow read of 0 bytes\n");
					done = true;
					continue;
				} else{
					for( int i = 0;  i < byte_count; i++){
#if PARSE
/*						previous_byte = serialRX[i];
						switch( state ){
						case 0: // wait start byte
								if( serialRX[i] > 0x7f && serialRX[i] < 0xc0){
									 gettimeofday(&tm,NULL);
									 printf("\n(%ld. %06ld) ",tm.tv_sec, tm.tv_usec);
								     state = 1;
									 last_state = state;
									 start_byte = serialRX[i];
								 	 end_byte = (serialRX[i] ^ 0x7f) | 0x80;
								}else {
									 state = 0;
								}
								break;
						case 1: // process MSG body
								if( serialRX[i] < 0x80){
									state = 1;
								}
								else {
									if( serialRX[i] != end_byte )
										printf("**Error endbyte: ",serialRX[i]);
									state = 0;
								}
								break;
						}
*/
						if( previous_byte == 0xc0 && (serialRX[i]>0x7f && serialRX[i]<0xc0 ))
						{	
									 gettimeofday(&tm,NULL);
									 printf("\n(%ld. %06ld) ",tm.tv_sec, tm.tv_usec);
						}

						if( previous_byte !=  0xc0 && (previous_byte & 0xc0)&& (serialRX[i]>0x7f && serialRX[i]<0xc0 ))
						{	
									 gettimeofday(&tm,NULL);
									 printf("\n(%ld. %06ld) ",tm.tv_sec, tm.tv_usec);
						}
#endif
						printf("%02x ", serialRX[i]);
						previous_byte = serialRX[i];
					}
				}
			}
			else if( FD_ISSET( tty_in, &inputs )){
				int byte_count = read( tty_in, stdin_buf, sizeof(stdin_buf));
				if( byte_count == 0){
					printf(" unknow read of 0 bytes\n");
					done = true;
					continue;
				}
				else{
					if( stdin_buf[0] == '~'){
						tilde = true;
				}
				
				if( tilde && stdin_buf[0] == '.'){
					done = true;
					tilde = false;
					printf("\n terminated by user\n");
				}
				
				if( tilde == true && stdin_buf[0] == '!'){
					memcpy( serialTX, (const void*)Req_10, sizeof( Req_10));
					int ret_val = 0;
					ret_val = write(serial, serialTX, sizeof(Req_10));
					printf("\n%d bytes is written: ", ret_val);
					for( int k = 0; k  < sizeof(Req_10); k++)
					{
						printf( "%02x ", serialTX[k]);
					}
					printf("\n");
					tilde = false;
				}
				
			}
		} 
	}
	fflush(stdout);
}

	if( tcsetattr( tty_in, TCSANOW, &option_old_ttyin) != 0)
		perror(" restore previous tty_in setting fail");
	if( tcsetattr( serial , TCSANOW, &option_old) != 0)
		perror(" restore previous serial setting fail");	
	close(serial);
	close( tty_in);
}
/*
uint16_t seed= 0x78AE;
		uint16_t array[1000];
		uint16_t *pp;
		int16_t cnt = 1000;
		pp = array;

		srand(seed);
		for(int k = 0; k < 1000; k++)
		{	
			array[k] = (uint16_t)rand();
		}
	
		while(cnt>=1)
		{
			ret_val = write(serial,pp,cnt);
			if(ret_val != -1){
				pp+= ret_val;
				cnt-=ret_val;
			}
		}

*/
