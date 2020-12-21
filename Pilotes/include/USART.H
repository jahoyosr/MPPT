#include "stm32f10x.h"

//=======================================================================================
// 	Auteur T.R.
//  Juin 2016 , 28/06
//  Processeur : STM32F103RB
//  Logiciel dev : µVision 4.70
// 
// Modif 28/06 ajout fonctionnalité IT en transmission str :
// void Init_IT_Serial_Transmit_Str(USART_TypeDef *USART, void (*IT_function) (void))
//=======================================================================================


#ifndef _USART_H__
#define _USART_H__



/* ======================================================================================
**************** EMISSION : PRINCIPE fct Put_String *****************************************
Chaine à transmettre = azerty.
En dur : 0x61 0x7A 0x65 0x72 0x74 0x79 0x0 ; Le 0 traduit la fin de la chaîne. C'est universel.

La fonction Put_String se contente d'émettre  0x61 0x7A 0x65 0x72 0x74 0x79
Elle aura détecté le \0 et ne l'aura pas émis. Ou si \0 n'existe pas, alors
la transmission s'arrêtera lors d'un dépassement max (voir define)
AUCUN CARACTERE DE CONTROLE N EST EMIS EN PLUS.

Pour être dans les clous et envoyer selon l'OS un ou plusieurs caractères de contrôle:
0x61 0x7A 0x65 0x72 0x74 0x79 \n \r  (\n = LF = 0xA et \r = CR = 0xD)
ce sera à la couche service de dessus d'envoyer deux chaînes :
0x61 0x7A 0x65 0x72 0x74 0x79 0x0
Puis
0x0A 0x0D 0x0


**************** RECEPTION *****************************************
Tous les protocoles finissent par CR mais sait-on jamais... MAC OS en \n ?
La détection du dernier caractère sera donc au choix.
Exemple :
0x61 0x7A 0x65 0x72 0x74 0x79 \n \r  donnera 
0x61 0x7A 0x65 0x72 0x74 0x79 \n \0 en dur
Eventuellement en couche service on pourra ou non supprimer \n si le protocole place
en fin de chaîne \n \r

OPTION EN RECEPTION:
le mode ECHO permet de renvoyer tous les caractères traités.

======================================================================================
*/




//===============================ressources sur le STM32F103RB===========================
// Rappel sur les ressources sur le STM32F103RB 
//
// 3 USART possibles :
// USART_1_TX = TIM1_CH2 = PA9
// USART_1_RX = TIM1_CH3 = PA10
// USART_2_TX = TIM2_CH3 = PA2
// USART_2_RX = TIM2_CH4 = PA3
// USART_3_TX = PB10
// USART_3_RX = PB11
//=======================================================================================


/*  TO DO...

Faire les 2 autres usart
Faire un bilan des tests.
Mettre en place la DMA émission.
Ajouter une fonction Etat Error USART (over run, bruit.)
*/

//===================================INFOS GENRALES======================================
/*
Voir fin de fichier pour le détail des fonctions


Le module permet d'envoyer et recevoir octet / string sur USART 1 à 3

Tout est géré par IT USART enfouie, priorité à donner.
Aucun test bloquant avant d'émettre est prévu (sauf flag matériel qui temporisent juste à l'appel
d'un Put_Char ou Put_String). Au user de faire attention.
De nombreux flags permettent de savoir si il y a une opération transmit / receive en cours

Voici typiquement comment on devrait écrire un put string :
	while(Flag_Transmission_Busy(USART1)==1);	
	Put_String(USART1,"Commande recue");
Ce qui garantit que la COM est libre pour envoyer.
Si le polling n'est pas fait, c'est le bourrage garantit, notamment en cas de full duplex.

Possibilité de mettre en place un callback à la réception d'un byte ou d'un string, sur les 3 USART.

ATTENTION :
Si les callback sont utilisés, il est primoridal de ne pas y rester pour ne pas bloquer l'IT USART enfouie.
A plus forte raison, il est INTERDIT dans un callback d'appeler une fct em/rec USART car l'IT USART s'interrompt => plantage.
CONSEIL => utiliser cette fonctionnalité que pour récupérer un flag ou une donnée à la volée,y rester le moins possible.
Mieux => ne pas utiliser les callback mais utiliser un polling régulier sur l'appli globale, genre OS.


*/
//=======================================================================================

//_________ USER DEFINE _________________________________________________________________

// Taille du buffer d'émission (réglable pour chaque USART)
#define Max_Car_In_String_Emission_1 500
#define Max_Car_In_String_Emission_2 500
#define Max_Car_In_String_Emission_3 500
// Taille du buffer de réception (réglable pour chaque USART)
#define Buf_Len_StrRec_USART1 30
#define Buf_Len_StrRec_USART2 30
#define Buf_Len_StrRec_USART3 30

// Caractère de terminaison, par défaut \r = 0xD :
#define Term_Car_USART1 0x0D
#define Term_Car_USART2 0x0D
#define Term_Car_USART3 0x0D

