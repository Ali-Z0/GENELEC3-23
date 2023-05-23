//--------------------------------------------------------
//	Mc32_DS18B20.c
//--------------------------------------------------------
//	Description :	Fonctions pour DS18B20
//                      ( en utilisant le DS2482-100 )
//	Auteur 		: 	C. HUBER
//      Date            :       22.05.2014
//	Version		:	V1.0
//	Compilateur	:	XC32 V1.31
// Modifications :
//
/*--------------------------------------------------------*/


#include "bsp.h"
#include "Mc32_DS18b20.h"
#include "Mc32_I2cUtilCCS.h"
#include "Mc32Delays.h"

// définitions pour les DS2482-100
//
#define ds2482_100_write  0x30 // adresse ds2482-100 = 0,0,1,1,ad2,ad1,ad0 et rd/_wr = 0
#define ds2482_100_read   0x31 // adresse ds2482-100 = 0,0,1,1,ad2,ad1,ad0 et rd/_wr = 1


// reset
#define ds2482_100_reset_code 0xf0 // code de reset du ds2482-100
#define ds2482_100_one_wire_device_reset 0xb4 // envoi d'un reset/presence sur la ligne one wire
// configuration
#define ds2482_100_write_config_code 0xd2 // code de commande pour écrire le byte de configuration
#define ds2482_100_config_byte_spu 0b10100101 // 1ws = 1 wire speed = standard (low) = 0
                                              // SPU = strong pullup = 1
                                              // PPM = presence pulse masking = 0
                                              // APU = active pullup = 1
#define ds2482_100_config_byte_nospu 0b11100001 // 1ws = 1 wire speed = standard (low) = 0
                                                // SPU = strong pullup = 0
                                                // PPM = presence pulse masking = 0
                                                // APU = active pullup = 1

// set read pointer
#define ds2482_100_set_read_pointer_code 0xe1 // code de commande pour positionner le pointeur de lecture
#define ds2482_100_set_read_pointer_to_status_register 0xf0 // positionne le pointeur de lecture sur le status register
#define ds2482_100_set_read_pointer_to_read_data_register 0xe1 // positionne le pointeur de lecture sur le read data register
#define ds2482_100_set_read_pointer_to_channel_selection_register 0xd2 // positionne le pointeur de lecture sur le channel selection register
#define ds2482_100_set_read_pointer_to_configuration_register 0xf0 // positionne le pointeur de lecture sur le configuration register

// write byte to 1 wire
#define ds2482_100_1wire_write_byte_code 0xa5 // code de commande pour écrire un byte sur le ds2482-100

// read byte to 1 wire
#define ds2482_100_1wire_read_byte_code 0x96 // code de commande pour lire un byte sur le ds2482-100
                                             // et dépose le résultat de la lecture dans le read data register
// write (and read) single bit
#define ds2482_100_1wire_single_bit_code 0x87 // code de commande pour écrire un bit slot sur la ligne one wire
                                              // on passe en paramètre la valeur du bit dans le msb
                                              // et on retrouve la valeur du bit (soit écrite soit lue par de ds2482)
                                              // dans le bit SBR du byte de status
#define ds2482_100_1wire_triplet_code 0x78 // code de commande pour réaliser la séquence complèle
                                              // on passe en paramètre la valeur du bit à officialiser en cas de problème dans le msb
                                              // et on retrouve la valeur du bit (soit écrite soit lue par de ds2482)
                                              // dans le bit SBR du byte de status
#define ds2482_100_1wire_single_bit_0 0x00       // valeur pour un single bit à 0
#define ds2482_100_1wire_single_bit_1 0x80       // valeur pour un single bit à 1

// définitions one wire pour les commandes
#define one_wire_read_rom_command_code 0x33
#define one_wire_match_rom_command_code 0x55
#define one_wire_search_rom_command_code 0xf0
#define one_wire_skip_rom_command_code 0xcc
#define one_wire_no_command_code 0xff

// définitions pour les sondes de température ds18b20
#define ds18b20_family_code 0x28
#define ds18b20_convert_T_command_code 0x44
#define ds18b20_copy_scratchpad_command_code 0x48
#define ds18b20_write_scratchpad_command_code 0x4e
#define ds18b20_read_scratchpad_command_code 0xbe
#define ds18b20_th_value 0x0
#define ds18b20_tl_value 0x0
#define ds18b20_config_byte 0b01111111 // 12 bits résolution
#define ds18b20_ok_status 0 // tout est ok
#define ds18b20_shorted 1 // ligne one wire court-circuitée
#define ds18b20_missing 2 // pas de capteur
#define ds18b20_temp_reading_crc_error 3 // erreur de crc lors de la lecture de la température
#define ds18b20_reset_condition 4 // condition de reset 85.0 °C
#define ds18b20_ident_code_reading_crc_error 5 // erreur de crc lors de la lecture du code d'identification

// définition de la température par défaut au power-up
// cette valeur n'est transmise qu'avant le premier accès à un canal de sonde
#define power_up_temp_value (200) // 20°C

typedef uint8_t byte;

// pour ds2482-100
byte ds2482_100_status;


// Premièrement une structure pour le scratchpad du DS18b20
typedef struct
       {
         byte temp_lsb,temp_msb,r2,r3,r4,r5,r6,r7,crc;
       } t_ds18b20_scratchpad;



