/*--------------------------------------------------------*/
// ETML Ecole Technique
// GesFifoTh32.c
/*--------------------------------------------------------*/
//	Description :
//	 Gestion d'un fifo de caract�re, utilisation de pointeur et
//   d'un descripteur de fifo
//
//	Auteur 		: 	C. Huber
//	Version		:	V1.6
//	Compilateur	:	XC32 V1.42 + Harmony 1.08
//
//  Modifications :
//   CHR 19.12.2014  remplacement typedef32 par stdint
//   CHR 20.12.2016  fifosize en int32_t pour permettre des
//                    fifo de grande taille
//   SCA 06.09.2022  v1.7 MPLABX 5.45/xc32 2.50/Harmony 2.06
//                   Enlev� bug dans GetCharFromFifo qui
//                   emp�chait un buffer > 256 �l�ments
//
/*--------------------------------------------------------*/

#include "GesFifoTh32.h"

/*---------------*/
/* InitFifo      */
/*===============*/

// Init avec possibilit� de fournir une valeur de remplissage
// Initialisation du descripteur de FIFO

void InitFifo ( S_fifo *pDescrFifo, int32_t FifoSize, int8_t *pDebFifo, int8_t InitVal )
{
   int32_t i;
   int8_t *pFif;
   pDescrFifo->fifoSize =   FifoSize;
   pDescrFifo->pDebFifo =   pDebFifo; // d�but du fifo
   // fin du fifo
   pDescrFifo->pFinFifo =   pDebFifo + (FifoSize - 1);
   pDescrFifo->pWrite   =   pDebFifo;  // d�but du fifo
   pDescrFifo->pRead     =   pDebFifo;  // d�but du fifo
   pFif = pDebFifo;
   for (i=0; i < FifoSize; i++) {
      *pFif  = InitVal;
      pFif++;
   }
} /* InitFifo */


/*---------------*/
/* GetWriteSpace */
/*===============*/

// Retourne la place disponible en �criture

int32_t GetWriteSpace ( S_fifo *pDescrFifo)
{
   int32_t writeSize;

   // D�termine le nb de car.que l'on peut d�poser
   writeSize = pDescrFifo->pRead - pDescrFifo->pWrite -1;
   if (writeSize < 0) {
      writeSize = writeSize + pDescrFifo->fifoSize;
    }
   return (writeSize);
} /* GetWriteSpace */


/*-------------*/
/* GetReadSize */
/*=============*/

// Retourne le nombre de caract�res � lire

int32_t GetReadSize ( S_fifo *pDescrFifo)
{
   int32_t readSize;

   readSize = pDescrFifo->pWrite - pDescrFifo->pRead;
   if (readSize < 0) {
       readSize = readSize +  pDescrFifo->fifoSize;
   }

   return (readSize);
} /* GetReadSize */

/*---------------*/
/* PutCharInFifo */
/*===============*/

// D�pose un caract�re dans le FIFO
// Retourne 0 si OK, 1 si FIFO full

uint8_t PutCharInFifo ( S_fifo *pDescrFifo, int8_t charToPut )
{
   uint8_t writeStatus;

   // test si fifo est FULL
   if (GetWriteSpace(pDescrFifo) == 0) {
      writeStatus = 1; // fifo FULL
   }
   else {
      // �crit le caract�re dans le FIFO
      *(pDescrFifo->pWrite) = charToPut;

      // incr�ment le pointeur d'�criture
      pDescrFifo->pWrite++;
      // gestion du rebouclement
      if (pDescrFifo->pWrite > pDescrFifo->pFinFifo) {
          pDescrFifo->pWrite = pDescrFifo->pDebFifo;
      }

      writeStatus = 0; // OK
   }
   return (writeStatus);
} // PutCharInFifo 


/*-----------------*/
/* GetCharFromFifo */
/*=================*/

// Obtient (lecture) un caract�re du fifo 
// retourne 0 si OK, 1 si empty
// le caract�re lu est retourn� par r�ference

uint8_t GetCharFromFifo ( S_fifo *pDescrFifo, int8_t *carLu )
{
   int32_t readSize;
   uint8_t readStatus;

   // d�termine le nb de car. que l'on peut lire
   readSize = GetReadSize(pDescrFifo);

   // test si fifo est vide
   if (readSize == 0) {
      readStatus = 1; // fifo EMPTY
      *carLu = 0;     // carLu = NULL
   }
   else {
      // lis le caract�re dans le FIFO
      *carLu = *(pDescrFifo->pRead);

      // incr�ment du pointeur de lecture
      pDescrFifo->pRead++;
      // gestion du rebouclement
      if (pDescrFifo->pRead > pDescrFifo->pFinFifo) {
          pDescrFifo->pRead = pDescrFifo->pDebFifo;
      }
      readStatus = 0; // OK
   }
   return (readStatus);
} // GetCharFromFifo 


