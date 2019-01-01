#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "lcd.h"
#include "key.h"  
#include "24cxx.h" 
#include "myiic.h"
#include "touch.h" 
#include "math.h"
#include "rtc.h"
#include "usmart.h"
#include "tsensor.h"
#include "adc.h"
#include "remote.h"
//#define len(array,len) {len = (sizeof(array) / sizeof(array[0]));}
struct timer timers[11] = {{0,0,0},{0,1,0}, {0,0,3}, {1,0,0}};
struct alarm alarms[11] = {{0,0,0,0},{8,15,0,0},{1,45,0,0}};
int timer_counter = 1;
int time_counter = 1;
int alarm_counter = 1;
int timer_flag = 0;
int alarm_flag = 0;
int t_size = 3;
int a_size = 2;
int z_size = 14;
u16 adcx;
float temp;
float temperate;	 
float fahrenheit;
//ALIENTEK Mini STM32开发板范例代码21
//触摸屏实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司
////////////////////////////////////////////////////////////////////////////////
//电容触摸屏专有部分
//画水平线
//x0,y0:坐标
//len:线长度
//color:颜色
void gui_draw_hline(u16 x0,u16 y0,u16 len,u16 color)
{
	if(len==0)return;
	LCD_Fill(x0,y0,x0+len-1,y0,color);	
}
//画实心圆
//x0,y0:坐标
//r:半径
//color:颜色
void gui_fill_circle(u16 x0,u16 y0,u16 r,u16 color)
{											  
	u32 i;
	u32 imax = ((u32)r*707)/1000+1;
	u32 sqmax = (u32)r*(u32)r+(u32)r/2;
	u32 x=r;
	gui_draw_hline(x0-r,y0,2*r,color);
	for (i=1;i<=imax;i++) 
	{
		if ((i*i+x*x)>sqmax)// draw lines from outside  
		{
 			if (x>imax) 
			{
				gui_draw_hline (x0-i+1,y0+x,2*(i-1),color);
				gui_draw_hline (x0-i+1,y0-x,2*(i-1),color);
			}
			x--;
		}
		// draw lines from inside (center)  
		gui_draw_hline(x0-x,y0+i,2*x,color);
		gui_draw_hline(x0-x,y0-i,2*x,color);
	}
}  
//两个数之差的绝对值 
//x1,x2：需取差值的两个数
//返回值：|x1-x2|
u16 my_abs(u16 x1,u16 x2)
{			 
	if(x1>x2)return x1-x2;
	else return x2-x1;
}  
//画一条粗线
//(x1,y1),(x2,y2):线条的起始坐标
//size：线条的粗细程度
//color：线条的颜色
void lcd_draw_bline(u16 x1, u16 y1, u16 x2, u16 y2,u8 size,u16 color)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	if(x1<size|| x2<size||y1<size|| y2<size)return; 
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{  
		gui_fill_circle(uRow,uCol,size,color);//画点 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}   


//画指针
void Pointer(void){
	gui_fill_circle(120,150,75,WHITE);
	u8 t;
	int x;
	int y;
	double tmp;
	t=calendar.sec;
	tmp=(double)t/60;
	x=120+(int)75*sin(tmp*2*3.1415926);
	y=150-(int)75*cos(tmp*2*3.1415926);
	LCD_DrawLine(120,150,x,y);
	tmp=(double)(calendar.min+(double)calendar.sec/60)/60;
	x=120+(int)60*sin(tmp*2*3.1415926);
	y=150-(int)60*cos(tmp*2*3.1415926);
	LCD_DrawLine(120,150,x,y);
	tmp=(double)(calendar.hour+(double)calendar.min/60)/12;
	x=120+(int)40*sin(tmp*2*3.1415926);
	y=150-(int)40*cos(tmp*2*3.1415926);
	LCD_DrawLine(120,150,x,y);
}
//表盘
void Clock(void){
	LCD_Draw_Circle(120,150,80);
	u8 a;
	int x;
	int y;
	double tmp;
	a=1;
	for(;a<=12;a=a+1){
		tmp=(double)a/12;
		x=120+(int)80*sin(tmp*2*3.1415926);
		y=150-(int)80*cos(tmp*2*3.1415926);
		gui_fill_circle(x,y,3,GRAYBLUE);
	}
		Pointer();
		}
//打印星期
void printWeek(u8 x,u8 y,u8 size){
	switch(calendar.week)
			{
				case 0:
					LCD_ShowString(x,y,200,size,size,"Sunday   ");
					break;
				case 1:
					LCD_ShowString(x,y,200,size,size,"Monday   ");
					break;
				case 2:
					LCD_ShowString(x,y,200,size,size,"Tuesday  ");
					break;
				case 3:
					LCD_ShowString(x,y,200,size,size,"Wednesday");
					break;
				case 4:
					LCD_ShowString(x,y,200,size,size,"Thursday ");
					break;
				case 5:
					LCD_ShowString(x,y,200,size,size,"Friday   ");
					break;
				case 6:
					LCD_ShowString(x,y,200,size,size,"Saturday ");
					break;  
			}
}
//
void test1(u8 x, u8 y){
	u8 i = 1;
	LCD_DrawLine(x,y+3,x,y+22);
	for(;i<5;i++){
		LCD_DrawLine(x+i,y+2+i,x+i,y+23-i);
	}
}
void test2(u8 x,u8 y){
	u8 i = 1;
	LCD_DrawLine(x+3,y,x+22,y);
	for(;i<5;i++){
		LCD_DrawLine(x+2+i,y+i,x+23-i,y+i);
	}
}
void test3(u8 x,u8 y){
	LCD_DrawLine(x+5,y+22,x+20,y+22);
	LCD_DrawLine(x+4,y+23,x+21,y+23);
	LCD_DrawLine(x+3,y+24,x+22,y+24);
	LCD_DrawLine(x+4,y+25,x+21,y+25);
	LCD_DrawLine(x+5,y+26,x+20,y+26);
}
//
void test4(u8 x, u8 y){
	u8 i = 1;
	LCD_DrawLine(x+25,y+3,x+25,y+22);
	for(;i<5;i++){
		LCD_DrawLine(x+25-i,y+2+i,x+25-i,y+23-i);
	}
}
void test5(u8 x,u8 y){
	u8 i = 1;
	LCD_DrawLine(x+3,y+48,x+22,y+48);
	for(;i<5;i++){
		LCD_DrawLine(x+2+i,y+48-i,x+23-i,y+48-i);
	}
}
void test6(u8 x,u8 y,u8 dig){
	switch(dig){
		case 0:
			test1(x,y);
			test1(x,y+23);
			test4(x,y);
			test4(x,y+23);
			test2(x,y);
			test5(x,y);
			break;
		case 1:
			test4(x,y);
			test4(x,y+23);
			break;
		case 2:
			test1(x,y+23);
			test4(x,y);
			test2(x,y);
			test3(x,y);
			test5(x,y);
			break;
		case 3:
			test4(x,y);
			test4(x,y+23);
			test2(x,y);
			test3(x,y);
			test5(x,y);
			break;
		case 4:
			test1(x,y);
			test4(x,y);
			test4(x,y+23);
			test3(x,y);
			break;
		case 5:
			test1(x,y);
			test4(x,y+23);
			test2(x,y);
			test3(x,y);
			test5(x,y);
			break;
		case 6:
			test1(x,y);
			test1(x,y+23);
			test4(x,y+23);
			test2(x,y);
			test3(x,y);
			test5(x,y);
			break;
		case 7:
			test4(x,y);
			test4(x,y+23);
			test2(x,y);
			break;
		case 8:
			test1(x,y);
			test1(x,y+23);
			test4(x,y);
			test4(x,y+23);
			test2(x,y);
			test3(x,y);
			test5(x,y);
			break;
		case 9:
			test1(x,y);
			test4(x,y);
			test4(x,y+23);
			test2(x,y);
			test3(x,y);
			test5(x,y);
			break;
		default:break;
	}
}
void test7(u8 x,u8 y,u8 dig){
	u8 a = dig/10;
	u8 b = dig%10;
	test6(x,y,a);
	test6(x+30,y,b);
}
void test8(u8 x,u8 y){
	gui_fill_circle(x+6,y+12,3,MAGENTA);
	gui_fill_circle(x+6,y+36,3,MAGENTA);	
}
//数字时钟
void Digit(void){	
			LCD_Color_Fill(20,70,240,200,WHITE);
			test7(20,125,calendar.hour);
			test7(95,125,calendar.min);
			test7(170,125,calendar.sec);
			test8(80,125);
			test8(155,125);
			LCD_ShowString(60,75,200,16,16,"00.00C/");	
			LCD_ShowxNum(60,75,(u8)temperate,2,16,0); //显示温度整数部分
			temperate-=(u8)temperate;	  
			LCD_ShowxNum(84,75,temperate*100,2,16,0X80);//显示温度小数部分
			LCD_ShowString(116,75,200,16,16,"00.00F");	
			LCD_ShowxNum(116,75,(u8)fahrenheit,2,16,0); //显示温度整数部分
			fahrenheit-=(u8)fahrenheit;	  
			LCD_ShowxNum(140,75,fahrenheit*100,2,16,0X80);//显示温度小数部分
			LED0=!LED0;
}
//检查闹钟
void checkAlarm(void){
	u8 i = 1;
	u8 a = 0;
	for(;i<a_size+1;i++){
		if(alarms[i].on_off){
			if(alarms[i].hour==calendar.hour&&alarms[i].minute==calendar.min&&alarms[i].second==calendar.sec){
				for(;a<10; a++) {
						LED0 = !LED0;
						delay_ms(100);
					}
			}
		}
	}
}
//reset
void Load_Draw_Dialog(void){
	LCD_Color_Fill(0,0,240,40,WHITE);//清屏   
 	POINT_COLOR=LIGHTGREEN;//设置字体为蓝色  
	//lcd_draw_bline(40,40,100,40,6,LIGHTGREEN);//左上按钮划线
	POINT_COLOR=GRAYBLUE;//设置字体为红色 
	LCD_ShowString(10,8,50,24,24,"Alarm");
	//gui_fill_circle(180,30,25,LIGHTGREEN);//右上按钮画圆
	LCD_ShowString(90,8,50,24,24,"Clock");
	//LCD_ShowString(45,80,200,24,24,"SIGN");//左下按钮签名
	LCD_ShowString(170,8,50,24,24,"Timer");
	/*
	lcd_draw_bline(180,75,163,105,10,LIGHTGREEN);//右下按钮三角
	lcd_draw_bline(163,105,197,105,10,LIGHTGREEN);
	lcd_draw_bline(197,105,180,75,10,LIGHTGREEN);
	*/
}
void select_button1(void){
	Load_Draw_Dialog();//清屏 
	//LCD_Color_Fill(0,0,60,40,GBLUE);//选中按钮区高亮
	//lcd_draw_bline(40,40,100,40,6,YELLOW);//左上按钮划线
	POINT_COLOR=RED;//设置字体为红色 
	LCD_ShowString(10,8,50,24,24,"Alarm");
}
void select_button2(void){
	Load_Draw_Dialog();//清屏 
	//LCD_Color_Fill(120,0,240,60,GBLUE);//选中按钮区高亮
	//gui_fill_circle(180,30,25,YELLOW);//右上按钮画圆
	POINT_COLOR=RED;//设置字体为红色 
	LCD_ShowString(90,8,50,24,24,"Clock");
}
void select_button3(void){
	Load_Draw_Dialog();//清屏 
	POINT_COLOR=RED;//设置字体为红色 
	LCD_ShowString(170,8,50,24,24,"Timer");
}
void left_button(void){
		lcd_draw_bline(25,260,10,280,5,BLUE);
		lcd_draw_bline(25,300,10,280,5,BLUE);
		delay_ms(100);
}
void right_button(void){
		lcd_draw_bline(215,260,230,280,5,BLUE);
		lcd_draw_bline(215,300,230,280,5,BLUE);
		delay_ms(100);
	}
void up_button(void){
	lcd_draw_bline(60,297,80,283,5,BLUE);
		lcd_draw_bline(100,297,80,283,5,BLUE);
		delay_ms(100);
	}
void down_button(void){
		lcd_draw_bline(140,283,160,297,5,BLUE);
		lcd_draw_bline(180,283,160,297,5,BLUE);
		delay_ms(100);
	}
void show_time(u16 x,u16 y,u8 hour,u8 min, u8 sec,u8 size){
	LCD_ShowString(x,y,100,size,size,"  :  :  ");	
	LCD_ShowxNum(x,y,hour,2,size,0x80);	
	LCD_ShowxNum(x+36,y,min,2,size,0x80);
	LCD_ShowxNum(x+72,y,sec,2,size,0x80);
}
void function1(void){
	POINT_COLOR=GRAYBLUE;//设置字体为红色 
	vu8 hour = alarms[alarm_counter].hour;
	vu8 minute = alarms[alarm_counter].minute;
	vu8 second = alarms[alarm_counter].second;
	show_time(40,266,hour,minute,second,24);
	if(alarms[alarm_counter].on_off){
		POINT_COLOR=RED;//设置字体为红色 
		LCD_Color_Fill(140,260,202,320,WHITE);
		LCD_ShowString(163,266,200,24,24,"On");	
	}else{
		LCD_Color_Fill(140,260,202,320,WHITE);
		LCD_ShowString(155,266,200,24,24,"Off");
	}
	LCD_ShowString(35,250,50,16,16,"ID:");	
	LCD_ShowNum(62,250,alarm_counter,1,16);
	POINT_COLOR=GRAYBLUE;//设置字体为红色
}

void function2(void){
	u8 hour;
	POINT_COLOR=GRAYBLUE;//设置字体为红色 
	LCD_Color_Fill(33,250,150,265,WHITE);	
		switch(time_counter){
			case 1: hour = calendar.hour;LCD_ShowString(155,266,200,24,24,"CHN");break;
			case 2: hour = calendar.hour;LCD_ShowString(155,266,200,24,24,"SIN");break;
			case 3: hour = (calendar.hour + 23)%24;LCD_ShowString(155,266,200,24,24,"THA");break;
			case 4: hour = (calendar.hour + 21)%24;LCD_ShowString(155,266,200,24,24,"IND");break;
			case 5: hour = (calendar.hour + 1)%24;LCD_ShowString(155,266,200,24,24,"JPN");break;
			case 6: hour = (calendar.hour + 1)%24;LCD_ShowString(155,266,200,24,24,"KOR");break;
			case 7: hour = (calendar.hour + 21)%24;LCD_ShowString(155,266,200,24,24,"PAK");break;
			case 8: hour = (calendar.hour + 11)%24;LCD_ShowString(155,266,200,24,24,"USA");break;
			case 9: hour = (calendar.hour + 16)%24;LCD_ShowString(155,266,200,24,24,"GBR");break;
			case 10: hour = (calendar.hour + 17)%24;LCD_ShowString(155,266,200,24,24,"FRA");break;
			case 11: hour = (calendar.hour + 17)%24;LCD_ShowString(155,266,200,24,24,"GER");break;
			case 12: hour = calendar.hour;LCD_ShowString(155,266,200,24,24,"MAS");break;
			case 13: hour = (calendar.hour + 1)%24;LCD_ShowString(155,266,200,24,24,"INA");break;
		}
	show_time(40,266,hour,calendar.min,calendar.sec,24);
}
void function3(void){	
	if(timer_counter==timer_flag){
		POINT_COLOR=RED;//设置字体为红色 
		u32 temp = CountDown%86400;     		//得到秒钟数 
		u8 hour = temp/3600;
		u8 min = (temp%3600)/60;
		u8 sec = (temp%3600)%60;
		show_time(40,266,hour,min,sec-1,24);
		LCD_Color_Fill(140,260,202,320,WHITE);
		LCD_ShowString(141,266,200,24,24,"Cancel");	
	}
	else{
		POINT_COLOR=GRAYBLUE;//设置字体为红色
		vu8 hour = timers[timer_counter].hour;
		vu8 minute = timers[timer_counter].minute;
		vu8 second = timers[timer_counter].second;
		show_time(40,266,hour,minute,second,24);
		LCD_Color_Fill(140,260,210,320,WHITE);
		LCD_ShowString(145,266,200,24,24,"Start");
	}
	LCD_ShowString(35,250,50,16,16,"ID:");	
	LCD_ShowNum(62,250,timer_counter,1,16);
	POINT_COLOR=GRAYBLUE;//设置字体为红色
}
void left_right(){
	lcd_draw_bline(215,260,230,280,5,GRAYBLUE);
		lcd_draw_bline(215,300,230,280,5,GRAYBLUE);
		lcd_draw_bline(25,260,10,280,5,GRAYBLUE);
		lcd_draw_bline(25,300,10,280,5,GRAYBLUE);
}
void up_down(){
	lcd_draw_bline(60,297,80,283,5,GRAYBLUE);
		lcd_draw_bline(100,297,80,283,5,GRAYBLUE);
		lcd_draw_bline(140,283,160,297,5,GRAYBLUE);
		lcd_draw_bline(180,283,160,297,5,GRAYBLUE);
}
void select_edit(u16 year, u8 month, u8 date, u8 hour, u8 min, u8 sec, u8 select){
		test7(60,40,year/100);test7(120,40,year%100);
		test7(40,120,month);test3(100,120);test7(130,120,date);
		test7(20,200,hour);test8(80,200);test7(95,200,min);test8(155,200),test7(170,200,sec);
		switch(select){
			case 0:POINT_COLOR=RED;test7(60,40,year/100);test7(120,40,year%100);POINT_COLOR=GRAYBLUE;break;//year
			case 1:POINT_COLOR=RED;test7(40,120,month);POINT_COLOR=GRAYBLUE;break;//month
			case 2:POINT_COLOR=RED;test7(130,120,date);POINT_COLOR=GRAYBLUE;break;//date
			case 3:POINT_COLOR=RED;test7(20,200,hour);POINT_COLOR=GRAYBLUE;break;//hour
			case 4:POINT_COLOR=RED;test7(95,200,min);POINT_COLOR=GRAYBLUE;break;//min
			case 5:POINT_COLOR=RED;test7(170,200,sec);POINT_COLOR=GRAYBLUE;break;//sec
		}
}
void edit(){
	u8 a = 0;
	u8 key = KEY_Scan(0);
	LCD_Clear(WHITE);
	u16 year = calendar.w_year;
	u8 month = calendar.w_month;
	u8 date = calendar.w_date;
	u8 hour = calendar.hour;
	u8 min = calendar.min;
	u8 sec = calendar.sec;
	select_edit(year,month,date,hour,min,sec, 0);
	while(1){
		up_down();
		select_edit(year,month,date,hour,min,sec, a);
		LED0=!LED0;tp_dev.scan(0);
		if(tp_dev.sta&TP_PRES_DOWN)			//触摸屏被按下
		{
			if(tp_dev.x[0]<lcddev.width&&tp_dev.y[0]<lcddev.height)
			{	 
				if(tp_dev.x[0]>60&&tp_dev.x[0]<180&&tp_dev.y[0]<100&&tp_dev.y[0]>40){//year
					a = 0;
					select_edit(year,month,date,hour,min,sec, a);
				}
				else if(tp_dev.x[0]>40&&tp_dev.x[0]<100&&tp_dev.y[0]>120&&tp_dev.y[0]<180){//month
					a = 1;
					select_edit(year,month,date,hour,min,sec, a);
				}
				else if(tp_dev.x[0]>130&&tp_dev.x[0]<190&&tp_dev.y[0]>120&&tp_dev.y[0]<180){//date
					a = 2;
					select_edit(year,month,date,hour,min,sec, a);
				}
				else if(tp_dev.x[0]>20&&tp_dev.x[0]<80&&tp_dev.y[0]>200&&tp_dev.y[0]<260){//hour
					a = 3;
					select_edit(year,month,date,hour,min,sec, a);
				}
				else if(tp_dev.x[0]>95&&tp_dev.x[0]<155&&tp_dev.y[0]>200&&tp_dev.y[0]<260){//min
					a = 4;
					select_edit(year,month,date,hour,min,sec, a);
				}
				else if(tp_dev.x[0]>170&&tp_dev.x[0]<230&&tp_dev.y[0]>200&&tp_dev.y[0]<260){//sec
					a = 5;
					select_edit(year,month,date,hour,min,sec, a);
				}
				else if(tp_dev.x[0]<120&&tp_dev.y[0]>260){//up
					up_button();
					switch(a){
						case 0: LCD_Color_Fill(60,40,180,100,WHITE); year=(year+1);break;
						case 1: LCD_Color_Fill(40,120,100,180,WHITE);month=month%12+1;break;
						case 2: LCD_Color_Fill(130,120,190,180,WHITE);if(year%4==0&&month==2){date=date%29+1;}else{date=date%28+1;}break;
						case 3: LCD_Color_Fill(20,200,80,260,WHITE);hour=(hour+1)%24;break;
						case 4: LCD_Color_Fill(95,200,155,260,WHITE);min=(min+1)%60;break;
						case 5: LCD_Color_Fill(170,200,230,260,WHITE);sec=(sec+1)%60;break;
					}
				}
				else if(tp_dev.x[0]>120&&tp_dev.y[0]>260){//down
					down_button();
					switch(a){
						case 0: LCD_Color_Fill(60,40,180,100,WHITE);year=(year-1);break;
						case 1: LCD_Color_Fill(40,120,100,180,WHITE);if(month==1){month=12;}else{month = month-1;};break;
						case 2: LCD_Color_Fill(130,120,190,180,WHITE);
								if(year%4==0&&month==2){
									if(date==1){date=29;}
									else{date=date-1;}
								}else{
									if(date==1){date=28;}
									else{date=date-1;}
								}
									break;
						case 3: LCD_Color_Fill(20,200,80,260,WHITE);hour=(hour+23)%24;break;
						case 4: LCD_Color_Fill(95,200,155,260,WHITE);min=(min+59)%60;break;
						case 5: LCD_Color_Fill(170,200,230,260,WHITE);sec=(sec+59)%60;break;
					}
				}
			}
		}
	 	key=KEY_Scan(0);
		if(key==WKUP_PRES){
			RTC_Set(year,month,date,hour,min,sec);
			break;
		}else if(key==KEY1_PRES){
			break;
		}
		delay_ms(100);
	}
}
////////////////////////////////////////////////////////////////////////////////
//5个触控点的颜色												 
//电阻触摸屏测试函数
void rtp_test(void)
{
	Load_Draw_Dialog();
	RTC_Set(2018,12,4,20,45,9);
	u8 mode = 1;
	u8 key = KEY_Scan(0);
	u8 i=0;	 
	u8 function = 1;
	vu16 old_year = 0;
	vu8 old_month = 0;
	vu8 old_date = 0;
	vu8 old_week = 0;
	vu8 old_hour = 0;
	vu8 old_min = 0;
	vu8 old_sec = 0;
	int len;
	int counter = 1;
	int bell = 0;
	
	while(RTC_Init())		//RTC初始化	，一定要初始化成功
	{ 
		
		LCD_ShowString(60,130,200,16,16,"RTC ERROR!   ");	
		delay_ms(800);
		LCD_ShowString(60,130,200,16,16,"RTC Trying...");	
	}	
	Clock();  
	LCD_ShowString(45,45,200,16,16,"    -  -     ");	   
	LCD_ShowString(195,45,200,16,16,"  :  :  ");	
	select_button1();
	while(1)
	{
			LCD_ShowxNum(109,45,calendar.w_date,2,16,0x80);
			LCD_ShowxNum(85,45,calendar.w_month,2,16,0x80);
			LCD_ShowNum(45,45,calendar.w_year,4,16);
		adcx=Get_Adc_Average(ADC_CH_TEMP,10);
		temp=(float)adcx*(3.3/4096);
		temperate=temp;//保存温度传感器的电压值
		adcx=temp;
		temp-=(u8)temp;				    			//减掉整数部分		  
 		temperate=(1.43-temperate)/0.0043+25;		//计算出当前温度值
		fahrenheit = temperate*9/5+32;				//计算华氏度 
		if(old_date!=calendar.w_date){LCD_Color_Fill(109,45,125,62,WHITE);old_date = calendar.w_date;}
		if(old_month!=calendar.w_month){LCD_Color_Fill(85,45,101,62,WHITE);old_month = calendar.w_month;}
		if(old_year!=calendar.w_year){LCD_Color_Fill(45,45,77,62,WHITE);	old_year = calendar.w_year;}
		if(old_week!=calendar.week){printWeek(145,45,16);}	
		if(old_sec!=calendar.sec){
			checkAlarm();
			if(mode==1){
				Pointer();
			}else if(mode==0){
				Digit();
			}
			if(timer_flag){
				if(CountDown == 0) {
					flag = 0;
					timer_flag=0;
					bell++;
					for(i=0; i<10; i++) {
						LED0 = !LED0;
						delay_ms(100);
					}
				}
				function3();
			}
			if(function==2){function2();}
			old_sec = calendar.sec;
		}
		left_right();
	
	 	key=KEY_Scan(0);
		POINT_COLOR = LBBLUE;
		tp_dev.scan(0); 	
		if(tp_dev.sta&TP_PRES_DOWN)			//触摸屏被按下
		{	
			if(tp_dev.x[0]<lcddev.width&&tp_dev.y[0]<lcddev.height)
			{	 
				if(tp_dev.x[0]<80&&tp_dev.y[0]<30){//alarm
					select_button1();//第一个按钮
					function1();
					function = 1;
				}
				else if(tp_dev.x[0]<160&&tp_dev.x[0]>80&&tp_dev.y[0]<30){//time
					select_button2();//右上按钮
					function2();
					function = 2;
				}
				else if(tp_dev.x[0]>160&&tp_dev.y[0]<30){//timer
					select_button3();//左下按钮
					function3();
					function = 3;
				}
				else if(tp_dev.x[0]>140&&tp_dev.x[0]<200&&tp_dev.y[0]>240){//start
					if(function==1){
						alarms[alarm_counter].on_off = !alarms[alarm_counter].on_off;
						function1();
					}else if(function==3){
						if(timer_flag==timer_counter){
							flag=0;
							timer_flag=0;
						}else{
							RTC_CountDown(timers[timer_counter].hour,timers[timer_counter].minute,timers[timer_counter].second);
							timer_flag = timer_counter;
						}
						function3();
					}
				}
				else{
					if(function==1){len=a_size+1; counter = alarm_counter;}
					else if(function==2){len=z_size+1; counter = time_counter;}
					else if(function==3){len=t_size+1; counter = timer_counter;}
					if(tp_dev.x[0]<40&&tp_dev.y[0]>240){
						left_button();//左翻
						if(counter == 1){
							counter = len;
						}
						counter--;
					}else if(tp_dev.x[0]>200&&tp_dev.y[0]>240){
						right_button();//右翻
						counter++;
						if(counter==len){
							counter = 1;
						}
					}
					if(function==1){
						alarm_counter = counter;
						function1();
					}else if(function == 2){
						time_counter = counter;
						function2();
					}else if(function == 3){
						timer_counter = counter;
						function3();
					}
			}
			}
		}
		delay_ms(100);	//没有按键按下的时候 	    
		if(key==KEY0_PRES)	//KEY0按下,则执行校准程序
		{/**
			LCD_Clear(WHITE);//清屏
		    TP_Adjust();  //屏幕校准 
			TP_Save_Adjdata();	 
			Load_Draw_Dialog();*/
			Load_Draw_Dialog();
			mode = 1;
			LCD_Color_Fill(0,70,240,200,WHITE);
			Clock();
			printf("mode=1");
		}
		else if(key==KEY1_PRES){
			Load_Draw_Dialog();
			gui_fill_circle(120,150,90,WHITE);
			mode = 0;
			printf("mode=0");
		}
		else if(key==WKUP_PRES){
			edit();
			LCD_Clear(WHITE);
			Clock();  
			Load_Draw_Dialog();
			POINT_COLOR=GRAYBLUE;//设置字体为红色 
			LCD_ShowString(45,45,200,16,16,"    -  -     ");
			LCD_ShowString(195,45,200,16,16,"  :  :  ");	
			printWeek(145,45,16);
		}
	}
}
const u16 POINT_COLOR_TBL[CT_MAX_TOUCH]={RED,GREEN,BLUE,BROWN,GRED};  
//电容触摸屏测试函数
void ctp_test(void)
	{
	u8 t=0;
	u8 i=0;	  	    
 	u16 lastpos[5][2];		//最后一次的数据 
	while(1)
	{
		tp_dev.scan(0);
		for(t=0;t<CT_MAX_TOUCH;t++)//最多5点触摸
		{
			if((tp_dev.sta)&(1<<t))//判断是否有点触摸？
			{
				if(tp_dev.x[t]<lcddev.width&&tp_dev.y[t]<lcddev.height)//在LCD范围内
				{
					if(lastpos[t][0]==0XFFFF)
					{
						lastpos[t][0] = tp_dev.x[t];
						lastpos[t][1] = tp_dev.y[t];
					}
					lcd_draw_bline(lastpos[t][0],lastpos[t][1],tp_dev.x[t],tp_dev.y[t],2,POINT_COLOR_TBL[t]);//画线
					lastpos[t][0]=tp_dev.x[t];
					lastpos[t][1]=tp_dev.y[t];
					if(tp_dev.x[t]>(lcddev.width-24)&&tp_dev.y[t]<16)
					{
						Load_Draw_Dialog();//清除
					}
				}
			}else lastpos[t][0]=0XFFFF;
		}
		
		delay_ms(5);i++;
		if(i%20==0)LED0=!LED0;
	}	
}
 int main(void)
 { 
	LED0=1;
 	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(9600);	 	//串口初始化为9600
	LED_Init();		  		//初始化与LED连接的硬件接口
	LCD_Init();			   	//初始化LCD 	
	KEY_Init();				//按键初始化		 	
	tp_dev.init();			//触摸屏初始化
	Adc_Init(); 				//ADC初始化	 
	Remote_Init();   
	usmart_dev.init(72);	//初始化USMART
	POINT_COLOR=RED;//设置字体为红色 
	LCD_ShowString(60,50,200,16,16,"Mini STM32");	
	LCD_ShowString(60,70,200,16,16,"TOUCH TEST");	
	LCD_ShowString(60,90,200,16,16,"CSE@SUSTech");
	LCD_ShowString(60,110,200,16,16,"2018/10/30");
	//if(tp_dev.touchtype!=0XFF)LCD_ShowString(60,130,200,16,16,"Press KEY0 to Adjust");//电阻屏才显示
	if(tp_dev.touchtype!=0XFF) LCD_ShowString(60,130,200,16,16,"This is a ");
	if(tp_dev.touchtype&0X80) LCD_ShowString(60,130,200,16,16,"CTP");//
	 else LCD_ShowString(60,130,200,16,16,"RTP");//
  delay_ms(10000);
	 LCD_Clear(WHITE);
	if(tp_dev.touchtype&0X80) ctp_test();	//电容屏测试
	else 
		rtp_test(); 						//电阻屏测试
}

