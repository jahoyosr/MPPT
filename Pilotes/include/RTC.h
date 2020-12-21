//________________________________________________________________________
//
//  COMPATIBLE avec "stm32f10x.h" et "Clock.h" (lors du passage KEIL4.53) 
//________________________________________________________________________
//


#include "stm32f10x.h"

/******************************************************************************
* File Name          : RTC.h
* Author             : T Rocacher
* Version            : 0
* Date               : 10 Avril 2013
* Description        : Module qui configure la RTC
*                     
*******************************************************************************/



#ifndef _RTC_H__
#define _RTC_H__

//=======================================================================================
// Rappel sur les ressources RTC, Backup Domaine
// NB: L'utilisation de cette lib n'a de sens que si le CNT qui sert à compter les
// secondes écoulées est alimenté en permanence donc il faut :
//	- soit ne jamais couper l'alimentation générale (pas génial pour l'économie d'NRJ)
// 	- soit placer une batterie de 3V sur la broche Vbatt (elle permet d'alimenter le
//	le BKP domain ainsi que l'oscillateur 32768 Hz avec le CNT.
//	
//=======================================================================================

/*
Description:
Le module permet de connaître à tout moment la date actuelle. Les données sont stockées sous la forme d'une
structure, StructDate.
Le format est le suivant : 1 8 04 2013 22 10 32
Traduire                 Lun 8/04/2013 22h 10mn 32s
 
Principe :
Le module repose sur l'utilisation du périphérique RTC et du "périphérique" BKP (Back-Up Domain).
Le RTC est une sorte de Timer très simplifié dont on peut régler le prescaler, la provenance de l'horloge
l'autorisation d'IT... Le compteur est un 32 bit.
Le BKP est une zone du uC qui est potentiellement alimenté par une source locale Vbat lorsque VDD disparaît.
Le CNT du RTC est inclu dans le BKP, c'est ce qui fait que même alimentation coupée, le CNT peut s'incrémenter
(à la seconde par exemeple). Par ailleurs, on utilise des registres du BKP pour mémoriser une date absolue (date
que le programmeur inscrit lors d'une mise à l'heure). A l'instant où la mise à l'heure est faite (BKP register mis
à jour), le CNT du RTC est remis à 0.
De cette manière, à tout moment, on peut retrouver la date actuelle grâce à une date absolue mémorisée et au CNT
qui court depuis cette date.


_________________________________________________________________________________________
Pour les programmeurs de périph :

Parmi les registres qui sont mappés à l'interieur du BKP, nous trouvons outre des registres de stockage, certains
registres clé du RTC et du RCC:

BKP_DR1 à BKP_DR10 (pour le STM 103. Le 107 en contient bien plus). Ils sont utilisés pour mémoriser les champs de la date
absolue.

RTC_CNT, RTC_PRL. PRL est le prescaler qui permet de descendre l'horloge d'entrée du module vers l'horloge d'entrée
du compteur

Certains bits du regsitre BDCR (BackUp Domain Control Reg, appartenant au RCC, RCC->BDCR
Bits RTCSEL1|RTCSEL0 = 01 ce qui permet de choisir LSE comme source quartz, 32.768 kHz.
Bit LSEON qui active l'oscillateur à quartz
bit RTCEN qui active le module RTC.

D'une manière générale, l'écriture dans le BKP (donc les 3 bits cités, les CNT et PRL, les BKP_DRx) ne
peuvent être écrit que si les bits PWREN et BKPEN du registre APB1ENR (RCC) sont à 1, ainsi que le
bit PWR_CR_DBP du registre CR (periph PWR).

Par ailleurs, l'écriture des registres RTC CNT et PRL doit suivre une procédure précise (16.3.4 du RM008), voir
code source.


*/


//=======================================================================================
// Definition de type
//=======================================================================================



typedef struct
{
u8 Day_Name;	// 1 à 7 (Lun à Dim)
u8 Day;       // 1 à 31
u8 Month;     // 1 à 12
u16 Year;     // 2013 à 2050
u8 Hour;      // 0 à 23
u8 Min;	     	// 0 à 59
u8 Sec; 			// 0 à 59
}StructDate;




//=======================================================================================
// Fonctions
//=======================================================================================

char Mise_A_l_Heure(StructDate Nvelle_Date);
// Configure la RTC (prescaler, source LSE (32.768 kHz)
// Place la date en guise de référence absolue.
// Initialise le CNT à 0 secondes.
// Renvoie un code d'erreur :
// 0 : la date est OK
// 1 : le nom du jour est hors [1-7]
// 2 : le jour est hors [1-31]
// 3 : le mois est hors [1-12]
// 4 : l'année est hors [2013-2050]
// 5 : l'heure est hors [0-23]
// 6 : les minutes sont hors [0-59]
// 7 ! les secondes sont hors [0-59]
//
// Tout code d'erreur (à part 0 ) provoque une non initialisation de la RTC.

StructDate* Lire_Date(void);
// Renvoie la date actuelle, plus exactement l'adresse de la structure
// de type StrucDate qui contient la date mise à jour à la volée
// (à partir du CNT du RTC et de la date absolue, enregistrée lors de la dernière mise à l'heure).



void Conf_IT_RTC_Sec(char Prio_RTC,void (*IT_function) (void));
// A utiliser si l'on veut exécuter une tâche toutes les secondes.
// exemple d'utilisation : Conf_IT_RTC_Sec(14,Top);
// NB configurer l'horloge en tout premier (CLOCK_Configure())





#endif
