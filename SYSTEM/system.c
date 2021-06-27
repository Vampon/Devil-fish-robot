#include "system.h"
#include "delay.h"
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

uint8_t System_Init(void)
{
	
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);	 
    
    //��AFIOʱ��
    RCC_APB2PeriphClockCmd(	RCC_APB2Periph_AFIO, ENABLE );
	
		//���õδ�ʱ��1ms�ж�
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
    SysTick_Config(SystemCoreClock /(1000));
	  delay_init();
	
		//�������Pwm  50hz ->�����ʼ��
		Motor_PWM_Init(19999,71);
		Motor_PWM_Set(1900,1900,1900,1900);
		delay_ms(1500);
		Motor_PWM_Set(1500,1500,1500,1500);		
		delay_ms(1500);
	
		duoji_init();
	  uart_init(9600);
	
		while(DS18B20_Init())	//DS18B20��ʼ��	
		{
			delay_ms(200);
			delay_ms(200);
		}
			NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
			usmart_dev.init(72);		//��ʼ��USMART		
			LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
			KEY_Init();					//��ʼ������
			LCD_Init();			   		//��ʼ��LCD    
			BEEP_Init();        		//��������ʼ��	 
			W25QXX_Init();				//��ʼ��W25Q128
			my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ��
			exfuns_init();				//Ϊfatfs��ر��������ڴ�  
			f_mount(fs[0],"0:",1); 		//����SD�� 
			f_mount(fs[1],"1:",1); 		//����FLASH. 
			POINT_COLOR=RED; 		
	
		return 0;
}




