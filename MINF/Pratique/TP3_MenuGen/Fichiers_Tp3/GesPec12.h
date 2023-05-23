#ifndef GesPEc12_h
#define GesPEc12_h

// GesPec12.h
// C. HUBER    14/05/2014
// Creation pour migration PIC32MX

// Fonctions pour la gestion du Pec12
//
//
// Principe : Il est n�cessaire d'appeler cycliquement la fonction ScanPec12
//            avec un cycle de 1 ms
//
//  Pour la gestion du Pec12, il y a 9 fonctions � disposition :
//       Pec12IsPlus       true indique un nouveau incr�ment
//       Pec12IsMinus      true indique un nouveau d�cr�ment
//       Pec12IsOK         true indique action OK
//       Pec12IsESC        true indique action ESC
//       Pec12NoActivity   true indique abscence d'activit� sur PEC12
//  Fonctions pour quittance des indications
//       Pec12ClearPlus    annule indication d'incr�ment
//       Pec12ClearMinus   annule indication de d�cr�ment
//       Pec12ClearOK      annule indication action OK
//       Pec12ClearESC     annule indication action ESC
//
//
//---------------------------------------------------------------------------


// d�finitions des types qui seront utilis�s dans cette application

#include <stdbool.h>
#include <stdint.h>

// structure du descripteur du PEC
// Migration : remplacement bool par champ 1 bit
typedef struct {
            uint8_t Inc : 1;             // �v�nement incr�ment
            uint8_t Dec : 1;             // �v�nement d�cr�ment
            uint8_t OK  : 1;              // �v�nement action OK
            uint8_t ESC : 1;             // �v�nement action ESC
            uint8_t NoActivity: 1 ;      // Indication de non activit�
            uint16_t PressDuration;   // Pour dur�e pression du P.B.
            uint16_t InactivityDuration;   // Pour dur�e inactivit�
} S_Pec12_Descriptor;




//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Principe utilisation des fonctions
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//
//
// ScanPec12 (bool ValA, bool ValB, bool ValPB)
//              Routine effectuant la gestion du Pec12
//              recoit la valeur des signaux et du boutons
//
// s'appuie sur le descripteur global.
// Apr�s l'appel le descripteur est mis � jour

// Comportement du PEC12
// =====================

// Attention 1 cran g�n�re une pulse compl�te (les 4 combinaisons)
// D'ou traitement uniquement au flanc descendand de B

// Dans le sens horaire CW:
//     __________                      ________________
// B:            |____________________|
//     ___________________                       _________
// A:                     |_____________________|                    

// Dans le sens anti-horaire CCW:
//     ____________________                      _________
// B:                      |____________________|
//     __________                       __________________
// A:            |_____________________|        

void ScanPec12 (bool ValA, bool ValB, bool ValPB);

void Pec12Init (void);

//       Pec12IsPlus       true indique un nouveau incr�ment
bool Pec12IsPlus    (void);
//       Pec12IsMinus      true indique un nouveau d�cr�ment
bool Pec12IsMinus    (void);
//       Pec12IsOK         true indique action OK
bool Pec12IsOK    (void);
//       Pec12IsESC        true indique action ESC
bool Pec12IsESC    (void);
//       Pec12NoActivity   true indique abscence d'activit� sur PEC12
bool Pec12NoActivity    (void);

//  Fonctions pour quittance des indications
//       Pec12ClearPlus    annule indication d'incr�ment
void Pec12ClearPlus   (void);
//       Pec12ClearMinus   annule indication de d�cr�ment
void Pec12ClearMinus   (void);
//       Pec12ClearOK      annule indication action OK
void Pec12ClearOK   (void);
//       Pec12ClearESC     annule indication action ESC
void Pec12ClearESC   (void);
void Pec12ClearInactivity   (void);


#endif
