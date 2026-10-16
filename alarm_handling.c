#include "main.h"
#include "ext_eeprom.h"
#include "ds1307.h"
#include <xc.h>
#include "time.h"
#include "timer0.h"
#include "clcd.h"






alarm_execution_t alarm_execution[MAX_EVENTS] = {0};



/*=================================================UTILS=========================================*/

unsigned char bcd_to_decimal(unsigned char bcd)
{
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}



unsigned char calculate_days_to_add(event_t *current_event, unsigned char current_hour, unsigned char current_minute)
{
    unsigned char days_to_add = 0;

    //check if the alarm time has already passed today
    if(current_event->hour < current_hour)
    {
        days_to_add = 1;
    }
    else if(current_event->hour == current_hour)
    {
        if(current_event->minute <= current_minute)
        {
            days_to_add = 1;
        }
    }

    //weekly alarm repeats on the same weekday
    if(current_event->trigger_type == W && days_to_add == 1)
    {
        days_to_add = 7;
    }

    return days_to_add;
}

void add_days_to_date(unsigned char *day, unsigned char *month, unsigned char *year, unsigned char days_to_add)
{
    unsigned char days_in_month;

    while(days_to_add > 0)
    {
        //find the number of days in the current month
        if(*month == 4 || *month == 6 || *month == 9 || *month == 11)
        {
            days_in_month = 30;
        }
        else if(*month == 2)
        {
            //check for leap year
            if((*year % 4) == 0)
            {
                days_in_month = 29;
            }
            else
            {
                days_in_month = 28;
            }
        }
        else
        {
            days_in_month = 31;
        }

        (*day)++;

        //move to the next month if required
        if(*day > days_in_month)
        {
            *day = 1;
            (*month)++;

            //move to the next year if required
            if(*month > 12)
            {
                *month = 1;
                (*year)++;
            }
        }

        days_to_add--;
    }
}


void save_next_occurrence(event_t *current_event, unsigned int address)
{
    unsigned char current_hour, current_minute;
    unsigned char day, month, year;
    unsigned char days_to_add;

    //read current date and time from DS1307
    //replace these with your existing RTC reading functions
    current_hour = bcd_to_decimal(read_ds1307(HOUR_ADDR) & 0x3F);
    current_minute = bcd_to_decimal(read_ds1307(MIN_ADDR) & 0x7F);

    day = bcd_to_decimal(read_ds1307(DATE_ADDR) & 0x3F);
    month = bcd_to_decimal(read_ds1307(MONTH_ADDR) & 0x1F);
    year = bcd_to_decimal(read_ds1307(YEAR_ADDR));

    //calculate how many days to add
    days_to_add = calculate_days_to_add(current_event, current_hour, current_minute);

    //calculate the next occurrence date
    add_days_to_date(&day, &month, &year, days_to_add);

    //save next occurrence date to EEPROM
    write_external_eeprom(day,address + EEPROM_NEXT_OCC_DAY_OFFSET);
    write_external_eeprom(month,address + EEPROM_NEXT_OCC_MONTH_OFFSET);
    write_external_eeprom(year,address + EEPROM_NEXT_OCC_YEAR_OFFSET);
}


void save_event_to_eeprom(event_t *current_event)
{
    
    unsigned char slot_found = NO;
    unsigned int address;

    if(read_external_eeprom(EEPROM_MAGIC_ADDR) != EEPROM_MAGIC_NUMBER)
    {
        
        //no event has been recorded yet
        //initialize all the active flags to zero
        for(unsigned char i = 0;i<MAX_EVENTS;i++)
        {
            write_external_eeprom(NO,(EEPROM_EVENT_START_ADDR + i*EEPROM_EVENT_SIZE + EEPROM_ACTIVE_FLAG_OFFSET));
        }
        //write the magic number
        write_external_eeprom(EEPROM_MAGIC_NUMBER,EEPROM_MAGIC_ADDR);
    }
    for(unsigned char i =0; i < MAX_EVENTS ; i++)
    {
        address = EEPROM_EVENT_START_ADDR+ (i * EEPROM_EVENT_SIZE);

        //when a acive flag is NO and execution of event that may have been there is finished, only then new event written
        if(read_external_eeprom(address + EEPROM_ACTIVE_FLAG_OFFSET) == NO && alarm_execution[i].active == NO)
        {
            slot_found = YES;
            break;
        }
    }
    if(slot_found == NO)
    {
        //there is no free space print that and go back to default message
        clcd_print("    SLOTS FULL    ",LINE1(0));
        clcd_print("                         ",LINE2(0));
        return;
    }

    write_external_eeprom(current_event->hour,address + EEPROM_HOUR_OFFSET );
    write_external_eeprom(current_event->minute,address + EEPROM_MINUTE_OFFSET );
    write_external_eeprom(current_event->duration_hour,address + EEPROM_DURATION_HOUR_OFFSET );
    write_external_eeprom(current_event->duration_minute,address + EEPROM_DURATION_MINUTE_OFFSET );
    write_external_eeprom(current_event->duration_second,address + EEPROM_DURATION_SECOND_OFFSET );
    write_external_eeprom(current_event->trigger_type,address + EEPROM_DURATION_TRIGGER_TYPE_OFFSET );
    
    //find and save next occurence of the current alarm
    save_next_occurrence(current_event,address);

    write_external_eeprom(YES,address + EEPROM_ACTIVE_FLAG_OFFSET);

    find_next_event(); // this will find the next event to be triggered
}


