#include "system.h"
#include "delay.h"
#include "sys.h"
#include "usart.h" 
#include "led.h" 		 	 
#include "lcd.h"  
#include "key.h"     
#include "usmart.h" 
#include "malloc.h"
#include "sdio_sdcard.h"  
#include "w25qxx.h"    
#include "ff.h"  
#include "exfuns.h"   
#include "text.h"
#include "piclib.h"	
#include "string.h"		
#include "math.h"	 
#include "ov7670.h" 
#include "beep.h" 
#include "timer.h" 
#include "exti.h"
//上浮->下潜
//前进->后退
//右转->左转
/*
0xFA
模式
速度 u16
数据校验位
0XFB
*/

uint8_t key;
uint16_t v;
u8 temperature;
extern u8 ov_sta;	//在exit.c里面定义
extern u8 ov_frame;	//在timer.c里面定义	   
//更新LCD显示
void camera_refresh(void)
{
	u32 j;
 	u16 color;	 
	if(ov_sta)//有帧中断更新？
	{
		LCD_Scan_Dir(U2D_L2R);		//从上到下,从左到右  
		if(lcddev.id==0X1963)LCD_Set_Window((lcddev.width-240)/2,(lcddev.height-320)/2,240,320);//将显示区域设置到屏幕中央
		else if(lcddev.id==0X5510||lcddev.id==0X5310)LCD_Set_Window((lcddev.width-320)/2,(lcddev.height-240)/2,320,240);//将显示区域设置到屏幕中央
		LCD_WriteRAM_Prepare();     //开始写入GRAM	
		OV7670_RRST=0;				//开始复位读指针 
		OV7670_RCK_L;
		OV7670_RCK_H;
		OV7670_RCK_L;
		OV7670_RRST=1;				//复位读指针结束 
		OV7670_RCK_H;
		for(j=0;j<76800;j++)
		{
			OV7670_RCK_L;
			color=GPIOC->IDR&0XFF;	//读数据
			OV7670_RCK_H; 
			color<<=8;  
			OV7670_RCK_L;
			color|=GPIOC->IDR&0XFF;	//读数据
			OV7670_RCK_H; 
			LCD->LCD_RAM=color;    
		}   							  
 		ov_sta=0;					//清零帧中断标记
		ov_frame++; 
		LCD_Scan_Dir(DFT_SCAN_DIR);	//恢复默认扫描方向 
	} 
}	   
//文件名自增（避免覆盖）
//组合成:形如"0:PHOTO/PIC13141.bmp"的文件名
void camera_new_pathname(u8 *pname)
{	 
	u8 res;					 
	u16 index=0;
	while(index<0XFFFF)
	{
		sprintf((char*)pname,"0:PHOTO/PIC%05d.bmp",index);
		res=f_open(ftemp,(const TCHAR*)pname,FA_READ);//尝试打开这个文件
		if(res==FR_NO_FILE)break;		//该文件名不存在=正是我们需要的.
		index++;
	}
}

extern u8 Res_com;
extern u8 Res_com2;


int main(void)
{	
	u8 res;							 
	u8 *pname;				//带路径的文件名    
	u8 i;	
	while(System_Init()){}
  while(1)
	{
		if(key==6)
		{
			v=V;
			//Motor_PWM_Set(1500+v,1500+v,1500+v,1500+v);
			mode_control(key,v);
			temperature=DS18B20_Get_Temp();	
			Send_to_PC();
			delay_ms(50);
		}
		else if(key==7)
		{
		  USART_SendData(USART2,0x31);
			while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//等待发送结束								   						    
			pname=mymalloc(SRAMIN,30);	//为带路径的文件名分配30个字节的内存		    
			while(pname==NULL)			//内存分配出错
			{	    
				Show_Str(30,190,240,16,"内存分配失败!",16,0);
				delay_ms(200);				  
				LCD_Fill(30,190,240,146,WHITE);//清除显示	     
				delay_ms(200);				  
			}   											  
			while(OV7670_Init())//初始化OV7670
			{
				Show_Str(30,190,240,16,"OV7670 错误!",16,0);
				delay_ms(200);
				LCD_Fill(30,190,239,206,WHITE);
				delay_ms(200);
			}
			Show_Str(30,190,200,16,"OV7670 正常",16,0);
			delay_ms(1500);	 		 
			//TIM6_Int_Init(10000,7199);			//10Khz计数频率,1秒钟中断									  
			EXTI8_Init();						//使能定时器捕获
			OV7670_Window_Set(12,176,240,320);	//设置窗口	  
			OV7670_CS=0;				    		    
			LCD_Clear(BLACK);
			while(1)
			{	
				//key=KEY_Scan(0);//不支持连按
				//if(key==KEY0_PRES)Res_com
				if(0x31==Res_com2||0x31==Res_com)
				{
					 Res_com2 = 0;
					 Res_com = 0;
			
					 LED1=0;	//点亮DS1,提示正在拍照
				 
					 res=bmp_encode(pname,(lcddev.width-240)/2,(lcddev.height-320)/2,240,320,0);
					 Show_Str(40,130,240,12,"图片传输中完成!",12,0);	
					 USART_SendData(USART2,0x32);
					 while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//等待发送结束				 
								
					BEEP=0;//关闭蜂鸣器
					LED1=1;//关闭DS1
					delay_ms(180);//等待1.8秒钟
					LCD_Clear(BLACK);
				}else delay_ms(5);
				camera_refresh();//更新显示
				i++;
				if(i==40)//DS0闪烁.
				{
					i=0;
					LED0=!LED0;
				}
			}	   								
		}
	}
}

