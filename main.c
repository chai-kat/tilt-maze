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

	i2c_config();
	accel_setup();
	display_init();

	uint32_t screen[128];
	uint8_t converted_screen[512];

	generate_blank_cell_array(screen);
	generate_maze(screen);
	display_image(0, converted_screen);

	const int dt = 0.1;
	for(;;) {
		// get acceleration
		int ax = conv_accel_to_g(accel_x()) * 10; // 1g = 10ms2
		int ay = conv_accel_to_g(accel_y()) * 10; // 1g = 10ms2

		while(!check_win()) {
			undraw_ball(ball.x, ball.y, screen);
			update_position(screen, dt);
			update_velocity(ax, ay, dt);
			draw_ball(ball.x, ball.y, screen);

			delay(10000);
		}
	}
	return 0;
}
