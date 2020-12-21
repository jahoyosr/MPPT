

#ifndef _MPPT_H__
#define _MPPT_H__
#include "stm32f10x.h"
#include "Toolbox_NRJ_V4.h"

// Duty Cycle of the PWM
#define D(val) TIM1->CCR1=((val)*Resolution)>>12

// Active Status LEDs
#define LED_1_on GPIOB->BSRR=(0x01<<5)
#define LED_2_on GPIOB->BSRR=(0x01<<10)

//Deactive Status LEDs
#define LED_1_off GPIOB->BRR=(0x01<<5)
#define LED_2_off GPIOB->BRR=(0x01<<10)

// This function configure the interuption from which we are going to read the ADCs
void Configure_IT(void (*IT_function)(void), float Periode_IT_us );
//This function configure the ADC
void Confugure_ADC(void);
//This function initalise all the GPIO port that we are going to use on this project
void Initialise_Olimex(void);


void IT_Timer2(void);
void IT_Timer4(void);
void Configure_Timer(void);

int Sensor_1(void); 
int Sensor_2(void); 
int Sensor_3(void); 
int Sensor_4(void); 

float Input_Voltage(void);
float Input_Current(void);
float Output_Voltage(void);
float Output_Current(void);

void Carrying_Triangle(float F_PWM_KHZ_Val);
void Carrying_UpRamp(float F_PWM_KHZ_Val);
void wait(volatile uint32_t cnt);
void Activate_PWM(void);
void Deactivate_PWM(void);


float Input_Voltage(void);
float Input_Current(void);
float Output_Voltage(void);
float Output_Current(void);
uint8_t ParseMessage(void);

#endif
