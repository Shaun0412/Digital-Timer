#include "keypad.h"
#include "time.h"
#include "main.h"

static unsigned char key_event = KEY_NONE;
static unsigned char active_key = ALL_RELEASED;
static unsigned char long_press_detected = NO;
static unsigned char ignore_until_release = NO;
static unsigned int start_tick = 0;

void key_handler_update(void)
{
    unsigned char key = read_digital_keypad(LEVEL);
    unsigned int current_tick = get_ticks();

    key_event = KEY_NONE;

    if(ignore_until_release == YES)
    {
        if(key == ALL_RELEASED)
        {
            ignore_until_release = NO;
            active_key = ALL_RELEASED;
            long_press_detected = NO;
        }
        return;
    }

    if(key != active_key)
    {
        if(key == ALL_RELEASED)
        {
            if(long_press_detected == NO)
            {
                if(active_key == UP)
                {
                    key_event = KEY_UP_SHORT;
                }
                else if(active_key == DOWN)
                {
                    key_event = KEY_DOWN_SHORT;
                }
            }
        }
        else
        {
            start_tick = current_tick;
            if(key == UP)
            {
                key_event = KEY_UP_PRESS;
            }
            else if(key == DOWN)
            {
                key_event = KEY_DOWN_PRESS;
            }
        }
        active_key = key;
        long_press_detected = NO;
    }
    else if((key == UP || key == DOWN) && long_press_detected == NO && (unsigned int)(current_tick - start_tick) >= LONG_PRESS_THRESHOLD)
    {
        long_press_detected = YES;
        if(key == UP)
        {
            key_event = KEY_UP_LONG;
        }
        else
        {
            key_event = KEY_DOWN_LONG;
        }
    }
}

unsigned char key_handler_get_event(void)
{
    return key_event;
}

unsigned char key_handler_inactive(unsigned int *previous_activity_time)
{
    if(active_key != ALL_RELEASED || key_event != KEY_NONE)
    {
        *previous_activity_time = get_ticks();
        return NO;
    }
    return (unsigned int)(get_ticks() - *previous_activity_time) >= INACTIVITY_THRESHOLD;
}

void key_handler_ignore_until_release(void)
{
    ignore_until_release = YES;
}
