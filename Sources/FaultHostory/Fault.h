
//----------------------------------------------------------------------------------------------------
//---------------------- Pride Power------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//* Project Name       : S133
//* File Name          : Fault.h
//* Author             : Judy
//* Version            : V1.0.0
//* Start Date         : 2016.6.7
//* Description        : 故障定义
//----------------------------------------------------------------------------------------------------
/*
#include "CellVolUnbalance1.h"
#include "CellVolUnbalance2.h"
#include "CellVolUnbalance3.h"
#include "CellVolUnbalance4.h"
#include "CurrentSencerError.h"
#include "CellOverVoltage1.h"
#include "CellOverVoltage2.h"
#include "CellOverVoltage3.h"
#include "CellOverVoltage4.h"
#include "ChargeOverCurrent1.h"
#include "ChargeOverCurrent2.h"
#include "ChargeOverCurrent3.h"
#include "ChargeOverCurrent4.h"
#include "DischargeOverCurrent1.h"
#include "DischargeOverCurrent2.h"
#include "DischargeOverCurrent3.h"
#include "DischargeOverCurrent4.h"
#include "OverTemp1.h"
#include "OverTemp2.h"
#include "OverTemp3.h"
#include "OverTemp4.h"
#include "BatteryOverVoltage1.h"
#include "BatteryOverVoltage2.h"
#include "BatteryOverVoltage3.h"
#include "BatteryOverVoltage4.h"
#include "SOCLow2.h"
#include "SOCLow3.h"
#include "SOCLow4.h"
#include "CellTempUnbalance1.h"
#include "CellTempUnbalance2.h"
#include "CellTempUnbalance3.h"
#include "CellTempUnbalance4.h"
#include "CellUnderVoltage1.h"
#include "CellUnderVoltage2.h"
#include "CellUnderVoltage3.h"
#include "CellUnderVoltage4.h"
#include "UnderTemp1.h"
#include "UnderTemp2.h"
#include "UnderTemp3.h"
#include "UnderTemp4.h"
#include "BatteryUnderVoltage1.h"
#include "BatteryUnderVoltage2.h"
#include "BatteryUnderVoltage3.h"
#include "BatteryUnderVoltage4.h"
*/

extern unsigned char  g_caution_Flag_1;  //故障状态
extern unsigned char  g_caution_Flag_2;
extern unsigned char  g_caution_Flag_3;
extern unsigned char  g_caution_Flag_4;
extern unsigned int g_errorCounter;



///////////////////放电电流//////////////////////
typedef union{
	unsigned int word;
	struct {
	
	  word F0_Battery_Under_Voltage:1; 
	  word F1_Inner_Communiction:1;	 
	  word F2_DisCharge_Over_Current:1;
	  word F3_Cell_Under_Voltage:1;
	  word F4_Current_Sensor:1;       
	  word F5_Temp_Sensor:1;
	  word F6_Cell_Unbalance:1;
	  word F7_Temp_Unbalance:1;
	  word F8_Over_Temp:1;
	  word F9_Under_Temp:1;
		word F1015_Reverse:6;   
		        		       
	}Bit;
}CUTDISCURT0;
extern CUTDISCURT0 CutDisCurt0;

typedef union{
	unsigned int word;
	struct {
	
	  word F0_Battery_Under_Voltage:1;  
	  word F1_DisCharge_Over_Current:1;
	  word F2_Cell_Under_Voltage:1;
	  word F3_Cell_Unbalance:1;  
	  word F4_Temp_Unbalance:1;	  
	  word F5_Under_Temp:1;
	  word F6_F_SOC_Low:1;	  
		word F715_Reverse:9;   		        		       
	}Bit;
}CUTDISCURT20;
extern CUTDISCURT20 CutDisCurt20;

typedef union{
	unsigned int word;
	struct {
	
	  word F0_Battery_Under_Voltage:1;  
	  word F1_DisCharge_Over_Current:1;
	  word F2_Cell_Under_Voltage:1;
	  word F3_Cell_Unbalance:1;  
	  word F4_Temp_Unbalance:1;	  
	  word F5_Under_Temp:1;
	  word F6_F_SOC_Low:1;	  
		word F7_15_Reverse:9;    	        		       
	}Bit;
}CUTDISCURT50;
extern CUTDISCURT50 CutDisCurt50;
///////////////////放电电流//////////////////////


