////----------------------------------------------------------
////      Mc32_I2cUtil_SM.c
////----------------------------------------------------------
////	Description :	utilitaire I2C avec State Machine
////                      conserve la découpe des fonctions CCS like
////                      utilisation du driver I2C de HArmony 1_03
////
////	Auteur 		: 	C. HUBER
////      Date création   :       29.05.2014
////	Version		:	V1.6
////	Compilateur	:	XC32 V1.40
////      PLIB            :       Harmony 1_06
////
////      MODIFICATIONS   :
////          CHR 24.04.2015 refonte en utilisant le driver I2C fourni par le MHC
////          CHR 10.05.2016 ajout include du driver i2c
////          SCA 11.04.2017 adaptation Harmony 1.08
////
////      ATTENTION :
////          Le no du module I2C est connu du driver I2C seulement
////          utilisation du DRV_I2C0_fonctxxx
////
////-----------------------------------------------------------
//
//
//
//#include "Mc32_I2cUtil_SM.h"
//// #include "system_config/default/framework/driver/i2c/drv_i2c_static.h"
//#include "system_config.h"    // pour bsp
//// #include "system_definitions.h"  // pour action sauvage
//
//
//#define I2C_CLOCK_FAST 400000
//#define I2C_CLOCK_SLOW 100000
//
////------------------------------------------------------------------------------
//// I2C_SM_init
////
//// Initialisationde l'I2C  (sans séquence)
////      si BOOL Fast = FALSE   LOW speed  100 KHz
////      si BOOL Fast = TRUE   HIGH speed  400 KHz
////
////  MODIF : toujour en fast avec utilisation  DRV_I2C0_Initialize()
////------------------------------------------------------------------------------
//
//void I2C_SM_init ( bool Fast, S_Descr_I2C_SM *pDSM )
//{
//   
//    I2C_SM_begin(pDSM);
//
//    DRV_I2C0_Initialize();    // besoin de la version modifiée
// 
//}
//
//// Fonction pour gestion de la machine d'état
//// ==========================================
//
//void I2C_SM_begin( S_Descr_I2C_SM *pDSM )
//{
//    pDSM->I2cMainSM = I2C_XSM_Idle;
//    pDSM->I2cSeqSM = 0;
//    pDSM->DebugCode = 0;
//}
//
//bool I2C_SM_isReady( S_Descr_I2C_SM *pDSM )
//{
//    bool answer = false;
//    if (pDSM->I2cMainSM == I2C_XSM_Ready ) {
//        answer = true;
//    }
//    return answer;
//}
//
////------------------------------------------------------------
//// I2C_SM_start
////
//// Effectue le start I2C en séquence
//
//void I2C_SM_start(S_Descr_I2C_SM *pDSM)
//{
//  
//    switch (pDSM->I2cMainSM ) {
//        
//        case I2C_XSM_Idle:
//            // demarre le traitement
//            pDSM->I2cMainSM = I2C_XSM_Busy;
//            pDSM->I2cSeqSM = 0;
//        break;
//        
//        case I2C_XSM_Busy :
//            switch ( pDSM->I2cSeqSM) {
//                case 0 :
//                    // Wait for the bus to be idle, then start the transfer
//                    if ( DRV_I2C0_MasterBusIdle() ) {
//                       // OK alors Start
//                       pDSM->I2cSeqSM = 1;
//                       if( DRV_I2C0_MasterStart() == false)
//                       {
//                           pDSM->DebugCode = 1;
//                       }
//                    }
//                break;
//
//                case 1 :
//                     // Wait for the signal to complete
//                     if ( DRV_I2C0_WaitForStartComplete()) {
//                        pDSM->I2cSeqSM = 0;
//                        pDSM->I2cMainSM = I2C_XSM_Ready;
//                    }
//                break;
//            } // end switch SeqSM
//        break;
//        case I2C_XSM_Ready :
//              // Attente relancement
//        break;
//    } // end switch
//} // end I2C_SM_start
//
//void I2C_SM_reStart( S_Descr_I2C_SM *pDSM)
//{
//     
//     switch (pDSM->I2cMainSM ) {
//
//        case I2C_XSM_Idle:
//            // demarre le traitement
//            pDSM->I2cMainSM = I2C_XSM_Busy;
//            pDSM->I2cSeqSM = 0;
//        break;
//
//        case I2C_XSM_Busy :
//            switch ( pDSM->I2cSeqSM) {
//                case 0 :
//                    // Restart the transfer
//                    if(DRV_I2C0_MasterRestart() == false)
//                    {
//                         pDSM->DebugCode = 2;
//                    }
//                    pDSM->I2cSeqSM = 1;
//                break;
//
//                case 1 :
//                     // Wait for the signal to complete
//                     if ( DRV_I2C0_WaitForStartComplete()) {
//                        pDSM->I2cSeqSM = 0;
//                        pDSM->I2cMainSM = I2C_XSM_Ready;
//                    }
//                break;
//            } // end switch SeqSM
//        break;
//        case I2C_XSM_Ready :
//              // Attente relancement
//        break;
//    } // end switch
//} // end I2C_SM_reStart
//
//
////------------------------------------------------------------------------------
//// I2C_SM__write
////
//// Réalisation du transfert d'un octet en séquence
//// Prévu pour appel cyclique
////
//// Le paramètre data est la valeur 8 bits à transmettre.
////
//// Fourni par référence le ack
//
//void I2C_SM_write(S_Descr_I2C_SM *pDSM, uint8_t data, bool *pAck )
//{
//   
//    switch (pDSM->I2cMainSM ) {
//
//        case I2C_XSM_Idle:
//            // demarre le traitement
//            pDSM->I2cMainSM = I2C_XSM_Busy;
//            pDSM->I2cSeqSM = 0;
//        break;
//
//        case I2C_XSM_Busy :
//            switch ( pDSM->I2cSeqSM) {
//                 case 0 :
//                    // // Wait for the bus to be idle (nécessaire aprés un reStart)
//                    if (DRV_I2C0_MasterBusIdle() ) {
//                         pDSM->I2cSeqSM = 1;
//                    }
//                break;
//                case 1 :
//                    // Wait for the transmitter to be ready
//                         // CHR cette étape n'existe pas dans le driver
//                         //  if (I2CTransmitterIsReady(ModuleId)) {
//
//                    // ajout sauvage N'EST pas nécessaire
//                    // Wait for the transmitter to be ready
//                    // while( PLIB_I2C_TransmitterIsReady(I2C_ID_2) == false);
//
//                    // Transmit the byte
//                    if (DRV_I2C0_ByteWrite(data) == false)
//                    {
//                               pDSM->DebugCode = 3;
//                    }
//                    pDSM->I2cSeqSM = 2;
//                break;
//
//                case 2 :
//                    // Wait for byte write completion
//                    if(DRV_I2C0_WaitForByteWriteToComplete() ) {
//                        // on peut obtenir Ack
//                        *pAck = DRV_I2C0_WriteByteAcknowledged();
//                        pDSM->I2cSeqSM = 0;
//                        pDSM->I2cMainSM = I2C_XSM_Ready;
//                    }
//                break;
//
//            } // end switch SeqSM
//        break;
//        case I2C_XSM_Ready :
//              // Attente relancement
//        break;
//    } // end switch
//  
//} // end I2C_SM__write
//
//
//
//
//
////------------------------------------------------------------------------------
//// I2C_SM_read  ( Pour appel cyclique)
////
//// La fonction retourne l'octet lu par référence.
//// Le paramètre ackTodo :
//// true  signifie qu'il faut effectuer l'acquittement.
//// false signifie qu'il ne faut pas effectuer l'acquittement.
////
//
//void I2C_SM_read(S_Descr_I2C_SM *pDSM, bool ackTodo, uint8_t *pData )
//{
//            
//    switch (pDSM->I2cMainSM ) {
//
//        case I2C_XSM_Idle:
//            // demarre le traitement
//            pDSM->I2cMainSM = I2C_XSM_Busy;
//            pDSM->I2cSeqSM = 0;
//        break;
//
//        case I2C_XSM_Busy :
//            switch ( pDSM->I2cSeqSM) {
//                case 0 :
//                    // Check for recieve overflow If OK Initiate clock to receive
//                    if(DRV_I2C0_SetUpByteRead() == false)
//                    {
//                         pDSM->DebugCode = 4;  // pour debug
//                    }
//                    pDSM->I2cSeqSM = 1;
//                break;
//
//                case 1 :
//                    // Wait until data availlable
//                    if (DRV_I2C0_WaitForReadByteAvailable() ) {
//                        *pData = DRV_I2C0_ByteRead();
//                        pDSM->I2cSeqSM = 2;
//                    }
//                break;
//
//                case 2 :
//                    // Wait until ready to ack
//                    if (ackTodo) {
//                        DRV_I2C0_MasterACKSend();
//                    } else {
//                        DRV_I2C0_MasterNACKSend();
//                    }
//                    pDSM->I2cSeqSM = 3;
//                    // ??? attente dans boucle                    
//                    // DRV_I2C0_WaitForACKOrNACKComplete();
//                    // pDSM->I2cSeqSM = 0;
//                    // pDSM->I2cMainSM = I2C_XSM_Ready;
//                    break;
//                    
//                case 3:                   
//                    // SCA changement Harmony 1.06 -> 1.08, la fonction DRV_I2C0_WaitForStopComplete
//                    // n'attend plus dans boucle
//                    if (DRV_I2C0_WaitForACKOrNACKComplete())
//                    {
//                        pDSM->I2cSeqSM = 0;
//                        pDSM->I2cMainSM = I2C_XSM_Ready;                    
//                    };
//                break;
//
//            } // end switch SeqSM
//        break;
//        case I2C_XSM_Ready :
//              // Attente relancement
//        break;
//    } // end switch
//    
//} // end I2C_SM_read
//
//
////------------------------------------------------------------------------------
//// I2C_SM_stop  ( Pour appel cyclique )
////
//// termine la transaction I2C master
////
////------------------------------------------------------------------------------
//
//void I2C_SM_stop(S_Descr_I2C_SM *pDSM)
//{
//    
//    switch (pDSM->I2cMainSM ) {
//
//        case I2C_XSM_Idle:
//            // demarre le traitement
//            pDSM->I2cMainSM = I2C_XSM_Busy;
//            pDSM->I2cSeqSM = 0;
//        break;
//
//        case I2C_XSM_Busy :
//            switch ( pDSM->I2cSeqSM) {
//                case 0 :
//                    // Disable receiver and stop I2C
//                    if ( DRV_I2C0_MasterStop() ) {
//                         pDSM->I2cSeqSM = 1;
//                    }
//                    // si pas OK on repete le stop
//                break;
//
//                case 1 :
//                    // Wait for the signal to complete
//                    // boucle d'attente dans la fonction
//                    //DRV_I2C0_WaitForStopComplete();
//                    //pDSM->I2cSeqSM = 0;
//                    //pDSM->I2cMainSM = I2C_XSM_Ready;
//                    
//                    // SCA changement Harmony 1.06 -> 1.08, la fonction DRV_I2C0_WaitForStopComplete
//                    // n'attend plus dans boucle
//                    // Wait for the signal to complete
//                    if (DRV_I2C0_WaitForStopComplete())
//                    {
//                        pDSM->I2cSeqSM = 0;
//                        pDSM->I2cMainSM = I2C_XSM_Ready; 
//                    }
//                break;
//            } // end switch SeqSM
//        break;
//        case I2C_XSM_Ready :
//              // Attente relancement
//        break;
//    } // end switch
//   
//} // end I2C_SM_stop