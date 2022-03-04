#ifndef DISPLAY_FUNCTIONS_
#define DISPLAY_FUNCTIONS_ 
#include <stdbool.h>

struct Ball {
	int vx, vy;
	int x, y;
};

uint8_t spi_send_recv(uint8_t data);

void delay(int cyc);

void display_init();
void convertbitsize(const uint32_t *data, uint8_t *result);
void display_image(const uint8_t *data);

void draw_ball (int x, int y,  uint32_t *screen);
void undraw_ball (int x, int y, uint32_t *screen);

void update_position (uint32_t *screen, double dt);
void update_velocity (int ax, int ay, double dt);

bool check_win ();

#endif