// Mode ECHO lors de la réception
#define ECHO_USART1
#define ECHO_USART2
#define ECHO_USART3


#define Null 0
#define ClearScreen 0xC
#define LF 0xA
#define CR 0xD
//=======================================================================================
//                      Configuration  
//=======================================================================================

void Init_Serial_Single_Mode(USART_TypeDef *USART,int Baud_Rate_bits_par_Sec, char Prio_USART); // OK
/*___________________________________________________________________________________________

Permet de préparer n'importe laquelle des 3 USART avec un débit donné (8bits, 1 stop, pas de parité)
Utilise une IT enfouie qui permet la transmission en IT et la réception en IT.
___________________________________________________________________________________________*/


void Init_IT_Serial_Receive_Byte(USART_TypeDef *USART, void (*IT_function) (void));
void Init_IT_Serial_Receive_Str(USART_TypeDef *USART, void (*IT_function) (void));
void Init_IT_Serial_Transmit_Str(USART_TypeDef *USART, void (*IT_function) (void));
/*___________________________________________________________________________________________

Ces trois fonctions permettent de renvoyer au user une IT pour indiquer l'événement de réception 
Byte ou String.
___________________________________________________________________________________________*/

void Reset_Var_USART(USART_TypeDef *USART); // OK
/*___________________________________________________________________________________________

Ne devrait pas être utilisée... Elle permet de remettre à 0 toutes les variables 
privées utilisée pour l'USART indiquée
___________________________________________________________________________________________*/



//=======================================================================================
// 							  flag 
//=======================================================================================
char Flag_Transmission_Busy(USART_TypeDef *USART); // OK
char Flag_Overrun_Transmit(USART_TypeDef *USART);
void Clear_Overrun_Transmit(USART_TypeDef *USART);
/*___________________________________________________________________________________________

Gestion du flag Flag_Transmission_Busy :
Géré automatiquement :
- mis à un automatiquement à l'appel des deux fonctions put
- mis à 0 automatiquement lors de la fin des processus put

Gestion du flag Flag_Overrun_Transmit :
- mis à un automatiquement à l'appel des deux fonctions put, si une transmission est en cours
- mis à 0 par la fonction RESET_VAR_USART, plus pertinent, pas la fonction VOID CLEAR_OVERRUN_TRANSMIT
___________________________________________________________________________________________*/

char Flag_Str_Received(USART_TypeDef *USART); // OK
char Flag_Byte_Received(USART_TypeDef *USART); //OK
char Flag_Overrun_Receive_Str(USART_TypeDef *USART);
void Clear_Overrun_Receive_Str(USART_TypeDef *USART);

/*___________________________________________________________________________________________

Gestion des Flag de réception :
Ils sont remis à 0 soit au reset général, soit lors du get
le flag_Byte_Received passe à 1 à chaque réception de byte.
Le flag_Str_Received passe à 1 en réception dernier caractère.


Le flag FLAG_OVERRUN_RECEIVE_STR
Mis à 1 en fin de réception string si le flag Str received était encore à 1.
Remis à 0 au reset général et par la fonction CLEAR_OVERRUN_RECEIVE_STR

___________________________________________________________________________________________*/


//=======================================================================================
// 							  Utilisation, rem : // OK signifie validé en reel serial 1
//=======================================================================================

char Get_Char(USART_TypeDef *USART); //OK
char * Get_String(USART_TypeDef *USART); // OK
int Get_Len_Receiv_String(USART_TypeDef *USART); //OK


void Put_Char(USART_TypeDef *USART,char Car); // OK
void Put_String(USART_TypeDef *USART,char * pCar); // OK
int Get_Len_Transm_String(USART_TypeDef *USART); //OK

/*
Les fonctions d'émission
=======================================================================================
La fonction PUT_STRING s'arrête sous deux conditions et ajoute un caractère de fin (voir define .h):
- rencontre du caractère null
ou
- atteinte du nombre max de caractères émis (voir define).
Elle utilise l'IT USART enfouie
La fonction  Put_Char n'utilise aucune IT. L'émission est directe.


INT GET_LEN_TRANSM_STRING(USART_TYPEDEF *USART);
Elle permet de récupérer si besoin la longueur du caractère émis (caractère null non compris).
!ATTENTION ! Cette longueur est mise à jour lorsque la transmission est finie. Il faut donc
insérer une attente fin de transmission :
	while(Flag_Transmission_Busy(USART1)==1);
	LEM=Get_Len_Transm_String(USART1);	

Les fonctions de réception
=======================================================================================
Deux fonctions :
CHAR GET_CHAR(USART_TYPEDEF *USART);
CHAR * GET_STRING(USART_TYPEDEF *USART);

Les deux exploitent l'interruption enfouie USART, en concurrence avec la transmission.
Les drapeaux FLAG_BYTE_RECEIVED et FLAG_STR_RECEIVED passent à 1 pour indiquer que les données
sont prêtes à être lues.
Si les données sont écrasées le flag FLAG_OVERRUN_RECEIVE passe à 1.

*/





#endif
