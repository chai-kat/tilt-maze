#include <pic32mx.h>
#include <stdint.h>
#include <stdlib.h>

#include "displayfunctions.h"
#include "i2c.h"
#include "accel.h"
#include "maze_functions.h"


extern struct Ball ball;

// Uses MCB32Tools stdlib, so must define stdout in accordance with:
// https://github.com/is1200-example-projects/mcb32tools/issues/6
void *stdout = (void *) 0;

// void interrupt_setup() {
// 	// add timer interrupt setup?
	
// 	// add accel interrupt setup?
// 	// accel_interrupts_setup();
	
// 	enable_interrupts();
// }

void interrupt_handler() {
	// fill later
}

// const uint32_t const maze[128] = {
// 	0xffffffff, 0x99999999, 0x99999999, 0xffffffff,
// 	0xffffffff, 0x99999999, 0x99999999, 0xffffffff,
// 	0xffffffff, 0x99999999, 0x99999999, 0xffffffff,
// 	0xffffffff, 0x99999999, 0x99999999, 0xffffffff,
// 	0xffffffff, 0x99999999, 0x99999999, 0xffffffff,
// 	0xffffffff, 0x99999999, 0x99999999, 0xffffffff,
// 	0xffffffff, 0x99999999, 0x99999999, 0xffffffff,
// 	0xffffffff, 0x99999999, 0x99999999, 0xffffffff,
// 	0xffffffff, 0x99999999, 0x99999999, 0xffffffff,
// 	0xffffffff, 0x99999999, 0x99999999, 0xffffffff,
// 	0xffffffff, 0x99999999, 0x99999999, 0xffffffff,
// 	0xffffffff, 0x99999999, 0x99999999, 0xffffffff,
// 	0xffffffff, 0x99999999, 0x99999999, 0xffffffff,
// 	0xffffffff, 0x99999999, 0x99999999, 0xffffffff,
// 	0xffffffff, 0x99999999, 0x99999999, 0xffffffff,
// 	0xffffffff, 0x99999999, 0x99999999, 0xffffffff,
// 	0xffffffff, 0x99999999, 0x99999999, 0xffffffff,
// 	0xffffffff, 0x99999999, 0x99999999, 0xffffffff,
// 	0xffffffff, 0x99999999, 0x99999999, 0xffffffff,
// 	0xffffffff, 0x99999999, 0x99999999, 0xffffffff,
// 	0xffffffff, 0x99999999, 0x99999999, 0xffffffff,
// 	0xffffffff, 0x99999999, 0x99999999, 0xffffffff,
// 	0xffffffff, 0x99999999, 0x99999999, 0xffffffff,
// 	0xffffffff, 0x99999999, 0x99999999, 0xffffffff,
// 	0xffffffff, 0x99999999, 0x99999999, 0xffffffff,
// 	0xffffffff, 0x99999999, 0x99999999, 0xffffffff,
// 	0xffffffff, 0x99999999, 0x99999999, 0xffffffff,
// 	0xffffffff, 0x99999999, 0x99999999, 0xffffffff,
// 	0xffffffff, 0x99999999, 0x99999999, 0xffffffff,
// 	0xffffffff, 0x99999999, 0x99999999, 0xfffffff,
// 	0xffffffff, 0x99999999, 0x99999999, 0xffffffff,
// 	0xffffffff, 0x99999999, 0x99999999, 0xffffffff,
// };


int main() {
	/* Set up peripheral bus clock */
	OSCCON &= ~0x180000;
	OSCCON |= 0x080000;
	
	/* Set up output pins */
	AD1PCFG = 0xFFFF;
	ODCE = 0x0;
	TRISECLR = 0xFF;
	PORTE = 0x0;
	
	/* Output pins for display signals */
	PORTF = 0xFFFF;
	PORTG = (1 << 9);
	ODCF = 0x0;
	ODCG = 0x0;
	TRISFCLR = 0x70;
	TRISGCLR = 0x200;
	
	/* Set up input pins */
	TRISDSET = (1 << 8);
	TRISFSET = (1 << 1);
	
	/* Set up SPI as master */
	SPI2CON = 0;
	SPI2BRG = 4;
	
	/* Clear SPIROV*/
	SPI2STATCLR &= ~0x40;
	/* Set CKP = 1, MSTEN = 1; */
    SPI2CON |= 0x60;
	
	/* Turn on SPI */
	SPI2CONSET = 0x8000;

	srand(725); // change random seed later?
	ball.x = 1;
	ball.y = 1;
	ball.vx = 3;
	ball.vy = 3;

	i2c_config();
	accel_setup();
	display_init();

	uint32_t screen[128];
	uint8_t converted_screen[512];

	display_image(0, converted_screen);
	
	draw_ball(ball.x, ball.y, screen);
	convertbitsize(screen, converted_screen);
	display_image(0, converted_screen);

	const double dt = 1;
	int wins = 0;
	for(;;) {
		generate_blank_cell_array(screen);
		generate_maze(screen);
		convertbitsize(screen, converted_screen);

		while(!check_win()) {
			// get acceleration
			int ax = conv_accel_to_g(accel_x()); // unit ms2
			int ay = conv_accel_to_g(accel_y()); // unit ms2

			PORTE = ball.vy;

			undraw_ball(ball.x, ball.y, screen);
			update_position(screen, dt);
			update_velocity(ax/6, ay/6, dt);
			draw_ball(ball.x, ball.y, screen);

			convertbitsize(screen, converted_screen);
			display_image(0, converted_screen);

			delay(20000);
		}

		for (int i=0; i < 6; i++) {
			PORTESET = 0xff;
			delay(500000);
			PORTECLR = 0xff;
			delay(500000);
		}

		// reset ball
		undraw_ball(ball.x, ball.y, screen);
		ball.x = 1;
		ball.y = 1;
		wins++; // TODO: Unused! Show with PORTE? 
	}
	return 0;
}
