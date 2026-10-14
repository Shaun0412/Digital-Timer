#include "xc.h"
#include "keypad.h"
#include "clcd.h"
#include "ds1307.h"
#include "timer0.h"
#include "time.h"
#include "main.h"
extern unsigned char long_press_detected;
extern unsigned char prev_key;

extern unsigned int previous_activity_time_configmode;
extern unsigned int previous_activity_time_set_or_view_alarm;

extern unsigned char config_mode_menu_index;
extern unsigned char set_or_view_alarm_menu_index;

extern state_t mode;


//Set alarm
//View Alarm
void set_or_view_alarm()
{
    unsigned char key;
    
    static unsigned int start_tick = 0;
    unsigned int current_tick;
    
    key = read_digital_keypad(LEVEL);
    
    
    // arrow to select
    /******************************    KEY HANDLING    ***************************************/
    //long press for 2seconds on UP button go to the next page in the present menu
    //long press for 2seconds on DOWN button goes to previous page
    //if inactivity is detected for 5 seconds then go back to default mode

    //LONG UP-> goes to next menu
    //LONG DOWN->goes back from current menu
    //SHORT UP/DOWN -> scrolls through the options//
    if(key == UP || prev_key == UP) 
    { 
        previous_activity_time_set_or_view_alarm = get_ticks();
        if(key == UP && prev_key == ALL_RELEASED)//the very first press
        {
            start_tick = get_ticks();
            long_press_detected = NO;//starting from scratch for long press
        }
        
        current_tick = get_ticks();
        
        if(prev_key == key && long_press_detected == NO)
        {
            if(current_tick - start_tick >= LONG_PRESS_THRESHOLD)
            {
                //execute long press operation
                long_press_detected = YES;
                //oepration
                if(set_or_view_alarm_menu_index == first_option_index)
                {
                    //selection of first option
                    /*==========================PRINT NEXT SCREEN===================== */
                    mode = m_set_alarm;
                    
                }
                else if (set_or_view_alarm_menu_index == second_option_index)
                {
                    //selection of first option
                    mode = m_view_alarm;
                    /*==========================PRINT NEXT SCREEN===================== */
                    
                }

            }
            
        }
        else if(key == ALL_RELEASED && long_press_detected == NO && prev_key == UP)
        {   
            //short press
            if(set_or_view_alarm_menu_index == first_option_index)
            {
                //moves arrow to second line
                set_or_view_alarm_menu_index = second_option_index;
                clcd_putch(' ',LINE1(0));
                clcd_putch('>',LINE2(0));
            }
            else if(set_or_view_alarm_menu_index == second_option_index)
            {
                //moves to first line
                set_or_view_alarm_menu_index = first_option_index;
                clcd_putch('>',LINE1(0));
                clcd_putch(' ',LINE2(0));
            }
        }
        
        
        
    }
    if(key == DOWN || prev_key == DOWN) 
    {
        previous_activity_time_set_or_view_alarm = get_ticks();
        if(key == DOWN && prev_key == ALL_RELEASED)//the very first press
        {
            start_tick = get_ticks();
            long_press_detected = NO;//starting from scratch for long press
        }
        
        current_tick = get_ticks();
        
        if(prev_key == key && long_press_detected == NO)
        {
            if(current_tick - start_tick >= LONG_PRESS_THRESHOLD)
            {
                //execute long press operation
                long_press_detected = YES;
                //operation
                print_screen_config_mode();
                config_mode_menu_index = first_option_index;
                previous_activity_time_configmode = get_ticks();
                mode = m_config_mode;
                
            }
            
        }
        else if(key == ALL_RELEASED && long_press_detected == NO && prev_key == DOWN)
        {   
            //short press
            if(set_or_view_alarm_menu_index== first_option_index)
            {
                //moves arrow to second line
                set_or_view_alarm_menu_index = second_option_index;
                clcd_putch(' ',LINE1(0));
                clcd_putch('>',LINE2(0));

            }
            else if(set_or_view_alarm_menu_index == second_option_index)
            {
                //moves arrow to first line 
                set_or_view_alarm_menu_index= first_option_index;
                clcd_putch('>',LINE1(0));
                clcd_putch(' ',LINE2(0)); 
            }

        }
    }
    else if(key == ALL_RELEASED && prev_key == ALL_RELEASED)
    {
        if((get_ticks() - previous_activity_time_set_or_view_alarm) >= INACTIVITY_THRESHOLD)
        {
            print_default_mode_clcd_screen();
            mode = m_default_mode;
        }
    }
        
    prev_key = key;    
    return;
}
