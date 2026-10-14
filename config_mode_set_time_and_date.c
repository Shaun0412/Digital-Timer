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
extern unsigned int previous_activity_time_set_time_and_date;
extern unsigned int previous_activity_time_set_time;
extern unsigned int previous_activity_time_confirm_time;
extern unsigned int previous_activity_time_set_date;
extern unsigned int previous_activity_time_confirm_date;
extern unsigned int previous_activity_time_set_or_view_alarm;
extern unsigned int previous_activity_time_set_alarm;
extern unsigned int previous_activity_time_view_alarm;


extern state_t mode;

extern unsigned char config_mode_menu_index;
extern unsigned char set_time_date_menu_index;

extern unsigned int blink_timer;

void set_time_and_date()
{
    unsigned char key;
    static unsigned int start_tick = 0;
    unsigned int current_tick;
    key = read_digital_keypad(LEVEL);
   //set time
    //set data
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
        previous_activity_time_set_time_and_date = get_ticks();
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
                if(set_time_date_menu_index == first_option_index)
                {
                    //selection of first option
                    /*==========================PRINT NEXT SCREEN===================== */
                    mode = m_set_time;
                    previous_activity_time_set_time = get_ticks();
                }
                else if (set_time_date_menu_index == second_option_index)
                {
                    //selection of first option
                    previous_activity_time_set_date = get_ticks();
                    mode = m_set_date;
                    /*==========================PRINT NEXT SCREEN===================== */
                }
            }

        }

        else if(key == ALL_RELEASED && long_press_detected == NO && prev_key == UP)
        {
            //short press
            if(set_time_date_menu_index == first_option_index)
            {
                //moves arrow to second line
              set_time_date_menu_index = second_option_index;
                clcd_putch(' ',LINE1(0));
                clcd_putch('>',LINE2(0));
            }
            else if(set_time_date_menu_index == second_option_index)
            {
                //moves to first line
                set_time_date_menu_index = first_option_index;
                clcd_putch('>',LINE1(0));
                clcd_putch(' ',LINE2(0));
            }
        }
    }
    if(key == DOWN || prev_key == DOWN)
    {
        previous_activity_time_set_time_and_date = get_ticks();
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
            if(set_time_date_menu_index== first_option_index)

            {
                //moves arrow to second line
                set_time_date_menu_index = second_option_index;
                clcd_putch(' ',LINE1(0));
                clcd_putch('>',LINE2(0));
            }
            else if(set_time_date_menu_index == second_option_index)
            {
                //moves arrow to first line
                set_time_date_menu_index = first_option_index;
                clcd_putch('>',LINE1(0));
                clcd_putch(' ',LINE2(0));
            }
        }
    }
    else if(key == ALL_RELEASED && prev_key == ALL_RELEASED)
    {
        if((get_ticks() - previous_activity_time_set_time_and_date) >= INACTIVITY_THRESHOLD)
        {
            print_default_mode_clcd_screen();
            mode = m_default_mode;
        }
    }
    prev_key = key;
    return;
}

typedef struct
{
    /* data */
    unsigned char hour,minute,second;
    unsigned char selected_field;
}edit_time_t;

edit_time_t current_time;

unsigned char previous_time[3];

typedef enum
{
    SECOND_FIELD,
    MINUTE_FIELD,
    HOUR_FIELD
}edit_field;

void print_confirm_time_clcd_display()
{
    clcd_print("PRESS UP-Save ",LINE1(0));
    clcd_print("      DOWN-Back  ",LINE2(0));
}

static void set_time_util()
{
    get_time(previous_time);//gets time in BCD

    //converts time into integer format


    blink_timer = get_ticks();

    current_time.hour =(((previous_time[0] & 0x3F) >> 4) * 10) + (previous_time[0] & 0x0F);
    current_time.minute =((previous_time[1] >> 4) * 10)+ (previous_time[1] & 0x0F);
    current_time.second =(((previous_time[2] & 0x7F) >> 4) * 10) + (previous_time[2] & 0x0F);
    current_time.selected_field = SECOND_FIELD;

    clcd_print("HH:MM:SS         ", LINE1(0));

    clcd_putch((current_time.hour/ 10) + '0', LINE2(0));
    clcd_putch((current_time.hour % 10) + '0', LINE2(1));

    clcd_putch(':', LINE2(2));
    clcd_putch((current_time.minute / 10) + '0', LINE2(3));
    clcd_putch((current_time.minute % 10) + '0', LINE2(4));

    clcd_putch(':', LINE2(5));
    clcd_putch((current_time.second / 10) + '0', LINE2(6));
    clcd_putch((current_time.second % 10) + '0', LINE2(7));
    clcd_print("         ",LINE2(8));

}

