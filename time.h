/* 
 * File:   time.h
 * Author: shaunvarghese
 *
 * Created on October 10, 2026, 10:00 AM
 */

#ifndef TIME_H
#define	TIME_H

unsigned char * clcd_get_time();
unsigned char* clcd_get_date();
void get_time(unsigned char *time);

unsigned int get_ticks();
#endif	/* TIME_H */

