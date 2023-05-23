/* ************************************************************************** */
/** gestion SPI via machine d'état
 *
 *  @Company
 *    ETML-ES - SCA
 *
 *  @File Name
 *    Mc32Ex8_2_spi_sm.c
 *
 *  @Summary
 *    gestion SPI via machine d'état
 *
 *  @Description
 *    gestion SPI via machine d'état
 *    Testé pour fonctionner avec LM70 sur SPI1
 * 
 *   Création 14.03.2017 SCA
 *  
*/

#include "Mc32Ex8_2_spi_sm.h"
#include "bsp.h"
#include "peripheral/spi/plib_spi.h"
#include "system/clk/sys_clk.h" // pour SYS_CLK_PeripheralFrequencyGet()
#include "system/int/sys_int.h"


//byte bidon pour envoi lorsqu'uniquement une lecture est demandée
//le spi étant full-duplex, une lecture implique forcément une écriture simultanée
#define DUMMY_BYTE  0x81    

SPI_STATES spiState = SPI_STATE_UNINITIALIZED;

//fonction à appeler 1x au démarrage pour init.
//code repris de la génération du driver par Hamony 1.08
void SPI_Init(void)
{
    // *** init du périph. SPI ***

 
}

//Ecriture.
//Comme le SPI est obligatoirement full-duplex,
//les données reçues ne seront pas traitées
void SPI_StartWrite(uint32_t nBytes, uint8_t* pBytesToWrite)
{

 
}

//Lecture/écriture.
//Comme le SPI est obligatoirement full-duplex,
//des données sont reçues simultanément à l'envoi
void SPI_StartReadWrite(uint32_t nBytes, uint8_t* pBytesToWrite)
{

  
}

//Lecture.
//Comme le SPI est obligatoirement full-duplex,
//il faut envoyer des données bidons pour faire une lecture
void SPI_StartRead(uint32_t nBytes)
{
 
 
}

//pour obtenir l'état interne de la SM spi
SPI_STATES SPI_GetState (void)
{
  
  
}

//lecture d'un byte dans buffer réception
uint8_t SPI_ReadByte(void)
{


}

//fonction à appeler périodiquement pour gestion SPI
//gestion de la machine d'état du SPI
void SPI_DoTasks(void)
{
 
 
}


/* *****************************************************************************
 End of File
 */
