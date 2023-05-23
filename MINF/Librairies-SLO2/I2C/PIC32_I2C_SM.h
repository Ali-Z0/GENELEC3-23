/*******************************************************************************
 *  _______  ___________  ___  ___   __               _______      _______ 
 * |   ____||           ||   \/   | |  |             |   ____|    /       |
 * |  |__   `---|  |----`|  \  /  | |  |      ______ |  |__      |   (----`
 * |   __|      |  |     |  |\/|  | |  |     |______||   __|      \   \    
 * |  |____     |  |     |  |  |  | |  `----.        |  |____ .----)   |   
 * |_______|    |__|     |__|  |__| |_______|        |_______||_______/                                                      
 * 
 * @file PIC32_I2C_SM.h
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

#ifndef PIC32_I2C_SM_H
#define PIC32_I2C_SM_H

#define I2C_CLK_FAST 400000
#define I2C_CLK_SLOW 100000

typedef enum {
    I2C_STATE_UNINITIALIZED,
    I2C_STATE_IDLE,
    I2C_STATE_START,
    I2C_STATE_STOP,        
    I2C_STATE_READ,
    I2C_STATE_WRITE,
} I2C_STATE;

typedef enum {
    I2C_SEQUENCE_IDLE,
    I2C_SEQUENCE_READY,
    I2C_SEQUENCE_SEND,
    I2C_SEQUENCE_BUSY,
    I2C_SEQUENCE_COMPLETE,
} I2C_SEQUENCE;

typedef struct {
    I2C_MODULE_ID   I2C_ID;
    I2C_STATE       STATE;
    I2C_SEQUENCE    SEQUENCE;
    uint32_t        SPEED;
}S_I2C;

void I2C_Init( S_I2C* pI2C );
void I2C_Tasks( S_I2C* pI2C );
void I2C_Start ( void );
void I2C_Stop ( void );
void I2C_Read ( void );
void I2C_Write ( void );

#endif