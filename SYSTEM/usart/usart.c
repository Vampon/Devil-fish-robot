#include "system.h"
#include "usart.h"	  

#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	  

void uart_init(u32 bound){
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
  
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
   
  //USART1_RX	  GPIOA.10��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  

  //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx |USART_Mode_Tx ;	//�շ�ģʽ

  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1 

}

/*�ò���Ϊ����ov7670ͼƬ���жϳ��򣬻�û���ҵ����õķ��������ע�͵���ֹ��ͻ*/
u8 Res_com;
//void USART1_IRQHandler(void)                	//����1�жϷ������
//	{
//	
//#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
//	OSIntEnter();    
//#endif
//	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
//		{
//	Res_com =USART_ReceiveData(USART1);	//��ȡ���յ�������
//		
//   		 
//     } 
//#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
//	OSIntExit();  											 
//#endif
//} 
//#endif	
u8 Res_com2;
//void USART2_IRQHandler(void)                	//����2�жϷ������
//	{
//	

//	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
//		{
//	Res_com2 =USART_ReceiveData(USART2);	//��ȡ���յ�������
//		
//   		 
//     } 

//} 



/*
FA (A1 A2 A3 A4 A5 A6) V8 V8 FB

*/
uint16_t V;
uint8_t  Mode[6];
uint8_t buf[10];
void USART1_IRQHandler(void)        //����1�жϷ������
{
	static uint8_t step=0;
	
	uint8_t Res;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
  {

		Res =USART_ReceiveData(USART1);	//��ȡ���յ�������
		switch(step)
		{
			case 0:
				if(Res==0xFA)
				{
					buf[0]=Res;	
					step=1;
				}
				else
				{
					step=0;
				}
				break;
			case 1:
				buf[1]=Res;				//A1
				step=2;				
				break;
			case 2:
				buf[2]=Res;				//V8
				step=3;	
				break;
			case 3:
				buf[3]=Res;				//V8+V8
				step=4;	
				break;
			case 4:
				buf[3]=Res;      //0x01 ��ͨ���� 0x02 ����ģʽ
				step=4;	
				break;
			case 5:            //֡β ��λ���ò���δ����
				buf[5]=Res;
				if(buf[5]==0xFB)
				{
					mode_update(buf[1]);
					V=(buf[2]<<8)|buf[3];
				}
				step=0;	
				break;
			default:
				step=0;
				break;
		}
		
	}


}
#endif	




