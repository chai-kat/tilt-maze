#ifndef ACCEL_H_
#define ACCEL_H_
#include <stdint.h>
#include <stdbool.h>

uint8_t get_register_single_byte(uint8_t);
void write_register_single_byte(uint8_t, uint8_t);
bool accel_data_available();
void accel_setup();
void accel_interrupts_setup();

uint16_t accel_x();
uint16_t accel_y();
uint16_t accel_z();

int conv_accel_to_g(uint16_t accel);

#endif //ACCEL_H_