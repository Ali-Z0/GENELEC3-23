//-----------------------------------------------------------
// File Name    : rtcc_driver.c                             |
// Project Name : 1920B Multiprise Minuterie PLC            |
// Version      : V1                                        |
// Date         : 07.06.2023                                |
// Author       : Alexandre Steffen                         |
//-----------------------------------------------------------

#include "rtcc_driver.h"
//#include "app.h"


void RTCC_Init(void)
{
   bool Fast = true;
   i2c_init(Fast);
}


void RTCC_Start(void)
{
    bool ack;
    do{
        i2c_start();
        ack = i2c_write(MCP79411_RTCC_W);        
    }while(!ack);
    i2c_write(MCP79411_RTCC_ADDR_SEC); //Adresse
    i2c_write(MCP79411_RTCC_CONF_ENABLE_OSCI); //Activer oscillator
    i2c_stop();
}


void RTCC_Stop(void){
    bool ack;
    do{
        i2c_start();
        ack = i2c_write(MCP79411_RTCC_W);        
    }while(!ack);
    i2c_write(MCP79411_RTCC_ADDR_SEC); //Adresse
    i2c_write(MCP79411_RTCC_CONF_DISABLE_OSCI); //Disable oscillator
    i2c_stop();    
}


void RTCC_ClearTime(void)
{
    RTCC_Stop();
    //RTCC_WriteTime(0, MCP79411_RTCC_ADDR_SEC, false); //pas necessaire car RTCC_Stop
    RTCC_WriteTime(0, MCP79411_RTCC_ADDR_MIN, true);
    RTCC_WriteTime(0, MCP79411_RTCC_ADDR_HOUR, true);
    RTCC_WriteTime(0, MCP79411_RTCC_ADDR_WEEKDAY, true);
    RTCC_WriteTime(0, MCP79411_RTCC_ADDR_DATE, true);
    RTCC_WriteTime(0, MCP79411_RTCC_ADDR_MONTH, true);
    RTCC_WriteTime(0, MCP79411_RTCC_ADDR_YEAR, true);
    RTCC_Alarm_ClearTriggerFlag(MCP79411_RTCC_ADDR_ALARM0_DAY);
RTCC_Alarm_ClearTriggerFlag(MCP79411_RTCC_ADDR_ALARM1_DAY);
    RTCC_Start();
}


void RTCC_SetTime(uint8_t second, uint8_t minute, uint8_t hour, uint8_t weekDay, uint8_t date, uint8_t month, uint8_t year)
{
    RTCC_Stop();
    //RTCC_WriteTime(second, MCP79411_RTCC_ADDR_HOUR, false); //pas necessaire car RTCC_Stop
    RTCC_WriteTime(minute, MCP79411_RTCC_ADDR_MIN, true);
    RTCC_WriteTime(hour, MCP79411_RTCC_ADDR_HOUR, true);
    RTCC_WriteTime(weekDay, MCP79411_RTCC_ADDR_WEEKDAY, true);
    RTCC_WriteTime(date, MCP79411_RTCC_ADDR_DATE, true);
    RTCC_WriteTime(month, MCP79411_RTCC_ADDR_MONTH, true);
    RTCC_WriteTime(year, MCP79411_RTCC_ADDR_YEAR, true);
    RTCC_Start();
}


void RTCC_WriteTime (uint8_t value, uint8_t registerAddress, bool pausedOscillator)
{
    bool ack;
    uint8_t val_d = value / 10;
    uint8_t val_u = value - (val_d * 10);
    
    //if(registerAddress != MCP79411_RTCC_ADDR_SEC)
    //{
    //	RTCC_Stop();
    //}
    if(pausedOscillator == false)
    {
        RTCC_Stop();
    }
    do{
        i2c_start();
        ack = i2c_write(MCP79411_RTCC_W);        
    }while(!ack);

    i2c_write(registerAddress); //Adresse
    switch(registerAddress)
    {
    	case MCP79411_RTCC_ADDR_HOUR:
    	case MCP79411_RTCC_ADDR_DATE:
        case MCP79411_RTCC_ADDR_ALARM0_HOUR:
        case MCP79411_RTCC_ADDR_ALARM1_HOUR:
    		i2c_write(0x00 | ((val_d << 4)&0x30) | (val_u & 0x0F));
    		break;
    	case MCP79411_RTCC_ADDR_MIN:
        case MCP79411_RTCC_ADDR_ALARM0_SEC:
        case MCP79411_RTCC_ADDR_ALARM1_SEC:
        case MCP79411_RTCC_ADDR_ALARM0_MIN:
        case MCP79411_RTCC_ADDR_ALARM1_MIN:
    		i2c_write(0x00 | ((val_d << 4)&0x70) | (val_u & 0x0F));
    		break;
    	case MCP79411_RTCC_ADDR_SEC:
            if(pausedOscillator == true)
                i2c_write(MCP79411_RTCC_CONF_DISABLE_OSCI | ((val_d << 4)&0x70) | (val_u &0x0F));
            else
                i2c_write(MCP79411_RTCC_CONF_ENABLE_OSCI | ((val_d << 4)&0x70) | (val_u &0x0F));
    		break;
    	case MCP79411_RTCC_ADDR_WEEKDAY:
    		i2c_write(0x00 | (value & 0x07));
    		break;
    	case MCP79411_RTCC_ADDR_MONTH:
    		i2c_write(0x00 | ((val_d << 4)&0x10) | (val_u & 0x0F));
    		break;
    	case MCP79411_RTCC_ADDR_YEAR:
    		i2c_write(0x00 | ((val_d << 4)&0xF0) | (val_u & 0x0F));
    		break;
    	default:
    		break;
    }
    i2c_stop();
    if(pausedOscillator == false)
    {
        RTCC_Start();
    }
    //if(registerAddress != MCP79411_RTCC_ADDR_SEC)
    //{
    //	RTCC_Start();
    //}
}