static unsigned char initialized = NO;
void set_time()
{
    static unsigned char blink_status = 1;
    if(initialized == NO)
    {
        set_time_util();
        initialized = YES;
        blink_status = 1;//restarting the blink
    }
    /**/

    
    unsigned char key;
    static unsigned int start_tick = 0;
    unsigned int current_tick;
    
    /*============BLINK THE SELECTED FIELD==============*/
    if(get_ticks() - blink_timer>= BLINK_THRESHOLD)
    {
        blink_timer = get_ticks();
        if(current_time.selected_field == SECOND_FIELD)
        {
            //if it is ON turn it off
            if(blink_status == 1)
            {
                blink_status = 0;
                clcd_putch(' ', LINE2(6));
                clcd_putch(' ', LINE2(7));
            }
            //if the field is OFF turn it ON
            else if(blink_status == 0)
            {
                blink_status = 1;
                clcd_putch((current_time.second / 10) + '0', LINE2(6));
                clcd_putch((current_time.second % 10) + '0', LINE2(7));  
            }

        }
        else if(current_time.selected_field == MINUTE_FIELD)
        {
            //if it is ON turn it off
            if(blink_status == 1)
            {
                blink_status = 0;
                clcd_putch(' ', LINE2(3));
                clcd_putch(' ', LINE2(4));
            }
            //if the field is OFF turn it ON
            else if(blink_status == 0)
            {
                blink_status = 1;
                clcd_putch((current_time.minute / 10) + '0', LINE2(3));
                clcd_putch((current_time.minute % 10) + '0', LINE2(4));  
            }
        }
        else if(current_time.selected_field == HOUR_FIELD)
        {
            //if it is ON turn it off
            if(blink_status == 1)
            {
                blink_status = 0;
                clcd_putch(' ', LINE2(0));
                clcd_putch(' ', LINE2(1));
            }
            //if the field is OFF turn it ON
            else if(blink_status == 0)
            {
                blink_status = 1;
                clcd_putch((current_time.hour / 10) + '0', LINE2(0));
                clcd_putch((current_time.hour % 10) + '0', LINE2(1));  
            }
        }
    }








    key = read_digital_keypad(LEVEL);
    //set time
    //set data
    // arrow to select

    /*****************************    KEY HANDLING    ***************************************/

    //long press for 2seconds on UP button go to the next page in the present menu

    //long press for 2seconds on DOWN button goes to previous page

    //if inactivity is detected for 5 seconds then go back to default mode

    //LONG UP-> SWITCH FIELD
    //LONG DOWN->ASK CONFIRMATION FOR SAVING TIME
    //SHORT UP -> INCREMENT SELECTED FIELD
    //SHORT DOWN -> DEC SELECTED FIELD
    if(key == UP || prev_key == UP)
    {
        previous_activity_time_set_time = get_ticks();
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
                //UP LONG PRESS
                long_press_detected = YES;
              //oepration->field switching circularly
                if(current_time.selected_field == SECOND_FIELD)
                {
                    //reprinting seconds, because it can remain blank if during blink it was OFF
                    clcd_putch((current_time.second / 10) + '0', LINE2(6));
                    clcd_putch((current_time.second % 10) + '0', LINE2(7));
                    current_time.selected_field = MINUTE_FIELD;
                }
                else if(current_time.selected_field == MINUTE_FIELD)
                {
                    //reprinting mins, because it can remain blank if during blink it was OFF
                    clcd_putch((current_time.minute / 10) + '0', LINE2(3));
                    clcd_putch((current_time.minute % 10) + '0', LINE2(4));
                    current_time.selected_field = HOUR_FIELD;
                }
                else if(current_time.selected_field == HOUR_FIELD)
                {
                    //reprinting hrs, because it can remain blank if during blink it was OFF
                    clcd_putch((current_time.hour / 10) + '0', LINE2(0));
                    clcd_putch((current_time.hour % 10) + '0', LINE2(1));
                    current_time.selected_field = SECOND_FIELD;
                }
            }

        }
        else if(key == ALL_RELEASED && long_press_detected == NO && prev_key == UP)
        {
            //UP SHORT PRESS
            if(current_time.selected_field == SECOND_FIELD)
            {
                //increment second and check for boundaries
                if(++current_time.second >= 60)
                {
                    current_time.second = 0;
                }
                //print the second
                clcd_putch(current_time.second/10 + '0',LINE2(6));
                clcd_putch(current_time.second %10 + '0',LINE2(7));
                blink_status = 1;
            }
            else if(current_time.selected_field == MINUTE_FIELD)
            {
                //increment minute and check for boundaries
                if(++current_time.minute >= 60)
                {
                 current_time.minute = 0;
                }
                //print the minute
                clcd_putch(current_time.minute/10 + '0',LINE2(3));
                clcd_putch(current_time.minute %10 + '0',LINE2(4));
                blink_status = 1;
            }
            else if(current_time.selected_field == HOUR_FIELD)
            {
                if(++current_time.hour >= 24)
                {
                    current_time.hour = 0;
                }
                //print the hour
                clcd_putch(current_time.hour/10 + '0',LINE2(0));
                clcd_putch(current_time.hour %10 + '0',LINE2(1));
                blink_status = 1; // restarting blink after every edit, to make bloinking consistent
            }
        }
    }
    if(key == DOWN || prev_key == DOWN)
    {
        previous_activity_time_set_time= get_ticks();
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
                //execute LONG PRESS operation
                long_press_detected = YES;
                //operation->ask for time confirmation
                previous_activity_time_confirm_time = get_ticks();
                print_confirm_time_clcd_display();
              mode = m_confirm_save_time;

            }
        }
        else if(key == ALL_RELEASED && long_press_detected == NO && prev_key == DOWN)
        {
            //SHORT PRESS
            //operation->decrement selected field
            if(current_time.selected_field == SECOND_FIELD)
            {
                //dec second and check for boundaries
                if (current_time.second == 0)
                {
                    current_time.second = 59;
                }
                else
                {
                    current_time.second--;
                }
                //print the second
                clcd_putch(current_time.second/10 + '0',LINE2(6));
                clcd_putch(current_time.second %10 + '0',LINE2(7));
            }
            else if(current_time.selected_field == MINUTE_FIELD)
            {
                //increment minute and check for boundaries
                if (current_time.minute == 0)
                {
                    current_time.minute = 59;
                }
                else
                {
                     current_time.minute--;
                }
                //print the minut
                clcd_putch(current_time.minute/10 + '0',LINE2(3));
                clcd_putch(current_time.minute %10 + '0',LINE2(4));
            }
            else if(current_time.selected_field == HOUR_FIELD)
            {
                if (current_time.hour == 0)
                {
                    current_time.hour = 23;
                }
                else
                {
                    current_time.hour--;
                }
                //print the hour
                clcd_putch(current_time.hour/10 + '0',LINE2(0));
                clcd_putch(current_time.hour %10 + '0',LINE2(1));
            }
        }
    }
    else if(key == ALL_RELEASED && prev_key == ALL_RELEASED)
    {
        if((get_ticks() - previous_activity_time_set_time) >= INACTIVITY_THRESHOLD)
        {
            print_default_mode_clcd_screen();
            initialized = NO;
            mode = m_default_mode;
        }
    }
    prev_key = key;

    return;

}



