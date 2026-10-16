/* 
 * File:   main.c
 * Author: shaunvarghese
 *
 * Created on October 8, 2026, 8:54 AM
 */

#pragma config WDTE = OFF

#include "xc.h"
#include "keypad.h"
#include "i2c.h"
#include "clcd.h"
#include "ds1307.h"
#include "timer0.h"
#include "time.h"
#include "main.h"


event_t event;

/*****TIMER variables external linking*****/
extern unsigned char clock_reg[];
extern unsigned char time[];
extern unsigned char date[];

extern volatile unsigned int one_second_flag;
extern volatile unsigned int count;
extern volatile unsigned int sys_ticks;//system global timing

unsigned int previous_activity_time_configmode;
extern unsigned char *config_mode_menu_options[];
unsigned char config_mode_menu_index = first_option_index;

/***SET/VIEW TIME and Date */
unsigned int previous_activity_time_set_time_and_date;
unsigned char set_time_date_menu_index = first_option_index;;
unsigned int previous_activity_time_set_time;
unsigned int previous_activity_time_confirm_time;
unsigned int previous_activity_time_set_date;
unsigned int previous_activity_time_confirm_date;

/**SET or View alarms */
unsigned int previous_activity_time_set_or_view_alarm;
unsigned char set_or_view_alarm_menu_index;
unsigned int previous_activity_time_set_alarm;
unsigned int previous_activity_time_view_alarm;
unsigned int previous_activity_confirm_alarm;


unsigned int blink_timer;

state_t mode = m_default_mode;


void init_leds()
{
    TRISB6 = 0; //RB6 as output
    TRISB7 = 0; //RB7 as output

    RB6 = 0;
    RB7 = 0;
}
static void init_config(void) 
{
    timer0_init();
    digital_keypad_init();
    init_clcd();
    i2c_init(100000);
    init_ds1307();
    init_leds();

}

void main(void) 
{
    init_config();
    print_default_mode_clcd_screen();
    find_next_event(); // loading events in eeprom at start
    while(1)
    {
        monitor_event();
        update_alarm_execution();
        update_alarm_leds();
        key_handler_update();
        switch(mode)
        {
            case m_default_mode : 
            {
                DEFAULT_MODE();
                break;
                
            }
            
            case m_config_mode : 
            {
                CONFIG_MODE();
                break;
            }
            case m_set_or_view_alarm: 
            {
                set_or_view_alarm();
                
                break;
            }
            case m_set_time_and_date: 
            {
                set_time_and_date();
                break;
            }
            case m_set_time:
            {
                set_time();
                break;
            }
            case m_confirm_save_time:
            {
                confirm_time();
                break;
            }
            case m_set_date:
            {
                set_date();
                break;
            }
            case m_confirm_save_date:
            {
                confirm_date();
                break;
            }
            case m_set_alarm:
            {
                set_event();
                break;
            }

            case m_confirm_save_alarm:
            {
                confirm_event();
                break;
            }
            case m_view_alarm:
            {
                break;
            }
            
            
        }
    }
    

}

void DEFAULT_MODE()
{
    unsigned char key;

    //update date and time
    
    clcd_get_date();
    clcd_print(date,LINE1(6));

    
    clcd_get_time();
    clcd_print(time,LINE2(6));
    

    key = key_handler_get_event();

    //display next alarm

    //detect switch press and change modes
    if(key == KEY_UP_PRESS || key == KEY_DOWN_PRESS)
    {
        //no idea what is required here
        previous_activity_time_configmode = get_ticks();
        config_mode_menu_index = first_option_index;
        print_screen_config_mode();
        mode = m_config_mode;
        key_handler_ignore_until_release();
        

    }
    return;
}
void print_default_mode_clcd_screen()
{
    clcd_print("DATE  ",LINE1(0));
    clcd_print("TIME  ",LINE2(0));

    clcd_print("  ",LINE1(14));
    clcd_print("  ",LINE2(14));

}

