#include "xc.h"
#include "keypad.h"
#include "clcd.h"
#include "ds1307.h"
#include "timer0.h"
#include "time.h"
#include "main.h"

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
    unsigned char key_event = key_handler_get_event();
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

    if(key_event == KEY_UP_LONG)
    {  
                //execute long press operation


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
    else if(key_event == KEY_UP_SHORT)
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
    if(key_handler_inactive(&previous_activity_time_set_time_and_date))
    {
            print_default_mode_clcd_screen();
            mode = m_default_mode;
        
    }

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

    print_two_digits(current_time.hour,LINE2(0));
    

    clcd_putch(':', LINE2(2));
    print_two_digits(current_time.minute,LINE2(3));
    

    clcd_putch(':', LINE2(5));
    print_two_digits(current_time.second,LINE2(6));
    
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

    
    unsigned char key_event = key_handler_get_event();
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
                print_two_digits(current_time.second,LINE2(6));
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
                print_two_digits(current_time.minute,LINE2(3)); 
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
                print_two_digits(current_time.hour,LINE2(0));
            }
        }
    }
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
    if(key_event == KEY_UP_LONG)
    {
                //UP LONG PRESS

              //oepration->field switching circularly
                if(current_time.selected_field == SECOND_FIELD)
                {
                    //reprinting seconds, because it can remain blank if during blink it was OFF
                    print_two_digits(current_time.second,LINE2(6));
                    current_time.selected_field = MINUTE_FIELD;
                }
                else if(current_time.selected_field == MINUTE_FIELD)
                {
                    //reprinting mins, because it can remain blank if during blink it was OFF
                    print_two_digits(current_time.minute,LINE2(3));
                    current_time.selected_field = HOUR_FIELD;
                }
                else if(current_time.selected_field == HOUR_FIELD)
                {
                    //reprinting hrs, because it can remain blank if during blink it was OFF
                    print_two_digits(current_time.hour,LINE2(0));
                    current_time.selected_field = SECOND_FIELD;
                }
            
    }
    else if(key_event == KEY_UP_SHORT)
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
                print_two_digits(current_time.second,LINE2(6));
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
                print_two_digits(current_time.minute,LINE2(3));
                blink_status = 1;
            }
            else if(current_time.selected_field == HOUR_FIELD)
            {
                if(++current_time.hour >= 24)
                {
                    current_time.hour = 0;
                }
                //print the hour
                print_two_digits(current_time.hour,LINE2(0));
                blink_status = 1; // restarting blink after every edit, to make bloinking consistent
            }
        
    }
    else if(key_event == KEY_DOWN_LONG)
    {
                //execute LONG PRESS operation

                //operation->ask for time confirmation
                previous_activity_time_confirm_time = get_ticks();
                print_confirm_time_clcd_display();
              mode = m_confirm_save_time;

            
    }
    else if(key_event == KEY_DOWN_SHORT)
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
                print_two_digits(current_time.second,LINE2(6));
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
                print_two_digits(current_time.minute,LINE2(3));
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
                print_two_digits(current_time.hour,LINE2(0));
            }
        
    }
    if(key_handler_inactive(&previous_activity_time_set_time))
    {
            print_default_mode_clcd_screen();
            initialized = NO;
            mode = m_default_mode;
        
    }


    return;

}