void confirm_time(void)
{
    unsigned char key = read_digital_keypad(LEVEL);
    static unsigned char ready = NO;

    if (ready == NO)
    {
        if (key == ALL_RELEASED)
        {
            ready = YES;
        }
        prev_key = key;
        return;
    }

    if (key == UP && prev_key == ALL_RELEASED)
    {
        unsigned char bcd_sec, bcd_min, bcd_hour;
        bcd_sec = ((current_time.second / 10) << 4) | (current_time.second % 10);
        bcd_min = ((current_time.minute / 10) << 4) | (current_time.minute % 10);
        bcd_hour = ((current_time.hour / 10) << 4) | (current_time.hour % 10);

        write_ds1307(0x80, SEC_ADDR);
        write_ds1307(bcd_min, MIN_ADDR);
        write_ds1307(bcd_hour, HOUR_ADDR);
        write_ds1307(bcd_sec, SEC_ADDR);

        initialized = NO;
        ready = NO;
        print_default_mode_clcd_screen();
        mode = m_default_mode;
    }
    else if (key == DOWN && prev_key == ALL_RELEASED)
    {
        initialized = NO;
        ready = NO;
        print_screen_set_time_date();
        set_time_date_menu_index = first_option_index;
        previous_activity_time_set_time_and_date = get_ticks();
        mode = m_set_time_and_date;
    }
    else if ((unsigned int)(get_ticks() - previous_activity_time_confirm_time) >= INACTIVITY_THRESHOLD)
    {
        initialized = NO;
        ready = NO;
        print_default_mode_clcd_screen();
        mode = m_default_mode;
    }

    prev_key = key;
}
typedef struct
{
    unsigned char day,month,year;
    unsigned char selected_field;
}edit_date_t;