// Quatrièmement une union pour pouvoir accéder à 16bits soit en 2 bytes, soit en 16bits, soit en 16 bits signés
typedef union {
                 struct
                 {
                   byte lsb,msb;
                 } octet;
                 uint16_t word;
                 int16_t signed_word;
              } t_16bits;

t_16bits new_temp;

typedef struct
{
         byte Ds18b20Ident[8]; //64bits: 8bits family code, 48bits unique code and 8 bits crc
         t_ds18b20_scratchpad Ds18b20_scratchpad; //scratchpad pour le ds18b20
         byte Ds18b20_status; // statut du ds18b20 selon définitions du h file
         t_16bits Temp16; // valeur de la température du ds18b20 ou de la Pt1k au seizième de degré
         t_16bits Last_temp16; // ancienne valeur de la température (pour ds18b20) au seizième de degré
        
} t_sensor;
//
// Nous allons avoir besoin d'une de ces structure
t_sensor sensor;
// Les trois capteurs étant accédés par des chips différents, on doit mettre en
// place une structure pour pouvoir les adresser individuellement dans une routine
// unique
uint8_t sensor_select = 0; //variable permettant de savoir à quel capteur on s'adresse
                    // 0=T1, 1=T2, 2=TRH
// nous avons également besoin de connaître les adresses des différents capteurs
// ces adresses (plus le bit de wr ou rd) sont sous la forme de bytes
uint8_t ds2482_100_write_address = ds2482_100_write;
// resp les adresses à utiliser pour l'écriture dans le chip de contrôle de T1, T2, TRH 
uint8_t ds2482_100_read_address = ds2482_100_read;
// resp les adresses à utiliser pour la lecture dans le chip de contrôle de T1, T2, TRH 

// autres variables pour la gestion du one wire
byte *ident_string_starting_address; // pointeur sur le début du string d'identification
byte *ident_string_ptr; // pointeur sur le string d'identification
byte one_wire_cpt_ident_bit; // compteur de bits pour les 64bits de l'identifiant
byte one_wire_cpt_ident_bit_action; // compteur des actions pour chaque bit
byte one_wire_match_rom_byte; // byte servant à envoyer les 64 bits un à un
const byte bit_mask[] = {1,2,4,8,16,32,64,128};
byte one_wire_crc; // pour le calcul du crc one wire
bool one_wire_search_rom_finish; // bit indiquant la fin de la fonction search rom
bool one_wire_search_rom_crc_ok; // bit indiquant que le crc reçu à la fin de match rom est ok
bool one_wire_match_rom_finish; // bit indiquant la fin de la fonction match rom
byte *ds18b20_scratchpad_ptr; // pointeur sur le début du scratchpad du ds18b20
byte ds18b20_read_scratchpad_action; // indique si l'on demande un byte ou si l'on lit un byte
byte ds18b20_read_scratchpad_nb_bytes; // nb de bytes encore à lire du scratchpad
byte *ds2438_scratchpad_ptr; // pointeur sur le début du scratchpad du ds2438
byte ds2438_read_scratchpad_action; // indique si l'on demande un byte ou si l'on lit un byte
byte ds2438_read_scratchpad_nb_bytes; // nb de bytes encore à lire du scratchpad


uint8_t config_switch; // variable de lecture des 4 switchs de configuration des tensions de sortie

typedef enum {one_wire_global_first, // première prise de contact avec la ligne one wire, y a-t-il quelqu'un?
              one_wire_ds18b20, // on traite le ds18b20
              one_wire_ds2438, // on traite le ds2438
              one_wire_ds2450, // on traite le ds2450
              one_wire_global_last  // on traite les résultats déterminés précédemment
             } t_sense_action_enum;
t_sense_action_enum t_sense_action;
             
