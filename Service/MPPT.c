
#include "Clock.h"
#include "stm32f10x.h"
#include "Timer_1234.h"
#include "GPIO.h"
#include "Timer_Systick.h"
#include "NVIC_IT_Ext.h"
#include "ADC_DMA.h"
#include "Toolbox_NRJ_V4.h"
#include "Timer_1234.h"
#include "MPPT.h"


/*List of the inputs and outputs
Inputs
	ADC
	PA0 - From a sensor (1/4) Tension d'entrée
	PA1 - From a sensor (2/4) Courant d'entrée
	PA2 - From a sensor (3/4)	Tension de sortie
	PA3 - From a sensor (4/4) Courant de sortie

	PWM
	PA8 - To control the mosfet driver

	Digital 
	PC6 - Enable received from the MMS subsystem

Outputs
	PC7 - Control the switch to be disconnected from the inverter subsystem 
	Undefined - Status Led 1
	Undefined - Status Led 2
*/

// Triangle carrying PWM - CMS bits in TIMx_CR1 to 11
#define PWM_Edge_Aligned(Timer) Timer->CR1=Timer->CR1&~(3<<5)
#define PWM_Center_Aligned(Timer) Timer->CR1=Timer->CR1|(3<<5)

// Choix de la fréquence PWM (en kHz)
#define FPWM_Khz 100.0

void Configure_IT(void (*IT_function)(void), float Periode_IT_us ){
	Systick_Prio_IT(1,IT_function); // 0 to 15 the 0 has the highest priority
	Systick_Period(Periode_IT_us);
	SysTick_Enable_IT;
	SysTick_On;
}

void Configure_ADC(void){
	Init_TimingADC_ActiveADC( ADC1, 0.0); 
	Single_Channel_ADC(ADC1, 1);
}
void IT_Timer3(void){
	
}
void IT_Timer4(void){
	
}
void Configure_Timer(void){
	Timer_1234_Init(TIM3,10.0); // 10 mesuré us 100kHz 
	Timer_1234_Init(TIM4,1000.0); // 1 mesuré ms 1kHz
	//Active_IT_Debordement_Timer(TIM3,5,IT_Timer3);
	//Active_IT_Debordement_Timer(TIM4,10,IT_Timer4);	
}



int Sensor_1(void){
	// Specif de la voie ADC
	ADC1->SQR3=(ADC1->SQR3) & ~0x001F;
	ADC1->SQR3=(ADC1->SQR3) | 0; 
	Start_ADC_Conversion(ADC1);
	// tant que EOC=0 rien
	while (((ADC1->SR)& 0x2) == 0){}
	ADC1->SR=(ADC1->SR)&~(0x1<<1); // mise à 0 du flag
	
	return (ADC1->DR);
	}

int Sensor_2(void){
	// Specif de la voie ADC
	ADC1->SQR3=(ADC1->SQR3) & ~0x001F;
	ADC1->SQR3=(ADC1->SQR3) | 1; 
	Start_ADC_Conversion(ADC1);
	// tant que EOC=0 rien
	while (((ADC1->SR)& 0x2) == 0){}
	ADC1->SR=(ADC1->SR)&~(0x1<<1); // mise à 0 du flag
	return (ADC1->DR);
}

int Sensor_3(void){
	// Specif de la voie ADC
	ADC1->SQR3=(ADC1->SQR3) & ~0x001F;
	ADC1->SQR3=(ADC1->SQR3) | 2; 
	Start_ADC_Conversion(ADC1);
	// tant que EOC=0 rien
	while (((ADC1->SR)& 0x2) == 0){}
	ADC1->SR=(ADC1->SR)&~(0x1<<1); // mise à 0 du flag
	return (ADC1->DR);
}

int Sensor_4(void) {
	// Specif de la voie ADC
	ADC1->SQR3=(ADC1->SQR3) & ~0x001F;
	ADC1->SQR3=(ADC1->SQR3) | 3; 
	Start_ADC_Conversion(ADC1);
	// tant que EOC=0 rien
	while (((ADC1->SR)& 0x2) == 0){}
	ADC1->SR=(ADC1->SR)&~(0x1<<1); // mise à 0 du flag
	return (ADC1->DR);
}

