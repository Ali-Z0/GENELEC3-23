/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.c

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */
#include "app.h"
#include <math.h>
#include "Mc32_Adc.h"
//#include "peripheral/adc/plib_adc.h"
#include "system_config/default/framework/driver/adc/drv_adc_static.h"

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Macros						                                      */
/* ************************************************************************** */
/* ************************************************************************** */
#define getAdcRes()	(ADC_V_REF / pow(2, ADC_NB_BITS))
/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: File Scope or Global Data                                         */
/* ************************************************************************** */
/* ************************************************************************** */
const uint16_t a_AdcChannels[16] = {ADC_AN0, ADC_AN1, ADC_AN2, ADC_AN3, ADC_AN4, ADC_AN5, ADC_AN6,
						  ADC_AN7, ADC_AN8, ADC_AN9, ADC_AN10, ADC_AN11, ADC_AN12, ADC_AN13,
						  ADC_AN14, ADC_AN15};
/* ************************************************************************** */



/* ************************************************************************** */
/* ************************************************************************** */
// Section: Local Functions                                                   */
/* ************************************************************************** */
/* ************************************************************************** */
//static float ADC_RawToVoltage (float raw){
//    float voltage = 0;
//    /* Raw ADC to voltage */
//    voltage = raw * ADC_RES;
//    /* Voltage before op-amp */
//    voltage = voltage / getAdcRes();
//    return voltage;    
//}

static uint16_t ADC_GetScanAdress (void) {
	uint16_t scanAdress = 0;
	uint16_t i = 0;
	for (i=0; i<15; i++)
	{
		scanAdress |= a_AdcChannels[i] << i;
	}
	return scanAdress;
}

static uint16_t ADC_GetNChannels (void) {
	uint16_t numberOfChannels = 0;
    uint16_t i = 0;
    
    
	for (i=0; i<15; i++)
	{
		numberOfChannels += a_AdcChannels[i];
	}
	return numberOfChannels;
}

static uint16_t ADC_GetSamplesPerCh (void) {
    
	uint16_t NumberOfChannels = 0;
    DRV_ADC_SAMPLES_PER_INTERRUPT Samples_per_interrupt;
    
    NumberOfChannels = ADC_GetNChannels();
    
    if(NumberOfChannels == 1)
        Samples_per_interrupt = DRV_ADC_1SAMPLE_PER_INTERRUPT;
    else if (NumberOfChannels == 2)
        Samples_per_interrupt = ADC_2SAMPLES_PER_INTERRUPT;
    else if (NumberOfChannels == 3)
        Samples_per_interrupt = ADC_3SAMPLES_PER_INTERRUPT;
     else if (NumberOfChannels == 4)
        Samples_per_interrupt = ADC_4SAMPLES_PER_INTERRUPT;   
    else if (NumberOfChannels == 5)
        Samples_per_interrupt = ADC_5SAMPLES_PER_INTERRUPT;
    else if (NumberOfChannels == 6)
        Samples_per_interrupt = ADC_6SAMPLES_PER_INTERRUPT;
    else if (NumberOfChannels == 7)
        Samples_per_interrupt = ADC_7SAMPLES_PER_INTERRUPT;
    else if (NumberOfChannels == 8)
        Samples_per_interrupt = ADC_8SAMPLES_PER_INTERRUPT; 
    else if (NumberOfChannels == 9)
        Samples_per_interrupt = ADC_9SAMPLES_PER_INTERRUPT;
    else if (NumberOfChannels == 10)
        Samples_per_interrupt = ADC_10SAMPLES_PER_INTERRUPT;
    else if (NumberOfChannels == 11)
        Samples_per_interrupt = ADC_11SAMPLES_PER_INTERRUPT;   
    else if (NumberOfChannels == 12)
        Samples_per_interrupt = ADC_12SAMPLES_PER_INTERRUPT;
    else if (NumberOfChannels == 13)
        Samples_per_interrupt = ADC_13SAMPLES_PER_INTERRUPT;
    else if (NumberOfChannels == 14)
        Samples_per_interrupt = ADC_14SAMPLES_PER_INTERRUPT;
    else if (NumberOfChannels == 15)
        Samples_per_interrupt = ADC_15SAMPLES_PER_INTERRUPT; 
    else if (NumberOfChannels == 16)
        Samples_per_interrupt = ADC_16SAMPLES_PER_INTERRUPT;
    
	return Samples_per_interrupt;
}

/* ************************************************************************** */


/* ************************************************************************** */
/* ************************************************************************** */
// Section: Interface Functions                                               */
/* ************************************************************************** */
/* ************************************************************************** */

/*  A brief description of a section can be given directly below the section
    banner.
 */

// *****************************************************************************

