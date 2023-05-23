/*******************************************************************************
 *  _______  ___________  ___  ___   __               _______      _______ 
 * |   ____||           ||   \/   | |  |             |   ____|    /       |
 * |  |__   `---|  |----`|  \  /  | |  |      ______ |  |__      |   (----`
 * |   __|      |  |     |  |\/|  | |  |     |______||   __|      \   \    
 * |  |____     |  |     |  |  |  | |  `----.        |  |____ .----)   |   
 * |_______|    |__|     |__|  |__| |_______|        |_______||_______/                                                      
 * 
 * @file PIC32_I2C_SM.c
 * @summary 
 * 
 * Handle I2C by state machine
 * 
 * @authors
 * 
 * The official version is available at : 
 * https://github.com/Miguel-SLO/MINF-GENELEC3.git
 * Only peoples mentionned in Readme.md contributed to this code
 * 
 * @creation 20.05.2023
 * @lastmodif 20.05.2023
 * 
 ******************************************************************************/

#include "app.h"
#include "peripheral\i2c\plib_i2c.h"
#include "PIC32_I2C_SM.h"


I2C_STATE i2cState = I2C_STATE_UNINITIALIZED;

void I2C_Init( S_I2C* pI2C )
{
    PLIB_I2C_Disable(pI2C->I2C_ID);
    
    PLIB_I2C_HighFrequencyEnable(pI2C->I2C_ID);

    PLIB_I2C_BaudRateSet(pI2C->I2C_ID,
        SYS_CLK_PeripheralFrequencyGet(CLK_BUS_PERIPHERAL_1), pI2C->SPEED);
    
    PLIB_I2C_SlaveClockStretchingEnable(pI2C->I2C_ID);
    
    PLIB_I2C_Enable(pI2C->I2C_ID);
}

void I2C_Tasks( S_I2C* pI2C )
{
    switch(pI2C->STATE)
    {
        /*--------------------------------------------------------------------*/
        case I2C_STATE_UNINITIALIZED:
        
            break;
        
        /*--------------------------------------------------------------------*/
        case I2C_STATE_IDLE:
            
            /* Waiting for an action ! */            
            break;
        
        /*--------------------------------------------------------------------*/
        case I2C_STATE_START:
            
            if( PLIB_I2C_BusIsIdle(pI2C->I2C_ID) )
                PLIB_I2C_MasterStart(pI2C->I2C_ID); 
            
            if( PLIB_I2C_StartWasDetected(pI2C->I2C_ID) )
                pI2C->STATE = I2C_STATE_IDLE;

            break;
        
        /*--------------------------------------------------------------------*/
        case I2C_STATE_STOP:
            
            switch(pI2C->SEQUENCE)
            {
                case I2C_SEQUENCE_IDLE:
                    if( PLIB_I2C_BusIsIdle(pI2C->I2C_ID) )
                    {
                        PLIB_I2C_MasterStop(pI2C->I2C_ID);
                        
                        pI2C->SEQUENCE = I2C_SEQUENCE_COMPLETE;
                    }
                    break;

                case I2C_SEQUENCE_COMPLETE:
                    if( PLIB_I2C_StopWasDetected(pI2C->I2C_ID) )
                    {
                        /* Reset */
                        pI2C->STATE = I2C_STATE_IDLE;
                        pI2C->SEQUENCE = I2C_SEQUENCE_IDLE;
                    }  
                    break;
                    
                default:
                    break;
            }            
            break;
        
        /*--------------------------------------------------------------------*/
        case I2C_STATE_READ:
            
            switch(pI2C->SEQUENCE)
            {
                case I2C_SEQUENCE_IDLE:
                    break;
                    
                case I2C_SEQUENCE_READY:
                    break;
                    
                case I2C_SEQUENCE_SEND:
                    break;
                    
                case I2C_SEQUENCE_BUSY:
                    break;
                    
                case I2C_SEQUENCE_COMPLETE:
                    break;
            }            
            break;
            
        /*--------------------------------------------------------------------*/
        case I2C_STATE_WRITE:
            
            switch(pI2C->SEQUENCE)
            {
                case I2C_SEQUENCE_IDLE:
                    break;
                    
                case I2C_SEQUENCE_READY:
                    break;
                    
                case I2C_SEQUENCE_SEND:
                    break;
                    
                case I2C_SEQUENCE_BUSY:
                    break;
                    
                case I2C_SEQUENCE_COMPLETE:
                    break;
            }            
            break;
            
    }
}

void I2C_Start ( void )
{
    
}
void I2C_Stop ( void )
{
    
}
void I2C_Read ( void )
{
    
}
void I2C_Write ( void )
{
    
}
