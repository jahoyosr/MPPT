
/*
STRUCTURE DES FICHIERS

COUCHE APPLI : 
Main_User.c : programme principal à modifier. Par défaut hacheur sur entrée +/-10V, sortie 1 PWM

COUCHE SERVICE :
MPPT.c : contient toutes les fonctions utilisables, qui "parlent" à un électrotechnicien

COUCHE DRIVER :
clock.c : contient LA fonction Clock_Configure() qui prépare le STM32. Lancée automatiquement à l'init IO
lib : bibliothèque qui gère les périphériques du STM : Drivers_STM32F103_107_Jan_2015_b
*/

/*
VOIR LE FICHIER MPPT.H POUR TOUTES INFOS SUR LES E/S
*/


#include "MPPT.h"
#include "GPIO.h"


//=================================================================================================================
// 					USER DEFINE
//=================================================================================================================

//float F,F1; //Hz
//#define K (6.0)//3.5 OK mais attention au courant absorbé ...

// Choix de la fréquence d'execution de l'algo MPPT(en kHz)
#define Fe_Khz 5.0

// Taille du vecteur de moyenne
#define FILTER_SIZE 200


//==========END USER DEFINE========================================================================================

// ========= Variable globales indispensables et déclarations fct d'IT ============================================
#define Te_us (1000.0/Fe_Khz) // Te_us est la période d'échantillonnage en float, exprimée en µs
void IT_Principale(void);
void delay_ms(uint32_t);
//=================================================================================================================

float V_in=0.0; // Tension d'entrée
float I_in=0.0; // Courant d'entrée
float V_out=0.0;	// Tension de sortie
float I_out=0.0; // Courant de sortie

float V_in_aux = 0.0;
float I_in_aux = 0.0;

//float Alpha;
//uint8_t CAN_Message=0; // Il y a un message sur le BUS CAN?
//uint8_t mode=0; // Mode choisi par le MSS

int duty_cycle;
int Daux;

float P_in;

float Dprev = 200; //0.7 de 0 à 1
float Vprev = 40;
float Pprev = 100;

//float Vavg = 0.0;
//float Vaux = 0.0;
//float Iavg = 0.0;
//float Iaux = 0.0;

int deltaD = 40; //0.0025 de 0 à 1
//int deltaD_ini = 100;

//float Pmax = 0.0;
//float Dmax = 0.0;

int i = 0;
int lock_MPPT = 0;
uint8_t waiting=0;
uint32_t del_ms=1000;
int alpha = 20;
int deltaAlpha = 10;
float Vtable[380];
float Ptable[380];
float Itable[380];
float Duty[380];
int tableIndex;


int main (void)
{
// !INITIALISATION! //	
		Initialise_Olimex(); // Initialiser péripheriques
		//_________ Ecrire ici toutes les CONFIGURATIONS des périphériques ________________________________
		// Conf IT
		Configure_IT(IT_Principale, Te_us);	
		
		Daux = Dprev;
		//D(2048);
		
		//wait(500);
	
		while(1){
			// Attendez jusqu'à que la moyennage soit prête
		/*	lock_MPPT = 0;
			while(!lock_MPPT);
			
			//Vpv = Vavg*3.3*7.59/4095.0;
			//Vcourant = Iavg*2.04/4095.0+1.48;
			//Ipv = 9.6*(Vcourant-2.5)*1.12;			
			P_in=V_in*I_in;
			
			
			// Table
//			if(alpha < 4000){
//				tableIndex=alpha/10-2;
//				Vtable[tableIndex] = V_in;
//				Ptable[tableIndex] = P_in;
//				Itable[tableIndex] = I_in;
//				Duty[tableIndex] = duty_cycle;
//				alpha += deltaAlpha;
//				duty_cycle = alpha;
//				
//			} else {
//				wait(25);
//	
//				if ((P_in-Pprev) > 0) {
//					if ((V_in-Vprev) > 0)
//							Daux -= deltaD;
//					else
//							Daux += deltaD;
//				} else {
//					if ((V_in-Vprev) > 0)
//							Daux += deltaD;
//					else
//							Daux -= deltaD;
//				}		
//				
//				Vprev = V_in;
//				Pprev = P_in;
//				
//				if (Daux < 204)
//					Daux = 204;
//				else if (Daux > 3900)
//					Daux = 3900;
//				else
//					Daux = Daux;
//				
//				duty_cycle = Daux;			
//				
//			}			
//			wait(5);
//		
//		}
	}
}





//=================================================================================================================
// 					FONCTION D'INTERRUPTION PRINCIPALE SYSTICK
//=================================================================================================================

void IT_Principale(void){
	
 // AJOUTER UNE LIMITATION A 5% / 95%, soit 204/3900 pour la PWM
 // Executer algo MPPT
	//V_in=Input_Voltage();
	//I_in=Input_Current();
	//V_out=Output_Voltage();
	//I_out=Output_Current();
	//Delay
	/*if(waiting){
		
	}else{*/		
	//D(duty_cycle);
	D(2048);	
	if(lock_MPPT == 0){
		V_in_aux += Input_Voltage();
		I_in_aux += Input_Current();
		i++;
		if(i == FILTER_SIZE){
			V_in = V_in_aux/FILTER_SIZE;
			I_in = I_in_aux/FILTER_SIZE;
			V_in_aux=0;
			I_in_aux=0;
			i=0;
			lock_MPPT = 1;
		}
	}
}	



void wait(uint32_t ms) {
	//waiting=1;
	volatile uint32_t cnt=6161*ms;
    while(cnt--)
        __NOP();
	//waiting=0;
}