void ADC_Init (void){
    uint16_t NumberOfChannels = 0;
    NumberOfChannels = ADC_GetNChannels();
    
    if(NumberOfChannels > 0)
    {
        //Configuration de l'adresse choisi ADC
        PLIB_ADC_InputScanMaskAdd(ADC_ID_1, ADC_GetScanAdress());
        // Configure l'ADC en mode alterné
        PLIB_ADC_ResultFormatSelect(ADC_ID_1, ADC_RESULT_FORMAT_INTEGER_16BIT);
        //Choisir ce mode -> Buffer alterné
        PLIB_ADC_ResultBufferModeSelect(ADC_ID_1, ADC_BUFFER_MODE_TWO_8WORD_BUFFERS);
        //mode multiplexage
        PLIB_ADC_SamplingModeSelect(ADC_ID_1, ADC_SAMPLING_MODE_MUXA);

        //la lecture des ADC est cadensée par le timer interne
        PLIB_ADC_ConversionTriggerSourceSelect(ADC_ID_1, ADC_CONVERSION_TRIGGER_INTERNAL_COUNT);
        //Tension de réference de l'ADC alimentation 3V3
        PLIB_ADC_VoltageReferenceSelect(ADC_ID_1, ADC_REFERENCE_VDD_TO_AVSS);
        PLIB_ADC_SampleAcquisitionTimeSet(ADC_ID_1, 0x1F);
        PLIB_ADC_ConversionClockSet(ADC_ID_1, SYS_CLK_FREQ, 32);

        //ADC fait 3 mesures par interruption (car 3 canaux utilisés) -> adapter en fct des ADC utilisés
        PLIB_ADC_SamplesPerInterruptSelect(ADC_ID_1, ADC_GetSamplesPerCh());
        //active le scan en mode multiplexage des entrées AN
        PLIB_ADC_MuxAInputScanEnable(ADC_ID_1);

        // Enable the ADC module
        PLIB_ADC_Enable(ADC_ID_1);
    }
}

S_ADCResults ADC_ReadAll( void ) {
    //structure de valeurs brutes des ADCs
    volatile S_ADCResults rawResult;
    // Traitement buffer
    ADC_RESULT_BUF_STATUS BufStatus;
    //stop sample/convert
    PLIB_ADC_SampleAutoStartDisable(ADC_ID_1);
    // traitement avec buffer alterné
    BufStatus = PLIB_ADC_ResultBufferStatusGet(ADC_ID_1);
    //Buffer 8 bits -> 0 à 7 -> expliqué après
    if (BufStatus == ADC_FILLING_BUF_0TO7) {
        rawResult.params.key[0] = PLIB_ADC_ResultGetByIndex(ADC_ID_1, 0);
        rawResult.params.key[1] = PLIB_ADC_ResultGetByIndex(ADC_ID_1, 1);
        rawResult.params.key[2] = PLIB_ADC_ResultGetByIndex(ADC_ID_1, 2);
        rawResult.params.key[3] = PLIB_ADC_ResultGetByIndex(ADC_ID_1, 3);
        rawResult.params.key[4] = PLIB_ADC_ResultGetByIndex(ADC_ID_1, 4);
        rawResult.params.key[5] = PLIB_ADC_ResultGetByIndex(ADC_ID_1, 5);
        rawResult.params.key[6] = PLIB_ADC_ResultGetByIndex(ADC_ID_1, 6);
        rawResult.params.key[7] = PLIB_ADC_ResultGetByIndex(ADC_ID_1, 7);
    }
    else //Buffer 8 bits -> 8 à 15
    {
        rawResult.params.key[0] = PLIB_ADC_ResultGetByIndex(ADC_ID_1, 8);
        rawResult.params.key[1] = PLIB_ADC_ResultGetByIndex(ADC_ID_1, 9);
        rawResult.params.key[2] = PLIB_ADC_ResultGetByIndex(ADC_ID_1, 10);
        rawResult.params.key[3] = PLIB_ADC_ResultGetByIndex(ADC_ID_1, 11);
        rawResult.params.key[4] = PLIB_ADC_ResultGetByIndex(ADC_ID_1, 12);
        rawResult.params.key[5] = PLIB_ADC_ResultGetByIndex(ADC_ID_1, 13);
        rawResult.params.key[6] = PLIB_ADC_ResultGetByIndex(ADC_ID_1, 14);
        rawResult.params.key[7] = PLIB_ADC_ResultGetByIndex(ADC_ID_1, 15);
    }
    // Retablit Auto start sampling
    PLIB_ADC_SampleAutoStartEnable(ADC_ID_1);

    //retourner valeurs lue
    return rawResult;
}

//float ADC_ReadVoltage ( void ) {
//    //structure de valeurs brutes des ADCs
//    volatile S_ADCResults rawResult;
//    /* Voltage variable */
//    float voltage = 0;
//    /* Pressure variable */
//    float pressure = 0;
//    /* Read ADC */
//    rawResult = Press_ReadAllADC();
//    /* Convert raw data to voltage */
//    voltage = Press_RawToVoltage(rawResult.AN3);
//    /* Get the pressure from the voltage */
//    pressure = Press_voltageToPressure(voltage);
//    
//    return pressure;
//}


/* *****************************************************************************
 End of File
 */