typedef enum {one_wire_reset_chip,
              one_wire_reset_presence_pulse_0, // première pulse reset/presence sur le capteur
              one_wire_charge_cap_cde,
              one_wire_charge_cap_wait,
              one_wire_reset_presence_pulse_1, // nouvelle pulse reset/presence sur le capteur
              one_wire_sensor_status,  // y a-t-il une sonde sur la ligne
              one_wire_ds18b20_search_rom_command, // search rom command pour la sonde de température ds18b20
              one_wire_ds18b20_search_rom, // search rom pour la sonde de température ds18b20
              one_wire_ds18b20_reset_presence_pulse_1, // après search rom il faut une nouvelle pulse de reset presence
              one_wire_ds18b20_match_rom_command_1,
              one_wire_ds18b20_match_rom_1,
              one_wire_ds18b20_convert_T_command,
              one_wire_ds18b20_first_wait_end_of_conversion,
              one_wire_ds18b20_second_wait_end_of_conversion,
              one_wire_ds18b20_reset_presence_pulse_2,
              one_wire_ds18b20_match_rom_command_2,
              one_wire_ds18b20_match_rom_2,
              one_wire_ds18b20_read_scratchpad_command,
              one_wire_ds18b20_read_scratchpad,
              one_wire_ds18b20_read_scratchpad_check_crc_and_store_value,
              one_wire_ds2438_charge_cap_cde_1,
              one_wire_ds2438_charge_cap_wait_1,
              one_wire_reset_presence_pulse_2, // nouvelle pulse de reset présence pour resetter tous les chips
              one_wire_re_config_ds2438_search_rom_command, // search rom command pour le ds2438
              one_wire_re_config_ds2438_search_rom, // search rom pour le ds2438
              one_wire_re_config_ds2438_reset_presence_pulse_2, // après search rom il faut une nouvelle pulse de reset presence
              one_wire_ds2438_reset_presence_pulse_1,
              one_wire_ds2438_match_rom_command_1,
              one_wire_ds2438_match_rom_1,
              one_wire_ds2438_write_scratchpad_command_Vdd, // écrit la configuration du ds2438 dans le scratchpad
              one_wire_ds2438_write_scratchpad_to_page_0_Vdd,
              one_wire_ds2438_select_vdd,
              one_wire_ds2438_reset_presence_pulse_1a, // on ne veut pas écrire le reste, on reset
              one_wire_ds2438_match_rom_command_1a,
              one_wire_ds2438_match_rom_1a,
              one_wire_ds2438_copy_scratchpad_command_Vdd, // écrit la configuration du ds2438 dans le scratchpad
              one_wire_ds2438_copy_scratchpad_to_page_0,
              one_wire_ds2438_copy_scratchpad_Vdd_wait_10to20ms,
              one_wire_ds2438_reset_presence_pulse_2,
              one_wire_ds2438_match_rom_command_2,
              one_wire_ds2438_match_rom_2,
              one_wire_ds2438_convert_V_command_1, // lancement de la conversion de vdd
              one_wire_ds2438_wait_10to20ms_for_vdd_conversion,
              one_wire_ds2438_reset_presence_pulse_2a, // à la fin de la conversion,nouvelle pulse de reset-présence
              one_wire_ds2438_match_rom_command_2a,
              one_wire_ds2438_match_rom_2a,
              one_wire_ds2438_recall_memory_page_command_Vdd,
              one_wire_ds2438_recall_memory_page_0_Vdd,
              one_wire_ds2438_wait_10to20ms_for_recall_memory_Vdd,
              one_wire_ds2438_reset_presence_pulse_3,
              one_wire_ds2438_match_rom_command_3,
              one_wire_ds2438_match_rom_3,
              one_wire_ds2438_read_scratchpad_command_Vdd,
              one_wire_ds2438_tx_page_0_Vdd,
              one_wire_ds2438_read_scratchpad_Vdd,
              one_wire_ds2438_read_scratchpad_check_crc_and_store_Vdd_value,
              one_wire_ds2438_reset_presence_pulse_4,
              one_wire_ds2438_match_rom_command_4,
              one_wire_ds2438_match_rom_4,
              one_wire_ds2438_write_scratchpad_command_Vad, // écrit la configuration du ds2438 dans le scratchpad
              one_wire_ds2438_write_scratchpad_to_page_0_Vad,
              one_wire_ds2438_select_vad,
              one_wire_ds2438_reset_presence_pulse_5, // on ne veut pas écrire le reste, on reset
              one_wire_ds2438_match_rom_command_5,
              one_wire_ds2438_match_rom_5,
              one_wire_ds2438_copy_scratchpad_command_Vad, // écrit la configuration du ds2438 dans le scratchpad
              one_wire_ds2438_copy_scratchpad_to_page_0_Vad,
              one_wire_ds2438_copy_scratchpad_Vad_wait_10to20ms,
              one_wire_ds2438_reset_presence_pulse_6,
              one_wire_ds2438_match_rom_command_6,
              one_wire_ds2438_match_rom_6,
              one_wire_ds2438_convert_V_command_2, // lancement de la conversion de vdd
              one_wire_ds2438_wait_10to20ms_for_vad_conversion,
              one_wire_ds2438_reset_presence_pulse_7, // à la fin de la conversion,nouvelle pulse de reset-présence
              one_wire_ds2438_match_rom_command_7,
              one_wire_ds2438_match_rom_7,
              one_wire_ds2438_recall_memory_page_command_Vad,
              one_wire_ds2438_recall_memory_page_0_Vad,
              one_wire_ds2438_wait_10to20ms_for_recall_memory_Vad,
              one_wire_ds2438_reset_presence_pulse_8,
              one_wire_ds2438_match_rom_command_8,
              one_wire_ds2438_match_rom_8,
              one_wire_ds2438_read_scratchpad_command_Vad,
              one_wire_ds2438_tx_page_0_Vad,
              one_wire_ds2438_read_scratchpad_Vad,
              one_wire_ds2438_read_scratchpad_check_crc_and_store_Vad_value,
              one_wire_compute_RH,
              one_wire_ds2450_charge_cap_cde_1,
              one_wire_ds2450_charge_cap_wait_1,
              one_wire_reset_presence_pulse_3, // pulse de reset pour resetter tous les chips one wire
              one_wire_re_config_ds2450_search_rom_command, // search rom command pour le ds2450
              one_wire_re_config_ds2450_search_rom, // search rom pour le ds2450
              one_wire_ds2450_charge_cap_cde_1bis,
              one_wire_ds2450_charge_cap_wait_1bis,
              one_wire_ds2450_write_memory_A08_A0f,
              one_wire_ds2450_conversions_start,
              one_wire_ds2450_reset_presence_pulse_1a,
              one_wire_ds2450_match_rom_command_1a,
              one_wire_ds2450_match_rom_1a,
              one_wire_ds2450_convert_command,
              one_wire_ds2450_convert_command_charge_cap_wait,
              one_wire_ds2450_convert_input_select_mask,
              one_wire_ds2450_convert_readout_control,
              one_wire_ds2450_convert_read_back_crc16_lsb_cde,
              one_wire_ds2450_convert_read_back_crc16_lsb,
              one_wire_ds2450_convert_read_back_crc16_msb,
              one_wire_ds2450_convert_wait_cde,
              one_wire_ds2450_convert_wait,
              one_wire_ds2450_after_convert_charge_cap_cde,
              one_wire_ds2450_after_convert_charge_cap_wait,
              one_wire_ds2450_reset_presence_pulse_2,
              one_wire_ds2450_match_rom_command_2,
              one_wire_ds2450_match_rom_2,
              one_wire_ds2450_read_memory_cde,
              one_wire_ds2450_read_memory_TA1,
              one_wire_ds2450_read_memory_TA2,
              one_wire_ds2450_read_memory,
              one_wire_ds2450_read_memory_ask_crc_lsb,
              one_wire_ds2450_read_memory_crc_lsb_ask_crc_msb,
              one_wire_ds2450_read_memory_crc_msb_and_check_crc,
              one_wire_ds2450_read_memory_reset_presence_pulse,
              one_wire_ds2450_affect_conv_values,
              one_wire_ds2450_conversions_stop,
              one_wire_ds2450_compute_T,
              one_wire_ds2450_last_reset,
              one_wire_ds2450_last_charge_cap,
              Compute_And_output_values,
              one_wire_next_channel
             } t_sense_enum;
