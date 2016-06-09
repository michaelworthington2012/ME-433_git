#include<xc.h>           // processor SFR definitions
#include<sys/attribs.h>  // __ISR macro
//#include "i2c_master_noint.h"
#include <math.h> 

void i2c_master_setup(void);              // set up I2C2 as a master, at 100 kHz
void i2c_master_start(void);              // send a START signal
void i2c_master_restart(void);            // send a RESTART signal
void i2c_master_send(unsigned char byte); // send a byte (either an address or data)
unsigned char i2c_master_recv(void);      // receive a byte of data
void i2c_master_ack(int val);             // send an ACK (0) or NACK (1)
void i2c_master_stop(void);               // send a stop
// DEVCFG0
#pragma config DEBUG = OFF // no debugging
#pragma config JTAGEN = OFF // no jtag
#pragma config ICESEL = ICS_PGx1 // use PGED1 and PGEC1
#pragma config PWP = OFF // no write protect
#pragma config BWP = OFF // no boot write protect
#pragma config CP = OFF // no code protect

// DEVCFG1
#pragma config FNOSC = PRIPLL // use primary oscillator with pll
#pragma config FSOSCEN = OFF // turn off secondary oscillator
#pragma config IESO = OFF // no switching clocks
#pragma config POSCMOD = HS // high speed crystal mode
#pragma config OSCIOFNC = OFF // free up secondary osc pins
#pragma config FPBDIV = DIV_1 // divide CPU freq by 1 for peripheral bus clock
#pragma config FCKSM = CSDCMD // do not enable clock switch
#pragma config WDTPS = PS1048576 // slowest wdt
#pragma config WINDIS = OFF // no wdt window
#pragma config FWDTEN = OFF // wdt off by default
#pragma config FWDTWINSZ = WINSZ_25 // wdt window at 25%

// DEVCFG2 - get the CPU clock to 48MHz
#pragma config FPLLIDIV = DIV_2 // divide input clock to be in range 4-5MHz
#pragma config FPLLMUL = MUL_20 // multiply clock after FPLLIDIV
#pragma config FPLLODIV = DIV_2 // divide clock after FPLLMUL to get 48MHz
#pragma config UPLLIDIV = DIV_2 // divider for the 8MHz input clock, then multiply by 12 to get 48MHz for USB
#pragma config UPLLEN = ON // USB clock on

// DEVCFG3
#pragma config USERID = 0x1234 // some 16bit userid, doesn't matter what
#pragma config PMDL1WAY = OFF // allow multiple reconfigurations
#pragma config IOL1WAY = OFF // allow multiple reconfigurations
#pragma config FUSBIDIO = ON // USB pins controlled by USB module
#pragma config FVBUSONIO = ON // USB BUSON controlled by USB module

// PIC is the master, ram is the slave
// SDO1(RA1) -> SDI
// SDI1(RB8) -> VOUTA 
// SCK1(RB14) -> SCK
// SS1(RB7) -> CS
#define CS LATBbits.LATB7       // chip select pin
#define pi 3.1415926

static volatile float Waveforms[100]; //wave form
static volatile float Waveformt[200]; //wave form
unsigned char read  = 0x00;
unsigned char GP7 = 0x00;

// send a byte via spi and return the response
unsigned char spi_io(unsigned char o) {
  SPI1BUF = o;
  while(!SPI1STATbits.SPIRBF) { // wait to receive the byte
    ;
  }
  return SPI1BUF;
}

void setVoltage(char channel, float voltage){//channel is 0 or 1 (for VoutA and VoutB)
    int v = voltage;
    if(channel == 0){
    CS = 0; 
    spi_io(v >> 4 | 0b01110000);
    spi_io(v << 4);
    CS = 1;}  
    else{
    CS = 0; 
    spi_io(v >> 4 | 0b11110000);
    spi_io(v << 4);
    CS = 1; }
}

void makesinewave(){
    int i = 0;
    for (i = 0; i < 100; ++i){
        Waveforms[i] = 124 + 125*sin(2*pi*10*0.001*i);}
}

void maketrianglewave(){
    int i = 0;
    for (i = 0; i < 200; ++i){    
        Waveformt[i] = 250*i*0.005;}
}

void spi1_init() {
  // when a command is beginning (clear CS to low) and when it
  // is ending (set CS high)
  TRISBbits.TRISB7 = 0b0;
  CS = 1;
  SS1Rbits.SS1R = 0b0100;   // assign SS1 to RB7
  SDI1Rbits.SDI1R = 0b0000; // assign SDI1 to RA1 
  RPB8Rbits.RPB8R = 0b0011; // assign SDO1 to RB8
  ANSELBbits.ANSB14 = 0;    // turn off AN10
  // setup spi
  SPI1CON = 0;              // turn off the spi module and reset it
  SPI1BUF;                  // clear the rx buffer by reading from it
  SPI1BRG = 0x1;            // baud rate to 12 MHz [SPI1BRG = (48000000/(2*desired))-1]
  SPI1STATbits.SPIROV = 0;  // clear the overflow bit
  SPI1CONbits.MODE32 = 0;  //MODE<32,16> = <0,0> ->communication is byte-wide (8 bits)
  SPI1CONbits.MODE16 = 0;
  SPI1CONbits.CKE = 1;      // data changes when clock goes from hi to lo (since CKP is 0)
  SPI1CONbits.MSTEN = 1;    // master operation
  SPI1CONbits.ON = 1;       // turn on spi 1
}

