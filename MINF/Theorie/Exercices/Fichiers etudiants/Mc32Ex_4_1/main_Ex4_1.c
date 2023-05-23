#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include <stdint.h>                     // Defines EXIT_FAILURE

// Fonction SimuPrintSymb
void SimuPrintSymb (const char *format, char val)
{
    char tmp = val;
}

// Fonction SerieSymboles
// Affiche une série de symboles avec possibilité de changement de ligne
void SerieSymboles (uint8_t n, char symb, bool IsEndl)
{
    uint8_t i;
    for (i=0; i < n; i++) {
        SimuPrintSymb("%c", symb);
    }
    if (IsEndl == true) {
        symb = '\n';
        SimuPrintSymb("%c", symb);
    }
}

// Fonction ComposeLigne
// Compose une ligne avec 2 séries
void ComposeLigne (uint8_t n1, char symb1,uint8_t n2, char symb2)
{
    SerieSymboles(n1, symb1, false);
    SerieSymboles(n2, symb2, true);
}

// Fonction DispMotif
// Compose un motif de n lignes
void DispMotif (uint8_t nbli, char symb1, char symb2)
{
   uint8_t i;
   for (i=1; i <= nbli; i++) {
        ComposeLigne(i, symb1, nbli-i, symb2);
    }
}


uint16_t ExtractHeures(uint32_t nbSecTot)
{
    return (nbSecTot / 3600);
}

uint16_t ExtractMinutes(uint32_t nbSecTot, uint16_t NbH)
{
    return ( (nbSecTot - ( NbH * 3600)) / 60);
}

uint16_t ExtractSecondes(uint32_t nbSecTot, uint16_t NbH, uint16_t NbM)
{
    return ( nbSecTot - ( NbH * 3600) - (NbM *60) );
}


typedef struct {
    uint16_t NbH;
    uint16_t NbM;
    uint16_t NbS;
} S_HMS;

S_HMS ExtractHMS(uint32_t nbSecTot)
{
    S_HMS tmp;
    
    tmp.NbH = nbSecTot / 3600;
    tmp.NbM = (nbSecTot - ( tmp.NbH * 3600)) / 60;
    tmp.NbS = nbSecTot - ( tmp.NbH * 3600) - (tmp.NbM *60);
    return ( tmp );
}

void main() {

   uint32_t NbSecT = 3675;
   uint16_t H, M, S;
   S_HMS ResHMS;
   bool match;

   // Appel des fonctions d'extractions
   H = ExtractHeures(NbSecT);
   M = ExtractMinutes(NbSecT, H);
   S = ExtractSecondes(NbSecT, H, M);
   
   ResHMS = ExtractHMS(NbSecT);

   if (H = ResHMS.NbH) match = true;
   if (M = ResHMS.NbM) match = true;
   if (S = ResHMS.NbS) match = true;


   // Appel de la fonction  DispMotif
   DispMotif(5, '*', '+');
   DispMotif(7, '!', '=');
}
