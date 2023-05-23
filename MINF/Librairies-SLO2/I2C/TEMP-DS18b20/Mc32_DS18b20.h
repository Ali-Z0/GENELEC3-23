#ifndef MC32_DS18B20_H
#define MC32_DS18B20_H

//--------------------------------------------------------
//	Mc32_DS18B20.h
//--------------------------------------------------------
//	Description :	Fonctions pour DS18B20
//                      ( en utilisant le DS2482-100 )
//	Auteur 		: 	C. HUBER
//      Date            :       22.05.2014
//	Version		:	V1.0
//	Compilateur	:	XC32 V1.31
// Modifications :
//
/*--------------------------------------------------------*/

#include <stdbool.h>
#include <stdint.h>

void init_oneWire(void);
// modif du passage de paramètres
void ReadDS18B20(uint8_t *Status, float *pTemp);

#endif

 

