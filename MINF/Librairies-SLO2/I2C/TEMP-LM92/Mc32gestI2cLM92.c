//--------------------------------------------------------
// Mc32gestI2cLm92.C
//--------------------------------------------------------
// Gestion I2C du slave LM92 (temperature sensor)
//	Description :	Fonctions pour DS18B20
//
//	Auteur 		: 	C. HUBER
//      Date            :       22.05.2014
//	Version		:	V1.0
//	Compilateur	:	XC32 V1.31
// Modifications :
//
/*--------------------------------------------------------*/



//#include "bsp_config.h" //pour debug (manipulation des LED)
#include "Mc32gestI2cLM92.h"
#include "Mc32_I2cUtilCCS.h"

// Définition pour LM92
#define lm92_rd    0x91         // lm92 address for read
#define lm92_wr    0x90         // lm92 address for write
#define lm92_temp_ptr  0x00     // adr. pointeur température

// Definitions du bus (pour mesures)
// #define I2C-SCK  SCL2/RA2      PORTAbits.RA2   pin 58
// #define I2C-SDA  SDa2/RA3      PORTAbits.RA3   pin 59

// Initialisation de la communication I2C et du LM92
// ------------------------------------------------

void I2C_InitLM92(void)  {

   bool Fast = true;
   i2c_init(Fast );
   I2C_WriteConfigLM92();

 }

// Confirmation du pointeur sur le registre de température
// Lecture du registre de temperature du LM92

void I2C_WriteConfigLM92(void)
{
    i2c_start();
    i2c_write(lm92_wr);	// adresse + écriture
    i2c_write(lm92_temp_ptr);	// sélection ptr. temp.
    i2c_reStart();
    i2c_write(lm92_rd);	// adresse + lecture
    i2c_read(1); 	// ack
    i2c_read(0);	// no ack
    i2c_stop();
} // I2C_WriteConfigLM92

// Lecture du registre de temperature du LM92
// Effectue uniquement la lecture, nécessite que le pointeur par
// défaut soit le registre de temperature

int16_t I2C_ReadRawTempLM92(void)
{
    //Déclaration des variables
    uint8_t msb = 1;
    uint8_t lsb = 1;
    int16_t RawTemp;

    // BSP_LEDOn(BSP_LED_6);  // provisoire : pour observation
    i2c_start();
    i2c_write(lm92_rd);	// adresse + lecture
    msb = i2c_read(1); 	// ack
    lsb = i2c_read(0);	// no ack
    i2c_stop();
    // BSP_LEDOff(BSP_LED_6);  // provisoire : pour observation
    RawTemp = msb;
    RawTemp = RawTemp << 8;
    RawTemp = RawTemp | lsb;
    return RawTemp;
} // end I2C_ReadRawTempLM92
 
// Confirmation du pointeur sur le registre de température
// Lecture du registre de temperature du LM92
int16_t I2C_WriteCfgReadRawTempLM92(void)
{
    //Déclaration des variables
    uint8_t msb = 1;
    uint8_t lsb = 1;
    int16_t RawTemp;

    // BSP_LEDOn(BSP_LED_6);  // provisoire : pour observation
    i2c_start();
    i2c_write(lm92_wr);	// adresse + écriture
    i2c_write(lm92_temp_ptr);	// sélection ptr. temp.

    i2c_reStart();
    i2c_write(lm92_rd);	// adresse + lecture
    msb = i2c_read(1); 	// ack
    lsb = i2c_read(0);	// no ack
    i2c_stop();
    // BSP_LEDOff(BSP_LED_6);  // provisoire : pour observation

    RawTemp = msb;
    RawTemp = RawTemp << 8;
    RawTemp = RawTemp | lsb;

    return RawTemp;
} // end I2C_WriteCfgReadRawTempLM92

// Passage par référence car problème avec return lors affectation
// a une variable globale dans réponse à interruption

void LM92_ConvRawToDeg( int16_t RowTemp, float *pTemp)
{
    float TempLoc;

    RowTemp = RowTemp / 8;
    // bit poids faible = 0.0625 degré
    TempLoc = RowTemp * 0.0625;
    *pTemp = TempLoc;
} // end LM92_ConvRawToDeg

 



