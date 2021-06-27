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
//�ϸ�->��Ǳ
//ǰ��->����
//��ת->��ת
/*
0xFA
ģʽ
�ٶ� u16
����У��λ
0XFB
*/

uint8_t key;
uint16_t v;
u8 temperature;
extern u8 ov_sta;	//��exit.c���涨��
extern u8 ov_frame;	//��timer.c���涨��	   
//����LCD��ʾ
void camera_refresh(void)
{
	u32 j;
 	u16 color;	 
	if(ov_sta)//��֡�жϸ��£�
	{
		LCD_Scan_Dir(U2D_L2R);		//���ϵ���,������  
		if(lcddev.id==0X1963)LCD_Set_Window((lcddev.width-240)/2,(lcddev.height-320)/2,240,320);//����ʾ�������õ���Ļ����
		else if(lcddev.id==0X5510||lcddev.id==0X5310)LCD_Set_Window((lcddev.width-320)/2,(lcddev.height-240)/2,320,240);//����ʾ�������õ���Ļ����
		LCD_WriteRAM_Prepare();     //��ʼд��GRAM	
		OV7670_RRST=0;				//��ʼ��λ��ָ�� 
		OV7670_RCK_L;
		OV7670_RCK_H;
		OV7670_RCK_L;
		OV7670_RRST=1;				//��λ��ָ����� 
		OV7670_RCK_H;
		for(j=0;j<76800;j++)
		{
			OV7670_RCK_L;
			color=GPIOC->IDR&0XFF;	//������
			OV7670_RCK_H; 
			color<<=8;  
			OV7670_RCK_L;
			color|=GPIOC->IDR&0XFF;	//������
			OV7670_RCK_H; 
			LCD->LCD_RAM=color;    
		}   							  
 		ov_sta=0;					//����֡�жϱ��
		ov_frame++; 
		LCD_Scan_Dir(DFT_SCAN_DIR);	//�ָ�Ĭ��ɨ�跽�� 
	} 
}	   
//�ļ������������⸲�ǣ�
//��ϳ�:����"0:PHOTO/PIC13141.bmp"���ļ���
void camera_new_pathname(u8 *pname)
{	 
	u8 res;					 
	u16 index=0;
	while(index<0XFFFF)
	{
		sprintf((char*)pname,"0:PHOTO/PIC%05d.bmp",index);
		res=f_open(ftemp,(const TCHAR*)pname,FA_READ);//���Դ�����ļ�
		if(res==FR_NO_FILE)break;		//���ļ���������=����������Ҫ��.
		index++;
	}
}

extern u8 Res_com;
extern u8 Res_com2;


int main(void)
{	
	u8 res;							 
	u8 *pname;				//��·�����ļ���    
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
			while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//�ȴ����ͽ���								   						    
			pname=mymalloc(SRAMIN,30);	//Ϊ��·�����ļ�������30���ֽڵ��ڴ�		    
			while(pname==NULL)			//�ڴ�������
			{	    
				Show_Str(30,190,240,16,"�ڴ����ʧ��!",16,0);
				delay_ms(200);				  
				LCD_Fill(30,190,240,146,WHITE);//�����ʾ	     
				delay_ms(200);				  
			}   											  
			while(OV7670_Init())//��ʼ��OV7670
			{
				Show_Str(30,190,240,16,"OV7670 ����!",16,0);
				delay_ms(200);
				LCD_Fill(30,190,239,206,WHITE);
				delay_ms(200);
			}
			Show_Str(30,190,200,16,"OV7670 ����",16,0);
			delay_ms(1500);	 		 
			//TIM6_Int_Init(10000,7199);			//10Khz����Ƶ��,1�����ж�									  
			EXTI8_Init();						//ʹ�ܶ�ʱ������
			OV7670_Window_Set(12,176,240,320);	//���ô���	  
			OV7670_CS=0;				    		    
			LCD_Clear(BLACK);
			while(1)
			{	
				//key=KEY_Scan(0);//��֧������
				//if(key==KEY0_PRES)Res_com
				if(0x31==Res_com2||0x31==Res_com)
				{
					 Res_com2 = 0;
					 Res_com = 0;
			
					 LED1=0;	//����DS1,��ʾ��������
				 
					 res=bmp_encode(pname,(lcddev.width-240)/2,(lcddev.height-320)/2,240,320,0);
					 Show_Str(40,130,240,12,"ͼƬ���������!",12,0);	
					 USART_SendData(USART2,0x32);
					 while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);//�ȴ����ͽ���				 
								
					BEEP=0;//�رշ�����
					LED1=1;//�ر�DS1
					delay_ms(180);//�ȴ�1.8����
					LCD_Clear(BLACK);
				}else delay_ms(5);
				camera_refresh();//������ʾ
				i++;
				if(i==40)//DS0��˸.
				{
					i=0;
					LED0=!LED0;
				}
			}	   								
		}
	}
}

