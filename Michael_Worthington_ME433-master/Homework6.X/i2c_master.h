#ifndef I2C_MASTER_H
#define I2C_MASTER_H
#define     CTRL1_XL    (char)  0x10    //  0b00010000
#define     CTRL2_G     (char)  0x11    //  0b00010001
#define     CTRL3_C     (char)  0x12    //  0b00010010
#define     CTRL1_VAL   (char)  0x80    //  0b10000000
#define     CTRL2_VAL   (char)  0x80    //  0b10000000
#define     CTRL3_VAL   (char)  0x04    //  0b00000100
#define     OUT_TEMP_L  (char)  0x20    //  0b00100000
#define     WHO_AM_I    (char)  0x0F    //  0b00001111
#define     WHOAMI_VAL  (char)  0x69    //  0b01101001
#define     IMU_ADDR    (char)  0x6B    //  0b1101011x(R/W)
char i2c_master_read(char device,char reg_addr);
void i2c_master_read_all(char device,char start_reg,int numval,unsigned char* array);
void i2c_master_write(char device, char reg_addr, char write);
void i2c_master_setup(void);            //  set up I2C2 as a master, at 100 kHz
void i2c_master_start(void);            //  send a START signal
void i2c_master_restart(void);          //  send a RESTART signal
void i2c_master_send(unsigned char byte); // send a byte (either an address or data)
unsigned char i2c_master_recv(void);    //  receive a byte of data
void i2c_master_ack(int val);           //  send an ACK (0) or NACK (1)
void i2c_master_stop(void);             //  send a stop
#endif