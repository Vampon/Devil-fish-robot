#include "system.h"
#include "usart.h"	  

#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	  

void uart_init(u32 bound){
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
  
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
   
  //USART1_RX	  GPIOA.10初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  

  //Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx |USART_Mode_Tx ;	//收发模式

  USART_Init(USART1, &USART_InitStructure); //初始化串口1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(USART1, ENABLE);                    //使能串口1 

}

/*该部分为传输ov7670图片的中断程序，还没有找到更好的方案因此先注释掉防止冲突*/
u8 Res_com;
//void USART1_IRQHandler(void)                	//串口1中断服务程序
//	{
//	
//#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
//	OSIntEnter();    
//#endif
//	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
//		{
//	Res_com =USART_ReceiveData(USART1);	//读取接收到的数据
//		
//   		 
//     } 
//#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
//	OSIntExit();  											 
//#endif
//} 
//#endif	
u8 Res_com2;
//void USART2_IRQHandler(void)                	//串口2中断服务程序
//	{
//	

//	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
//		{
//	Res_com2 =USART_ReceiveData(USART2);	//读取接收到的数据
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
void USART1_IRQHandler(void)        //串口1中断服务程序
{
	static uint8_t step=0;
	
	uint8_t Res;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
  {

		Res =USART_ReceiveData(USART1);	//读取接收到的数据
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
				buf[3]=Res;      //0x01 普通控制 0x02 拍照模式
				step=4;	
				break;
			case 5:            //帧尾 上位机该部分未调整
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




