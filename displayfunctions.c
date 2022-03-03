#include <pic32mx.h>
#include <stdint.h>
#include <stdbool.h>
#include "displayfunctions.h"

// #define DISPLAY_VDD PORTFbits.RF6
// #define DISPLAY_VBATT PORTFbits.RF5
// #define DISPLAY_COMMAND_DATA PORTFbits.RF4
// #define DISPLAY_RESET PORTGbits.RG9

// #define DISPLAY_VDD_PORT PORTF
// #define DISPLAY_VDD_MASK 0x40
// #define DISPLAY_VBATT_PORT PORTF
// #define DISPLAY_VBATT_MASK 0x20
// #define DISPLAY_COMMAND_DATA_PORT PORTF
// #define DISPLAY_COMMAND_DATA_MASK 0x10
// #define DISPLAY_RESET_PORT PORTG
// #define DISPLAY_RESET_MASK 0x200

#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

#define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200)
#define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)

#define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)
#define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)

#define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)
#define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)

const int BALL_REP = 0b00000000000000000000000000000011;

struct Ball ball;

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
	//DISPLAY_COMMAND_DATA_PORT &= ~DISPLAY_COMMAND_DATA_MASK;
	DISPLAY_CHANGE_TO_COMMAND_MODE; //clear and set to be able to send command
	delay(10);
	//DISPLAY_VDD_PORT &= ~DISPLAY_VDD_MASK;
	DISPLAY_ACTIVATE_VDD; //turn on power

	delay(1000000);
	
	spi_send_recv(0xAE);
	//DISPLAY_RESET_PORT &= ~DISPLAY_RESET_MASK;
	DISPLAY_ACTIVATE_RESET;
	delay(10);
	//DISPLAY_RESET_PORT |= DISPLAY_RESET_MASK;
	DISPLAY_DO_NOT_RESET;
	delay(10);
	
	spi_send_recv(0x8D);
	spi_send_recv(0x14);
	
	spi_send_recv(0xD9);
	spi_send_recv(0xF1);
	
	//DISPLAY_VBATT_PORT &= ~DISPLAY_VBATT_MASK;
	//turn on vcc
    DISPLAY_ACTIVATE_VBAT;
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
		//DISPLAY_COMMAND_DATA_PORT &= ~DISPLAY_COMMAND_DATA_MASK;
		DISPLAY_CHANGE_TO_COMMAND_MODE; //set to recieve command

		spi_send_recv(0x22);
		spi_send_recv(i);
		
		spi_send_recv(x & 0xF);
		spi_send_recv(0x10 | ((x >> 4) & 0xF));
		
		//DISPLAY_COMMAND_DATA_PORT |= DISPLAY_COMMAND_DATA_MASK;
		DISPLAY_CHANGE_TO_DATA_MODE;

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
	screen[x] = screen[x] & ~(BALL_REP  << (y));
	screen[x+1] = screen[x+1] & ~(BALL_REP  << (y));	
}


// int sx,sy | -2 <= sx,sy <= 2
// if sx,sy > 0: dx = 1, return;
// else calculate sx and sy

void update_position (uint32_t *screen, double dt) {
	// number of scheduled steps
	static int sx, sy;

	int dx = 0;
	int dy = 0;
	
	// ball going left
	if (ball.vx<0) {
		if ((screen[ball.x-1] & (BALL_REP  << ball.y)) > 0) {
			dx = 0;	//don't move left
			ball.vx = 0;
			if (sx < 0) {sx = 0;} //reset sx because wall in direction of travel
		}
		else {
			dx = ball.vx * dt;
			if (sx == 0) {sx = ball.vx*dt;}
		}
	}

	// ball going right
	if (ball.vx>0) {
		if ((screen[ball.x+2] & (BALL_REP << ball.y)) > 0) {
			dx = 0;	//don't move right
			ball.vx = 0;
			if (sx > 0) {sx = 0;} //reset sx because wall in direction of travel	
		}
		else {
			// dx = ball.vx * dt;
			if (sx == 0) {sx = ball.vx * dt;}
		}	
	}

	// ball going up
	if (ball.vy<0) {
		if (((screen[ball.x] & (0b00000000000000000000000000000001  << (ball.y-1))) > 0) 
		|| ((screen[ball.x+1] & (0b00000000000000000000000000000001  << (ball.y-1))) > 0)) {
			dy = 0;	//don't move up	
			ball.vy = 0;
			if (sy < 0) {sy = 0;} //reset sy because wall in direction of travel
		}
		else {
			dy = ball.vy*dt;
			if (sy == 0) {sy = ball.vy*dt;}
		}
	}

	//ball going down
	if (ball.vy>0) {
		// to check if the ball can move one pixel downwards (bottom of ball is offset by 1, hence 10 instead of 01)
		if (((screen[ball.x] & (0b00000000000000000000000000000010  << (ball.y+1))) > 0) 
		|| ((screen[ball.x+1] & (0b00000000000000000000000000000010  << (ball.y+1))) > 0)) {
			dy = 0;	//don't move down
			ball.vy = 0;
			if (sy > 0) {sy = 0;} //reset sy because wall in direction of travel
		}
		else {
			if (sy == 0) {sy = ball.vy*dt;}
		}
	}

	// guard against too many steps before recalculation.
	const int MAX_STEPS = 1+1;
	sx %= MAX_STEPS;
	sy %= MAX_STEPS;

	// can get sign of k by doing k % 2.
	if (sx > 0) {
		ball.x++;
		sx--;
		return;
	}
	else if (sx < 0) {
		ball.x--;
		sx++;
		return;
	}

	if (sy > 0) {
		ball.y++;
		sy--;
	}
	else if (sy < 0) {
		ball.y--;
		sy++;
	}

	// ball.x += dx;
	// ball.y += dy;
}

void update_velocity (int ax, int ay, double dt) {
    ball.vx = ball.vx + ax*dt;
    ball.vy = ball.vy + ay*dt;
}

bool check_win ()  {
    return (ball.x == 125) & (ball.y == 29);
}