void Carrying_Triangle(float F_PWM_KHZ_Val) {
	Resolution=PWM_Init(TIM1, 1, 2*F_PWM_KHZ_Val); 
	Bloque_Timer(TIM1);
	CNT(TIM1)=0;	
	CptUp(TIM1);
	PWM_Center_Aligned(TIM1);
}

void Carrying_UpRamp(float F_PWM_KHZ_Val){
	Resolution=PWM_Init(TIM1, 1, F_PWM_KHZ_Val); 
	Bloque_Timer(TIM1);
	CNT(TIM1)=0;
	CptUp(TIM1);
	PWM_Edge_Aligned(TIM1);
}

void Activate_PWM(void){	
	(RCC->APB2ENR)=(RCC->APB2ENR) | RCC_APB2ENR_TIM1EN;	
	//Config OCy en sortie, implicite mais bon :
	TIM1->CCMR1 = (TIM1->CCMR1)& ~(0x03<<0);	//00 ds le champs CCyS
	//PWM en mode Edge aligned OCyM=110
	TIM1->CCMR1 = (TIM1->CCMR1)& ~(0x07<<4);
	TIM1->CCMR1 = (TIM1->CCMR1)|(0x06<<4);	
	// polarité positive bit CCyP = 0, reg CCER
	TIM1->CCER = (TIM1->CCER)& ~(0x01<<1);
	// validation du canal y, bit CCyE, reg CCER
	TIM1->CCER = (TIM1->CCER)|(0x01<<0);
}

void Deactivate_PWM(void){
	(RCC->APB2ENR)=(RCC->APB2ENR) | RCC_APB2ENR_TIM1EN;	
	// validation du canal y, bit CCyE, reg CCER
	TIM1->CCER = (TIM1->CCER)&~(0x01<<0);
}
float Input_Voltage(){
	int adc=Sensor_1();
	return ((adc/4096.0)*3.3)*(50.3/3.3);
}

float Input_Current(){
	int adc= Sensor_2();
	return ((((adc*3.3/4096.0)-0.375)/4.25)*(40.8))-2.16;
}

float Output_Voltage(){
	int adc = Sensor_3();
	return ((adc/4096.0)*3.3)*(52.3/5.6);
}

float Output_Current(){
	int adc = Sensor_4();
	return ((((adc*3.3/4096.0)-0.375)/4.25)*(40.8))-2.16;
}

void Initialise_Olimex(void){
	//Inputs
		//Configure the ADCs pins
		GPIO_Configure(GPIOA, 0, INPUT, ANALOG); // PA0
		GPIO_Configure(GPIOA, 1, INPUT, ANALOG); // PA1	
		GPIO_Configure(GPIOA, 2, INPUT, ANALOG); // PA2
		GPIO_Configure(GPIOA, 3, INPUT, ANALOG); // PA3
		
		//Configure the PWM pin
		GPIO_Configure(GPIOA, 8, OUTPUT, ALT_PPULL );	// PA8
	
		//Configure the ShutDown Driver pin
		GPIO_Configure(GPIOA, 5, OUTPUT, OUTPUT_PPULL );	// PA5
		
		//Configure the Enable pin
		GPIO_Configure(GPIOC, 6, INPUT, INPUT_FLOATING); // PC6
	
	//Outputs
		//Configure the Switch pin
		GPIO_Configure(GPIOC, 7, OUTPUT, OUTPUT_PPULL); // PC7
		//Configure the Led Pins
		GPIO_Configure(GPIOB, 5, OUTPUT, OUTPUT_PPULL); // PB5
		GPIO_Configure(GPIOB, 10, OUTPUT, OUTPUT_PPULL); // PB10
	
		Configure_ADC();
		Configure_Timer();	
		// Conf PWM  
		Bras_1_On;
		Carrying_Triangle(FPWM_Khz);
		Activate_PWM();	
		Start_PWM;
}
uint8_t ParseMessage(){
	int x=0;
	return x;
}