//Read RTCC Part
uint8_t RTCC_ReadHour (uint8_t registerAddress){
    bool ack;
    uint8_t hour, hour_d, hour_u;
    
    do{
        i2c_start();
        ack = i2c_write(MCP79411_RTCC_W);        
    }while(!ack);
    i2c_write(registerAddress); //Adresse
    i2c_reStart(); //Restart
    i2c_write(MCP79411_RTCC_R);
    hour = i2c_read(0); ///NO ACK 
    i2c_stop();
    
    hour_d = ((hour & 0x30) >> 4);
    hour_u = hour & 0x0F; 
    hour = ((hour_d*10) + hour_u);
    
    return (hour);
}

uint8_t RTCC_ReadMin (uint8_t registerAddress){
    bool ack;
    uint8_t min, min_d, min_u;
    
    do{
        i2c_start();
        ack = i2c_write(MCP79411_RTCC_W);        
    }while(!ack);
    i2c_write(registerAddress);//Adresse
    i2c_reStart(); //Restart
    i2c_write(MCP79411_RTCC_R);
    min = i2c_read(0); ///NO ACK 
    i2c_stop();
    
    min_d = ((min & 0x70) >> 4);
    min_u = (min & 0x0F);
    min = ((min_d*10)+min_u);
            
    return (min);
}

bool RTCC_Alarm_ReadTriggerFlag (uint8_t registerAddress){
    bool ack;
    uint8_t registerValue;
    bool valueTrigger;
    
    do{
        i2c_start();
        ack = i2c_write(MCP79411_RTCC_W);        
    }while(!ack);
    i2c_write(registerAddress);//Adresse
    i2c_reStart(); //Restart
    i2c_write(MCP79411_RTCC_R);
    registerValue = i2c_read(0); ///NO ACK 
    i2c_stop();
    
    valueTrigger = ((registerValue & 0x08) >> 2);
            
    return (valueTrigger);
}

void RTCC_Alarm_ClearTriggerFlag (uint8_t registerAddress)
{
    bool ack;
    RTCC_Stop();
    
    do{
        i2c_start();
        ack = i2c_write(MCP79411_RTCC_W);        
    }while(!ack);
    i2c_write(registerAddress); //Address
    i2c_write(0x00 | 0x08); //Enables both alarms
    i2c_stop();
    
    RTCC_Start();
}


//Alarm
void RTCC_Alarms_Init(void)
{
	RTCC_Alarm_SetTrigger(RTCC_ALARM_TRIGER_REGISTER, MCP79411_RTCC_ADDR_ALARM0_DAY); //Set ALMxC2:0 to 001 for minutes match
    RTCC_Alarm_SetTrigger(RTCC_ALARM_TRIGER_REGISTER, MCP79411_RTCC_ADDR_ALARM1_DAY); //Set ALMxC2:0 to 001 for minutes match
	RTCC_Alarms_Enable_Interupt();

    RTCC_Stop();
    RTCC_WriteTime(ALARM_WARN_MIN, MCP79411_RTCC_ADDR_ALARM0_MIN, true);
    RTCC_WriteTime(ALARM_WARN_HOUR, MCP79411_RTCC_ADDR_ALARM0_HOUR, true);
    RTCC_WriteTime(ALARM_SHUTDOWN_MIN, MCP79411_RTCC_ADDR_ALARM1_MIN, true);
    RTCC_WriteTime(ALARM_SHUTDOWN_HOUR, MCP79411_RTCC_ADDR_ALARM1_HOUR, true);
    RTCC_Start();
}


void RTCC_Alarms_Enable_Interupt(void)   //Set MFP pin to Alarm Int
{
    bool ack;
    //RTCC_Stop();
    
    do{
        i2c_start();
        ack = i2c_write(MCP79411_RTCC_W);        
    }while(!ack);
    i2c_write(MCP79411_RTCC_ADDR_CONTROL); //Address
    i2c_write(0x30); //Enables both alarms
    i2c_stop();
    
    //RTCC_Start();
}

void RTCC_Alarm_SetTrigger(uint8_t value, uint8_t registerAddress)
{
    bool ack;
    
    //RTCC_Stop();

    do{
        i2c_start();
        ack = i2c_write(MCP79411_RTCC_W);        
    }while(!ack);

    i2c_write(registerAddress); //Adresse
    i2c_write(0x00 | (value&0xF0));
    i2c_stop();

    //RTCC_Start();
}