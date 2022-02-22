#ifndef I2C_H_
#define I2C_H_

void i2c_config();

void wait_i2c_idle();
bool i2c_send(uint8_t data);
uint8_t i2c_recv();

void i2c_ack();
void i2c_nack();

void i2c_start();
void i2c_restart();
void i2c_stop();

#endif //I2C_H_