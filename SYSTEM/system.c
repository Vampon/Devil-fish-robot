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
    
    //打开AFIO时钟
    RCC_APB2PeriphClockCmd(	RCC_APB2Periph_AFIO, ENABLE );
	
		//配置滴答定时器1ms中断
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
    SysTick_Config(SystemCoreClock /(1000));
	  delay_init();
	
		//电机控制Pwm  50hz ->电机初始化
		Motor_PWM_Init(19999,71);
		Motor_PWM_Set(1900,1900,1900,1900);
		delay_ms(1500);
		Motor_PWM_Set(1500,1500,1500,1500);		
		delay_ms(1500);
	
		duoji_init();
	  uart_init(9600);
	
		while(DS18B20_Init())	//DS18B20初始化	
		{
			delay_ms(200);
			delay_ms(200);
		}
			NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
			usmart_dev.init(72);		//初始化USMART		
			LED_Init();		  			//初始化与LED连接的硬件接口
			KEY_Init();					//初始化按键
			LCD_Init();			   		//初始化LCD    
			BEEP_Init();        		//蜂鸣器初始化	 
			W25QXX_Init();				//初始化W25Q128
			my_mem_init(SRAMIN);		//初始化内部内存池
			exfuns_init();				//为fatfs相关变量申请内存  
			f_mount(fs[0],"0:",1); 		//挂载SD卡 
			f_mount(fs[1],"1:",1); 		//挂载FLASH. 
			POINT_COLOR=RED; 		
	
		return 0;
}




