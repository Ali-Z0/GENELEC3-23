#ifndef Mc32GestI2CLM92_SM_H
#define Mc32GestI2CLM92_SM_H
//----------------------------------------------------------
// Mc32GestI2cLM92_SM.h
//----------------------------------------------------------
//	Description :	Gestion par I2C du capteur temperature LM92 du Kit
//                      Version State Machine
//                      Utilisation du plib Harmony 1_06
//	Auteur 		: 	C. Huber
//	Version		:	V1.6    10.05.2016
//	Compilateur	:	XC32 V1.40
//
//----------------------------------------------------------
//      Modifications :
//      28.05.2015  CHR remplacement typedef32.h par stdint.h et adaptations
//      10.05.2016  CHR maj version et mise en ordre des commentaire pour éviter des warnings
//      (SCA 11.04.2017 adaptation Harmony 1.08 (pas de modif de ce fichier))
//
// Principe utilisation :
// ----------------------
//
// a) déclarer un descripteur
//
// b) appeler I2C_LM92_SM_Init avec &Descripteur
//
// c) Appeler cycliquement (Cycle rapide nécessaire)
//    I2C_LM92_SM_Execute avec &Descripteur
//
// d) Obtention des résultat (Test dans cycle lents)
//    if (I2C_LM92_SM_IsReady(&pDescr) == true) {
//        // Obtention de la température
//        float Mytemp = I2C_LM92_SM_GetTemp(&Descr);
//        // Relance le traitement
//        I2C_LM92_SM_Restart(&Descr);
//    }
//----------------------------------------------------------

#include <stdint.h>

#include "Mc32_I2cUtil_SM.h"

// enumeration  Etat principal
typedef enum { LM92_SM_Idle, LM92_SM_Busy, LM92_SM_ready} E_LM92_state;

// enumeration  Etapes de la séquence
typedef enum { LM92_I2CSEQ_Idle,
               LM92_I2CSEQ_Start,
               LM92_I2CSEQ_WriteAddrW,
               LM92_I2CSEQ_WritePtrTemp,
               LM92_I2CSEQ_ReStart,
               LM92_I2CSEQ_WriteAddrR,
               LM92_I2CSEQ_ReadMsb,
               LM92_I2CSEQ_ReadLsb,
               LM92_I2CSEQ_Stop,
                                } E_LM92_Sequence;

// Descripteur LM92 pour traitement par machine d'état
typedef struct {
    // I2C_MODULE i2cModuleId;         // Id du Module I2C
    E_LM92_state Lm92state;         // Etat principal
    E_LM92_Sequence Lm92Sequence;   // Etapes de la séquence
    S_Descr_I2C_SM I2cSmInfo;       // Descripteur pour fonction I2C_SM
    uint8_t Lsb;
    uint8_t Msb;
    int16_t RawTemp;        // valeur brute registre températue
    int32_t TempMilli;      // température en millième de degré
    float Temperature;     // température en degré
} S_Descr_LM92_SM;

// Descripteur gestion LM92 par State Machine
extern S_Descr_LM92_SM DescrLm92;     // descripteur LM92

//--------------------------------------------------------
// Lm92SmInit
// Init pour appel extérieur
//--------------------------------------------------------

void Lm92SmInit(bool Fast);

// prototypes des fonctions
void I2C_LM92_SM_Init(S_Descr_LM92_SM *pDescr, bool Fast);
void I2C_LM92_SM_Execute(S_Descr_LM92_SM *pDescr);
void I2C_LM92_SM_Restart(S_Descr_LM92_SM *pDescr);
bool I2C_LM92_SM_IsReady(S_Descr_LM92_SM *pDescr);

float I2C_LM92_SM_GetTemp(S_Descr_LM92_SM *pDescr);
int32_t I2C_LM92_SM_GetTempMilli(S_Descr_LM92_SM *pDescr);
int16_t I2C_LM92_SM_GetRawTemp(S_Descr_LM92_SM *pDescr);

#endif
