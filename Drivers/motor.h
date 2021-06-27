#ifndef __SJANT_MOTOR_H
#define __SJANT_MOTOR_H
#include "system.h"

void Motor_PWM_Init(u16 arr,u16 psc);
void Motor_PWM_Set(int16_t motor1 , int16_t motor2 , int16_t motor3 , int16_t motor4);

extern int16_t Motor_PWM[4];
extern int16_t Last_PWM[4];
#endif
