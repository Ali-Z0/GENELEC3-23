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

    #define     ADC_RES                 (3.3/1024.0)

	#define 	ADC_CH_ON				1
	#define 	ADC_CH_OFF				0

	/* --------- CHANNELS SELECTION --------- */
	#define		ADC_AN0					ADC_CH_OFF
	#define		ADC_AN1					ADC_CH_ON
	#define		ADC_AN2					ADC_CH_ON
	#define		ADC_AN3					ADC_CH_OFF
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

    #define		ADC_N_CHANNELS	(ADC_AN0 + ADC_AN1 + ADC_AN2 + ADC_AN3 + ADC_AN4 + ADC_AN5 + ADC_AN6 + ADC_AN7 + ADC_AN8 + ADC_AN9 + ADC_AN10 + ADC_AN11 + ADC_AN12 + ADC_AN13 + ADC_AN14 + ADC_AN15)
	/* ************************************************************************** */
    

    // *****************************************************************************
    // *****************************************************************************
    // Section: Data Types
    // *****************************************************************************
    // *****************************************************************************
    typedef struct{ 
        union {
            struct {
            #if(ADC_AN0 == ADC_CH_ON)
                uint16_t AN0;
            #endif
            #if(ADC_AN1 == ADC_CH_ON)
                uint16_t AN1;
            #endif
            #if(ADC_AN2 == ADC_CH_ON)
                uint16_t AN2;
            #endif
            #if(ADC_AN3 == ADC_CH_ON)
                uint16_t AN3;
            #endif
            #if(ADC_AN4 == ADC_CH_ON)
                uint16_t AN4;
            #endif
            #if(ADC_AN5 == ADC_CH_ON)
                uint16_t AN5;
            #endif
            #if(ADC_AN6 == ADC_CH_ON)
                uint16_t AN6;
            #endif
            #if(ADC_AN7 == ADC_CH_ON)
                uint16_t AN7;
            #endif
            #if(ADC_AN8 == ADC_CH_ON)
                uint16_t AN8;
            #endif
            #if(ADC_AN9 == ADC_CH_ON)
                uint16_t AN9;
            #endif
            #if(ADC_AN10 == ADC_CH_ON)
                uint16_t AN10;
            #endif
            #if(ADC_AN11 == ADC_CH_ON)
                uint16_t AN11;
            #endif
            #if(ADC_AN12 == ADC_CH_ON)
                uint16_t AN12;
            #endif
            #if(ADC_AN13 == ADC_CH_ON)
                uint16_t AN13;
            #endif
            #if(ADC_AN14 == ADC_CH_ON)
                uint16_t AN14;
            #endif
            #if(ADC_AN15 == ADC_CH_ON)
                uint16_t AN15;
            #endif
            }channels;
            
            uint16_t key[ADC_N_CHANNELS];
        }params;
    }S_ADCResults;
    // *****************************************************************************


    // *****************************************************************************
    // *****************************************************************************
    // Section: Interface Functions
    // *****************************************************************************
    // *****************************************************************************

    /* Convert voltage into pressure in [Bar] */
//    float Press_voltageToPressure(float voltage);
    
    /* Convert raw adc value to voltage */
//    float Press_RawToVoltage(float raw);
    
    void ADC_Init (void);
    
    S_ADCResults ADC_ReadAll( void ) ;
    
//    float Press_readPressure( void );
    // *****************************************************************************
    


    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _EXAMPLE_FILE_NAME_H */

/* *****************************************************************************
 End of File
 */
