

#ifndef DS1307_H
#define	DS1307_H

#define SLAVE_WRITE 0xD0 //11010000
#define SLAVE_READ 0xD1 //11010001 

#define SEC_ADDR 0X00
#define MIN_ADDR 0X01
#define HOUR_ADDR 0X02
#define DAY_ADDR 0x03
#define DATE_ADDR 0x04
#define MONTH_ADDR 0x05
#define YEAR_ADDR 0x06


void init_ds1307();
unsigned char read_ds1307(unsigned char addr);
void write_ds1307(unsigned char data, unsigned char addr);


void display_time(unsigned char *clock_reg);
void get_time(unsigned char *clock_reg);


#endif	/* DS1307_H */