void initExpander(){
    i2c_master_start();
    i2c_master_send(0x40);    //write
    i2c_master_send(0x00);  //IODIR
    i2c_master_send(0xf0); //7:4->1;3:0->0
    i2c_master_stop();
}


unsigned char getExpander(){
    i2c_master_start();
    i2c_master_send(0x40);    
    i2c_master_send(0x09);//GPIO
    i2c_master_restart();
    i2c_master_send(0x41);//read
    read = i2c_master_recv();
    i2c_master_ack(1);
    i2c_master_stop();
    
    return read;
}
unsigned char setlow(int pin){
    unsigned char b1=0xff;
    unsigned char b2, b3, b4;
    b2 = b1 << (pin+1);
    b3 = b1 >> (8-pin);
    b4 = b2 ^ b3;
    return b4;
}
void setExpander(int pin, int out){
        
        getExpander();
        i2c_master_start();
        i2c_master_send(0x40);    
        i2c_master_send(0x0A);
        if(out == 1){
            i2c_master_send((1 << pin)|read);
        }
        if(out == 0){
            unsigned char temp;
            temp = setlow(pin);
            i2c_master_send(read&temp);
        }        
        i2c_master_stop();   
}


int main(){
    __builtin_disable_interrupts();
    // set the CP0 CONFIG register to indicate that kseg0 is cacheable (0x3)
    __builtin_mtc0(_CP0_CONFIG, _CP0_CONFIG_SELECT, 0xa4210583);
    // 0 data RAM access wait states
    BMXCONbits.BMXWSDRM = 0x0;
    // enable multi vector interrupts
    INTCONbits.MVEC = 0x1;
    // disable JTAG to get pins back
    DDPCONbits.JTAGEN = 0;  
    // do your TRIS and LAT commands here
    TRISBbits.TRISB4 = 1;   // pin B4 as input  
    TRISAbits.TRISA4 = 0;  // pin A4 as output
    //LATAbits.LATA4 = 1;       
    __builtin_enable_interrupts();
    
    makesinewave();
    maketrianglewave();
    spi1_init();
    i2c_master_setup();
    initExpander();
       /* i2c_master_start();
        i2c_master_send(0x40);
        i2c_master_send(0x0A);
        i2c_master_send(0x00);
        i2c_master_stop();*/


    while(1) {
        _CP0_SET_COUNT(0); // Reset the core counter
        //setExpander(3, 1);
        GP7 = (getExpander() >> 7);
        while(_CP0_GET_COUNT() < 12000){;}
        if(GP7 == 1){
            setExpander(0, 1);
        }
        else{
            setExpander(0, 0);
            
        }
        static int s = 0;
        static int t = 0;
        setVoltage(0,Waveforms[s]);
        setVoltage(1,Waveformt[t]);
        s++;
        t++;
        if(s >= 100){ s = 0;}
        if(t >= 200){ t = 0;}        
        while(_CP0_GET_COUNT() < 24000){;}    
    }
}

void i2c_master_setup(void) {
  I2C2BRG = 233;                    // I2CBRG = [1/(2*100kHz) - 104ns]*48MHz - 2 
  ANSELBbits.ANSB2 = 0;             // turn off default analog pin B2
  ANSELBbits.ANSB3 = 0;             // turn off default analog pin B3
  I2C2CONbits.ON = 1;               // turn on the I2C2 module
}

// Start a transmission on the I2C bus
void i2c_master_start(void) {
    I2C2CONbits.SEN = 1;            // send the start bit
    while(I2C2CONbits.SEN) { ; }    // wait for the start bit to be sent
}

void i2c_master_restart(void) {     
    I2C2CONbits.RSEN = 1;           // send a restart 
    while(I2C2CONbits.RSEN) { ; }   // wait for the restart to clear
}

void i2c_master_send(unsigned char byte) { // send a byte to slave
  I2C2TRN = byte;                   // if an address, bit 0 = 0 for write, 1 for read
  while(I2C2STATbits.TRSTAT) { ; }  // wait for the transmission to finish
  if(I2C2STATbits.ACKSTAT) {        // if this is high, slave has not acknowledged
    // ("I2C2 Master: failed to receive ACK\r\n");
  }
}

unsigned char i2c_master_recv(void) { // receive a byte from the slave
    I2C2CONbits.RCEN = 1;             // start receiving data
    while(!I2C2STATbits.RBF) { ; }    // wait to receive the data
    return I2C2RCV;                   // read and return the data
}

void i2c_master_ack(int val) {        // sends ACK = 0 (slave should send another byte)
                                      // or NACK = 1 (no more bytes requested from slave)
    I2C2CONbits.ACKDT = val;          // store ACK/NACK in ACKDT
    I2C2CONbits.ACKEN = 1;            // send ACKDT
    while(I2C2CONbits.ACKEN) { ; }    // wait for ACK/NACK to be sent
}

void i2c_master_stop(void) {          // send a STOP:
  I2C2CONbits.PEN = 1;                // comm is complete and master relinquishes bus
  while(I2C2CONbits.PEN) { ; }        // wait for STOP to complete
}