edit_date_t current_date;

unsigned char previous_date[3];

typedef enum
{
    DAY_FIELD,
    MONTH_FIELD,
    YEAR_FIELD
}edit_date_field;
static unsigned char initialized_date = NO;


//different months ahve different number of days 
static unsigned char get_max_days(unsigned char month,unsigned char year)
{
    if(month == 2)
    {
        if(year % 4 == 0)
            return 29;
        return 28;
    }
    if(month == 4 || month == 6 || month == 9 || month == 11)
        return 30;
    return 31;
}
void print_confirm_date_clcd_display()
{
    clcd_print("PRESS UP-Save   ",LINE1(0));
    clcd_print("      DOWN-Back ",LINE2(0));
}





static void set_date_util()
{
    blink_timer = get_ticks();

    previous_date[0] = read_ds1307(DATE_ADDR);
    previous_date[1] = read_ds1307(MONTH_ADDR);
    previous_date[2] = read_ds1307(YEAR_ADDR);

    current_date.day = ((previous_date[0] >> 4) * 10) + (previous_date[0] & 0x0F);
    current_date.month = (((previous_date[1] & 0x1F) >> 4) * 10) + (previous_date[1] & 0x0F);
    current_date.year = ((previous_date[2] >> 4) * 10) + (previous_date[2] & 0x0F);

    current_date.selected_field = DAY_FIELD;

    clcd_print("Set Date        ",LINE1(0));

    clcd_putch(current_date.day/10 + '0',LINE2(0));
    clcd_putch(current_date.day%10 + '0',LINE2(1));
    clcd_putch('/',LINE2(2));

    clcd_putch(current_date.month/10 + '0',LINE2(3));
    clcd_putch(current_date.month%10 + '0',LINE2(4));
    clcd_putch('/',LINE2(5));

    clcd_putch(current_date.year/10 + '0',LINE2(6));
    clcd_putch(current_date.year%10 + '0',LINE2(7));
    clcd_print("        ",LINE2(8));
}





