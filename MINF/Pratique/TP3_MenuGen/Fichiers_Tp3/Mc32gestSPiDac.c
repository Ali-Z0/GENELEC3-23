/*--------------------------------------------------------*/
// Mc32GestSpiDac.c
/*--------------------------------------------------------*/
//	Description :	Gestion DAC Spi du Kit (LTC2604)
//
//	Auteur 		: 	C. Huber
//	Version		:	V1.2
//	Compilateur	:	XC32 V1.40 + Harmony 1.06
//
//  Modifications :
//   CHR 10.02.2015 Adaptation plib_spi 
//   CHR 06.03.2015 ajout fonction SPI_CfgWriteToDac
//   CHR 24.05.2016 ajout #include "Mc32Delays.h"
//   CHR 25.05.2016 utilisation de SYS_CLK_PeripheralFrequencyGet(CLK_BUS_PERIPHERAL_1)
//   SCA 04.04.2022 Config des signaux ctrl (CS et CLR) 
//                  en sortie au préalable à l'init du DAC
//                  MPLABX 5.45, xc32 2.50, Harmony 2.06
/*--------------------------------------------------------*/


#include "app.h"   // pour system_config et autre
#include "Mc32gestSpiDac.h"
#include "Mc32SpiUtil.h"
#include "peripheral\SPI\plib_spi.h"
#include "Mc32Delays.h"

// SPI_ID_1 correspond au SPI1 !
#define KitSpi1 (SPI_ID_1)

uint32_t ConfigReg;     // pour lecture de SPI1CON
uint32_t BaudReg;       // pour lecture de SPI1BRG

// Initialisation de la communication SPI et du DAC
// ------------------------------------------------

// Pour observation des signaux SPI
// SPI-SCK   RD10 pin 70
// SPI-MOSI  RD0  pin 72
// SPI-MISO  RC4  pin  9
// SPI-CS_DA RD4  pin 81

// Ancienne configuration pour macro XC32
// uint32_t  config = SPI_OPEN_MODE8 | SPI_OPEN_ON | SPI_OPEN_CKP_HIGH |
//                    SPI_OPEN_MSTEN ;


//Initialisation du SPI pour utilisation avec DAC LTC2604
void SPI_ConfigureLTC2604(void)
{
   PLIB_SPI_Disable(KitSpi1);

   PLIB_SPI_BufferClear(KitSpi1);
   PLIB_SPI_StopInIdleDisable(KitSpi1);
   PLIB_SPI_PinEnable(KitSpi1, SPI_PIN_DATA_OUT);
   PLIB_SPI_CommunicationWidthSelect(KitSpi1, SPI_COMMUNICATION_WIDTH_8BITS);
   // Config SPI clock à 20 MHz 
   PLIB_SPI_BaudRateSet(KitSpi1, SYS_CLK_PeripheralFrequencyGet(CLK_BUS_PERIPHERAL_1), 20000000);
   // Config polarité traitement des signaux SPI
   // pour input à confirmer
   // Polarité clock OK
   // Phase output à confirmer
   PLIB_SPI_InputSamplePhaseSelect(KitSpi1, SPI_INPUT_SAMPLING_PHASE_IN_MIDDLE );
   PLIB_SPI_ClockPolaritySelect(KitSpi1, SPI_CLOCK_POLARITY_IDLE_HIGH);
   PLIB_SPI_OutputDataPhaseSelect(KitSpi1, SPI_OUTPUT_DATA_PHASE_ON_IDLE_TO_ACTIVE_CLOCK);
   PLIB_SPI_MasterEnable(KitSpi1);
   PLIB_SPI_FramedCommunicationDisable(KitSpi1);
   PLIB_SPI_FIFOEnable(KitSpi1);     // Enhenced buffer mode

   PLIB_SPI_Enable(KitSpi1);

   // Contrôle le la configuration
   ConfigReg = SPI1CON;
   BaudReg = SPI1BRG;
}

// Initialisation des signaux de controle du DAC, reset du DAC, puis config. du periph. SPI
void SPI_InitLTC2604(void)
{  
    TRISDbits.TRISD4 = 0;   //SPI-CS_DA en sortie (RD4)
    TRISDbits.TRISD9 = 0;   //DAC_CLR en sortie (RD9)       
    
   //Initialisation SPI DAC
   CS_DAC = 1;
   // Impulsion reset du DAC
   DAC_CLEAR = 0;
   delay_us(500);
   DAC_CLEAR = 1;

   // LTC2604 MAX 50 MHz choix 20 MHz
   SPI_ConfigureLTC2604();
}

// Envoi d'une valeur sur le DAC  LTC2604
// Sans reconfiguration du SPI
// Indication du canal 0 à 3
void SPI_WriteToDac(uint8_t NoCh, uint16_t DacVal)
{
   //Déclaration des variables
   uint8_t MSB;
   uint8_t LSB;
   
   //Sélection du canal
   //3 -> Set and Update, 0/1/2/3 Sélection canal A/B/C/D, F tous canaux
   NoCh = NoCh + 0x30;

   // Selon canal
   MSB =  DacVal >> 8;
   LSB =  DacVal;

   CS_DAC = 0;
   spi_write1(NoCh);
   spi_write1(MSB);
   spi_write1(LSB);
 
   //Fin de transmission
   CS_DAC = 1;

} // SPI_WriteToDac
   

// Envoi d'une valeur sur le DAC  LTC2604
// Avec reconfiguration du SPI
// Indication du canal 0 à 3
void SPI_CfgWriteToDac(uint8_t NoCh, uint16_t DacVal)
{
   //Déclaration des variables
   uint8_t MSB;
   uint8_t LSB;

   // Reconfiguration du SPI
   SPI_ConfigureLTC2604();

   //Sélection du canal
   //3 -> Set and Update, 0/1/2/3 Sélection canal A/B/C/D, F tous canaux
   NoCh = NoCh + 0x30;

   // Selon canal
   MSB =  DacVal >> 8;
   LSB =  DacVal;

   CS_DAC = 0;
   spi_write1(NoCh);
   spi_write1(MSB);
   spi_write1(LSB);

   // Fin de transmission
   CS_DAC = 1;

} // SPI_CfgWriteToDac


 