unsigned char earliest_event_index = 0xFF;
unsigned char earliest_year = 0xFF;
unsigned char earliest_month = 0xFF;
unsigned char earliest_day = 0xFF;
unsigned char earliest_hour = 0xFF;
unsigned char earliest_minute = 0xFF;


//this fn will find which event to be triggered when one is succesfully triggered or new one is added
void find_next_event()
{
    earliest_event_index = 0xFF;
    if(read_external_eeprom(EEPROM_MAGIC_ADDR) != EEPROM_MAGIC_NUMBER)
    {
        return; //EEPROM has not been initialized yet
    }
    unsigned int address;
    unsigned char day,month,year,hour,minute;
    earliest_event_index = 0xFF; //reseting index so that when old one is triggered, the index can be just disgarded
    for(unsigned char i = 0;i<MAX_EVENTS;i++)
    {
        address = (EEPROM_EVENT_START_ADDR + (i*EEPROM_EVENT_SIZE));

        if(read_external_eeprom(address+ EEPROM_ACTIVE_FLAG_OFFSET ) == YES)
        {
            //read next occurrence date
            day = read_external_eeprom(address + EEPROM_NEXT_OCC_DAY_OFFSET);
            month = read_external_eeprom(address + EEPROM_NEXT_OCC_MONTH_OFFSET);
            year = read_external_eeprom(address + EEPROM_NEXT_OCC_YEAR_OFFSET);

            //read event time
            hour = read_external_eeprom(address + EEPROM_HOUR_OFFSET);
            minute = read_external_eeprom(address + EEPROM_MINUTE_OFFSET);

            //compare with the earliest event (next step)
            compare_event(i,day,month,year,hour,minute);
            
        }

    }
}

void compare_event(unsigned char index, unsigned char day,unsigned char month,unsigned char year,unsigned char hour,unsigned char minute)
{

    if(earliest_event_index == 0xFF ||
       year < earliest_year ||
       (year == earliest_year && month < earliest_month) ||
       (year == earliest_year && month == earliest_month && day < earliest_day) ||
       (year == earliest_year && month == earliest_month && day == earliest_day && hour < earliest_hour) ||
       (year == earliest_year && month == earliest_month && day == earliest_day && hour == earliest_hour && minute < earliest_minute))
        {
            earliest_event_index = index;
            earliest_year = year;
            earliest_month = month;
            earliest_hour = hour;
            earliest_minute = minute;
            earliest_day = day;
            
        }
    return;
    
}



void monitor_event()
{
    if(earliest_event_index == 0xFF)
    {
        //No events saved yet
        return;
    }
    //HH:MM, DD:MM:YY
    unsigned char current_hour,current_day,current_minute,current_year,current_month;

    current_hour = bcd_to_decimal(read_ds1307(HOUR_ADDR) & 0x3F);
    current_minute = bcd_to_decimal(read_ds1307(MIN_ADDR) & 0x7F);
    current_day = bcd_to_decimal(read_ds1307(DATE_ADDR) & 0x3F);
    current_month = bcd_to_decimal(read_ds1307(MONTH_ADDR) & 0x1F);
    current_year = bcd_to_decimal(read_ds1307(YEAR_ADDR));  


    if(current_minute == earliest_minute &&current_hour == earliest_hour && current_day == earliest_day && current_month == earliest_month && current_year == earliest_year)
    {
        
        //trigger alarm
        start_event(earliest_event_index);

        //process this events next lifetime after this execution
        process_triggered_event();

    }


}


