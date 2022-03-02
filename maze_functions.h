#ifndef MAZE_FUNCTIONS_H
#define MAZE_FUNCTIONS_H

void generate_blank_cell_array(uint32_t* cell_array);
void remove_vertical(uint32_t* cell_array, uint8_t x, uint8_t y);
void remove_horizontal(uint32_t* cell_array, uint8_t x, uint8_t y);

bool rand_tf();

void generate_maze(uint32_t* screen);

#endif