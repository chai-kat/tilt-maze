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
	*dx = ball.vx * accel_x() * dt;
	*dy = ball.vy * accel_y() * dt;
}

int main() {
	return 0;
}
