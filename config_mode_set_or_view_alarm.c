//#include "xc.h"
#include "keypad.h"
#include "clcd.h"
#include "ds1307.h"
#include "timer0.h"
#include "time.h"
#include "main.h"
#include "ext_eeprom.h"

extern unsigned int previous_activity_time_configmode;
extern unsigned int previous_activity_time_set_or_view_alarm;
extern unsigned int previous_activity_time_set_alarm;
extern unsigned int previous_activity_time_view_alarm;
extern unsigned int previous_activity_confirm_alarm;


extern unsigned char config_mode_menu_index;
extern unsigned char set_or_view_alarm_menu_index;

extern state_t mode;

extern unsigned int blink_timer;

static unsigned char initialized = NO;

//Set alarm
//View Alarm
void set_or_view_alarm()
{
    unsigned char key_event = key_handler_get_event();
    // arrow to select
    /******************************    KEY HANDLING    ***************************************/
    //long press for 2seconds on UP button go to the next page in the present menu
    //long press for 2seconds on DOWN button goes to previous page
    //if inactivity is detected for 5 seconds then go back to default mode

    //LONG UP-> goes to next menu
    //LONG DOWN->goes back from current menu
    //SHORT UP/DOWN -> scrolls through the options//
    if(key_event == KEY_UP_LONG)
    {
                //execute long press operation

                //oepration
                if(set_or_view_alarm_menu_index == first_option_index)
                {
                    //selection of first option
                    /*==========================PRINT NEXT SCREEN===================== */
                    mode = m_set_alarm;
                    previous_activity_time_set_alarm = get_ticks();
                    
                }
                else if (set_or_view_alarm_menu_index == second_option_index)
                {
                    
                    mode = m_view_alarm;
                    previous_activity_time_view_alarm = get_ticks();
                   
                }

            
    }
    else if(key_event == KEY_UP_SHORT)
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
    else if(key_event == KEY_DOWN_LONG)
    {
                //execute long press operation

                //operation
                print_screen_config_mode();
                config_mode_menu_index = first_option_index;
                previous_activity_time_configmode = get_ticks();
                mode = m_config_mode;
                
            
    }
    else if(key_event == KEY_DOWN_SHORT)
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
    if(key_handler_inactive(&previous_activity_time_set_or_view_alarm))
    {
            print_default_mode_clcd_screen();
            mode = m_default_mode;
        
    }
    
    return;
}



static void set_event_util()
{
    unsigned char current_time[3];
    get_time(current_time);
    event.hour =(((current_time[0] & 0x3F) >> 4) * 10) + (current_time[0] & 0x0F);
    event.minute =((current_time[1] >> 4) * 10)+ (current_time[1] & 0x0F);
    
    event.duration_hour = 0;
    event.duration_minute = 0;
    event.duration_second = 10;

    event.trigger_type = O;

    
    event.selected_field = EVENT_HOUR_FIELD;
    blink_timer = get_ticks();


    /*:LINE1-> TIME- HH:MM     *  */
    clcd_print("TIME- ",LINE1(0));
    
    //HH:
    clcd_putch(event.hour/10 + '0', LINE1(6));
    clcd_putch(event.hour%10 + '0', LINE1(7));
    clcd_putch(':', LINE1(8));

    //MM:
    clcd_putch(event.minute/10 +'0',LINE1(9));
    clcd_putch(event.minute%10 +'0',LINE1(10));
    
    clcd_print("    ",LINE1(11));
    //*
    clcd_putch('*',LINE1(15));

    /*LINE2->DUR - */
    clcd_print("DUR - ",LINE2(0));
    //HH:
    clcd_putch(event.duration_hour/10 + '0', LINE2(6));
    clcd_putch(event.duration_hour%10 + '0', LINE2(7));
    clcd_putch(':', LINE2(8));

    //MM:
    clcd_putch(event.duration_minute/10 + '0', LINE2(9));
    clcd_putch(event.duration_minute%10 + '0', LINE2(10));
    clcd_putch(':', LINE2(11));

    //SS
    clcd_putch(event.duration_second/10 + '0', LINE2(12));
    clcd_putch(event.duration_second %10 + '0', LINE2(13));
    clcd_putch(' ',LINE2(14));

    //Trigger-> O
    clcd_putch('O',LINE2(15));
    
    

}


