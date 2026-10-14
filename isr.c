
#include "timer0.h"
#include <xc.h>

volatile unsigned int one_second_flag;
volatile unsigned int count;
volatile unsigned int sys_ticks;
void __interrupt() isr(void)
{
    if(TMR0IF)
    {
        
        ++count;
        TMR0 = TMR0 + 56;
        ++sys_ticks; //increases every 10ms
        if(count == 100)
        {
            one_second_flag = 1; //indicates one_second
            count = 0;
            
        }
        TMR0IF = 0;
        
    }
}