#ifndef Mc32GestI2CLM92_H
#define Mc32GestI2CLM92_H

/*--------------------------------------------------------*/
// Mc32GestI2cLM92.h
/*--------------------------------------------------------*/
//	Description :	Gestion par I2C du capteur temperature LM92 du Kit
//
//	Auteur 		: 	C. Huber
//	Version		:	V1.2    12.04.2016
//	Compilateur	:	XC32 V1.40 et Harmony 1.06
//
/*--------------------------------------------------------*/

#include <stdint.h>

// prototypes des fonctions
void I2C_InitLM92(void);
void I2C_WriteConfigLM92(void);
int16_t I2C_ReadRawTempLM92(void);
int16_t I2C_WriteCfgReadRawTempLM92(void);
// Passage par référence car problème avec return lors affectation
// a une variable globale dans réponse à interruption
void LM92_ConvRawToDeg( int16_t RowTemp, float *pTemp);

#endif