void confirm_time(void)
{
    unsigned char key = key_handler_get_event();
    

    if (key == KEY_UP_PRESS)
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
        print_default_mode_clcd_screen();
        mode = m_default_mode;
    }
    else if (key == KEY_DOWN_PRESS)
    {
        initialized = NO;
        print_screen_set_time_date();
        set_time_date_menu_index = first_option_index;
        previous_activity_time_set_time_and_date = get_ticks();
        mode = m_set_time_and_date;
    }
    else if (key_handler_inactive(&previous_activity_time_confirm_time))
    {
        initialized = NO;
        print_default_mode_clcd_screen();
        mode = m_default_mode;
    }
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

    print_two_digits(current_date.day,LINE2(0));
    clcd_putch('/',LINE2(2));

    print_two_digits(current_date.month,LINE2(3));
    clcd_putch('/',LINE2(5));

    print_two_digits(current_date.year,LINE2(6));
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

    unsigned char key_event = key_handler_get_event();
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
                print_two_digits(current_date.year,LINE2(6));
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
                print_two_digits(current_date.month,LINE2(3));
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
                print_two_digits(current_date.day,LINE2(0));
            }
        }
    }
    //LONG UP-> SWITCH FIELD
    //LONG DOWN->ASK CONFIRMATION FOR SAVING DATE
    //SHORT UP -> INCREMENT SELECTED FIELD
    //SHORT DOWN -> DEC SELECTED FIELD

    if(key_event == KEY_UP_LONG)
    {

                if(current_date.selected_field == DAY_FIELD)
                {
                    print_two_digits(current_date.day,LINE2(0));
                    current_date.selected_field = MONTH_FIELD;
                }
                else if(current_date.selected_field == MONTH_FIELD)
                {
                    print_two_digits(current_date.month,LINE2(3));
                    current_date.selected_field = YEAR_FIELD;
                }
                else if(current_date.selected_field == YEAR_FIELD)
                {
                    print_two_digits(current_date.year,LINE2(6));
                    current_date.selected_field = DAY_FIELD;
                }
            
    }
    else if(key_event == KEY_UP_SHORT)
    {
            if(current_date.selected_field == DAY_FIELD)
            {
                if(++current_date.day > get_max_days(current_date.month,current_date.year))
                {
                    current_date.day = 1;
                }

                print_two_digits(current_date.day,LINE2(0));
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
                    print_two_digits(current_date.day,LINE2(0));
                    
                }

                print_two_digits(current_date.month,LINE2(3));
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
                    print_two_digits(current_date.day,LINE2(0));
    
                }

                print_two_digits(current_date.year,LINE2(6));
                blink_status = 1;
            }
        
    }
    else if(key_event == KEY_DOWN_LONG)
    {

                previous_activity_time_confirm_date = get_ticks();
                print_confirm_date_clcd_display();
                mode = m_confirm_save_date;
            
    }
    else if(key_event == KEY_DOWN_SHORT)
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

                print_two_digits(current_date.day,LINE2(0));
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
                    print_two_digits(current_date.day,LINE2(0));
                }

                print_two_digits(current_date.month,LINE2(3));
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
                    print_two_digits(current_date.day,LINE2(0));
                }

                print_two_digits(current_date.year,LINE2(6));
                blink_status = 1;
            }
        
    }
    if(key_handler_inactive(&previous_activity_time_set_date))
    {
            print_default_mode_clcd_screen();
            initialized_date = NO;
            mode = m_default_mode;
        
    }

    return;
}

void confirm_date(void)
{
    unsigned char key = key_handler_get_event();
    

    if(key == KEY_UP_PRESS)
    {
        unsigned char bcd_day,bcd_month,bcd_year;

        bcd_day = ((current_date.day/10)<<4) | (current_date.day%10);
        bcd_month = ((current_date.month/10)<<4) | (current_date.month%10);
        bcd_year = ((current_date.year/10)<<4) | (current_date.year%10);

        write_ds1307(bcd_day,DATE_ADDR);
        write_ds1307(bcd_month,MONTH_ADDR);
        write_ds1307(bcd_year,YEAR_ADDR);

        initialized_date = NO;
        print_default_mode_clcd_screen();
        mode = m_default_mode;
    }
    else if(key == KEY_DOWN_PRESS)
    {
        initialized_date = NO;
        print_screen_set_time_date();
        set_time_date_menu_index = first_option_index;
        previous_activity_time_set_time_and_date = get_ticks();
        mode = m_set_time_and_date;
    }
    else if(key_handler_inactive(&previous_activity_time_confirm_date))
    {
        initialized_date = NO;
        print_default_mode_clcd_screen();
        mode = m_default_mode;
    }
}