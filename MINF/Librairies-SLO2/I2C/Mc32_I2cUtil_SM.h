#ifndef MC32_I2CUTIL_SM_H
#define MC32_I2CUTIL_SM_H
//--------------------------------------------------------
//	Mc32_I2cUtilCCS.h
//--------------------------------------------------------
//	Description :	utilitaire I2C avec State Machine
//                      conserve la découpe des fonctions CCS like
//
//	Auteur 		: 	C. HUBER
//      Date            :       24.04.2015
//	Version		:	V1.6
//	Compilateur	:	XC32 V1.40
//      plib            :       Harmony 1_06
//
// Modifications :
//      CHR   24.04.2015 utilisation stdint.h
//                       suppression du I2C module ID dans les paramères
//      CHR   10.05.2016 mise à jour version compilateur et Harmony
//      (SCA 11.04.2017 adaptation Harmony 1.08 (pas de modif de ce fichier))
/*--------------------------------------------------------*/

#include <stdbool.h>
#include <stdint.h>

// KIT 32MX795F512L Constants
#define KIT_I2C_BUS   I2C_ID_2    // n'est plus utilisée

typedef enum { I2C_XSM_Idle, I2C_XSM_Busy, I2C_XSM_Ready} E_I2C_XSM;

typedef struct {
   E_I2C_XSM I2cMainSM;
   int I2cSeqSM;
   int DebugCode;
} S_Descr_I2C_SM;

bool I2C_SM_isReady( S_Descr_I2C_SM *pDSM );
void I2C_SM_begin( S_Descr_I2C_SM *pDSM );

void I2C_SM_init(  bool Fast, S_Descr_I2C_SM *pDSM );
void I2C_SM_start(S_Descr_I2C_SM *pDSM);
void I2C_SM_reStart( S_Descr_I2C_SM *pDSM);
void I2C_SM_write( S_Descr_I2C_SM *pDSM, uint8_t data, bool *pAck );
void I2C_SM_read( S_Descr_I2C_SM *pDSM, bool ackTodo, uint8_t *pData );
void I2C_SM_stop(S_Descr_I2C_SM *pDSM);

#endif
