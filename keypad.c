#include <xc.h>
#include "keypad.h"

#define _XTAL_FREQ 20000000

void digital_keypad_init(void)
{
    KEYPORT |= 0x3F;  // RB0 to RB5 as inputs
}

unsigned char read_digital_keypad(unsigned char mode)
{
    static unsigned char stable_key = ALL_RELEASED;
    static unsigned char previous_state_key = ALL_RELEASED;

    unsigned char key = PORTB & 0x3F;

    // Check for a stable change in key state
    if (key != stable_key)
    {
        __delay_ms(10);

        key = PORTB & 0x3F;

        if (key != stable_key)
        {
            stable_key = key;
        }
    }

    if (mode == LEVEL)
    {
        return stable_key;
    }

    if (mode == STATE)
    {
        unsigned char result = ALL_RELEASED;

        // Generate an event only for a new press
        if (stable_key != ALL_RELEASED &&
            previous_state_key == ALL_RELEASED)
        {
            result = stable_key;
        }

        previous_state_key = stable_key;

        return result;
    }

    return ALL_RELEASED;
}