///////////////////充电电流//////////////////////

typedef union{
	unsigned int word;
	struct {
	
	  word F0_Battery_Over_Voltage1:1; 
	  word F1_Battery_Over_Voltage2:1;
	  word F2_Inner_Communiction:1;
	  word F3_Charge_Over_Current:1;
	  word F4_Cell_Over_Voltage1:1;
	  word F5_Cell_Over_Voltage2:1;
	  word F6_Current_Sensor:1;       
	  word F7_Temp_Sensor:1;
	  word F8_Cell_Unbalance:1;
	  word F9_Temp_Unbalance:1;  
	  word F10_Over_Temp:1;
	  word F11_Under_Temp:1;
		word F1215_Reverse:4;   
		        		       
	}Bit;
}CUTCHACURT0;
extern CUTCHACURT0 CutChaCurt0;

typedef union{
	unsigned int word;
	struct {
	
	  word F0_Charge_Over_Current:1;  
	  word F1_Cell_Unbalance:1;  
	  word F2_Temp_Unbalance:1;
	  word F3_Over_Temp:1;	  
	  word F4_Under_Temp:1;	  
		word F515_Reverse:11;   		        		       
	}Bit;
}CUTCHACURT20;
extern CUTCHACURT20 CutChaCurt20;

typedef union{
	unsigned int word;
	struct {
	
	  word F0_Battery_Over_Voltage:1;
	  word F1_Charge_Over_Current:1;
	  word F2_Cell_Over_Voltage:1;  
	  word F3_Cell_Unbalance:1;  
	  word F4_Temp_Unbalance:1;
	  word F5_Over_Temp:1;	  
	  word F6_Under_Temp:1;	  
		word F715_Reverse:9;    	        		       
	}Bit;
}CUTCHACURT50;
extern CUTCHACURT50 CutChaCurt50;
///////////////////充电电流//////////////////////



///////////////////DC充电电流//////////////////////

typedef union{
	unsigned int word;
	struct {
	
	  word F0_Battery_Over_Voltage1:1; 
	  word F1_Battery_Over_Voltage2:1;
	  word F2_Inner_Communiction:1;
	  word F3_Charge_Over_Current:1;
	  word F4_Cell_Over_Voltage1:1;
	  word F5_Cell_Over_Voltage2:1;
	  word F6_Cell_Over_Voltage3:1;
	  word F7_Cell_Over_Voltage4:1;
	  word F8_Current_Sensor:1;       
	  word F9_Temp_Sensor:1;
	  word F10_Cell_Unbalance:1;
	  word F11_Temp_Unbalance:1;  
	  word F12_Over_Temp:1;
	  word F13_Under_Temp:1;
	  word F14_Communication_With_Charger:1;
		word F15_Reverse:1;   
		        		       
	}Bit;
}CUTDCCHACURT0;
extern CUTDCCHACURT0 CutDCChaCurt0;

typedef union{
	unsigned int word;
	struct {
	
	  word F0_Charge_Over_Current:1;  
	  word F1_Cell_Unbalance:1;  
	  word F2_Temp_Unbalance:1;
	  word F3_Over_Temp:1;	  
	  word F4_Under_Temp:1;	  
		word F515_Reverse:11;   		        		       
	}Bit;
}CUTDCCHACURT20;
extern CUTDCCHACURT20 CutDCChaCurt20;

typedef union{
	unsigned int word;
	struct {
	
	  word F0_Battery_Over_Voltage:1;
	  word F1_Charge_Over_Current:1;  
	  word F2_Cell_Unbalance:1;  
	  word F3_Temp_Unbalance:1;
	  word F4_Over_Temp:1;	  
	  word F5_Under_Temp:1;	  
		word F615_Reverse:10;    	        		       
	}Bit;
}CUTDCCHACURT50;
extern CUTDCCHACURT50 CutDCChaCurt50;

///////////////////DC充电电流//////////////////////

extern Bool errorCurrSensorIniatial(void);
extern unsigned char TaskFaultProcess(void);
extern void CarFaultDone(void);


