
//---------------------- Pride Power------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//* Project Name       : S133
//* File Name          : AC_ChangerProcess.h
//* Author             : Judy
//* Version            : V1.0.0
//* Start Date         : 2015.7.13
//* Description        : 该文件是对CAN发送的报文进行BIT定义
//----------------------------------------------------------------------------------------------------
extern unsigned char  plug_AC_CP_Connect;
extern unsigned char slowRechargeFinished;
extern unsigned char ACTem1;
extern unsigned char ACTem2;

typedef union{
	unsigned char byte;
	struct {
	
	  byte Flag0:1; 
	  byte Flag1:1;	 
	  byte Flag2:1;
	  byte Flag3:1;
	  byte AC_CCState:1;      
	  byte ElectronicLock:1;
	  byte Flag6:1;
	  byte Flag7:1; 
		        		       
	}Bit;
}AC_CHANGERSTATE;
extern AC_CHANGERSTATE AC_ChangerState;


typedef union{
	unsigned char byte;
	struct {
	
	  byte Flag0:3;//0~2 
	  byte F3_S_ChgWakeUp:1;//3	 
	  byte F4_S_ChgWorkState:4;//4~7
		        		       
	}Bit;
}AC_CHANGERWORKSTATE;
extern AC_CHANGERWORKSTATE AC_ChangerWorkState;
//extern unsigned int OBC_Out_DC_Vol;
extern float GetRequestCurrentAC(void);
extern void TaskRechargeAC(void);

//***************************************************************************