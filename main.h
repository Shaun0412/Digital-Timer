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
#define INACTIVITY_THRESHOLD 490 //5 second
#define BLINK_THRESHOLD 49 //0.5 hz toggling

typedef enum 
{
    first_option_index = 0,
    second_option_index
}menu_indices;
 //records time of previous activity from system_ticks


/****Fn PROTOTYPES********** */
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

void print_default_mode_clcd_screen();

#endif	/* MAIN_H */