void process_triggered_event()
{
    unsigned char day,month,year; //current next occurence values

    event_trigger_t trigger_val = get_trigger_value(earliest_event_index);

    get_next_occurence(earliest_event_index,&day,&month,&year);

    if(trigger_val == O)
    {
        write_external_eeprom(NO , EEPROM_EVENT_START_ADDR + (earliest_event_index * EEPROM_EVENT_SIZE) + EEPROM_ACTIVE_FLAG_OFFSET);
    }
    else if(trigger_val == D || trigger_val == W)
    {
        // add 1 day to the next occurence of the event
        if(trigger_val == D)
            add_days_to_date(&day,&month,&year,1);
        else
            add_days_to_date(&day,&month,&year,7);//add seven days

        //save the next occurence

        write_external_eeprom(day,(EEPROM_EVENT_START_ADDR + (earliest_event_index * EEPROM_EVENT_SIZE)) + EEPROM_NEXT_OCC_DAY_OFFSET);
        write_external_eeprom(month,(EEPROM_EVENT_START_ADDR + (earliest_event_index * EEPROM_EVENT_SIZE)) + EEPROM_NEXT_OCC_MONTH_OFFSET);
        write_external_eeprom(year,(EEPROM_EVENT_START_ADDR + (earliest_event_index * EEPROM_EVENT_SIZE)) + EEPROM_NEXT_OCC_YEAR_OFFSET);

        
    }

    find_next_event(); // find the next earliest event to be triggered

}


event_trigger_t get_trigger_value(unsigned char index)
{
    return read_external_eeprom((EEPROM_EVENT_START_ADDR + (index * EEPROM_EVENT_SIZE)) + EEPROM_DURATION_TRIGGER_TYPE_OFFSET);
}

void get_next_occurence(unsigned char index,unsigned char *day, unsigned char *month, unsigned char *year)
{
    if(day != (void*) 0)
        *day = read_external_eeprom((EEPROM_EVENT_START_ADDR + (index * EEPROM_EVENT_SIZE)) + EEPROM_NEXT_OCC_DAY_OFFSET);
    if(month != (void*) 0)
        *month = read_external_eeprom((EEPROM_EVENT_START_ADDR + (index * EEPROM_EVENT_SIZE)) + EEPROM_NEXT_OCC_MONTH_OFFSET);
    if(year != (void*) 0)
        *year = read_external_eeprom((EEPROM_EVENT_START_ADDR + (index * EEPROM_EVENT_SIZE)) + EEPROM_NEXT_OCC_YEAR_OFFSET);

}



void start_event(unsigned char index)
{
    unsigned char hour, minute,second;


    unsigned int address = EEPROM_EVENT_START_ADDR + (index * EEPROM_EVENT_SIZE);

    hour = read_external_eeprom( address + EEPROM_DURATION_HOUR_OFFSET);
    minute = read_external_eeprom( address + EEPROM_DURATION_MINUTE_OFFSET);
    second  = read_external_eeprom( address + EEPROM_DURATION_SECOND_OFFSET);

    
    alarm_execution[index].remaining_seconds = (hour* 3600UL) + (minute*60UL) + second;


    if(alarm_execution[index].remaining_seconds > 0)
    {
        alarm_execution[index].active = YES;
    }
}


void update_alarm_execution()
{
    static unsigned int previous_tick = 0;
    static unsigned int tick_count = 0;

    unsigned int current_tick = get_ticks();


    unsigned int elapsed_ticks = (unsigned int)(current_tick - previous_tick);
    previous_tick = current_tick;


    tick_count += elapsed_ticks;
    //approximately 98 ticks = 1 second
    while(tick_count >= 98)
    {
        tick_count -= 98;

        for(unsigned char i = 0;i < MAX_EVENTS;i++)
        {
            if(alarm_execution[i].active == YES)
            {
                if(alarm_execution[i].remaining_seconds > 0)
                {
                    alarm_execution[i].remaining_seconds--;
                }

                if(alarm_execution[i].remaining_seconds == 0)
                {
                    alarm_execution[i].active = NO;
                }
            }
        }
    }
}

void update_alarm_leds()
{
    //evetn 0 and 2 -> RB6 LED
    //event 1 and 3 -> RB7 LED

    //RB6 handles events 0 and 2
    if(alarm_execution[0].active == YES || alarm_execution[2].active == YES)
    {
        RB6 = 1;
    }
    else
    {
        RB6 = 0;
    }

    //RB7 handles events 1 and 3
    if(alarm_execution[1].active == YES || alarm_execution[3].active == YES)
    {
        RB7 = 1;
    }
    else
    {
        RB7 = 0;
    }
}

