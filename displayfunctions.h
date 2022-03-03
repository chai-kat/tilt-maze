#ifndef DISPLAY_FUNCTIONS_   /* Include guard */
#define DISPLAY_FUNCTIONS_ 
#include <stdbool.h>

extern struct Ball {
	int vx, vy;
	int x, y;
};

uint8_t spi_send_recv(uint8_t data);

void display_init();
void convertbitsize(const uint32_t *data, uint8_t *result);
void display_image(int x, const uint8_t *data);

void draw_ball (int x, int y,  uint32_t *screen);
void undraw_ball (int x, int y, uint32_t *screen);

void update_position (uint32_t *screen, double dt);
void update_velocity (int ax, int ay, double dt);

bool check_win ();

#endif // FOO_H_