t_sense_enum t_sense;

const byte ds18b20_crc_table[256] =
    { 0,94,188,226,97,63,221,131,194,156,126,32,163,253,31,65,
      157,195,33,127,252,162,64,30,95,1,227,189,62,96,130,220,
      35,125,159,193,66,28,254,160,225,191,93,3,128,222,60,98,
      190,224,2,92,223,129,99,61,124,34,192,158,29,67,161,255,
      70,24,250,164,39,121,155,197,132,218,56,102,229,187,89,7,
      219,133,103,57,186,228,6,88,25,71,165,251,120,38,196,154,
      101,59,217,135,4,90,184,230,167,249,27,69,198,152,122,36,
      248,166,68,26,153,199,37,123,58,100,134,216,91,5,231,185,
      140,210,48,110,237,179,81,15,78,16,242,172,47,113,147,205,
      17,79,173,243,112,46,204,146,211,141,111,49,178,236,14,80,
      175,241,19,77,206,144,114,44,109,51,209,143,12,82,176,238,
      50,108,142,208,83,13,239,177,240,174,76,18,145,207,45,115,
      202,148,118,40,171,245,23,73,8,86,180,234,105,55,213,139,
      87,9,235,181,54,104,138,212,149,203,41,119,244,170,72,22,
      233,183,85,11,136,214,52,106,43,117,151,201,74,20,246,168,
      116,42,200,150,21,75,169,247,182,232,10,84,215,137,107,53 };
//
// table de constantes pour le contrôle (calcul) du crc msb 
const byte frame_crc_high_table[256] =
    { 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81,
      0x40, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0,
      0x80, 0x41, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x00, 0xc1, 0x81, 0x40, 0x01,
      0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x01, 0xc0, 0x80, 0x41,
      0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x00, 0xc1, 0x81,
      0x40, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x01, 0xc0,
      0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x01,
      0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40,
      0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81,
      0x40, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0,
      0x80, 0x41, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x00, 0xc1, 0x81, 0x40, 0x01,
      0xc0, 0x80, 0x41, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41,
      0x00, 0xc1, 0x81, 0x40, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81,
      0x40, 0x01, 0xc0, 0x80, 0x41, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0,
      0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x01,
      0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81, 0x40, 0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41,
      0x00, 0xc1, 0x81, 0x40, 0x01, 0xc0, 0x80, 0x41, 0x01, 0xc0, 0x80, 0x41, 0x00, 0xc1, 0x81,
      0x40
    };