void set_date()
{
    static unsigned char blink_status = 1;
    if(initialized_date == NO)
    {
        set_date_util();
        initialized_date = YES;
        blink_status = 1;
    }

    unsigned char key;
    static unsigned int start_tick = 0;
    unsigned int current_tick;
    /*==================BLINK LOGIC=======================*/
    /*============BLINK THE SELECTED FIELD==============*/
    if(get_ticks() - blink_timer>= BLINK_THRESHOLD)
    {
        blink_timer = get_ticks();
        if(current_date.selected_field == YEAR_FIELD)
        {
            //if it is ON turn it off
            if(blink_status == 1)
            {
                blink_status = 0;
                clcd_putch(' ', LINE2(6));
                clcd_putch(' ', LINE2(7));
            }
            //if the field is OFF turn it ON
            else if(blink_status == 0)
            {
                blink_status = 1;
                clcd_putch((current_date.year / 10) + '0', LINE2(6));
                clcd_putch((current_date.year% 10) + '0', LINE2(7));  
            }

        }
        else if(current_date.selected_field == MONTH_FIELD)
        {
            //if it is ON turn it off
            if(blink_status == 1)
            {
                blink_status = 0;
                clcd_putch(' ', LINE2(3));
                clcd_putch(' ', LINE2(4));
            }
            //if the field is OFF turn it ON
            else if(blink_status == 0)
            {
                blink_status = 1;
                clcd_putch((current_date.month / 10) + '0', LINE2(3));
                clcd_putch((current_date.month % 10) + '0', LINE2(4));  
            }
        }
        else if(current_date.selected_field == DAY_FIELD)
        {
            //if it is ON turn it off
            if(blink_status == 1)
            {
                blink_status = 0;
                clcd_putch(' ', LINE2(0));
                clcd_putch(' ', LINE2(1));
            }
            //if the field is OFF turn it ON
            else if(blink_status == 0)
            {
                blink_status = 1;
                clcd_putch((current_date.day / 10) + '0', LINE2(0));
                clcd_putch((current_date.day % 10) + '0', LINE2(1));  
            }
        }
    }
    key = read_digital_keypad(LEVEL);

    //LONG UP-> SWITCH FIELD
    //LONG DOWN->ASK CONFIRMATION FOR SAVING DATE
    //SHORT UP -> INCREMENT SELECTED FIELD
    //SHORT DOWN -> DEC SELECTED FIELD

    if(key == UP || prev_key == UP)
    {
        previous_activity_time_set_date = get_ticks();

        if(key == UP && prev_key == ALL_RELEASED)
        {
            start_tick = get_ticks();
            long_press_detected = NO;
        }

        current_tick = get_ticks();

        if(prev_key == key && long_press_detected == NO)
        {
            if((unsigned int)(current_tick - start_tick) >= LONG_PRESS_THRESHOLD)
            {
                long_press_detected = YES;

                if(current_date.selected_field == DAY_FIELD)
                {
                    clcd_putch((current_date.day / 10) + '0', LINE2(0));
                    clcd_putch((current_date.day % 10) + '0', LINE2(1)); 
                    current_date.selected_field = MONTH_FIELD;
                }
                else if(current_date.selected_field == MONTH_FIELD)
                {
                    clcd_putch((current_date.month / 10) + '0', LINE2(3));
                    clcd_putch((current_date.month % 10) + '0', LINE2(4)); 
                    current_date.selected_field = YEAR_FIELD;
                }
                else if(current_date.selected_field == YEAR_FIELD)
                {
                    clcd_putch((current_date.year / 10) + '0', LINE2(6));
                    clcd_putch((current_date.year % 10) + '0', LINE2(7)); 
                    current_date.selected_field = DAY_FIELD;
                }
            }
        }
        else if(key == ALL_RELEASED && long_press_detected == NO && prev_key == UP)
        {
            if(current_date.selected_field == DAY_FIELD)
            {
                if(++current_date.day > get_max_days(current_date.month,current_date.year))
                {
                    current_date.day = 1;
                }

                clcd_putch(current_date.day/10 + '0',LINE2(0));
                clcd_putch(current_date.day%10 + '0',LINE2(1));
            }
            else if(current_date.selected_field == MONTH_FIELD)
            {
                if(++current_date.month > 12)
                {
                    current_date.month = 1;
                }

                if(current_date.day > get_max_days(current_date.month,current_date.year))
                {
                    current_date.day = get_max_days(current_date.month,current_date.year);
                    clcd_putch(current_date.day/10 + '0',LINE2(0));
                    clcd_putch(current_date.day%10 + '0',LINE2(1));
                    
                }

                clcd_putch(current_date.month/10 + '0',LINE2(3));
                clcd_putch(current_date.month%10 + '0',LINE2(4));
                blink_status = 1;
            }
            else if(current_date.selected_field == YEAR_FIELD)
            {
                if(++current_date.year >= 100)
                {
                    current_date.year = 0;
                }

                if(current_date.day > get_max_days(current_date.month,current_date.year))
                {
                    current_date.day = get_max_days(current_date.month,current_date.year);
                    clcd_putch(current_date.day/10 + '0',LINE2(0));
                    clcd_putch(current_date.day%10 + '0',LINE2(1));
    
                }

                clcd_putch(current_date.year/10 + '0',LINE2(6));
                clcd_putch(current_date.year%10 + '0',LINE2(7));
                blink_status = 1;
            }
        }
    }

    if(key == DOWN || prev_key == DOWN)
    {
        previous_activity_time_set_date = get_ticks();

        if(key == DOWN && prev_key == ALL_RELEASED)
        {
            start_tick = get_ticks();
            long_press_detected = NO;
        }

        current_tick = get_ticks();

        if(prev_key == key && long_press_detected == NO)
        {
            if((unsigned int)(current_tick - start_tick) >= LONG_PRESS_THRESHOLD)
            {
                long_press_detected = YES;

                previous_activity_time_confirm_date = get_ticks();
                print_confirm_date_clcd_display();
                mode = m_confirm_save_date;
            }
        }
        else if(key == ALL_RELEASED && long_press_detected == NO && prev_key == DOWN)
        {
            if(current_date.selected_field == DAY_FIELD)
            {
                if(current_date.day == 1)
                {
                    current_date.day = get_max_days(current_date.month,current_date.year);
                }
                else
                {
                    current_date.day--;
                }

                clcd_putch(current_date.day/10 + '0',LINE2(0));
                clcd_putch(current_date.day%10 + '0',LINE2(1));
            }
            else if(current_date.selected_field == MONTH_FIELD)
            {
                if(current_date.month == 1)
                {
                    current_date.month = 12;
                }
                else
                {
                    current_date.month--;
                }

                if(current_date.day > get_max_days(current_date.month,current_date.year))
                {
                    current_date.day = get_max_days(current_date.month,current_date.year);
                    clcd_putch(current_date.day/10 + '0',LINE2(0));
                    clcd_putch(current_date.day%10 + '0',LINE2(1));
                }

                clcd_putch(current_date.month/10 + '0',LINE2(3));
                clcd_putch(current_date.month%10 + '0',LINE2(4));
                blink_status = 1;
            }
            else if(current_date.selected_field == YEAR_FIELD)
            {
                if(current_date.year == 0)
                {
                    current_date.year = 99;
                }
                else
                {
                    current_date.year--;
                }

                if(current_date.day > get_max_days(current_date.month,current_date.year))
                {
                    current_date.day = get_max_days(current_date.month,current_date.year);
                    clcd_putch(current_date.day/10 + '0',LINE2(0));
                    clcd_putch(current_date.day%10 + '0',LINE2(1));
                }

                clcd_putch(current_date.year/10 + '0',LINE2(6));
                clcd_putch(current_date.year%10 + '0',LINE2(7));
                blink_status = 1;
            }
        }
    }
    else if(key == ALL_RELEASED && prev_key == ALL_RELEASED)
    {
        if((unsigned int)(get_ticks() - previous_activity_time_set_date) >= INACTIVITY_THRESHOLD)
        {
            print_default_mode_clcd_screen();
            initialized_date = NO;
            mode = m_default_mode;
        }
    }

    prev_key = key;
    return;
}

