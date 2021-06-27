#include "task.h"


//�����ٶ�������0-500֮��

void mode_control(uint8_t key,uint16_t v)
{
	switch(key)
	{
		case 0://�ϸ�
			//duoji_up();
			Motor_PWM_Set(1500+v,1500+v,1500+v,1500+v);
			break;
		case 1://��Ǳ
			//duoji_up();
			Motor_PWM_Set(1500-v,1500-v,1500-v,1500-v);
			break;
		case 2://ǰ��
			//duoji_front();
			Motor_PWM_Set(1500+v,1500+v,1500+v,1500+v);
			break;
		case 3://����
			//duoji_front();
			Motor_PWM_Set(1500+v,1500+v,1500-v,1500-v);
			break;
		case 4://��ת
			//duoji_front();
			Motor_PWM_Set(1500+v,1500+v,1500-v,1500+v);
			break;
		case 5://��ת
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
	if(mode==0x01) //����״̬
	{
		key=6;
	}
	if(mode==0x02) //����״̬
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



