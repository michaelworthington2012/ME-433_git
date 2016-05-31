#include    "PIC32_config.h"
#include    <stdio.h>
#include    "ILI9163C.h"
#include    "i2c_master.h"



void LCD_makeString(char xpos, char ypos, char* print_string);
void LCD_drawChar(char xpos, char ypos, char ch);

unsigned short text = BLACK;
unsigned short screen = RED;
    
int main(void) {
    // PIC32 Setup
    __builtin_disable_interrupts();
    __builtin_mtc0(_CP0_CONFIG,_CP0_CONFIG_SELECT,0xa4210583);
    BMXCONbits.BMXWSDRM = 0x0;  // 0 data RAM access wait states
    INTCONbits.MVEC = 0x1;      // enable multi vector interrupts
    DDPCONbits.JTAGEN = 0;      // disable JTAG to get pins back
    TRISAbits.TRISA4 = 0;       // RA4 is output
    TRISBbits.TRISB4 = 1;       // RB4 is input
    LATAbits.LATA4 = 0;	
    SPI1_init();
    LCD_init();
    i2c_master_setup();
    
    __builtin_enable_interrupts();
    
    
    while(1){
    LCD_clearScreen(screen);
   
    LCD_makeString(10,50,"Hi Charlie");
     LCD_makeString(10,75,"Go Cats!");
    LATAbits.LATA4 = 1;
    //LCD_drawPixel(64, 64, BLUE);
    
    //LCD_clearScreen(GREEN);
    //LCD
    //LATAbits.LATA4 = 1;
    return (EXIT_SUCCESS);
    }
}

/*static unsigned char pGammaSet[15]= {0x36,0x29,0x12,0x22,0x1C,0x15,0x42,0xB7,0x2F,0x13,0x12,0x0A,0x11,0x0B,0x06};
static unsigned char nGammaSet[15]= {0x09,0x16,0x2D,0x0D,0x13,0x15,0x40,0x48,0x53,0x0C,0x1D,0x25,0x2E,0x34,0x39};

unsigned char spi_io(unsigned char); // send and rx a byte over spi
void LCD_command(unsigned char); // send a command to the LCD
void LCD_data(unsigned char); // send data to the LCD
void LCD_data16(unsigned short); // send 16 bit data to the LCD
void LCD_init(void); // send the initializations to the LCD
void LCD_drawPixel(unsigned short, unsigned short, unsigned short); // set the x,y pixel to a color
void LCD_setAddr(unsigned short, unsigned short, unsigned short, unsigned short); // set the memory address you are writing to
void LCD_clearScreen(unsigned short); // set the color of every pixel
*/


void LCD_makeString(char xpos, char ypos, char* print_string)
{
    int i =0;
    int strLength = strlen(print_string);
    for (; i < strLength;    i++)
    {
        LCD_drawChar(xpos+i*5,ypos,print_string[i]);
    }
}
void LCD_drawChar(char xpos, char ypos, char ch)
{
    if(xpos>123||ypos>121){return;}
    int x=0,y=0;
    short color;
    for(;x<5;x++)
    {
        for(;y<8;y++)
        {
            if(ASCII[ch-0x20][x]&(1<<y))    {color = text;}
            else                            {color = screen;}
            LCD_drawPixel(xpos+x, ypos+y, color);
        }
        y = 0;
    }
}