#include <pic32mx.h>
#include <stdint.h>
#include <stdbool.h>
#include "i2c.h"

// TODO: handle I2COV (I2C overflow), I2CxRCR fully read before I2CxRCV finished
// need to check I2C1STAT<6> for I2COV bit.

/* configure i2c bus*/
void i2c_config() {
	// want to set i2c baud rate to around 400 kHz (fast mode)
	// want to divide PBCLK by 100 = 0x2C, all other bits 0.
	// as given Table 24-2 of Section 24 PIC32 Family Reference
	I2C1BRG = 0x02c;

	// set I2C1CON<15> = 1, to enable I2C module
	I2C1CONSET = 1 << 15;

    //TODO: possibly set SIDL bit? 


    // read data and throw it away (x only exists in local context)
    int x = I2C1RCV;

    // TODO: don't have to send start in this function, DELETE???
	// check stop bit I2C1STAT<4> to ensure idle, then assert start condition on SDA1 SCL1
	// while(!(I2C1STAT & 0x10));

	// I2C1CONSET = 1 << 0; // set start enable
}

/* wait until i2c bus idle*/
void wait_i2c_idle() {
    // "five least bits of I2CxCON must be 0 ... This ensures the master logic is inactive" (pg23)
    // Refer also "Table 24-3 Master Message Protocol States" for above (pg27)
    // I2C1STAT<14> = TRSTAT set when data in transmission, (pg9,32)
    // I2C1CON<4:0> = ACKEN, RCEN, PEN, RSEN, SEN. All cleared by module when bus idle (pg8)
    while(I2C1CON & 0x1F || I2C1STAT & (1 << 14));
}

/* send a byte of data on bus, return ack/nack*/
bool i2c_send(uint8_t data) {
    wait_i2c_idle();
    I2C1TRN = data;
    wait_i2c_idle();

    // I2C1STAT<15> = ACKSTAT set (=1) when acknowledge NOT received, (pg9)
    return !(I2C1STAT & (1 << 15));
}

/* return byte of data from bus*/
uint8_t i2c_recv() {
    wait_i2c_idle();
    // I2C1CON<3> = RCEN = 1 to enable receive
    I2C1CONSET = (1 << 3);
    wait_i2c_idle();
    
    // TODO: figure out why??
    I2C1STATCLR = 1 << 6; //I2COV = 0, WHY??

    return I2C1RCV;
}

/* send ack*/
void i2c_ack() {
    // Fig 24-11
    wait_i2c_idle();
    I2C1CONSET = (1 << 4); //set I2C1CON<4> = ACKEN to enable ACK send. 
    I2C1CONCLR = (1 << 5); //clear I2C1CON<5> = ACKDT to send 0 = ACK
}

/* send nack*/
void i2c_nack() {
    // Fig 24-12
    wait_i2c_idle();
    I2C1CONSET = (1 << 4); //set I2C1CON<4> = ACKEN to enable ACK send. 
    I2C1CONSET = (1 << 5); //set I2C1CON<5> = ACKDT to send 1 = NACK
}

/* send start sequence on bus*/
void i2c_start() {
    // From "24.5.1 Generating a Start Bus Event" (pg21)
    // TODO: second wait (below line) borrowed from hello-temperature. Not sure why.
    // second wait likely to see if PIC hardware has deasserted the bus
    wait_i2c_idle();
    I2C1CONSET = (1 << 0); // set I2C1CON<0> = SEN
    wait_i2c_idle();

}

/* send restart sequence on bus*/
void i2c_restart() {
    wait_i2c_idle();
    I2C1CONSET = (1 << 1); // set I2C1CON<1> = RSEN
    // second wait likely to see if PIC hardware has deasserted the bus
    wait_i2c_idle();
}

/* send stop sequence on bus*/
void i2c_stop() {
    wait_i2c_idle();
    wait_i2c_idle();
    I2C1CONSET = (1 << 2); // set I2C1CON<2> = PEN
    wait_i2c_idle();
}


// References: 
// PIC32 Family Reference Manual, Section 24. [1]
// is1200/example-projects/hello-temperature  [2]