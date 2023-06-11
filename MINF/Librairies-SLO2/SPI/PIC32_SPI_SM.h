/*******************************************************************************
 *  _______  ___________  ___  ___   __               _______      _______ 
 * |   ____||           ||   \/   | |  |             |   ____|    /       |
 * |  |__   `---|  |----`|  \  /  | |  |      ______ |  |__      |   (----`
 * |   __|      |  |     |  |\/|  | |  |     |______||   __|      \   \    
 * |  |____     |  |     |  |  |  | |  `----.        |  |____ .----)   |   
 * |_______|    |__|     |__|  |__| |_______|        |_______||_______/                                                      
 * 
 * @file PIC32_SPI_SM.h
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

#ifndef PIC32_SPI_SM_H
#define PIC32_SPI_SM_H

#include <stdint.h>

/* SPI STATE MACHINE */
typedef enum
{
	SPI_STATE_UNINITIALIZED,
    SPI_STATE_IDLE,
    SPI_STATE_IDLE_READ_DATA_AVAILABLE,
    SPI_STATE_BUSY_WRITE,        
    SPI_STATE_BUSY_READ_WRITE,
    SPI_STATE_BUSY_READ,
} SPI_STATE;

/**
 * SPI_Init
 * 
 * One time call when starting programm
 * Refer to #defines to select parameters
 */
void SPI_Init(void);

/**
 * SPI_DoTasks
 * 
 * State machine handling
 * Should be call cyclically 
 */
void SPI_Tasks(void);

/**
 * SPI_StartRead
 * 
 * Write one or multiple dummy bytes
 * to receive datas back to be read
 * 
 * @param nBytes : Number of bytes to write
 */
void SPI_StartRead(uint8_t nBytes);

/**
 * SPI_StartWrite
 * 
 * Write one or multiple bytes by SPI
 * datas received back are not considered
 * 
 * @param nBytes : Number of bytes to write
 * @param pBytesToWrite : Pointer to datas to write
 */
void SPI_StartWrite(uint8_t nBytes, uint8_t* pBytesToWrite);

/**
 * SPI_StartReadWrite
 * 
 * Simultaneous write and read
 * 
 * @param nBytes : Number of bytes to write
 * @param pBytesToWrite : Pointer to datas to write
 */
void SPI_StartReadWrite(uint8_t nBytes, uint8_t* pBytesToWrite);

/**
 * SPI_GetState
 * @return Current state of SPI state machine
 */
SPI_STATE SPI_GetState (void);

/**
 * SPI_UpdateState
 * 
 * Update state of SPI SM
 * 
 * @param NewState
 */
void SPI_UpdateState(SPI_STATE NewState);

/**
 * SPI_ReadByte
 * 
 * Get the first byte in the SPI buffer 
 * 
 * @return Byte in reception buffer
 */
uint8_t SPI_ReadByte(void);

#endif /* PIC32_SPI_SM_H */
