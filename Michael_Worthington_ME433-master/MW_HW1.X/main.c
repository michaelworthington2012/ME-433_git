#include<xc.h>           // processor SFR definitions
//#include<sys/attribs.h>  // __ISR macro

// DEVCFG0
//#pragma config DEBUG = x // no debugging
//#pragma config JTAGEN = x // no jtag
//#pragma config ICESEL = x // use PGED1 and PGEC1
//#pragma config PWP = x // no write protect
//#pragma config BWP = x // no boot write protect
//#pragma config CP = x // no code protect

// DEVCFG1
//#pragma config FNOSC = x // use primary oscillator with pll
//#pragma config FSOSCEN = x // turn off secondary oscillator
//#pragma config IESO = x // no switching clocks
//#pragma config POSCMOD = x // high speed crystal mode
//#pragma config OSCIOFNC = x // free up secondary osc pins
//#pragma config FPBDIV = x // divide CPU freq by 1 for peripheral bus clock
//#pragma config FCKSM = x // do not enable clock switch
//#pragma config WDTPS = x // slowest wdt
//#pragma config WINDIS = x // no wdt window
//#pragma config FWDTEN = x // wdt off by default
//#pragma config FWDTWINSZ = x // wdt window at 25%

// DEVCFG2 - get the CPU clock to 48MHz
//#pragma config FPLLIDIV = x // divide input clock to be in range 4-5MHz
//#pragma config FPLLMUL = x // multiply clock after FPLLIDIV
//#pragma config FPLLODIV = x // divide clock after FPLLMUL to get 48MHz
//#pragma config UPLLIDIV = x // divider for the 8MHz input clock, then multiply by 12 to get 48MHz for USB
//#pragma config UPLLEN = x // USB clock on

// DEVCFG3
//#pragma config USERID = 0 // some 16bit userid, doesn't matter what
//#pragma config PMDL1WAY = x // allow multiple reconfigurations
//#pragma config IOL1WAY = x // allow multiple reconfigurations
//#pragma config FUSBIDIO = x // USB pins controlled by USB module
//#pragma config FVBUSONIO = x // USB BUSON controlled by USB module

// DEVCFG3
#pragma config USERID = 1234
#pragma config PMDL1WAY = OFF           // Peripheral Module Disable Configuration (Allow multiple reconfigurations)
#pragma config IOL1WAY = OFF            // Peripheral Pin Select Configuration (Allow multiple reconfigurations)
#pragma config FUSBIDIO = ON            // USB USID Selection (Controlled by the USB Module)
#pragma config FVBUSONIO = ON           // USB VBUS ON Selection (Controlled by USB Module)

// DEVCFG2
#pragma config FPLLIDIV = DIV_2         // PLL Input Divider (2x Divider)
#pragma config FPLLMUL = MUL_24         // PLL Multiplier (24x Multiplier)
#pragma config UPLLIDIV = DIV_2         // USB PLL Input Divider (2x Divider)
#pragma config UPLLEN = ON              // USB PLL Enable (Enabled)
#pragma config FPLLODIV = DIV_2         // System PLL Output Clock Divider (PLL Divide by 2)

// DEVCFG1
#pragma config FNOSC = PRIPLL           // Oscillator Selection Bits (Primary Osc w/PLL (XT+,HS+,EC+PLL))
#pragma config FSOSCEN = OFF            // Secondary Oscillator Enable (Disabled)
#pragma config IESO = OFF               // Internal/External Switch Over (Disabled)
#pragma config POSCMOD = HS             // Primary Oscillator Configuration (HS osc mode)
#pragma config OSCIOFNC = ON            // CLKO Output Signal Active on the OSCO Pin (Enabled)
#pragma config FPBDIV = DIV_1           // Peripheral Clock Divisor (Pb_Clk is Sys_Clk/1)
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor Selection (Clock Switch Disable, FSCM Disabled)
#pragma config WDTPS = PS1048576        // Watchdog Timer Postscaler (1:1048576)
#pragma config WINDIS = OFF             // Watchdog Timer Window Enable (Watchdog Timer is in Non-Window Mode)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (WDT Disabled (SWDTEN Bit Controls))
#pragma config FWDTWINSZ = WINSZ_25     // Watchdog Timer Window Size (Window Size is 25%)

// DEVCFG0
#pragma config JTAGEN = OFF             // JTAG Enable (JTAG Disabled)
#pragma config ICESEL = ICS_PGx1        // ICE/ICD Comm Channel Select (Communicate on PGEC1/PGED1)
#pragma config PWP = OFF                // Program Flash Write Protect (Disable)
#pragma config BWP = OFF                // Boot Flash Write Protect bit (Protection Disabled)
#pragma config CP = OFF                 // Code Protect (Protection Disabled)

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
    TRISBbits.TRISB4 = 1;
    TRISAbits.TRISA4 = 0;
    LATAbits.LATA4 = 1;
    
    while(1) {
        delay();
        LATAbits.LATA4 = !LATAbits.LATA4;
	    // use _CP0_SET_COUNT(0) and _CP0_GET_COUNT() to test the PIC timing
		// remember the core timer runs at half the CPU speed
    }
    return 0;
    
}

void delay(void) {
    int j;
    for (j = 0; j<1600; j++) {
        
    }
}