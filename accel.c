#include "pic32mx.h"
#include <stdint.h>
#include "stdbool.h"
#include "accel.h"
#include "i2c.h"
#include "LSM6DSO.h"

#ifndef ACCEL_SENSOR_ADDR
#define ACCEL_SENSOR_ADDR 0x6B
#endif

uint8_t get_register_single_byte(uint8_t register_address) {
    uint8_t datareturn;

    // address + write bit
    do {
        i2c_start();
    } while (!i2c_send(ACCEL_SENSOR_ADDR << 1 | 0));

    while(!i2c_send(register_address));

    i2c_restart();

    // address + read bit
    i2c_send((ACCEL_SENSOR_ADDR << 1) | 1);
    datareturn = i2c_recv();
    i2c_nack();
    i2c_stop();

    return datareturn;
}

/* write a single byte of data given in "data" to register given in register_address*/
void write_register_single_byte(uint8_t register_address, uint8_t data) {
    // address + write bit
    do {
        i2c_start();
    } while (!i2c_send((ACCEL_SENSOR_ADDR << 1) | 0));

    while(!i2c_send(register_address));
    while(!i2c_send(data)); // send until ACK
    i2c_stop();
}

bool accel_data_available() {
    // check STATUS_REG<0> = XLDA
    // data ready if XLDA == 1
    uint8_t status_reg_value = get_register_single_byte(STATUS_REG);
    return (status_reg_value & 0x1);
}

bool gyro_data_available() {
    // check STATUS_REG<1> = GDA
    // data ready if GDA == 1
    uint8_t status_reg_value = get_register_single_byte(STATUS_REG);
    return (status_reg_value & 0x2);
}

void accel_setup() {
    // from application note, 4.1 Startup Sequence
    write_register_single_byte(INT1_CTRL, 0x01);
    write_register_single_byte(CTRL1_XL, 0x60);

    //add code for data ready interrupt? 
}

// sets up data ready interrupt
void accel_interrupts_setup() {

}

/* 
returns 2s complement binary number of output data in x-direction
output is scaled integer of sensor mode max value.
e.g. if mode is ±2g, then accel_value = 2*(value/MAX_VALUE_INT_16_BIT)
similarily for ±16g, give accel_value= 16*(value/MAX_VALUE_INT_16_BIT)
*/
uint16_t accel_x() {
    uint16_t datareturn;

    while(!accel_data_available());
    datareturn = get_register_single_byte(OUTX_H_A);
    datareturn = datareturn << 8;
    datareturn |= get_register_single_byte(OUTX_L_A);

    return datareturn;
}


/* 
returns 2s complement binary number of output data in y-direction
output is scaled integer of sensor mode max value.
e.g. if mode is ±2g, then accel_value = 2*(value/MAX_VALUE_INT_16_BIT)
similarily for ±16g, give accel_value= 16*(value/MAX_VALUE_INT_16_BIT)
*/
uint16_t accel_y() {
    uint16_t datareturn;

    while(!accel_data_available());
    datareturn = get_register_single_byte(OUTY_H_A);
    datareturn = datareturn << 8;
    datareturn |= get_register_single_byte(OUTY_L_A);

    return datareturn;
}

/* 
returns 2s complement binary number of output data in z-direction
output is scaled integer of sensor mode max value.
e.g. if mode is ±2g, then accel_value = 2*(value/MAX_VALUE_INT_16_BIT)
similarily for ±16g, give accel_value= 16*(value/MAX_VALUE_INT_16_BIT)
*/
uint16_t accel_z() {
    uint16_t datareturn;
    
    while(!accel_data_available());
    datareturn = get_register_single_byte(OUTZ_H_A);
    datareturn = datareturn << 8;
    datareturn |= get_register_single_byte(OUTZ_L_A);

    return datareturn;
}

int conv_accel_to_g(uint16_t accel) {
    const int ACCEL_MODE = 2; // implies max ±2g
    return (ACCEL_MODE * (accel/32767)); // 32767 max value of 16-bit 2s complement int
}

// References:
// https://cdn.sparkfun.com/assets/2/3/c/6/5/lsm6dso.pdf [1]
// https://cdn.sparkfun.com/assets/c/b/2/e/f/dm00517282-lsm6dso-alwayson-3d-accelerometer-and-3d-gyroscope-stmicroelectronics.pdf [2]