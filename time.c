


#include <xc.h>
#include "i2c.h"
#include "ds1307.h"
#include "clcd.h"

unsigned char time[10];
unsigned char date[10];
unsigned char clock_reg[10];

extern volatile unsigned int sys_ticks;
static void get_rtc_info()
{
    //DATA IS IN BCD format
    clock_reg[0] = read_ds1307(HOUR_ADDR);
    clock_reg[1] = read_ds1307(MIN_ADDR);
    clock_reg[2] = read_ds1307(SEC_ADDR);
    
    clock_reg[3] = read_ds1307(DAY_ADDR);
    clock_reg[4] = read_ds1307(DATE_ADDR);
    clock_reg[5] = read_ds1307(MONTH_ADDR);
        
    clock_reg[6] = read_ds1307(YEAR_ADDR);
}

void get_time(unsigned char *time)
{
    time[0] = read_ds1307(HOUR_ADDR);
    time[1] =read_ds1307(MIN_ADDR);
    time[2] = read_ds1307(SEC_ADDR);
    return;
}

unsigned char * clcd_get_time()
{
    get_rtc_info();
    //HH
    time[0]=((clock_reg[0]>>4) &(0x03))+'0';
    time[1]=(clock_reg[0] & 0x0F)+'0';
    time[2] = ':';
    //MM
    time[3] =((clock_reg[1]>>4) & 0x07)+ '0';
    time[4] = (clock_reg[1] & 0x0F)+'0';
    time[5] = ':';
    
    //SS
    time[6] = ((clock_reg[2]>>4) & 0x07) +'0';
    time[7] = (clock_reg[2] & 0x0F) +'0';
    time[8] = '\0';

}


unsigned char* clcd_get_date()
{
    get_rtc_info();

    date[0] = ((clock_reg[4] >> 4) & 0x03) + '0';
    date[1] = (clock_reg[4] & 0x0F) + '0';
    date[2] = '-';

    date[3] = ((clock_reg[5] >> 4) & 0x01) + '0';
    date[4] = (clock_reg[5] & 0x0F) + '0';
    date[5] = '-';

    date[6] = ((clock_reg[6] >> 4) & 0x0F) + '0';
    date[7] = (clock_reg[6] & 0x0F) + '0';
    date[8] = '\0';

    return date;
}




unsigned int get_ticks()
{
    unsigned int timer0_status = TMR0IE;//
    unsigned int system_ticks;


    // sys_ticks is a 16-bit variable, but PIC16F877A is an 8-bit MCU.
    // Reading it requires two byte reads. If Timer0 interrupts between
    // these reads and updates sys_ticks, we may get an incorrect value.
    // So temporarily disable Timer0 interrupt for a consistent read.

    /*
     * Example: sys_ticks = 0x00FF (255)
     *
     * 1. CPU reads lower byte = 0xFF.
     * 2. Timer0 interrupt occurs, incrementing sys_ticks to 0x0100 (256).
     * 3. CPU reads upper byte = 0x01.
     *
     * Result: CPU may combine them as 0x01FF (511),
     * instead of the correct value 255 or 256.
     **/
    
    TMR0IE = 0;//disabling timer interrupts for safe read 
    system_ticks = sys_ticks;
    TMR0IE = timer0_status; // restoring timer0 status

    return system_ticks;

}