void print_confirm_event_clcd_display()
{
    clcd_print("SAVE THIS ALARM?",LINE1(0));
    clcd_print("UP:YES DOWN:NO  ",LINE2(0));


}
static unsigned char *get_event_field(unsigned char *max_value, unsigned char *address)
{
    switch(event.selected_field)
    {
        case EVENT_HOUR_FIELD:
            *max_value = 24;
            *address = LINE1(6);
            return &event.hour;
        case EVENT_MINUTE_FIELD:
            *max_value = 60;
            *address = LINE1(9);
            return &event.minute;
        case EVENT_DURATION_HOUR_FIELD:
            *max_value = 24;
            *address = LINE2(6);
            return &event.duration_hour;
        case EVENT_DURATION_MINUTE_FIELD:
            *max_value = 60;
            *address = LINE2(9);
            return &event.duration_minute;
        case EVENT_DURATION_SECOND_FIELD:
            *max_value = 60;
            *address = LINE2(12);
            return &event.duration_second;
        default:
            *max_value = 3;
            *address = LINE2(15);
            return &event.trigger_type;
    }
}

static void display_event_field(unsigned char visible)
{
    unsigned char max_value,address;
    unsigned char *value = get_event_field(&max_value,&address);

    if(event.selected_field == EVENT_TRIGGER_TYPE_FIELD)
    {
        clcd_putch(visible ? "ODW"[*value] : ' ',address);
    }
    else if(visible)
    {
        clcd_putch(*value / 10 + '0',address);
        clcd_putch(*value % 10 + '0',address + 1);
    }
    else
    {
        clcd_putch(' ',address);
        clcd_putch(' ',address + 1);
    }
}

static void change_event_field(unsigned char increment)
{
    unsigned char max_value,address;
    unsigned char *value = get_event_field(&max_value,&address);

    if(increment)
    {
        if(++(*value) >= max_value)
        {
            *value = 0;
        }
    }
    else
    {
        if(*value == 0)
        {
            *value = max_value - 1;
        }
        else
        {
            --(*value);
        }
    }
    display_event_field(YES);
}

void set_event()
{
    static unsigned char blink_status = 1;
    if(initialized == NO)
    {
        initialized = YES;
        set_event_util();
        blink_status = 1;
    }

    unsigned char key_event = key_handler_get_event();
    if(get_ticks() - blink_timer >= BLINK_THRESHOLD)
    {
        blink_timer = get_ticks();
        blink_status = !blink_status;
        display_event_field(blink_status);
    }

    if(key_event == KEY_UP_LONG)
    {
        display_event_field(YES);
        if(event.selected_field == EVENT_TRIGGER_TYPE_FIELD)
        {
            event.selected_field = EVENT_HOUR_FIELD;
        }
        else
        {
            event.selected_field++;
        }
        blink_status = 1;
        blink_timer = get_ticks();
    }
    else if(key_event == KEY_UP_SHORT)
    {
        change_event_field(YES);
        blink_status = 1;
        blink_timer = get_ticks();
    }
    else if(key_event == KEY_DOWN_LONG)
    {
        previous_activity_confirm_alarm = get_ticks();
        print_confirm_event_clcd_display();
        mode = m_confirm_save_alarm;
    }
    else if(key_event == KEY_DOWN_SHORT)
    {
        change_event_field(NO);
        blink_status = 1;
        blink_timer = get_ticks();
    }
    if(key_handler_inactive(&previous_activity_time_set_alarm))
    {
        print_default_mode_clcd_screen();
        initialized = NO;
        mode = m_default_mode;
    }

    return;
}



//===================================================================================================
void confirm_event()
{
    
    unsigned char key = key_handler_get_event();
    

    if (key == KEY_UP_PRESS)
    {
        save_event_to_eeprom(&event);

        initialized = NO; 

        print_default_mode_clcd_screen();
        mode = m_default_mode;
    }
    else if (key == KEY_DOWN_PRESS)
    {
        initialized = NO; 

        print_screen_set_or_view_alarm();
        set_or_view_alarm_menu_index = first_option_index;
        previous_activity_time_set_or_view_alarm = get_ticks();
        mode = m_set_or_view_alarm;
    }
    else if (key_handler_inactive(&previous_activity_confirm_alarm))
    {
        initialized = NO;
        print_default_mode_clcd_screen();
        mode = m_default_mode;
    }
}





//===================================================================================================
//VIEW ALARM
//===================================================================================================


void view_alarm_util()
{
    /*:LINE1-> TIME- HH:MM     *  */
    clcd_print("TIME- ",LINE1(0));
    /*LINE2->DUR - */
    clcd_print("DUR - ",LINE2(0));
    
}

