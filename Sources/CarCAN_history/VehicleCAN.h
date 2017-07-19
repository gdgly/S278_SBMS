//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//---------------------- Pride Power------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//* Project Name       : S133
//* File Name          : VehicleCAN.h
//* Author             : Judy
//* Version            : V1.0.0
//* Start Date         : 2015.7.13
//* Description        : 该文件是对CAN发送的报文进行BIT定义
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------


typedef union{
	unsigned char byte;
	struct {
		byte B1_CellHigh:1;       
		byte B2_CellLow:1;              
		byte B3_SOCHigh:1;              
		byte B4_SOCLow:1;
		byte B5_ChaOverCurt:1;
		byte B6_DisChaOverCurt:1;
		byte B7_CellTemOver:1;
		byte B8_CellError:1;
	}Bit;
}CAN1818BYTE6;    
extern CAN1818BYTE6      Can1818Byte6;

typedef union{
	unsigned char byte;
	struct {
		byte B1_VolHigh:1;       
		byte B2_VolLow:1;              
		byte B3_BalanceAction:1;              
		byte B4_TemUnbalance:1;
		byte B57_ErrortLevel:3;
		byte B8_flag8:1;
	}Bit;
}CAN1818BYTE7;    
extern CAN1818BYTE7      Can1818Byte7;

typedef union{
	unsigned char byte;
	struct {
		byte B16_flag:6;       
		byte B7_RelayState:1;//继电器状态,0断开,1闭合
		byte B8_WorkState:1;//0:放电；1:充电
	}Bit;
}CAN1818BYTE8;    
extern CAN1818BYTE8      Can1818Byte8;


typedef union{
	unsigned char byte;
	struct {
		byte B1_Num1ConnectError:1;      
		byte B2_Num2ConnectError:1;
		byte B3_Num3ConnectError:1;
	  byte B4_Num4ConnectError:1;
	  byte B5_Num5ConnectError:1;
	  byte B6_Num6ConnectError:1;
	  byte B78_flag:2;	
	}Bit;
}CAN181CBYTE3;    
extern CAN181CBYTE3       Can181cByte3;


typedef union{
	unsigned char byte;
	struct {
	  byte B14_flag:4;
	  byte B57_ErrorDone:3;
	  byte B8_ConnectState:1;	
	}Bit;
}CAN181CBYTE5;    
extern CAN181CBYTE5       Can181cByte5;


typedef union{
	unsigned char byte;
	struct {
	  byte B1_InsulationError:1;
	  byte B2_HighVolConError:1;
	  byte B3_WatchDog:1;
	  byte B4_ChanerContr:1;
	  byte B57_Battertype:3;
	  byte B8_ICdo:1; 	
	}Bit;
}CANF100BYTE3;    
extern CANF100BYTE3       Canf100Byte3;


/////////////////////////////////////////////////////////////////////////////////
extern float BiggestFeedbackPower;  //最大反馈功率:Byte3
extern float BiggestDischargePower; //最大放电功率:Byte5
extern float biggestFeedbackCurrent;  //最大允许回馈电流
extern float biggestDischargeCurrent; //最大允许放电电流
extern unsigned char  SwithState;   //紧急开关:Byte6
extern unsigned int stateCode; //状态机编码
extern unsigned char pusleLimitSec;     //最大回馈电流限制时间
extern unsigned char St_heatManage;//动力电池热管理状态：0没工作；1加热过程中；2cooling

extern unsigned char LCellBoxNum;//最低电压对应的箱号
extern unsigned char LCellLocation;//最低单体电压箱内位置
extern unsigned char HCellBoxNum;//最高电压对应的箱号
extern unsigned char HCellLocation;//最高单体电压箱内位置

extern unsigned char LTemBoxNum;//最低温度对应的箱号
extern unsigned char LTemLocation;//最低温度箱内位置
extern unsigned char HTemBoxNum;//最高温度对应的箱号
extern unsigned char HTemLocation;//最高温度箱内位置

extern unsigned char Box1HTem;
extern unsigned char Box2HTem;
extern unsigned char Box3HTem;
extern unsigned char Box4HTem;
extern unsigned char Box5HTem;
extern unsigned char Box6HTem;
extern unsigned char Box1LTem;
extern unsigned char Box2LTem;
extern unsigned char Box3LTem;
extern unsigned char Box4LTem;
extern unsigned char Box5LTem;
extern unsigned char Box6LTem;

extern unsigned char ChaStartH;
extern unsigned char ChaStartM;
extern unsigned char ChaStartS;
extern unsigned char ChaOverH;
extern unsigned char ChaOverM;
extern unsigned char ChaOverS;
extern float ChangerVol;
extern float ChangerCurt;

extern void BMS1_transmit1818(void); 
extern void BMS2_transmit1819(void);
extern void BMS3_transmit181A(void);
extern void BMS4_transmit181B(void);
extern void BMS5_transmit181C(void);
extern void BMS6_transmit181D(void);
extern void BMS7_transmit18F2(void);
extern void BMS_transmitFF2A(void);
extern void BMS_transmitFF2C(void);
extern void BMS_transmitFF2D(void);
extern void BMS_transmitFF2E(void);
extern void BMS_transmitFF30(void);
extern void BMS_transmitF100(void);
extern void BMS_transmitFF33(void);
extern void BMS_transmitFF34(void);
extern void BMS_transmitFF50(void);
extern void BMS_transmitFF51(void);     
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////end/////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////