// table de constantes pour le contrôle (calcul) du crc lsb 
const byte frame_crc_low_table[256] =
    { 0x00, 0xc0, 0xc1, 0x01, 0xc3, 0x03, 0x02, 0xc2, 0xc6, 0x06, 0x07, 0xc7, 0x05, 0xc5, 0xc4,
      0x04, 0xcc, 0x0c, 0x0d, 0xcd, 0x0f, 0xcf, 0xce, 0x0e, 0x0a, 0xca, 0xcb, 0x0b, 0xc9, 0x09,
      0x08, 0xc8, 0xd8, 0x18, 0x19, 0xd9, 0x1b, 0xdb, 0xda, 0x1a, 0x1e, 0xde, 0xdf, 0x1f, 0xdd,
      0x1d, 0x1c, 0xdc, 0x14, 0xd4, 0xd5, 0x15, 0xd7, 0x17, 0x16, 0xd6, 0xd2, 0x12, 0x13, 0xd3,
      0x11, 0xd1, 0xd0, 0x10, 0xf0, 0x30, 0x31, 0xf1, 0x33, 0xf3, 0xf2, 0x32, 0x36, 0xf6, 0xf7,
      0x37, 0xf5, 0x35, 0x34, 0xf4, 0x3c, 0xfc, 0xfd, 0x3d, 0xff, 0x3f, 0x3e, 0xfe, 0xfa, 0x3a,
      0x3b, 0xfb, 0x39, 0xf9, 0xf8, 0x38, 0x28, 0xe8, 0xe9, 0x29, 0xeb, 0x2b, 0x2a, 0xea, 0xee,
      0x2e, 0x2f, 0xef, 0x2d, 0xed, 0xec, 0x2c, 0xe4, 0x24, 0x25, 0xe5, 0x27, 0xe7, 0xe6, 0x26,
      0x22, 0xe2, 0xe3, 0x23, 0xe1, 0x21, 0x20, 0xe0, 0xa0, 0x60, 0x61, 0xa1, 0x63, 0xa3, 0xa2,
      0x62, 0x66, 0xa6, 0xa7, 0x67, 0xa5, 0x65, 0x64, 0xa4, 0x6c, 0xac, 0xad, 0x6d, 0xaf, 0x6f,
      0x6e, 0xae, 0xaa, 0x6a, 0x6b, 0xab, 0x69, 0xa9, 0xa8, 0x68, 0x78, 0xb8, 0xb9, 0x79, 0xbb,
      0x7b, 0x7a, 0xba, 0xbe, 0x7e, 0x7f, 0xbf, 0x7d, 0xbd, 0xbc, 0x7c, 0xb4, 0x74, 0x75, 0xb5,
      0x77, 0xb7, 0xb6, 0x76, 0x72, 0xb2, 0xb3, 0x73, 0xb1, 0x71, 0x70, 0xb0, 0x50, 0x90, 0x91,
      0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9c, 0x5c,
      0x5d, 0x9d, 0x5f, 0x9f, 0x9e, 0x5e, 0x5a, 0x9a, 0x9b, 0x5b, 0x99, 0x59, 0x58, 0x98, 0x88,
      0x48, 0x49, 0x89, 0x4b, 0x8b, 0x8a, 0x4a, 0x4e, 0x8e, 0x8f, 0x4f, 0x8d, 0x4d, 0x4c, 0x8c,
      0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
      0x40
    };
// variables pour le contrôle (calcul) du crc
byte crc16_high;
byte crc16_low;
byte* crc16_byte_ptr;
byte crc16_data_len;
// byte crc16_data[12];
uint16_t Timer0Reload;

//10ms tics wait temp conversion completion
bool temp_bit_10ms_tics_count = false; // enables / disables 10ms temp tics counting
//10 ms temp tics counter
byte temp_ten_ms_tics;                    // nb of 10ms tics to wait
//10ms tics wait voltage conversion completion
bool Vconv_bit_10ms_tics_count = false; // enables / disables 10ms temp tics counting
//10 ms Vconv tics counter
byte Vconv_ten_ms_tics;                    // nb of 10ms tics to wait
//10ms tics wait recall memory completion
bool Recall_mem_bit_10ms_tics_count = false; // enables / disables 10ms temp tics counting
//10 ms recall memory tics counter
byte Recall_mem_ten_ms_tics;                    // nb of 10ms tics to wait


bool zero_offset_tic; // 1 tic toutes les 500ms
byte zero_10ms_tics; // 10ms tics pour l'ajustage du zéro
bool zero_10ms_tics_count; //enables / disables zero adjust tics counting
//
//
byte scratch;                    // byte à tout faire

//
// Fonction pour action One-Wire par le DS2482_100
//

/***********************************************************************************/
void ds2482_100_write_one_byte(uint8_t write_and_dest_chip, uint8_t byte0) {
  i2c_start();
  i2c_write(write_and_dest_chip);
  i2c_write(byte0);
  i2c_stop();
}

