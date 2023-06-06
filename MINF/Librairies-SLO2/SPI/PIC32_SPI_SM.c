/*******************************************************************************
 *	 ______   _______   __  __   _                 ______    _____ 
 *	|  ____| |__   __| |  \/  | | |               |  ____|  / ____|
 *	| |__       | |    | \  / | | |       ______  | |__    | (___  
 *	|  __|      | |    | |\/| | | |      |______| |  __|    \___ \ 
 *	| |____     | |    | |  | | | |____           | |____   ____) |
 *	|______|    |_|    |_|  |_| |______|          |______| |_____/                                                                                                              
 * 
 * @file PIC32_SPI_SM.c
 * @summary 
 * 
 * Handle SPI by state machine
 * 
 * @authors
 * Created  by Miguel Santos
 * Inspired by Caroline Miéville
 * Reviewed by Ali Zoubir
 * 
 * The official version is available at : 
 * https://github.com/Miguel-SLO/MINF-GENELEC3.git
 * 
 * @creation 20.05.2023
 * @lastmodif 20.05.2023
 * 
 ******************************************************************************/



#include "PIC32_SPI_SM.h"
#include "peripheral/spi/plib_spi.h"
#include "system/clk/sys_clk.h"

/*****************************************************************************/

/* Select which SPI to use */
#define SPI_ID SPI_ID_1

/* Select peripheral CS */
#define SPI_CS CS_LM70

/* Select SPI frequency to use */
#define SPI_FREQ 5000000

/* Dummy byte is used when reading only 
 * to shift peripheric internal register */
#define DUMMY_BYTE  0x81

/*****************************************************************************/

/* State machine variable */
SPI_STATE spiState = SPI_STATE_UNINITIALIZED;

/*****************************************************************************/

/**
 * SPI_Init
 * 
 * One time call when starting programm
 * Refer to #defines to select parameters
 */
void SPI_Init (void)
{
	PLIB_SPI_Disable(SPI_ID);
	PLIB_SPI_BufferClear(SPI_ID);
	PLIB_SPI_StopInIdleDisable(SPI_ID);
	PLIB_SPI_PinEnable(SPI_ID, SPI_PIN_DATA_OUT);
	PLIB_SPI_CommunicationWidthSelect(SPI_ID, SPI_COMMUNICATION_WIDTH_8BITS);
	PLIB_SPI_BaudRateSet(SPI_ID, SYS_CLK_PeripheralFrequencyGet(CLK_BUS_PERIPHERAL_1), SPI_FREQ);
	PLIB_SPI_InputSamplePhaseSelect(SPI_ID, SPI_INPUT_SAMPLING_PHASE_IN_MIDDLE );
	PLIB_SPI_ClockPolaritySelect(SPI_ID, SPI_CLOCK_POLARITY_IDLE_LOW);
	PLIB_SPI_OutputDataPhaseSelect(SPI_ID, SPI_OUTPUT_DATA_PHASE_ON_IDLE_TO_ACTIVE_CLOCK);
	PLIB_SPI_MasterEnable(SPI_ID);
	PLIB_SPI_FramedCommunicationDisable(SPI_ID);
	PLIB_SPI_FIFOEnable(SPI_ID);
	PLIB_SPI_Enable(SPI_ID);
  
	spiState = SPI_STATE_IDLE;
}

/*****************************************************************************/

/**
 * SPI_DoTasks
 * 
 * State machine handling
 * Should be call cyclically 
 */
void SPI_Tasks (void)
{
	switch(spiState)
	{
		case SPI_STATE_UNINITIALIZED :
			/* Use SPI_Init() at launch */
			break;
			
		case SPI_STATE_IDLE :
            /* Waiting for a start */
			break;		
			
		case SPI_STATE_BUSY_WRITE :
            
            if(!PLIB_SPI_IsBusy(SPI_ID))
            {
                SPI_CS = 1;
                spiState = SPI_STATE_IDLE;
            }            
			break;
            
		case SPI_STATE_BUSY_READ :	
		case SPI_STATE_BUSY_READ_WRITE :
            
            if(!PLIB_SPI_IsBusy(SPI_ID) && !PLIB_SPI_ReceiverFIFOIsEmpty(SPI_ID))
            {
                SPI_CS = 1;
                spiState = SPI_STATE_IDLE_READ_DATA_AVAILABLE;
            }            
			break;
			
		case SPI_STATE_IDLE_READ_DATA_AVAILABLE :
            
            /* Use SPI_ReadByte to get datas in FIFO */
            if (PLIB_SPI_ReceiverFIFOIsEmpty(SPI_ID))
                spiState = SPI_STATE_IDLE;
			break;			
	} 
}

/*****************************************************************************/

/**
 * SPI_StartRead
 * 
 * Write one or multiple dummy bytes
 * to receive datas back to be read
 * 
 * @param nBytes : Number of bytes to write
 */
void SPI_StartRead (uint8_t nBytes)
{
    /* Watchdog */
    if(spiState == SPI_STATE_IDLE)
    {
        uint8_t iData = 0;

        SPI_CS = 0;

        for( iData = 0 ; iData < nBytes ; iData++ )
            PLIB_SPI_BufferWrite(SPI_ID, DUMMY_BYTE);	

        spiState = SPI_STATE_BUSY_READ;
    }   
}

/*****************************************************************************/

/**
 * SPI_StartWrite
 * 
 * Write one or multiple bytes by SPI
 * datas received back are not considered
 * 
 * @param nBytes : Number of bytes to write
 * @param pBytesToWrite : Pointer to datas to write
 */
void SPI_StartWrite(uint8_t nBytes, uint8_t* pBytesToWrite)
{
    /* Watchdog */
    if(spiState == SPI_STATE_IDLE)
    {
        uint8_t iData = 0;

        SPI_CS = 0;

        for( iData = 0 ; iData < nBytes ; iData++ )
            PLIB_SPI_BufferWrite(SPI_ID, *(pBytesToWrite + iData));	

        spiState = SPI_STATE_BUSY_WRITE;
    }
}

/*****************************************************************************/

/**
 * SPI_StartReadWrite
 * 
 * Simultaneous write and read
 * 
 * @param nBytes : Number of bytes to write
 * @param pBytesToWrite : Pointer to datas to write
 */
void SPI_StartReadWrite(uint8_t nBytes, uint8_t* pBytesToWrite)
{
    /* Watchdog */
    if(spiState == SPI_STATE_IDLE)
    {
        uint8_t iData = 0;

        SPI_CS = 0;
        
        for( iData = 0 ; iData < nBytes ; iData++ )
            PLIB_SPI_BufferWrite(SPI_ID, *(pBytesToWrite + iData));	

        spiState = SPI_STATE_BUSY_READ_WRITE;
    }
}

/*****************************************************************************/

/**
 * SPI_GetState
 * @return Current state of SPI state machine
 */
SPI_STATE SPI_GetState (void)
{
    return spiState; 
}

/*****************************************************************************/

/**
 * SPI_UpdateState
 * 
 * Update state of SPI SM
 * 
 * @param SPI_STATE NewState
 */
void SPI_UpdateState (SPI_STATE NewState)
{
    spiState = NewState; 
}

/*****************************************************************************/

/**
 * SPI_ReadByte
 * 
 * Get the first byte in the SPI buffer
 * 
 * @return Byte in reception buffer
 */
uint8_t SPI_ReadByte (void)
{
    return PLIB_SPI_BufferRead(SPI_ID); 
}

/*****************************************************************************/