void print_event(unsigned char hour,unsigned char min, unsigned char dur_hour,unsigned char dur_min,unsigned char dur_sec,event_trigger_t trig )
{
    
    
    //HH:
    print_two_digits(hour,LINE1(6));
    clcd_putch(':', LINE1(8));

    //MM:
    print_two_digits(min,LINE1(9));
    
    clcd_print("    ",LINE1(11));
    //*
    clcd_putch('*',LINE1(15));

    
    //HH:
    print_two_digits(dur_hour,LINE2(6));
    clcd_putch(':', LINE2(8));

    //MM:
    print_two_digits(dur_min,LINE2(9));
    clcd_putch(':', LINE2(11));

    //SS
    print_two_digits(dur_sec,LINE2(12));
    clcd_putch(' ',LINE2(14));

    //Trigger-> O
    clcd_putch("ODW"[trig],LINE2(15));
}
unsigned char find_next_event_index(unsigned char index)
{
    unsigned char count = 0;

    do
    {
        ++index;

        if(index >= MAX_EVENTS)
            index = 0;

        if(get_active_flag(index) == YES)
            return index;

        count++;

    }while(count < MAX_EVENTS);

    return 0xFF; //No active events
}


unsigned char find_previous_event_index(unsigned char index)
{
    unsigned char count = 0;

    do
    {
        if(index == 0)
            index = MAX_EVENTS - 1;
        else
            --index;

        if(get_active_flag(index) == YES)
            return index;

        count++; //this checks if every slot is checked

    }while(count < MAX_EVENTS);

    return 0xFF; //No active events
}
void view_alarm()
{
    

    unsigned int address;
    unsigned char hour, min, dur_hour, dur_min, dur_sec;
    event_trigger_t trig_type;

    static unsigned char i = 0;
    static unsigned char initialized_view = NO;

    unsigned char key_event = key_handler_get_event();
    unsigned char refresh = NO; //local variable so resetted NO in every iteration

    //INITIALIZATION
    if(initialized_view == NO)
    {
        clcd_print("VIEW START      ", LINE1(0));

    i = 0;

    if(get_active_flag(i) != YES)
    {
        i = find_next_event_index(i);
    }

    clcd_print("EEPROM DONE     ", LINE1(0));

    initialized_view = YES;
    refresh = YES;

    previous_activity_time_view_alarm = get_ticks();
    }

    if(key_event == KEY_UP_SHORT)
    { 
        if(i != 0xFF)
        {
            i=find_next_event_index(i);
            refresh = YES;
        }
            
    }
    else if(key_event == KEY_DOWN_SHORT)
    {
        if(i != 0xFF)
        {
            i = find_previous_event_index(i);
            refresh = YES;
        }
    }
    
    else if(key_event == KEY_DOWN_LONG)
    {
        //delete that event
        if(i!=0xFF)
        {
            delete_event(i);
            find_next_event();//recalculating alarm 

            //finding next index to display on screen
            i = find_next_event_index(i);
            refresh = YES;
        }
    }
    else if(key_event == KEY_UP_LONG)
    {
        initialized_view = NO;

        print_screen_set_or_view_alarm();

        previous_activity_time_set_or_view_alarm = get_ticks();
        mode = m_set_or_view_alarm;

        return;
    }
    if(refresh == YES)
    {
        if(i == 0xFF)
        {
            clcd_print("No Events Saved ",LINE1(0));
            clcd_print("Press UP->BACK  ",LINE2(0));

            return;

        }
        else
        {
            address = EEPROM_EVENT_START_ADDR + (i * EEPROM_EVENT_SIZE);

            hour = read_external_eeprom(address + EEPROM_HOUR_OFFSET);
            min = read_external_eeprom(address + EEPROM_MINUTE_OFFSET);

            dur_hour = read_external_eeprom(address + EEPROM_DURATION_HOUR_OFFSET);
            dur_min = read_external_eeprom(address + EEPROM_DURATION_MINUTE_OFFSET);
            dur_sec = read_external_eeprom(address + EEPROM_DURATION_SECOND_OFFSET);

            trig_type = get_trigger_value(i);

            view_alarm_util();

            print_event(hour,min,dur_hour,dur_min,dur_sec,trig_type);
        }
    }
    if(key_handler_inactive(&previous_activity_time_view_alarm))
    {
        initialized_view = NO;

        print_default_mode_clcd_screen();
        mode = m_default_mode;
    }
    
      

}

void delete_event(unsigned char i)
{
    write_external_eeprom(NO,(EEPROM_EVENT_START_ADDR + i * EEPROM_EVENT_SIZE) + EEPROM_ACTIVE_FLAG_OFFSET);
    return;
}

unsigned char get_active_flag(unsigned char i)
{
    return read_external_eeprom(EEPROM_EVENT_START_ADDR + (i* EEPROM_EVENT_SIZE) + EEPROM_ACTIVE_FLAG_OFFSET);
}