/***********************************************************************************/
void ds2482_100_write_two_bytes(uint8_t write_and_dest_chip, uint8_t byte1, uint8_t byte2) {
  i2c_start();
  i2c_write(write_and_dest_chip);
  i2c_write(byte1);
  i2c_write(byte2);
  i2c_stop();
}
/***********************************************************************************/
void ds2482_100_write_three_bytes(uint8_t write_and_dest_chip, uint8_t byte1, uint8_t byte2, uint8_t byte3) {
  i2c_start();
  i2c_write(write_and_dest_chip);
  i2c_write(byte1);
  i2c_write(byte2);
  i2c_write(byte3);
  i2c_stop();
}
/***********************************************************************************/
byte ds2482_100_read_one_wire_byte(uint8_t read_and_source_chip){
   byte ret_byte;
  // read byte
  i2c_start();
  i2c_write(read_and_source_chip);
  ret_byte = i2c_read(0); // no ack
  i2c_stop();
  return ret_byte;
}
/***********************************************************************************/
void one_wire_read_status(uint8_t read_and_source_chip) {
  // lecture du status de ds2482-800 ?
  i2c_start();
  i2c_write(read_and_source_chip);
  ds2482_100_status = i2c_read(0); // no ack
  i2c_stop();
}
/***********************************************************************************/
bool one_wire_channel_busy(uint8_t read_and_source_chip) {
  // y a-t-il une transmission en cours sur la ligne one wire ?
  bool fin;

  one_wire_read_status(read_and_source_chip);
  fin =  ((ds2482_100_status & 0x01) > 0); // retourne un 1 si une transmission est en cours
  return fin;
}
/***********************************************************************************/
void one_wire_end_xmit(uint8_t read_and_source_chip) {
  do {
  }
  while (one_wire_channel_busy(read_and_source_chip));
}
/*******************************************************************************************/
void one_wire_crc_computation(byte *ptr_on_byte, uint8_t nb_bytes) {
  uint8_t cnt_crc;
  one_wire_crc = 0; //initialisation
  for (cnt_crc = 0; cnt_crc < nb_bytes; cnt_crc++) {
    one_wire_crc = ds18b20_crc_table[*ptr_on_byte ^ one_wire_crc];
    ptr_on_byte++;
  }
}
/***********************************************************************************/
void one_wire_search_rom() {
  // on doit déterminer le code d'identification (64bits) du ds18b20
  // ce code est composé de (du lsb au msb de droite à gauche):
  // 8 bits family code (0x28 pour un ds18b20) puis48 bits d'identification unique puis 8 bits de crc

  // pour chaque bit, on doit d'abord lire un bit sur la ligne, puis son complément, puis déterminer
  // quel est le bit réel (utile en cas de multiples circuits du même type) puis écrire ce bit
  // Les 8 premiers bits sont connus, c'est le family code
  // Pour les autres, il faut y aller bit à bit
  // Et à la fin contrôler le crc

  // En entrant à cet endroit, on a à disposition:
  //byte *ident_string_starting_address; // pointeur sur le début du string d'identification
  //byte *ident_string_ptr; // pointeur sur le string d'identification
  //byte one_wire_cpt_ident_bit; // compteur de bits pour les 64bits de l'identifiant
  //byte one_wire_cpt_ident_bit_action; // compteur des actions pour chaque bit: 0=lire bit, 1=lire bit complément, 2=écrire le bit
  //byte one_wire_match_rom_byte; // byte servant à envoyer les 64 bits un à un
  //const byte bit_mask[] = {1,2,4,8,16,32,64,128};
  //byte one_wire_crc; // pour le calcul du crc one wire
  //bool one_wire_search_rom_finish, // bit indiquant la fin de la fonction search rom
  //bool one_wire_search_rom_crc_ok; // bit indiquant que le crc reçu à la fin de match rom est ok
  // Pour les 8 premiers bits, on sait ce qu'on veut écrire, c'est le family code sur lequel on pointe 
  // Mais on doit tout de même respecter la séquence
  if (one_wire_cpt_ident_bit <= 7) {
    ds2482_100_write_two_bytes(ds2482_100_write_address,ds2482_100_write_config_code, ds2482_100_config_byte_spu);
    switch (one_wire_cpt_ident_bit_action) {
      case 0: {
        // on doit lire un bit, donc on écrit un 1
        ds2482_100_write_two_bytes(ds2482_100_write_address,ds2482_100_1wire_single_bit_code, ds2482_100_1wire_single_bit_1);
        one_wire_cpt_ident_bit_action++;
      }
      break;
      case 1: {
        // on doit lire le complément de ce bit, donc on écrit un 1
        ds2482_100_write_two_bytes(ds2482_100_write_address,ds2482_100_1wire_single_bit_code, ds2482_100_1wire_single_bit_1);
        one_wire_cpt_ident_bit_action++;
      }
      break;
      case 2: {
        // il faut isoler et envoyer le bon bit
        if (*ident_string_starting_address & (bit_mask[one_wire_cpt_ident_bit])) {
          ds2482_100_write_two_bytes(ds2482_100_write_address,ds2482_100_1wire_single_bit_code, ds2482_100_1wire_single_bit_1);
        }
        else {
          ds2482_100_write_two_bytes(ds2482_100_write_address,ds2482_100_1wire_single_bit_code, ds2482_100_1wire_single_bit_0);
        }
        one_wire_cpt_ident_bit_action = 0;
        one_wire_cpt_ident_bit++;
      }
      break;
    }
  }
  else { // cpt_ident_bit > 7
    // Depuis ici, on ne s'adresse plus qu'au bon chip, il n'y a donc plus d'ambiguïté, on peut
    // sans autre utiliser le système standard du triplet pour les 7 bytes suivants (6 bytes serial number + 1 byte crc)
    // ici aussi on doit utiliser cpt_ident_bit_action car on doit d'abord réaliser ce triplet puis lire le résultat dans le status
    switch (one_wire_cpt_ident_bit_action) {
      case 0: {
        // on doit exécuter le triplet, pas de possibilité de spu
        ds2482_100_write_two_bytes(ds2482_100_write_address,ds2482_100_1wire_triplet_code, ds2482_100_1wire_single_bit_0);
        one_wire_cpt_ident_bit_action = 1;
      }
      break;
      case 1: {
        // on doit lire ce bit dans le status
        one_wire_read_status(ds2482_100_read_address);
        // rotate right of pointed byte
        ident_string_ptr = ident_string_starting_address + (one_wire_cpt_ident_bit / 8);
        *ident_string_ptr /= 2;
        if (ds2482_100_status & 0x20) { // keep only sbr
        // put the bit in the right place
          *ident_string_ptr += 128;
        }
        one_wire_cpt_ident_bit_action = 0;
        if (one_wire_cpt_ident_bit < 63) {
          one_wire_cpt_ident_bit++;
        }
        else { //one_wire_cpt_ident_bit >= 63
          one_wire_crc_computation(ident_string_starting_address,7);
          ident_string_ptr = ident_string_starting_address + 7; //crc
          if (one_wire_crc == *ident_string_ptr) { //crc ok?
            one_wire_search_rom_crc_ok = true;
          }
          else {
            // si le crc n'est pas bon, on l'indique
            one_wire_search_rom_crc_ok = false;
          }
          one_wire_search_rom_finish = true;
        }
      }
      break;
    }
  }
}

