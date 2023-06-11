//-----------------------------------------------------------
// File Name    : rtcc_driver.h                             |
// Project Name : 1920B Multiprise Minuterie PLC            |
// Version      : V1                                        |
// Date         : 07.06.2023                                |
// Author       : Alexandre Steffen                         |
//-----------------------------------------------------------

#include "Mc32_I2cUtilCCS.h"

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Constants                                                         */
/* ************************************************************************** */
/* ************************************************************************** */

//Define pour la RTCC
#define MCP79411_RTCC_W     0xDE //Adresse SRAM (1101 111) + Write (0)
#define MCP79411_RTCC_R     0xDF //Adresse SRAM (1101 111) + Read (1)

#define MCP79411_RTCC_ADDR_SEC 0x00
#define MCP79411_RTCC_ADDR_MIN 0x01
#define MCP79411_RTCC_ADDR_HOUR 0x02
#define MCP79411_RTCC_ADDR_WEEKDAY 0x03
#define MCP79411_RTCC_ADDR_DATE 0x04
#define MCP79411_RTCC_ADDR_MONTH 0x05
#define MCP79411_RTCC_ADDR_YEAR 0x06
#define MCP79411_RTCC_ADDR_CONTROL 0x07


#define MCP79411_RTCC_ADDR_ALARM0_SEC 0x0A
#define MCP79411_RTCC_ADDR_ALARM0_MIN 0x0B
#define MCP79411_RTCC_ADDR_ALARM0_HOUR 0x0C
#define MCP79411_RTCC_ADDR_ALARM0_DAY 0x0D	//Contains also configs for alarm reaction

#define MCP79411_RTCC_ADDR_ALARM1_SEC 0x11
#define MCP79411_RTCC_ADDR_ALARM1_MIN 0x12
#define MCP79411_RTCC_ADDR_ALARM1_HOUR 0x13
#define	MCP79411_RTCC_ADDR_ALARM1_DAY 0x14	//Contains also configs for alarm reaction

#define RTCC_ALARM_TRIGER_REGISTER 0x10 //APLxPOL + ALMxC2:0

#define RTCC_ALARM0 false
#define RTCC_ALARM1 true

#define ALARM_WARN_MIN	7
#define ALARM_WARN_HOUR 19
#define ALARM_SHUTDOWN_MIN 8
#define ALARM_SHUTDOWN_HOUR 19

//bit 7 = enable
#define MCP79411_RTCC_CONF_ENABLE_OSCI 0x80
#define MCP79411_RTCC_CONF_DISABLE_OSCI 0x00

typedef struct
{
	uint8_t alarmWarnMin;
	uint8_t alarmWarnHour;
	uint8_t alarmShutdownMin;
	uint8_t alarmShutdownHour;
}ALARM_DATA;

void RTCC_Init(void);
void RTCC_Start(void);
void RTCC_Stop(void);
void RTCC_ClearTime(void);

void RTCC_SetTime(uint8_t second, uint8_t minute, uint8_t hour, uint8_t weekDay, uint8_t date, uint8_t month, uint8_t year);
void RTCC_WriteTime(uint8_t value, uint8_t registerAddress, bool pausedOscillator);

uint8_t RTCC_ReadHour(uint8_t registerAddress);
uint8_t RTCC_ReadMin(uint8_t registerAddress);

bool RTCC_Alarm_ReadTriggerFlag(uint8_t registerAddress);
void RTCC_Alarm_ClearTriggerFlag(uint8_t registerAddress);
void RTCC_Alarms_Init(void);
void RTCC_Alarms_Enable_Interupt(void);
void RTCC_Alarm_SetTrigger(uint8_t value, uint8_t registerAddress);
