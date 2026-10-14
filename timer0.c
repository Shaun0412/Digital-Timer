
#include <xc.h>
#include "timer0.h"
extern volatile unsigned int one_second_flag;
extern volatile unsigned int count;
extern volatile unsigned int sys_ticks;
void timer0_init(void)
{
    GIE = 1;
    
    OPTION_REG = 0x07; //1:256 prescale, timer mode, scaling assigned to timer0
    TMR0 = 56 + TMR0; //preload

    TMR0IE = 1;
    TMR0IF = 0;
}