/***********************************************************************************/
void one_wire_match_rom() {
  // cette fonction effectue l'envoi des 64bits de l'ident sur lequel on pointe
  // on ne contrôle rien, le contrôle est implicitement fait plus loin en contrôlant
  // le crc des valeurs lues
  // on doit envoyer le string d'identification (64bits) du ds...
  // ce code est composé de (du lsb au msb de droite à gauche):
  // 8 bits family code (0x28 pour un ds18b20, 0x20 pour le ds2450, 0x26 pour le ds2438) puis48 bits d'identification unique puis 8 bits de crc

  // pour chaque bit, on doit d'abord déterminer sa valeur puis l'envoyer

  // En entrant à cet endroit, on a à disposition:
  //byte *ident_string_starting_address; // pointeur sur le début du string d'identification
  //byte *ident_string_ptr; // pointeur sur le string d'identification
  //byte one_wire_cpt_ident_bit; // compteur de bits pour les 64bits de l'identifiant
  //byte one_wire_cpt_ident_bit_action; // compteur des actions pour chaque bit: 0=lire bit, 1=lire bit complément, 2=écrire le bit
  //byte one_wire_match_rom_byte; // byte servant à envoyer les 64 bits un à un
  //const byte bit_mask[] = {1,2,4,8,16,32,64,128};
  //bool one_wire_match_rom_finish, // bit indiquant la fin de la fonction match rom
  // Pour les 8 premiers bits, on sait ce qu'on veut écrire, c'est le family code sur lequel on pointe 
  // Mais on doit tout de même respecter la séquence
  
  switch (one_wire_cpt_ident_bit_action) {
    case 0: { // envoi du bit
      // on pointe sur le bon byte
      ident_string_ptr = ident_string_starting_address + (one_wire_cpt_ident_bit / 8);
      // on charge ce bon byte
      one_wire_match_rom_byte = *ident_string_ptr;
      // il faut isoler et envoyer le bon bit
      ds2482_100_write_two_bytes(ds2482_100_write_address,ds2482_100_write_config_code, ds2482_100_config_byte_spu);
      if (one_wire_match_rom_byte & (bit_mask[one_wire_cpt_ident_bit % 8])) {
        ds2482_100_write_two_bytes(ds2482_100_write_address,ds2482_100_1wire_single_bit_code, ds2482_100_1wire_single_bit_1);
      }
      else {
        ds2482_100_write_two_bytes(ds2482_100_write_address,ds2482_100_1wire_single_bit_code, ds2482_100_1wire_single_bit_0);
      }
      one_wire_cpt_ident_bit++;
      // tous les 8 bits, on recharge la capacité
      if (one_wire_cpt_ident_bit %8 == 0) {
        temp_ten_ms_tics = 3; // nb of 10ms tics to wait 30ms (in fact 20ms to 30ms)
        temp_bit_10ms_tics_count = true;
        one_wire_cpt_ident_bit_action = 1;
      }
      if (one_wire_cpt_ident_bit > 63) {
        one_wire_match_rom_finish = true;
      }
    }
    break;
    case 1: { // attente de la recharge de la capacité
      if (temp_ten_ms_tics == 0) {
        temp_bit_10ms_tics_count = false;
        one_wire_cpt_ident_bit_action = 0;
      }
    }
    break;
  }
}
/***********************************************************************************/
void crc16_computation(void) {
// cette fonction calcule le crc du ds2450 selon les paramètres passés:
//byte crc16_high; // résultat du msb du crc
//byte crc16_low; // résultat du lsb du crc
//byte* crc16_byte_ptr; // pointeur sur le string
//byte crc16_data_len; // nombre de bytes pris en compte
byte crc16_index;
  while (crc16_data_len--) {
    crc16_index = crc16_low ^ *crc16_byte_ptr++;
    crc16_low = crc16_high ^ frame_crc_high_table[crc16_index];
    crc16_high = frame_crc_low_table[crc16_index];
  }
}

/***********************************************************************************/
void init_oneWire() {


  // initialisations des températures et des humidités
 
    sensor.Ds18b20Ident[0] = ds18b20_family_code, //0x28 ds18b20 family code
    sensor.Ds18b20_status = ds18b20_missing; // no temp sensor (ds18b20) on line

    sensor.Temp16.word = power_up_temp_value; // 20°C par défaut
    sensor.Last_temp16.word = power_up_temp_value; // 20°C par défaut
   
  // Initialisations pour la séquence de lecture de la température et de l'humidité
  sensor_select=0; //T1
  t_sense_action = one_wire_global_first;
  t_sense = one_wire_reset_chip;
  //Prepare et active tous les
  //modes d'interruptions
  // enable_interrupts(INT_RTCC);
  // enable_interrupts(GLOBAL);
}

/***********************************************************************************/
 
uint8_t Step;

