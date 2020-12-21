//________________________________________________________________________
//
//  COMPATIBLE avec "stm32f10x.h" et "Clock.h" (lors du passage KEIL4.53) 
//________________________________________________________________________
//


//=======================================================================================
// 	Auteur T.Rocacher
//  revu Mars 2011
//  Processeur : STM32F103RB
//  Logiciel dev : �Vision 4
//  Module qui configure le NVIC et les IT externe
//
//=======================================================================================

//=======================================================================================

// Modif TR :
// 09/01 D�cembre 2015
// Ajout de la macro qui permet de donner baisser le flag d'IT ext
// #define Clear_Flag_IT_Ext_5_15(Voie) (EXTI->PR)|(0x01<<Voie)



#include "stm32f10x.h"
#ifndef _NVIC_IT_EXT_H__
#define _NVIC_IT_EXT_H__

void NVIC_Enable_IT(char NumeroIT);
/*
Permet de valider une ligne d'interruption, hors coeur, de 0 � 63. Le num�ro 
est pass� en param�tre.
*/
void NVIC_Disable_IT(char NumeroIT);
/*
Permet d'invalider une ligne d'interruption, hors coeur, de 0 � 63. Le num�ro 
est pass� en param�tre.
*/
void NVIC_Prio_IT(char NumeroIT, char Prio);
/*
Fixe la priorit� de la ligne d'interruption sp�cifi�e.
*/



char NVIC_Ext_IT (GPIO_TypeDef * Port, char Broche, char Front, char Techno, char Prio, void (*IT_function) (void));
// Definitions :
// Front
#define FALLING_EGDE 0
#define RISING_EDGE 1
#define FALLING_RISING_EDGE 2

/*
place la broche du port indiqu�e en entree IT sur front up/down/les deux
La fonction est "cl� en main" pour configurer l'IT sur une broche externe 
voie 0 � 4, port A, B ou C)
pour les autres voies, 5 � 15, c'est un peu plus complexe.
Retourne 1 si erreur, 0 sinon

Le user sp�cifie :
 - port
 - num�ro de broche
 - front actif qui d�clenche l'IT
 - Techno de la broche (flottant, pull down...)
 - Niveau de priorit�
 - nom de la fonction � lancer lors de l'IT
 Tout est param�tr�, jusqu'au NVIC.  Le flag d'IT est mis � 0 automatiquement

 Exemple de code :
NVIC_Ext_IT (GPIOA, 2, RISING_EDGE, INPUT_FLOATING,8,Cligno);
 La broche GPIOA.2 provoque au front montant une IT qui lance la fct Cligno(), avec
 une priorit� de 8. L'entr�e est flottante.


 Pour les broches, de 5 � 15, le STM32 n'utilise que 2 vecteurs d'IT.
 C'est pourquoi, contrairement aux broches 0 � 4, c'est au user de chercher
 le flag lev�, et de le remettre � 0.
 Ce flag, c'est le bit de m�me rang que la broche, du registre EXTI->PR

 Exemple, remise � 0 du flag correspondant � la broche 10:
 Clear_Flag_IT_Ext_5_15(10)

*/

//============== MODIF TR 09 Janvier 2015==================
#define Clear_Flag_IT_Ext_5_15(Voie) EXTI->PR=(EXTI->PR)|(0x01<<Voie)
//============== FIN MODIF TR 09 Janvier 2015==============

#endif
