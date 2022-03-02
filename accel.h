#ifndef ACCEL_H_
#define ACCEL_H_

uint8_t get_register_single_byte(uint8_t);
void write_register_single_byte(uint8_t, uint8_t);
bool accel_data_available();
void accel_setup();

int16_t accel_x();
int16_t accel_y();
int16_t accel_z();

#endif //ACCEL_H_