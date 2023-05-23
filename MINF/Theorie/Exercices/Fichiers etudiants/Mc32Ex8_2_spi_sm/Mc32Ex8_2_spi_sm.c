/* ************************************************************************** */
/** gestion SPI via machine d'�tat
 *
 *  @Company
 *    ETML-ES - SCA
 *
 *  @File Name
 *    Mc32Ex8_2_spi_sm.c
 *
 *  @Summary
 *    gestion SPI via machine d'�tat
 *
 *  @Description
 *    gestion SPI via machine d'�tat
 *    Test� pour fonctionner avec LM70 sur SPI1
 * 
 *   Cr�ation 14.03.2017 SCA
 *  
*/

#include "Mc32Ex8_2_spi_sm.h"
#include "bsp.h"
#include "peripheral/spi/plib_spi.h"
#include "system/clk/sys_clk.h" // pour SYS_CLK_PeripheralFrequencyGet()
#include "system/int/sys_int.h"


//byte bidon pour envoi lorsqu'uniquement une lecture est demand�e
//le spi �tant full-duplex, une lecture implique forc�ment une �criture simultan�e
#define DUMMY_BYTE  0x81    

SPI_STATES spiState = SPI_STATE_UNINITIALIZED;

//fonction � appeler 1x au d�marrage pour init.
//code repris de la g�n�ration du driver par Hamony 1.08
void SPI_Init(void)
{
    // *** init du p�riph. SPI ***

 
}

//Ecriture.
//Comme le SPI est obligatoirement full-duplex,
//les donn�es re�ues ne seront pas trait�es
void SPI_StartWrite(uint32_t nBytes, uint8_t* pBytesToWrite)
{

 
}

//Lecture/�criture.
//Comme le SPI est obligatoirement full-duplex,
//des donn�es sont re�ues simultan�ment � l'envoi
void SPI_StartReadWrite(uint32_t nBytes, uint8_t* pBytesToWrite)
{

  
}

//Lecture.
//Comme le SPI est obligatoirement full-duplex,
//il faut envoyer des donn�es bidons pour faire une lecture
void SPI_StartRead(uint32_t nBytes)
{
 
 
}

//pour obtenir l'�tat interne de la SM spi
SPI_STATES SPI_GetState (void)
{
  
  
}

//lecture d'un byte dans buffer r�ception
uint8_t SPI_ReadByte(void)
{


}

//fonction � appeler p�riodiquement pour gestion SPI
//gestion de la machine d'�tat du SPI
void SPI_DoTasks(void)
{
 
 
}


/* *****************************************************************************
 End of File
 */
