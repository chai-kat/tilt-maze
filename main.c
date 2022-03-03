#include <pic32mx.h>
#include <stdint.h>

#include "displayfunctions.h"
#include "i2c.h"
#include "accel.h"
#include "maze_functions.h"


// Uses MCB32Tools stdlib, so must define stdout in accordance with:
// https://github.com/is1200-example-projects/mcb32tools/issues/6
void *stdout = (void *) 0;

// ball

struct Ball {
	int x; // x position on screen
	int y; // y position on screen
	int vx; // x velocity
	int vy; // y velocity
} ball;

// get suggested new position, THEN update 
void ball_next_position(int* dx, int* dy, int dt) {
	// gives acceleration in milligs so div by 2000 which is the chosen range.
	*dx = ball.vx * accel_x() * dt;
	*dy = ball.vy * accel_y() * dt;
}

int main() {
	srand(725); // change random seed later?

	i2c_config();
	accel_setup();
	
	uint32_t screen[128];

	generate_blank_cell_array(screen);
	generate_maze(screen);
	
	for(;;) {
		
	}
	return 0;
}
