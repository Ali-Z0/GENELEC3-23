//--------------------------------------------------------
// Mc32gestI2cEEprom.C
//--------------------------------------------------------
// Gestion I2C de la SEEPROM du MCP79411 (Solution exercice)
//	Description :	Fonctions pour EEPROM MCP79411
//
//	Auteur 		: 	C. HUBER
//      Date            :       26.05.2014
//	Version		:	V1.0
//	Compilateur	:	XC32 V1.31
// Modifications :
//
/*--------------------------------------------------------*/



#include "Mc32gestI2cSeeprom.h"
#include "Mc32_I2cUtilCCS.h"
#include "Mc32Delays.h"


// Définition pour MCP79411
#define MCP79411_EEPROM_R    0xAF         // MCP79411 address for read
#define MCP79411_EEPROM_W    0xAE         // MCP79411 address for write
// La EEPROM du 79411 est de 1 Kbits donc 128 octets
#define MCP79411_EEPROM_BEG   0x00         // addr. début EEPROM
#define MCP79411_EEPROM_END   0x7F         // addr. fin EEPROM

// Definitions du bus (pour mesures)
// #define I2C-SCK  SCL2/RA2      PORTAbits.RA2   pin 58
// #define I2C-SDA  SDa2/RA3      PORTAbits.RA3   pin 59




// Initialisation de la communication I2C et du MCP79411
// ------------------------------------------------

void I2C_InitMCP79411(void)
{
   bool Fast = true;
   i2c_init( Fast );
} //end I2C_InitMCP79411

// Ecriture d'un bloc dans l'EEPROM du MCP79411 
void I2C_WriteSEEPROM(void *SrcData, uint32_t EEpromAddr, uint16_t NbBytes)
{
    /* Nombre de pages a ecrire */
    uint32_t NbPages = 0;
    /* Compteur de pages ecrites */
    uint32_t cntPage = 0;
    /* Compteur des bytes */
    uint8_t i = 0;
    /* Variable qui contient les donnees du byte a ecrire */
    uint8_t byteToWrite = 0;
    /* Compteur acknowledge / erreurs */
    uint8_t commStat = 0;
    /* Compteur time-out */
    uint32_t cntTimeout = 0;
    /* Compteur bytes envoyes */
    uint32_t cntSentBytes = 0;
    
    /* Calcul le nombre de pages a ecrire */
    NbPages = (uint32_t)((float)(NbBytes / (float)NB_BYTES_PAGES) + 0.5 );
    
    /* Gere l'ecriture d'une page */
    for(cntPage = 0; cntPage < NbPages; cntPage++)
    {
        do{
            /* Entete de la communication */
            i2c_start();
            commStat += i2c_write(MCP79411_EEPROM_W);
            commStat += i2c_write(EEpromAddr+(cntPage*8));
            /* Compteur de timeout pour eviter bouclage infini (~50 secondes) */
            cntTimeout ++;
        } while((!commStat)&&(cntTimeout<TIME_OUT));
        
        /* Ecriture des 8 bytes */
        for(i = 0; i < NB_BYTES_PAGES; i++){
            /* Obtention du byte a ecrire */
            byteToWrite = *((uint8_t *)SrcData + cntSentBytes);
            /* Ecriture */
            i2c_write(byteToWrite);
            cntSentBytes ++;
        }
        /* Arret de la communication */
        i2c_stop();
        /* Attente apres page */
        delay_ms(5);
    }
     
} // end I2C_WriteSEEPROM

// Lecture d'un bloc dans l'EEPROM du MCP79411
void I2C_ReadSEEPROM(void *DstData, uint32_t EEpromAddr, uint16_t NbBytes)
{
        /* Compteur time-out */
        uint32_t cntTimeout = 0;
        /* Compteur acknowledge / erreurs */
        uint8_t commStat = 0;
        /* Compteur des bytes */
        uint8_t i = 0;
        /* Byte lu */
        uint8_t byteToRead = 0;
        /* Nombre de pages a ecrire */
        uint32_t NbPages = 0;
        /* Compteur de pages ecrites */
        uint32_t cntPage = 0;
        /* Compteur bytes lus */
        uint32_t cntReadBytes = 0;
        /* Variable nombre de byte a envoyer sur la page */
        uint8_t nbBytesPage = NB_BYTES_PAGES;
        
        /* Calcul le nombre de pages a ecrire */
        NbPages = (uint32_t)((float)(NbBytes / (float)NB_BYTES_PAGES) + 0.5 );
        
        /* Gere la lecture d'une page */
        for(cntPage = 0; cntPage < NbPages; cntPage++)
        {
            /* Commence la comunication jusqu'a validation */
            do{
                /* Entete de la communication */
                i2c_start();
                commStat += i2c_write(MCP79411_EEPROM_W);
                commStat += i2c_write(EEpromAddr+(cntPage*8));
                i2c_reStart();
                commStat += i2c_write(MCP79411_EEPROM_R);
                /* Compteur de timeout pour eviter bouclage infini (~50 secondes) */
                cntTimeout ++;
            } while((!commStat)&&(cntTimeout<TIME_OUT));
            
            /* Si derniere page atteinte */
            if(cntPage >= (NbPages-1))
            {
                /* Calcul les bytes restants */
                nbBytesPage = (NbBytes % NB_BYTES_PAGES);
                /* Si il restait exactement 8 bytes a ecrire */
                if(nbBytesPage == 0)
                    nbBytesPage = 8;
            }
            
            /* Lecture de tous les bytes */
            for(i = 0; i < nbBytesPage; i++){
                if(i < (nbBytesPage-1))
                    byteToRead = i2c_read(1);
                else
                    byteToRead = i2c_read(0);
                
                *((uint8_t*)DstData + cntReadBytes) = byteToRead;
                cntReadBytes++;
            }
            i2c_stop();
            /* Attente apres page */
            delay_ms(5);
        }
        
        
    
} // end I2C_ReadSEEPROM
   





 