void ReadDS18B20(uint8_t *Status, float *pTemp)
{
   float Temp = 21.5;
   uint8_t nb_bytes;
    
    Step = 0;
   // reset du DS2482
   ds2482_100_write_one_byte(ds2482_100_write, ds2482_100_reset_code);
   one_wire_end_xmit(ds2482_100_read_address);      
   
    Step = 1;
   // one wire reset/presence pulse 
   ds2482_100_write_one_byte(ds2482_100_write, ds2482_100_one_wire_device_reset);
   one_wire_end_xmit(ds2482_100_read_address); 
   
   one_wire_read_status(ds2482_100_read);
        switch (ds2482_100_status & 0x06) { // keep only sd and ppd
           case 4:
           case 6:  // line shorted
               sensor.Ds18b20_status = ds18b20_shorted; // short circuit (ds18b20) on line
           break;
           case 0:  // no sensor
               sensor.Ds18b20_status = ds18b20_missing; // no sensor on line (ds18b20)
           break;
           case 2:  // normal operation, not shorted and presence pulse ok
             sensor.Ds18b20_status = ds18b20_ok_status;
           break;              
        } // end switch Ds2482_100_status    
      *Status = ds2482_100_status & 0x06;
  
   
   if (*Status != 2) {
   
      goto ExitReadDs18b20;
   }
    Step = 2;
   // Commande 0XCC SKIP ROM
   ds2482_100_write_two_bytes(ds2482_100_write_address,ds2482_100_1wire_write_byte_code, one_wire_skip_rom_command_code);
   one_wire_end_xmit(ds2482_100_read_address);  
   
    Step = 3;
   // Force strong Pullup
   ds2482_100_write_two_bytes(ds2482_100_write_address,ds2482_100_write_config_code, ds2482_100_config_byte_spu);
   
   // Commande 0X44 début conversion T
   ds2482_100_write_two_bytes(ds2482_100_write_address,ds2482_100_1wire_write_byte_code, ds18b20_convert_T_command_code);
   one_wire_end_xmit(ds2482_100_read_address);  
   Step = 4;
   
   
   // Attente bit recu = 0
   // L'attente n'est possible que si circuit alimenté
   // BSP_LEDOn(BSP_LED_7);  // provisoire : pour observation
   delay_ms(750);  // fait 720 ms en pratique
   // BSP_LEDOff(BSP_LED_7);  // provisoire : pour observation

   ds2482_100_write_two_bytes(ds2482_100_write_address,ds2482_100_write_config_code, ds2482_100_config_byte_nospu);
   //one_wire_end_xmit(ds2482_100_read_address); 
   
   Step = 5;
   // ds2482_100_write_one_byte(ds2482_100_write, ds2482_100_reset_code);
   // one_wire_end_xmit(ds2482_100_read_address);  
    
    // one wire reset/presence pulse 
   ds2482_100_write_one_byte(ds2482_100_write, ds2482_100_one_wire_device_reset);
   one_wire_end_xmit(ds2482_100_read_address);  
    Step = 6;
   // Commande 0xCC SKIP ROM
   ds2482_100_write_two_bytes(ds2482_100_write_address,ds2482_100_1wire_write_byte_code, one_wire_skip_rom_command_code);
   one_wire_end_xmit(ds2482_100_read_address); 
   Step = 7;
   
   // Commande 0xBE read scratchpad
   // on va lire 8 bytes de données et un crc, soit 9 bytes
   // de manière à gagner des bytes de code, on va le faire d'un coup
   ds2482_100_write_two_bytes(ds2482_100_write, ds2482_100_1wire_write_byte_code, ds18b20_read_scratchpad_command_code);
   one_wire_end_xmit(ds2482_100_read_address); 
   Step = 8;
   ds18b20_scratchpad_ptr = &sensor.Ds18b20_scratchpad.temp_lsb;
  
     // on doit gérer le séquencement des actions de lecture
         // 1) demander la lecture d'un byte au ds18b20
         // 2) lire le byte et demander le suivant              
   for (nb_bytes = 0 ; nb_bytes < 9 ; nb_bytes ++) {
   
      // Envoi la commande la lecture d'un byte
      ds2482_100_write_one_byte(ds2482_100_write,ds2482_100_1wire_read_byte_code);
      one_wire_end_xmit(ds2482_100_read_address); 
      
      Step = 20 + nb_bytes;
      // Effectue la lecture d'un byte
         // Pointeur DS2482 sur Read data
         // lecture dans le DS2482
      ds2482_100_write_two_bytes(ds2482_100_write,ds2482_100_set_read_pointer_code, ds2482_100_set_read_pointer_to_read_data_register);
      *ds18b20_scratchpad_ptr = ds2482_100_read_one_wire_byte(ds2482_100_read_address);
       Step = 30 + nb_bytes;
      // Remettre pointeur sur le Status pour attendre fin action
      ds2482_100_write_two_bytes(ds2482_100_write,ds2482_100_set_read_pointer_code, ds2482_100_set_read_pointer_to_status_register);
      one_wire_end_xmit(ds2482_100_read_address); 
      Step = 40 + nb_bytes;
      ds18b20_scratchpad_ptr++; // pointe sur le byte suivant
      
   }   
    
   // Expression de la température en degré
   new_temp.octet.msb = sensor.Ds18b20_scratchpad.temp_msb;
   new_temp.octet.lsb = sensor.Ds18b20_scratchpad.temp_lsb;
   Temp = new_temp.signed_word * 0.0625;
ExitReadDs18b20:   
   *pTemp = Temp;
}
 
 

