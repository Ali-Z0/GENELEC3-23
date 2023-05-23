// Canevas manipulation GenSig avec menu
// C. HUBER  09/02/2015
// Fichier Generateur.C
// Gestion  du générateur

// Prévu pour signal de 40 echantillons

// Migration sur PIC32 30.04.2014 C. Huber


#include "Generateur.h"
#include "DefMenuGen.h"
#include "Mc32gestSpiDac.h"

// T.P. 2016 100 echantillons
#define MAX_ECH 100



// Initialisation du  générateur
void  GENSIG_Initialize(S_ParamGen *pParam)
{
}
  

// Mise à jour de la periode d'échantillonage
void  GENSIG_UpdatePeriode(S_ParamGen *pParam)
{
}

// Mise à jour du signal (forme, amplitude, offset)
void  GENSIG_UpdateSignal(S_ParamGen *pParam)
{
   
   
}


// Execution du générateur
// Fonction appelée dans Int timer3 (cycle variable variable)

// Version provisoire pour test du DAC à modifier
void  GENSIG_Execute(void)
{
   static uint16_t EchNb = 0;
   const uint16_t Step = 65535 / MAX_ECH;

   SPI_WriteToDac(0, Step * EchNb );      // sur canal 0
   EchNb++;
   EchNb = EchNb % MAX_ECH;
}
