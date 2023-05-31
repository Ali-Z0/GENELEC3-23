/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.h

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

#ifndef _PRESS_ADC_H    /* Guard against multiple inclusion */
#define _PRESS_ADC_H


/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */
#include "app.h"
/* This section lists the other files that are included in this file.
 */

/* TODO:  Include other files here if needed. */


/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif


	/* ************************************************************************** */
	/* ************************************************************************** */
	/* Section: Constants                                                         */
	/* ************************************************************************** */
	/* ************************************************************************** */
	#define     ADC_V_GAIN          	1
	#define		ADC_V_REF				3.3
	#define		ADC_NB_BITS				10
	//#define     ADC_AN_SCAN_ADDRES  	0x0008

	#define 	ADC_CH_ON				1
	#define 	ADC_CH_OFF				0


	/* --------- CHANNELS SELECTION --------- */
	#define		ADC_AN0					ADC_CH_OFF
	#define		ADC_AN1					ADC_CH_OFF
	#define		ADC_AN2					ADC_CH_OFF
	#define		ADC_AN3					ADC_CH_ON
	#define		ADC_AN4					ADC_CH_OFF
	#define		ADC_AN5					ADC_CH_OFF
	#define		ADC_AN6					ADC_CH_OFF
	#define		ADC_AN7					ADC_CH_OFF
	#define		ADC_AN8					ADC_CH_OFF
	#define		ADC_AN9					ADC_CH_OFF
	#define		ADC_AN10				ADC_CH_OFF
	#define		ADC_AN11				ADC_CH_OFF
	#define		ADC_AN12				ADC_CH_OFF
	#define		ADC_AN13				ADC_CH_OFF
	#define		ADC_AN14				ADC_CH_OFF
	#define		ADC_AN15				ADC_CH_OFF
	/* ************************************************************************** */
    

    // *****************************************************************************
    // *****************************************************************************
    // Section: Data Types
    // *****************************************************************************
    // *****************************************************************************
    typedef struct{ 
        uint16_t AN0;
		uint16_t AN1;
		uint16_t AN2;
		uint16_t AN3;
		uint16_t AN4;
		uint16_t AN5;
    }S_ADCResults;
    // *****************************************************************************


    // *****************************************************************************
    // *****************************************************************************
    // Section: Interface Functions
    // *****************************************************************************
    // *****************************************************************************

    /* Convert voltage into pressure in [Bar] */
    float Press_voltageToPressure(float voltage);
    
    /* Convert raw adc value to voltage */
    float Press_RawToVoltage(float raw);
    
    void Press_InitADC (void);
    
    S_ADCResults Press_ReadAllADC( void );
    
    float Press_readPressure( void );
    // *****************************************************************************
    


    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _EXAMPLE_FILE_NAME_H */

/* *****************************************************************************
 End of File
 */
