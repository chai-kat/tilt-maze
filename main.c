#include <pic32mx.h>
#include <stdint.h>

#include "displayfunctions.h"
#include "i2c.h"
#include "accel.h"
#include "maze_functions.h"


// Uses MCB32Tools stdlib, so must define stdout in accordance with:
// https://github.com/is1200-example-projects/mcb32tools/issues/6
void *stdout = (void *) 0;

void interrupt_setup() {
	// add timer interrupt setup?
	
	// add accel interrupt setup?
	// accel_interrupts_setup();
	
	enable_interrupts();
}

void interrupt_handler() {
	// fill later
}


int main() {
	srand(725); // change random seed later?

	i2c_config();
	accel_setup();
	
	uint32_t screen[128];

	generate_blank_cell_array(screen);
	generate_maze(screen);
	
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
