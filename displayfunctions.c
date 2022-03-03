#include <pic32mx.h>
#include <stdint.h>

#define DISPLAY_VDD PORTFbits.RF6
#define DISPLAY_VBATT PORTFbits.RF5
#define DISPLAY_COMMAND_DATA PORTFbits.RF4
#define DISPLAY_RESET PORTGbits.RG9


#define DISPLAY_VDD_PORT PORTF
#define DISPLAY_VDD_MASK 0x40
#define DISPLAY_VBATT_PORT PORTF
#define DISPLAY_VBATT_MASK 0x20
#define DISPLAY_COMMAND_DATA_PORT PORTF
#define DISPLAY_COMMAND_DATA_MASK 0x10
#define DISPLAY_RESET_PORT PORTG
#define DISPLAY_RESET_MASK 0x200

void delay(int cyc) {
	int i;
	for(i = cyc; i > 0; i--);
}

uint8_t spi_send_recv(uint8_t data) {
	while(!(SPI2STAT & 0x08));
	SPI2BUF = data;
	while(!(SPI2STAT & 0x01));
	return SPI2BUF;
}

void display_init() {
	DISPLAY_COMMAND_DATA_PORT &= ~DISPLAY_COMMAND_DATA_MASK;
	delay(10);
	DISPLAY_VDD_PORT &= ~DISPLAY_VDD_MASK;
	delay(1000000);
	
	spi_send_recv(0xAE);
	DISPLAY_RESET_PORT &= ~DISPLAY_RESET_MASK;
	delay(10);
	DISPLAY_RESET_PORT |= DISPLAY_RESET_MASK;
	delay(10);
	
	spi_send_recv(0x8D);
	spi_send_recv(0x14);
	
	spi_send_recv(0xD9);
	spi_send_recv(0xF1);
	
	DISPLAY_VBATT_PORT &= ~DISPLAY_VBATT_MASK;
	delay(10000000);
	
	spi_send_recv(0xA1);
	spi_send_recv(0xC8);
	
	spi_send_recv(0xDA);
	spi_send_recv(0x20);
	
	spi_send_recv(0xAF);
}

void convertbitsize(const uint32_t *data, uint8_t *result) {
	int i;
	int j = 0;
	for (i=0; i<128; i++) {
		uint32_t value = data[i];
		result[j++] = ~(value & 0x000000ff) >> 0;
	}
	for (i=0; i<128; i++) {
		uint32_t value = data[i];
		result[j++] = ~(value & 0x0000ff00) >> 8;
	}
	for (i=0; i<128; i++) {
		uint32_t value = data[i];
		result[j++] = ~(value & 0x00ff0000) >> 16;
	}
	for (i=0; i<128; i++) {
		uint32_t value = data[i];
		result[j++] = ~(value & 0xff000000) >> 24;
	}
}

void display_image(int x, const uint8_t *data) {
	int i, j;
	
	for(i = 0; i < 4; i++) {
		DISPLAY_COMMAND_DATA_PORT &= ~DISPLAY_COMMAND_DATA_MASK;
		spi_send_recv(0x22);
		spi_send_recv(i);
		
		spi_send_recv(x & 0xF);
		spi_send_recv(0x10 | ((x >> 4) & 0xF));
		
		DISPLAY_COMMAND_DATA_PORT |= DISPLAY_COMMAND_DATA_MASK;
		
		for(j = 0; j < 32*4; j++)
			spi_send_recv(~data[i*32*4 + j]);
	}
}

void draw_ball (int x, int y,  uint32_t *screen) {
    if(x + 2 > 128 || y + 2 > 32) return; // control input
	screen[x] = screen[x] | (BALL_REP  << (y));
	screen[x+1] = screen[x+1] | (BALL_REP  << (y));			
}

void undraw_ball (int x, int y, uint32_t *screen) {
	screen[x] = screen[x] & (~BALL_REP  << (y));
	screen[x+1] = screen[x+1] & (~BALL_REP  << (y));	
}

struct Ball {
	int x;
	int y;
} ball;

void update_position (uint32_t *screen) {
	int kx;
	int ky;

	if (ax<0) {
		if ((screen[ball.x-1] & (BALL_REP  << ball.y)) > 0) {
			kx = 0;	//don't move left	
		}
		else {
			kx = ax;
		}
	}
	if (ax>0) {
		if ((screen[ball.x+2] & (BALL_REP  << ball.y)) > 0) {
			kx = 0;	//don't move right	
		}
		else {
			kx = ax;
		}	
	}
	if (ay<0) {
		if ((screen[ball.x] & (0b00000000000000000000000000000001  << (ball.y-1))) > 0) {
			ky = 0;	//don't move up	
		}
		else {
			ky = ay;
		}
	}
	if (ay>0) {
		// to check if the ball can move one pixel downwards (bottom of ball is offset by 1, hence 10 instead of 01)
		if ((screen[ball.x] & (0b00000000000000000000000000000010  << (ball.y+1))) > 0) {
			ky = 0;	//don't move down	
		}
		else {
			ky = ay;
		}
	}
	ball.x += kx;
	ball.y += ky;
}

bool check_win ()  {
    return (ball.x == 125) & (ball.y == 29);
}