void confirm_date(void)
{
    unsigned char key = read_digital_keypad(LEVEL);
    static unsigned char ready = NO;

    if(ready == NO)
    {
        if(key == ALL_RELEASED)
        {
            ready = YES;
        }
        prev_key = key;
        return;
    }

    if(key == UP && prev_key == ALL_RELEASED)
    {
        unsigned char bcd_day,bcd_month,bcd_year;

        bcd_day = ((current_date.day/10)<<4) | (current_date.day%10);
        bcd_month = ((current_date.month/10)<<4) | (current_date.month%10);
        bcd_year = ((current_date.year/10)<<4) | (current_date.year%10);

        write_ds1307(bcd_day,DATE_ADDR);
        write_ds1307(bcd_month,MONTH_ADDR);
        write_ds1307(bcd_year,YEAR_ADDR);

        initialized_date = NO;
        ready = NO;
        print_default_mode_clcd_screen();
        mode = m_default_mode;
    }
    else if(key == DOWN && prev_key == ALL_RELEASED)
    {
        initialized_date = NO;
        ready = NO;
        print_screen_set_time_date();
        set_time_date_menu_index = first_option_index;
        previous_activity_time_set_time_and_date = get_ticks();
        mode = m_set_time_and_date;
    }
    else if((unsigned int)(get_ticks() - previous_activity_time_confirm_date) >= INACTIVITY_THRESHOLD)
    {
        initialized_date = NO;
        ready = NO;
        print_default_mode_clcd_screen();
        mode = m_default_mode;
    }

    prev_key = key;
}