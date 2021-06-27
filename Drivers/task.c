#include "task.h"


//串口速度数据在0-500之间

void mode_control(uint8_t key,uint16_t v)
{
	switch(key)
	{
		case 0://上浮
			//duoji_up();
			Motor_PWM_Set(1500+v,1500+v,1500+v,1500+v);
			break;
		case 1://下潜
			//duoji_up();
			Motor_PWM_Set(1500-v,1500-v,1500-v,1500-v);
			break;
		case 2://前进
			//duoji_front();
			Motor_PWM_Set(1500+v,1500+v,1500+v,1500+v);
			break;
		case 3://后退
			//duoji_front();
			Motor_PWM_Set(1500+v,1500+v,1500-v,1500-v);
			break;
		case 4://左转
			//duoji_front();
			Motor_PWM_Set(1500+v,1500+v,1500-v,1500+v);
			break;
		case 5://右转
			//duoji_front();
			Motor_PWM_Set(1500+v,1500+v,1500+v,1500-v);
			break;		
		default:
			break;
	}

}

//u16 V_control(int v)
//{

//	

//}


void mode_update(uint8_t mode)
{
	if(mode==0xA1)
	{
		key=0;
	}
	if(mode==0xA2)
	{
		key=1;
	}
	if(mode==0xA3)
	{
		key=2;
	}
	if(mode==0xA4)
	{
		key=3;
	}
	if(mode==0xA5)
	{
		key=4;
	}
	if(mode==0xA6)
	{
		key=5;
	}
	if(mode==0x01) //控制状态
	{
		key=6;
	}
	if(mode==0x02) //拍照状态
	{
		key=7;
	}

}


u8 TxBuffer[3];

void Send_to_PC(void)
{
	int i;
//	TxBuffer[0]=0XFA;
	TxBuffer[0]=(u8)temperature;
//	TxBuffer[2]=0XFB;

	for(i=0;i<1;i++)
	{
		USART_SendData(USART1, TxBuffer[i]);
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);
	}
}



