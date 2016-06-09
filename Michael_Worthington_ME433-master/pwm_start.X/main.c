/* 
 * File:   main.c
 * Author: michael_worthington
 *
 * Created on June 5, 2016, 1:52 PM
 */
#include<xc.h>           // processor SFR definitions
#include <stdio.h>
#include <stdlib.h>
#include "PIC32_config.h"

void delay(void);

int main() {

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
    __builtin_enable_interrupts();
    
    // setting A4 as output and B4 as input
    TRISBbits.TRISB15 = 0; // b15 is an output
    TRISBbits.TRISB13 = 0; //b 13 is also an output
    TRISBbits.TRISB8 = 0; 
    TRISBbits.TRISB9 = 0; 
    
    LATBbits.LATB13 = 1; // set b13 as high 
    
    TRISBbits.TRISB4 = 1; // set B as an input
    TRISAbits.TRISA4 = 0; // set A as an output
    LATAbits.LATA4 = 1; // Set output as high to test
    
    while(1) {
        //delay();
        
	     
        
          // use _CP0_SET_COUNT(0) and _CP0_GET_COUNT() to test the PIC timing
		// remember the core timer runs at half the CPU speed
		// remember the core timer runs at half the CPU speed
    }
    return 0;
    
}

void delay(void) {
    //_CPO_SET_COUNT(0);
    //while(_CPO_GET_COUNT()<4) {
       // ;
    //}
}

