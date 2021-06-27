#ifndef _SYSTEM_H
#define _SYSTEM_H
#include "stm32f10x.h"
#include "stdio.h"
#include "math.h"
#include "motor.h"
#include "duoji.h"
#include "task.h"
#include "usart.h"	
#include "delay.h"
#include "ds18b20.h"




uint8_t System_Init(void);
extern uint8_t key;
extern uint16_t v;
extern u8 temperature;




#endif




