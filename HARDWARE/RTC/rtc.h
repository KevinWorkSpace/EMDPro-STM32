#ifndef __RTC_H
#define __RTC_H	  
#include "stdint.h"
#include "inttypes.h"
#include "stdlib.h"
#include "sys.h"
//Mini STM32开发板
//RTC实时时钟 驱动代码			 
//正点原子@ALIENTEK
//2010/6/6

//时间结构体
typedef struct 
{
	vu8 hour;
	vu8 min;
	vu8 sec;			
	//公历日月年周
	vu16 w_year;
	vu8  w_month;
	vu8  w_date;
	vu8  week;		 
}_calendar_obj;					 
extern _calendar_obj calendar;	//日历结构体
extern u32 CountDown;    //倒计时的秒数
extern int flag;     //判断是否需要显示倒计时

struct alarm
{	
	vu8 hour;
	vu8 minute;
	vu8 second;
	vu8 on_off;
};
struct timer
{	
	vu8 hour;
	vu8 minute;
	vu8 second;
};
	
extern struct timer timers[11];
extern struct alarm alarms[11];
extern int timer_counter;
extern int time_counter;
extern int alarm_counter;
extern int timer_flag;
extern int alarm_flag;
extern int t_size;
extern int a_size;
extern int z_size;

extern u8 const mon_table[12];	//月份日期数据表
void Disp_Time(u8 x,u8 y,u8 size);//在制定位置开始显示时间
void Disp_Week(u8 x,u8 y,u8 size,u8 lang);//在指定位置显示星期
u8 RTC_Init(void);        //初始化RTC,返回0,失败;1,成功;
u8 Is_Leap_Year(u16 year);//平年,闰年判断
u8 RTC_Get(void);         //更新时间   
u8 RTC_Get_Week(u16 year,u8 month,u8 day);
u8 RTC_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);//设置时间			
u8 RTC_CountDown(u8 hour,u8 min,u8 sec); 
void add_alarm(u8 hour,u8 min,u8 sec);
void edit_alarm(u8 id,u8 hour,u8 min, u8 sec);
void add_timer(u8 hour,u8 min,u8 sec);
void edit_timer(u8 id,u8 hour,u8 min, u8 sec);
void del_alarm(u8 id);
void del_timer(u8 id);
#endif


