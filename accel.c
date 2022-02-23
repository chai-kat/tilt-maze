#include <stdint.h>
#include "accel.h"
#include "i2c.h"

/* 
page 18,19 [1] I2C operation Table 11 onwards.

single byte write:
- send start
- send address + rw bit (w)
    - get ack
- send sub address
    - get ack
- send data
     -get ack
-stop

multi-byte write:
- send start
- send address + rw bit (w)
    - get ack
- send sub address
    - get ack
- send data
     -get ack
- send data
     -get ack
-stop

single byte read:
- send start
- send address + rw bit (w)
    - get ack
- send sub address
    - get ack
- REPEATED START
- send address + rw bit (r)
     -get ack
- RECV data
    -send nack
-stop


multi-byte read:
- send start
- send address + rw bit (w)
    - get ack
- send sub address
    - get ack
- REPEATED START
- send address + rw bit (r)
     -get ack
- RECV data
    -send ack
- recv data
    -send ack
- recv data
    -send nack
-stop

*/

#ifndef ACCEL_SENSOR_ADDR
#define ACCEL_SENSOR_ADDR 0x6B
#endif

void accel_setup() {

}

/* returns 2s complement binary number of output data*/
uint16_t accel_x() {
    uint16_t datareturn;

    do {
        i2c_start();
    } while (!i2c_send(ACCEL_SENSOR_ADDR));

    // TODO: wrap this line??? 
    i2c_send(0x29); // x high subaddress (OUTX_H_A), page 41 [1]
    i2c_restart();
    i2c_send(ACCEL_SENSOR_ADDR);
    datareturn = i2c_recv();
    datareturn = (datareturn << 8);
    i2c_nack();
    i2c_stop();

    do {
        i2c_start();
    } while(!i2c_send(ACCEL_SENSOR_ADDR));

    // TODO: wrap this line???
    i2c_send(0x28); // x low subaddress (OUTX_L_A), page 41 [1] 
    i2c_restart();
    i2c_send(ACCEL_SENSOR_ADDR);
    datareturn = i2c_recv();
    i2c_nack();
    i2c_stop();
    
    return datareturn;
}

uint16_t accel_y() {
   uint16_t datareturn;

    do {
        i2c_start();
    } while (!i2c_send(ACCEL_SENSOR_ADDR));

    // TODO: wrap this line??? 
    i2c_send(0x2B); // y high subaddress (OUTY_H_A), page 41 [1]
    i2c_restart();
    i2c_send(ACCEL_SENSOR_ADDR);
    datareturn = i2c_recv();
    datareturn = (datareturn << 8);
    i2c_nack();
    i2c_stop();

    do {
        i2c_start();
    } while (!i2c_send(ACCEL_SENSOR_ADDR));

    // TODO: wrap this line???
    i2c_send(0x2A); // y low subaddress (OUTY_L_A), page 41 [1] 
    i2c_restart();
    i2c_send(ACCEL_SENSOR_ADDR);
    datareturn = i2c_recv();
    i2c_nack();
    i2c_stop();

    return datareturn;
}

uint16_t accel_z() {
   uint16_t datareturn;

    do {
        i2c_start();
    } while (!i2c_send(ACCEL_SENSOR_ADDR));

    // TODO: wrap this line??? 
    i2c_send(0x2D); // z high subaddress (OUTZ_H_A), page 41 [1]
    i2c_restart();
    i2c_send(ACCEL_SENSOR_ADDR);
    datareturn = i2c_recv();
    datareturn = (datareturn << 8);
    i2c_nack();
    i2c_stop();

    do {
        i2c_start();
    } while (!i2c_send(ACCEL_SENSOR_ADDR));

    // TODO: wrap this line???
    i2c_send(0x2C); // z low subaddress (OUTZ_L_A), page 41 [1] 
    i2c_restart();
    i2c_send(ACCEL_SENSOR_ADDR);
    datareturn = i2c_recv();
    i2c_nack();
    i2c_stop();

    return datareturn;
}

// References:
// https://cdn.sparkfun.com/assets/2/3/c/6/5/lsm6dso.pdf [1]
// https://cdn.sparkfun.com/assets/c/b/2/e/f/dm00517282-lsm6dso-alwayson-3d-accelerometer-and-3d-gyroscope-stmicroelectronics.pdf [2]