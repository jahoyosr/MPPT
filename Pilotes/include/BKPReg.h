//________________________________________________________________________
//
//  COMPATIBLE avec "stm32f10x.h" et "Clock.h" (lors du passage KEIL4.53) 
//________________________________________________________________________
//


#include "stm32f10x.h"

/******************************************************************************
* File Name          : BKPReg.h
* Author             : T Rocacher
* Version            : 0
* Date               : 14/09/2016
* Description        : Module qui configure les backup reg.
* ATTENTION          : Si le RTC est utilis� (RTC.c), il risque d'y avoir imcompatibilit�
											 Le RTC utilise DR1 � DR7 => il reste plus que DR8, DR9 et DR10 pour le STM103 (10 reg 16 bits)
											 Si plus de registres n�cessaires => ne pas utiliser le RTC, ou utiliser le STM107 qui contient 42 reg 16 bits.
*******************************************************************************/



#ifndef _BKPREG_H__
#define _BKPREG_H__

//=======================================================================================
// Rappel sur le Backup Domaine : m�morise des mots de 16 bits gr�ce � la tension de secours Vbat
//	
//=======================================================================================

/*


_________________________________________________________________________________________
Pour les programmeurs de p�riph :

D'une mani�re g�n�rale, l'�criture dans le BKP (les BKP_DRx) ne
peuvent �tre �crit que si les bits PWREN et BKPEN du registre APB1ENR (RCC) sont � 1, ainsi que le
bit PWR_CR_DBP du registre CR (periph PWR).




*/


//=======================================================================================
// Definition de type
//=======================================================================================







//=======================================================================================
// Fonctions
//=======================================================================================

void Reset_BKP_Domain(void);
void Init_Write_DR(void);

// Macro d'�criture dans les DR :
#define Write_DR(DR,Value)	BKP->DR = (Value)

// Exemple : �crire dans les deux registre DR2 et DR3 les valeurs 30 et 45:
//  Init_Write_DR();
//  Write_DR(DR2,30);
//  Write_DR(DR3,45);

#endif
