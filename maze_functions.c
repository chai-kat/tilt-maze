#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "maze_functions.h"

// NOTE: uses STDLIB. Included -lc flag in make.

// NOTE: Cell size hardcoded as 4x4. 
// Don't really want to mess around with weirder divisions.

const int ROW_SIZE = 32;
const int COL_SIZE = 8;

const int SCREEN_ROW_SIZE = 128; // 4 * ROW_SIZE;
const int SCREEN_COL_SIZE = 32;  // 4 * COL_SIZE;

void generate_blank_cell_array(uint32_t* cell_array) {
    for(int i = 0; i < SCREEN_ROW_SIZE; i += 4) {
        cell_array[i] = 0xffffffff;
        cell_array[i+1] = 0x99999999;
        cell_array[i+2] = 0x99999999;
        cell_array[i+3] = 0xffffffff;
    }
}

// removes bottom wall from block at position x,y, top from x, y+1
void remove_vertical(uint32_t* cell_array, uint8_t x, uint8_t y) {
    if (y >= COL_SIZE) {return;}
    // generate removal mask. only need two bits to be 0, in centre of cell (pos 1,2)
    // so only need to shift 0110 to a 4k+2 position and then invert
    uint32_t mask= ~(0b0110 << ((y*4) + 2));

    // need columns 1,2 so 4*x + 1,2
    cell_array[(4*x)+1] &= mask;
    cell_array[(4*x)+2] &= mask;
}

// removes RHS wall from block at position x,y, LHS from x+1, y
void remove_horizontal(uint32_t* cell_array, uint8_t x, uint8_t y) {
    // must check that x is less than row length - 1, cant remove horizontal on last cell.
    // must check that y is less than column len(- 0) can't remove horizontal out of bounds.
    if (x > (ROW_SIZE-1)) {return;}
    if (y > COL_SIZE) {return;}

    // generate removal mask. AND with 0s in the removal positions, rest 1s.
    // basically need 1001 in the right position (only aligned to a multiple of 4)
    // << shifts in 0s that we want to make 1s, so we just take the inverse, shift that, then invert again.
    uint32_t mask = ~(0b0110 << (y*4));

    // remove right
    cell_array[(4*x)+3] &= mask;
    // remove left
    cell_array[(4*x)+4] &= mask;
}

// return true or false at random
bool rand_tf(){
    return (rand() > (RAND_MAX/2));
}

// generates in place, have to pass a screen array
void generate_maze(uint32_t* screen) {
    generate_blank_cell_array(screen);

    // Initialize the cells of the first row to each exist in their own set.
    uint8_t row_array[ROW_SIZE];
    for(int i=0; i < ROW_SIZE; i++) {
        row_array[i] = i;
    }

    // NOTE: correct
    // remove joins at random for first row
    // condition random.choice([True, False]) and (row_array[i] != row_array[i+1])
    for (int i = 0; i < ROW_SIZE - 1; i++) {
        if (rand_tf() & (row_array[i] != row_array[i+1])) {
            row_array[i+1] = row_array[i];
            remove_horizontal(screen, i, 0);
        }
    }

    // repeat down the rows. already done one so start at 1.
    // also want to handle last row differently so COL_SIZE - 1
    uint8_t next_row_array[ROW_SIZE];
    for (int row = 1; row < COL_SIZE-1; row++) {
        // NOTE: correct.
        // next_row_array = range(i*ROW_SIZE, (i+1) * ROW_SIZE)
        for(int column = 0; column < ROW_SIZE; column++) {
            next_row_array[column] = row * ROW_SIZE + column;
        }
        
        //----- NOTE: correct. randomly create vertical connections
        int amt_vertical_conns = 0;
        for(int column = 0; column < ROW_SIZE - 1; column++) {
            // if the next item set number same just randomly make connection
            // can do column + 1 because we only go to ROW_SIZE - 1
            if (row_array[column] == row_array[column+1]) {
                if (rand_tf()) {
                    // have to remove row-1 because rows start at 1!
                    remove_vertical(screen, column, row-1);
                    next_row_array[column] = row_array[column];
                    amt_vertical_conns++;
                }
            }
            // if not same, then need to set connections to 0 after taking action
            // if no conns then make. otherwise random.
            else {
                if (rand_tf() | (amt_vertical_conns == 0)) {
                    // have to remove row-1 because rows start at 1!
                    remove_vertical(screen, column, row-1);
                    next_row_array[column] = row_array[column];
                    amt_vertical_conns++;
                }
                // reset
                amt_vertical_conns = 0;
            }
        }
        // handle last (ROW_SIZE - 1) column. if amt_conns 0 then we know its new set
        if (amt_vertical_conns == 0) {
            // have to remove row-1 because rows start at 1!
            remove_vertical(screen, ROW_SIZE - 1, row-1);
            next_row_array[ROW_SIZE - 1] = row_array[ROW_SIZE - 1];
        }
        //----- NOTE: correct. end vertical connections

        // NOTE: correct NOW? 
        // randomly join next row
        // condition random.choice([True, False]) and (next_row_array[i] != next_row_array[i+1])
        for (int i = 0; i < ROW_SIZE - 1; i++) {
            if (rand_tf() & (next_row_array[i] != next_row_array[i+1])) {
                next_row_array[i+1] = next_row_array[i];
                remove_horizontal(screen, i, row);
            }
        }

        // row_array = next_row_array;
        for(int i = 0; i < ROW_SIZE; i++) {
            row_array[i] = next_row_array[i];
        }
    }    
    // For the last row, connect vertically to it
    // then join all adjacent cells that do not share a set,
    for(int column = 0; column < ROW_SIZE; column++) {
            next_row_array[column] = (COL_SIZE-1) * ROW_SIZE + column;
    }

    // for(int column = (COL_SIZE - 1) * ROW_SIZE; column < COL_SIZE * ROW_SIZE; column++) {
    //         next_row_array[column] = column;
    // }
    
    //----- randomly create vertical connections
    int amt_vertical_conns = 0;
    for(int column = 0; column < ROW_SIZE - 1; column++) {
        // if the next item set number same just randomly make connection
        // can do column + 1 because we only go to ROW_SIZE - 1
        if (row_array[column] == row_array[column+1]) {
            if (rand_tf()) {
                // COL_SIZE-1 because row 7, then - 1 again because have to remove from row above
                remove_vertical(screen, column, COL_SIZE - 2);
                next_row_array[column] = row_array[column];
                amt_vertical_conns++;
            }
        }
        // if not same, then need to set connections to 0 after taking action
        // if no conns then make. otherwise random.
        else {
            if (rand_tf() | (amt_vertical_conns == 0)) {
                // have to remove row-1 because rows start at 1!
                remove_vertical(screen, column, COL_SIZE - 2);
                next_row_array[column] = row_array[column];
            }
            // reset
            amt_vertical_conns = 0;
        }
    }

    // handle last (ROW_SIZE - 1) column. if amt_conns 0 then we know its new set
    if (amt_vertical_conns == 0) {
        // have to remove row-1 because rows start at 1!
        remove_vertical(screen, ROW_SIZE - 1, COL_SIZE - 2);
        next_row_array[ROW_SIZE - 1] = row_array[ROW_SIZE - 1];
    }
    //----- end vertical connections
    
    // join condition: (next_row_array[i] != next_row_array[i+1])
    for (int i = 0; i < ROW_SIZE-1; i++) {
        // if 
        if(next_row_array[i] != next_row_array[i+1]) {
            next_row_array[i+1] = next_row_array[i];
            remove_horizontal(screen, i, COL_SIZE - 1);
        }
    }
}