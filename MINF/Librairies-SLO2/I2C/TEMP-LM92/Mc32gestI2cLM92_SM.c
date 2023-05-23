//--------------------------------------------------------
// Mc32gestI2cLm92_SM.C
//--------------------------------------------------------
// Gestion I2C du slave LM92 (temperature sensor)
//	Description :	Fonctions pour machine état du LM92
//
//	Auteur 		: 	C. HUBER
//      Date            :       24.04.2015
//	Version		:	V1.6
//	Compilateur	:	XC32 V1.40 & Harmony 1_06
// Modifications :
//      CHR 24.04.2015 : adadatation aux types de stdint.h
//      CHR 10.05.2016 : modification version compilateur et Harmony
//      (SCA 11.04.2017 adaptation Harmony 1.08 (pas de modif de ce fichier))
//----------------------------------------------------------




#include "Mc32gestI2cLM92_SM.h"
#include "Mc32_I2cUtil_SM.h"
#include "system_config.h"    // pour bsp

// Compilation conditionelle (Mettre en commentaire pour ne pas utiliser les leds)
#define USE_LED_MEASURE true
// Utilisation BSP_LED 5 & 6

// Définition pour LM92
#define lm92_rd    0x91         // lm92 address for read
#define lm92_wr    0x90         // lm92 address for write
#define lm92_temp_ptr  0x00     // adr. pointeur température

// Definitions du bus (pour mesures)
// #define I2C-SCK  SCL2/RA2      PORTAbits.RA2   pin 58
// #define I2C-SDA  SDa2/RA3      PORTAbits.RA3   pin 59

// Descripteur gestion LM92 par State Machine
//S_Descr_LM92_SM DescrLm92;     // descripteur LM92
     
//--------------------------------------------------------
// Lm92SmInit
// Init pour appel extérieur
//--------------------------------------------------------
/*
void Lm92SmInit(bool Fast)
{
    I2C_LM92_SM_Init(&DescrLm92, true);
}*/

//--------------------------------------------------------
// I2C_LM92_SM_Init
// Initialisation mécanisme SM du LM92 et communication I2C
// --------------------------------------------------------

void I2C_LM92_SM_Init(S_Descr_LM92_SM *pDescr, bool Fast)
{

   //pDescr->i2cModuleId = ModuleId;
   pDescr->Lm92state = LM92_SM_Idle;
   pDescr->Lm92Sequence =  LM92_I2CSEQ_Idle;
   pDescr->RawTemp = 0;
   pDescr->TempMilli = 0;
   pDescr->Temperature = 0.0;
   
   I2C_SM_init(  Fast, &pDescr->I2cSmInfo );
}

// --------------------
// I2C_LM92_SM_Restart
//
// Sort de l'état Ready et passe en Idle
//

void I2C_LM92_SM_Restart(S_Descr_LM92_SM *pDescr) {
   pDescr->Lm92state = LM92_SM_Idle;
   pDescr->Lm92Sequence =  LM92_I2CSEQ_Idle;
 
}

// --------------------
// I2C_LM92_SM_IsReady

bool I2C_LM92_SM_IsReady(S_Descr_LM92_SM *pDescr) {
    bool answer = false;
    if (pDescr->Lm92state == LM92_SM_ready ) {
        answer = true;
    }
    return answer;
}

