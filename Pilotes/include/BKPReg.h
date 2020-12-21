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
* ATTENTION          : Si le RTC est utilisé (RTC.c), il risque d'y avoir imcompatibilité
											 Le RTC utilise DR1 à DR7 => il reste plus que DR8, DR9 et DR10 pour le STM103 (10 reg 16 bits)
											 Si plus de registres nécessaires => ne pas utiliser le RTC, ou utiliser le STM107 qui contient 42 reg 16 bits.
*******************************************************************************/



#ifndef _BKPREG_H__
#define _BKPREG_H__

//=======================================================================================
// Rappel sur le Backup Domaine : mémorise des mots de 16 bits grâce à la tension de secours Vbat
//	
//=======================================================================================

/*


_________________________________________________________________________________________
Pour les programmeurs de périph :

D'une manière générale, l'écriture dans le BKP (les BKP_DRx) ne
peuvent être écrit que si les bits PWREN et BKPEN du registre APB1ENR (RCC) sont à 1, ainsi que le
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

// Macro d'écriture dans les DR :
#define Write_DR(DR,Value)	BKP->DR = (Value)

// Exemple : écrire dans les deux registre DR2 et DR3 les valeurs 30 et 45:
//  Init_Write_DR();
//  Write_DR(DR2,30);
//  Write_DR(DR3,45);

#endif
