/* 
 * File:   main.h
 * Author: shaunvarghese
 *
 * Created on October 10, 2026, 12:57 PM
 */

#ifndef MAIN_H
#define	MAIN_H

#define UP SW3
#define DOWN SW4
/****** CPU MODES/STATES ******/
typedef enum state
{
    m_default_mode,
    m_config_mode,
    m_set_time_and_date,
    m_set_or_view_alarm,
    m_confirm_save_alarm,
    m_set_time,
    m_confirm_save_time,
    m_confirm_save_date,
    m_set_date,
    m_set_alarm,
    m_view_alarm

}state_t;

/*****CONFIG MODE*****/
#define NO 0
#define YES 1
#define LONG_PRESS_THRESHOLD 198 //2second->one second is almost 97
#define INACTIVITY_THRESHOLD 600 //5 second
#define BLINK_THRESHOLD 49 //0.5 hz toggling

typedef enum 
{
    first_option_index = 0,
    second_option_index
}menu_indices;
 //records time of previous activity from system_ticks


/****Fn PROTOTYPES********** */
#define KEY_NONE 0
#define KEY_UP_PRESS 1
#define KEY_DOWN_PRESS 2
#define KEY_UP_SHORT 3
#define KEY_DOWN_SHORT 4
#define KEY_UP_LONG 5
#define KEY_DOWN_LONG 6

void key_handler_update(void);
unsigned char key_handler_get_event(void);
unsigned char key_handler_inactive(unsigned int *previous_activity_time);
void key_handler_ignore_until_release(void);

void DEFAULT_MODE();
void CONFIG_MODE();

//configuration menu prototypes
void set_or_view_alarm();
void set_time_and_date();
void set_time();
void confirm_time();
void set_date();
void confirm_date();

void print_screen_config_mode();
void print_screen_set_time_date();
void print_confirm_time_clcd_display();
void print_screen_set_or_view_alarm();
void print_default_mode_clcd_screen();





/*========================================================================================================*/
//SET ALARM
/*========================================================================================================*/
typedef struct
{
    unsigned char hour;
    unsigned char minute;

    unsigned char duration_hour;
    unsigned char duration_minute;
    unsigned char duration_second;

    unsigned char trigger_type;
    unsigned char selected_field;
}event_t;

extern event_t event;

typedef enum
{
    O,
    D,
    W
}event_trigger_t;

typedef enum
{
    EVENT_HOUR_FIELD,
    EVENT_MINUTE_FIELD,
    EVENT_DURATION_HOUR_FIELD,
    EVENT_DURATION_MINUTE_FIELD,
    EVENT_DURATION_SECOND_FIELD,
    EVENT_TRIGGER_TYPE_FIELD
}selected_event_field_t;


typedef struct
{
    unsigned char active; // YES if the alarm is currently executing
    unsigned long remaining_seconds; //remaining execution duration

}alarm_execution_t;


void set_event();
void confirm_event();


/*========================================================================================================*/
/*========================================================================================================*/

/*========================================================================================================*/
//ALARM EEPROM storing and monitoring
/*========================================================================================================*/
#define MAX_EVENTS 4
#define EEPROM_MAGIC_ADDR 0x00
#define EEPROM_COUNT_ADDR 0x01
#define EEPROM_EVENT_START_ADDR 0x02
#define EEPROM_EVENT_SIZE 12
#define EEPROM_MAGIC_NUMBER 0xA5



void monitor_event();
void process_triggered_event();
void start_event(unsigned char index);




unsigned char calculate_days_to_add(event_t *current_event, unsigned char current_hour, unsigned char current_minute);
void add_days_to_date(unsigned char *day, unsigned char *month, unsigned char *year, unsigned char days_to_add);
void save_next_occurrence(event_t *current_event, unsigned int address);
void save_event_to_eeprom(event_t *current_event);
void find_next_event();
void compare_event(unsigned char index, unsigned char day,unsigned char month,unsigned char year,unsigned char hour,unsigned char minute);
void update_alarm_execution();
void update_alarm_leds();


void get_next_occurence(unsigned char index,unsigned char *day, unsigned char *month, unsigned char *year);
event_trigger_t get_trigger_value(unsigned char index);

unsigned char bcd_to_decimal(unsigned char bcd);
/*========================================================================================================*/
/*========================================================================================================*/
#endif	/* MAIN_H */





