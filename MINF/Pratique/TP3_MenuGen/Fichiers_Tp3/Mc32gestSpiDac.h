/*--------------------------------------------------------*/
// Mc32GestSpiDac.h
/*--------------------------------------------------------*/
//	Description :	Gestion DAC Spi du Kit (LTC2604)
//
//	Auteur 		: 	C. Huber
//	Version		:	V1.2
//	Compilateur	:	XC32 V1.40 + Harmony 1.06
//
//  Modifications :
//   CHR 06.03.2015  ajout fonction SPI_CfgWriteToDac
//   CHR 24.05.2016  maj version compilateur et Harmony 
//
/*--------------------------------------------------------*/

#ifndef Mc32GestSpiDac_H
#define Mc32GestSpiDac_H

#include <stdint.h>

// Prototypes des fonctions
void SPI_InitLTC2604(void);
void SPI_WriteToDac(uint8_t Noch, uint16_t DacVal);
void SPI_CfgWriteToDac(uint8_t NoCh, uint16_t DacVal);


#endif //Mc32GestSpiDac_H