// Execution de la lecture du registre de température du LM92
// Prévu pour appel cyclique
void I2C_LM92_SM_Execute(S_Descr_LM92_SM *pDescr)
{
    //Déclaration des variables
    int16_t RawTemp;
    bool AckBit;

    switch ( pDescr->Lm92state )  {
        case  LM92_SM_Idle :
            // Passe à Busy
            pDescr->Lm92state = LM92_SM_Busy;
            pDescr->Lm92Sequence = LM92_I2CSEQ_Start;
            #ifdef USE_LED_MEASURE
                BSP_LEDOn(BSP_LED_6);   // Marque début séquence active
            #endif
        break;

        case  LM92_SM_Busy :
            // Effectue la lecture par étapes
            switch (pDescr->Lm92Sequence)  {
                case LM92_I2CSEQ_Start :
                    // i2c_start();
                    I2C_SM_start( &pDescr->I2cSmInfo);
                    if (I2C_SM_isReady (&pDescr->I2cSmInfo)){
                        pDescr->Lm92Sequence = LM92_I2CSEQ_WriteAddrW;
                        I2C_SM_begin(&pDescr->I2cSmInfo); // redemare la SM
                        #ifdef USE_LED_MEASURE
                            BSP_LEDToggle(BSP_LED_5);
                        #endif
                    }
                break;
               
                case LM92_I2CSEQ_WriteAddrW :
                    //  i2c_write(lm92_wr);	 adresse + écriture
                    I2C_SM_write( &pDescr->I2cSmInfo, lm92_wr, &AckBit);
                    if (I2C_SM_isReady (&pDescr->I2cSmInfo)){
                        pDescr->Lm92Sequence = LM92_I2CSEQ_WritePtrTemp;
                        I2C_SM_begin(&pDescr->I2cSmInfo); // redemare la SM
                        #ifdef USE_LED_MEASURE
                            BSP_LEDToggle(BSP_LED_5);
                        #endif
                    }
                break;

                case LM92_I2CSEQ_WritePtrTemp :
                    // i2c_write(lm92_temp_ptr);	sélection ptr. temp.
                    I2C_SM_write( &pDescr->I2cSmInfo, lm92_temp_ptr, &AckBit);
                    if (I2C_SM_isReady (&pDescr->I2cSmInfo)){
                        pDescr->Lm92Sequence = LM92_I2CSEQ_ReStart;
                        I2C_SM_begin(&pDescr->I2cSmInfo); // redemare la SM
                        #ifdef USE_LED_MEASURE
                            BSP_LEDToggle(BSP_LED_5);
                        #endif
                    }
                break;

                case LM92_I2CSEQ_ReStart :
                    // i2c_reStart();
                    I2C_SM_reStart( &pDescr->I2cSmInfo);
                    if (I2C_SM_isReady (&pDescr->I2cSmInfo)){
                        pDescr->Lm92Sequence = LM92_I2CSEQ_WriteAddrR;
                        I2C_SM_begin(&pDescr->I2cSmInfo); // redemare la SM
                        #ifdef USE_LED_MEASURE
                            BSP_LEDToggle(BSP_LED_5);
                        #endif
                    }
                break;
                case LM92_I2CSEQ_WriteAddrR :
                    //  i2c_write(lm92_rd);	 adresse + lecture
                    I2C_SM_write( &pDescr->I2cSmInfo, lm92_rd, &AckBit);
                    if (I2C_SM_isReady (&pDescr->I2cSmInfo)){
                        pDescr->Lm92Sequence = LM92_I2CSEQ_ReadMsb;
                        I2C_SM_begin(&pDescr->I2cSmInfo); // redemare la SM
                        #ifdef USE_LED_MEASURE
                            BSP_LEDToggle(BSP_LED_5);
                        #endif
                    }
                break;
                
                case LM92_I2CSEQ_ReadMsb :
                    //  msb = i2c_read(1); 	// ack
                    I2C_SM_read( &pDescr->I2cSmInfo, true, &pDescr->Msb);
                    if (I2C_SM_isReady (&pDescr->I2cSmInfo)){
                        pDescr->Lm92Sequence = LM92_I2CSEQ_ReadLsb;
                        I2C_SM_begin(&pDescr->I2cSmInfo); // redémarre la SM
                        #ifdef USE_LED_MEASURE
                            BSP_LEDToggle(BSP_LED_5);
                        #endif
                    }
                break;

                case LM92_I2CSEQ_ReadLsb :
                   //  lsb = i2c_read(0); 	// no ack
                    I2C_SM_read(&pDescr->I2cSmInfo, false, &pDescr->Lsb);
                    if (I2C_SM_isReady (&pDescr->I2cSmInfo)){
                        pDescr->Lm92Sequence = LM92_I2CSEQ_Stop;
                        I2C_SM_begin(&pDescr->I2cSmInfo); // rédemarre la SM
                        #ifdef USE_LED_MEASURE
                            BSP_LEDToggle(BSP_LED_5);
                        #endif
                    }
                break;

                case LM92_I2CSEQ_Stop :
                    // i2c_stop();
                    I2C_SM_stop(&pDescr->I2cSmInfo);
                    if (I2C_SM_isReady (&pDescr->I2cSmInfo)){
                        // Effectue les calculs des températures
                        RawTemp = pDescr->Msb;
                        RawTemp = RawTemp << 8;
                        RawTemp = RawTemp | pDescr->Lsb;
                        pDescr->RawTemp = RawTemp;
                        RawTemp = RawTemp / 8;
                        // bit poid faible = 0.0625 degré
                        pDescr->TempMilli = RawTemp * 62.5;
                        pDescr->Temperature = RawTemp * 0.0625;

                        pDescr->Lm92Sequence = LM92_I2CSEQ_Idle;
                        I2C_SM_begin(&pDescr->I2cSmInfo); // redemare la SM
                        pDescr->Lm92state = LM92_SM_ready;
                        #ifdef USE_LED_MEASURE
                            BSP_LEDOff(BSP_LED_6); // marque fin séquence
                        #endif
                    }
                break;
                 
                case LM92_I2CSEQ_Idle :
                    // ajout pour eviter Warning
                break;
            }
        break;

        case  LM92_SM_ready :
            // Les résultats sont disponibles
            // Attente du Restart de l'utilisateur
            // après lecture des résultats
        break;
    }
       
} // end I2C_LM92_SM_Execute
   

float I2C_LM92_SM_GetTemp(S_Descr_LM92_SM *pDescr)
{
    return pDescr->Temperature;
}

int32_t I2C_LM92_SM_GetTempMilli(S_Descr_LM92_SM *pDescr) {
    return pDescr->TempMilli;
}

int16_t I2C_LM92_SM_GetRawTemp(S_Descr_LM92_SM *pDescr) {
    return pDescr->RawTemp;
}

 



