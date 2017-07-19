//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//---------------------- Pride Power------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//* Project Name       : fotun_Logistics
//* File Name          : VehicleCAN.h
//* Author             : WsM
//* Version            : V1.0.0
//* Start Date         : 2016.7.8
//* Description        : 该文件根据项目通信协议发送相关的报文到整车或交流充电机
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

//////////////////////VCU发送的//////////////////////////
typedef union{
	unsigned char byte;
	struct {
	
	  byte LowVolPowerOff:1;    //bit0   VCU低压下电指令
    byte HighVolPowerOn:1;	  //bit1   VCU高压上电指令
		byte ACCState:2;          //bit2~3 钥匙开关    
		byte flag:4;              //bit4~7     快充继电器状态
	}Bit;
}VCU_REQUEST;

typedef union{
	unsigned char byte;
	struct {
	
	  byte Curt:4;         //bit0~3  保留
    byte OBCRequest:1;	  //bit4    OBC上电
		byte OBCSleep:1;      //bit5    OBC休眠    
		byte flag2:2;          //bit6~7 保留  
	}Bit;
}OBC_REQUEST;




typedef union{
	unsigned char byte;
	struct {
	
	  byte CellVolReq:1;    //bit0   电池包各节点单体电压
    byte CellTempReq:1;	  //bit1   电池包各节点温度
		byte PackMessageReq:1; //bit2  电池包信息  
		byte flag:5;              //bit3~7    预留
	}Bit;
}VCU_CELLREQUEST;

typedef union{
	unsigned char byte;
	struct {
	
	  byte VCULife:4;    //bit0~bit3   VCULife
    byte PowerOnOffReq:2;	  //bit4~bit5  高压上下电指令
		byte flag:2;              //bit6~7    预留
	}Bit;
}VCU_CONTROL;


typedef union{
	unsigned char byte;
	struct {
	
	  byte PState:1;  //bit0    VCU正极继电器状态
		byte flag:7;    //bit6~7    预留
	}Bit;
}VCU_PRELSTATE;

typedef union{
	unsigned char byte;
	struct {
	
	  byte flag1:2;  //bit0~1    预留
	  byte Charge_Switch:1;  //bit2   充电开关
	  byte downC_Switch:1;   //bit3   降弓开关
	  byte flag2:1;  //bit4    预留
	  byte downC_OK:1;   //bit5   降弓到位
		byte flag3:2;    //bit6~7    预留
	}Bit;
}VCU_CHGCONTROL;

typedef union{
	unsigned char byte;
	struct {
	
	  byte flag1:2;  //bit0~1    预留
	  byte Parking_Brake:2;  //bit2~3   驻车信号
		byte flag2:4;    //bit6~7    预留
	}Bit;
}VCU_PARKINGBRAKE;
//////////////////////VCU发送的//////////////////////////



 //////////////////////VCU发送的状态信息//////////////////////////
typedef union{
	unsigned char byte;
	struct {
	
	  byte BMS_Life:4;    //bit0~bit3  life
	  byte St_BMS :4;    //bit4~bit7   BMS当前状态
     		       
	}Bit;
}STATUS_GROUP1;

typedef union{
	unsigned char byte;
	struct {
	
	  byte BMS_PowerOff_Req:2;  //bit0~bit1     BMS高压下电请求 ??????自定义的
	  byte St_BMS_Balance:2;    //bit2~bit3  当前均衡状态
	  byte St_Ele_Relay :2;     //bit4~bit5  受电带接触器连接状态
	  byte St_CHG_Connect :2;   //bit6~bit7  充电连接状态
     		       
	}Bit;
}STATUS_GROUP2;

typedef union{
	unsigned char byte;
	struct {
	
	  byte St_CHG_Mode :2;   //bit0~bit1   充电模式 
	  byte St_Charge:2;        //bit2~bit3  充电状态
	  byte Fault_Level :2;    //bit4~bit5  故障等级
	  byte St_N_Relay :1;    //bit6  负极继电器状态？？？？？？？？？？？
	  byte St_P_Relay :1;    //bit7  正极继电器状态？？？？？？？？？？自定义的
     		       
	}Bit;
}STATUS_GROUP3;      

typedef union{
	unsigned char byte;
	struct {
	  byte St_BMS_System :1;    //bit0  BMS系统状态位
	  byte St_BMU_System :1;    //bit1  BMU系统状态位
	  byte St_LECU_System :2;    //bit2~bit3  BMU系统状态位
	  byte St_Bat_Fault:2;    //bit4~bit5  电池组故障状态
	  byte Mode_BMS_Work :2;   //bit6~bit7 当前电池工作状态 
     		       
	}Bit;
}STATUS_GROUP4;
//////////////////////VCU发送的状态信息//////////////////////////


//////////////////////BMS发送的故障信息//////////////////////////
typedef union{
	unsigned char byte;
	struct {
	
	  byte F0_Fire_Warning:2;  //bit0~1 火灾预警
	  byte F2_Ele_Relay_Con:2;  //bit2~3 充电插座过温
    byte flag:4;  //bit4~7    //预留
	  	 
	}Bit;
}ERROR_GROUP0;

typedef union{
	unsigned char byte;
	struct {
	
	  byte F0_Power_Vol:1;  //bit0整车供电24V异常报警
	  byte F1_HLVol_Lock:1;  //bit1 高低压互锁
    byte F2_SOC_Change:1;  //bit2 SOC跳变
    byte F3_BMS_Protect:1;  //bit3 BMS极限故障自我保护
	  byte F4_flag1:1;  //bit4 预留
    byte F5_Chg_C_Over:1;  //bit5 充电电流超限
    byte F6_flag2:1;  //bit6 预留
    byte F7_Chg_Ins_Low:1;  //bit7 充电绝缘低报警
	  	 
	}Bit;
}ERROR_GROUP6;



typedef union{
	unsigned char byte;
	struct {
	
	  byte St_CHG_Allow:1;  //bit0 充电允许状态位
	  byte St_DisCHG_Allow:1;  //bit1   放电允许状态位
    byte F2_DCChg_Neg_Relay_Con:1;  //bit2直流充电负接触器粘连故障、、、、、、、、、、、、
		byte F3_Ele_Relay_Con:1;  //bit3 受电带充电正接触器粘连
		byte F4_Ch_Over_I:2;  //bit4~5  充电过流
	  byte F6_Ins_Err:2;  //bit6~7	绝缘电阻低
	  	 
	}Bit;
}ERROR_GROUP1;    

typedef union{
	unsigned char byte;
	struct {
	
	  byte F0_Cell_Over_V:2;  //bit0~1 单体电池过压
	  byte F2_Cell_Under_V:2;  //bit2~3 单体电池欠压
		byte F4_Temp_Over:2;  //bit4~5     单体温度过温
	  byte flag:2;  //bit6~7	    电池组差异（系统不匹配）预留
	  	 
	}Bit;
}ERROR_GROUP2; 

typedef union{
	unsigned char byte;
	struct {
	
	  byte F0_Sub_Com_Err:1; //bit0     内部通讯故障
	  byte F1_V_CAN_Err:1;      //bit1  整车CAN通信故障
	  byte F2_P_Con_Err:1;  //bit2    主正继电器粘连
	  byte F3_PreCha_Failure:1;  //bit3 预充电失败、、、、、、、、、、、、、、、
		byte F4_N_Con_Err:1;  //bit4     主负继电器粘连
	  byte F5_DC_Con_Err:1;  //bit5	    外接充电正接触器粘连
	  byte F6_DisChg_Over_I:2;  //bit6~7	    放电电流过大
	  	 
	}Bit;
}ERROR_GROUP3; 

typedef union{
	unsigned char byte;
	struct {
	
	  byte F0_Bat_Over_V:2;  //bit0~1   电池过压
	  byte F2_Bat_Under_V:2;  //bit2~3 电池欠压
		byte F4_SOC_Over:2;  //bit4~5     SOC高
	  byte F6_SOC_Low:2;  //bit6~7	    SOC低
	  	 
	}Bit;
}ERROR_GROUP4; 

typedef union{
	unsigned char byte;
	struct {
	
	 
	  byte F0_FeedB_Over_I:2;  //bit0~1   回馈过流
		byte F2_Cell_Dif_T:2;  //bit2~3     单体温差过大
	  byte F4_Cell_Dif_V:2;  //bit4~5	    电池单体压差大
	  byte F6_Cell_Under_T:2;  //bit6~7 单体温度低
	  	 
	}Bit;
}ERROR_GROUP5;

typedef union{
	unsigned char byte;
	struct {
	
	 
	  byte flag:4;  //bit0~3   预留
		byte F4_HC_Rel_Error:1;  //bit4     热管理机组接触器故障（只针对水冷系统）
	  byte F5_HC_Error:1;  //bit5	    热管理机组接触器故障（只针对热管理系统）
	  byte F6_H_Neg_Rel:1;  //bit6 电池组加热负极接触器故障
	  byte F7_H_Pos_Rel:1;  //bit7 电池组加热正极接触器故障
	  	 
	}Bit;
}ERROR_GROUP7;


 
//////////////////////BMS发送的故障信息//////////////////////////

 
//******************************************************************************
//* 以下是用于外部的变量
//******************************************************************************
extern float g_SOH;

extern VCU_REQUEST VCU_Request;     //0xCFF0BEF
extern OBC_REQUEST OBC_Request;
extern VCU_CELLREQUEST VCU_CellRequest;//0x1801D2D0
extern VCU_CONTROL VCU_Control;       //0x1802D2D0
extern VCU_PRELSTATE VCU_PRelState;    //整车报文待增加
extern VCU_CHGCONTROL VCU_ChgControl;   //0x0C0217A7
extern VCU_PARKINGBRAKE VCU_ParkBrake;  //0x18FEF117



extern STATUS_GROUP1 status_group1;
extern STATUS_GROUP2 status_group2;
extern STATUS_GROUP3 status_group3;
extern STATUS_GROUP4 status_group4;

extern ERROR_GROUP0 Error_Group0;
extern ERROR_GROUP6 Error_Group6;
extern ERROR_GROUP1 Error_Group1;
extern ERROR_GROUP2 Error_Group2;
extern ERROR_GROUP3 Error_Group3;
extern ERROR_GROUP4 Error_Group4;
extern ERROR_GROUP5 Error_Group5;
extern ERROR_GROUP7 Error_Group7;

extern float g_SOH;


extern float BiggestDischargeCurt;              //最大放电电流
extern float BiggestFeedbackCurt;              //最大回馈电流
extern float BiggestDisCurtContinuous;          //最大持续放电电流
extern float BiggestFeedbackCurtContinuous;     //最大持续回馈电流

extern float BiggestDischargePower;                  //最大瞬时放电功率
extern float BiggestFeedbackPower;                   //最大瞬时回馈功率
extern float BiggestDisPowerContinuous;              //最大持续放电功率
extern float BiggestFeedbackContinuous;              //最大持续回馈功率

extern unsigned int	m_askpower;
extern unsigned int chargeRemainderTime;              //剩余充电时间

extern unsigned char Enable_Charger_Output;
extern unsigned char Enable_Charger_Sleep;

extern unsigned char Box_Num_g_lowestCellVoltage;    //最低单体所在箱号
extern unsigned char Box_Num_g_highestCellVoltage;   //最高单体所在箱号
extern unsigned char Box_Num_g_lowestTemperature;    //最低温度所在箱号
extern unsigned char Box_Num_g_highestTemperature;   //最高温度所在箱号

extern unsigned char Box_Posi_g_lowestCellVoltage;   //最低单体所在箱内位置
extern unsigned char Box_Posi_g_highestCellVoltage;  //最高单体所在箱内位置
extern unsigned char Box_Posi_g_lowestTemperature;   //最低温度所在箱内位置
extern unsigned char Box_Posi_g_highestTemperature;  //最高温度所在箱内位置


extern unsigned char St_heatManage;//动力电池热管理状态：0没工作；1加热过程中；2cooling


extern unsigned long int chargeAH ;   //实际充电总安时
extern unsigned long int dischargeAH ;//实际放电总安时
extern unsigned int Charge_Times;                    //充电次数
extern float DC_ChargePower;//充电电量
extern float VehicleSpeed;                         //车速信号

//******************************************************************************
//* 以下是用于外部的函数
//******************************************************************************

extern void BMS_To_VCU_BasicMsg0(void);
extern void BMS_To_VCU_BasicMsg(void);
/*extern void BMS_To_VCU_BasicMsg1(void);
extern void BMS_To_VCU_BasicMsg2(void);
extern void BMS_To_VCU_CHGMsg1(void);

extern void BMS_To_VCU_BatteryMsg1(void);
extern void BMS_To_VCU_BatteryMsg2(void);
extern void BMS_To_VCU_BatteryMsg3(void);
extern void BMS_To_VCU_BatteryMsg4(void);
extern void BMS_To_VCU_BatteryMsg5(void);
extern void BMS_To_VCU_BatteryMsg6(void);
extern void BMS_To_VCU_BatteryMsg7(void);
extern void BMS_To_VCU_BatteryMsg8(void);
extern void BMS_To_VCU_BatCellVol(void);
extern void BMS_To_VCU_BatCellVolData(unsigned char group);
void BMS_To_VCU_BatCellTemp(void);
void BMS_To_VCU_BatCellTempData(unsigned char group);
*/                    


//extern void BMS_To_VCU_CellVoltage(unsigned char number);  //TO 车载：CAN0
//extern void BMS_To_VCU_CellTem(unsigned char number);//动力电池管理信息报文18 ID：0x1C1227EE 
//extern void BMS_To_VCU_Fault(void);

//extern void bmsToPcTestCar(void);

extern unsigned char FireWarning;//火灾预警信号
extern unsigned char InsRelayState;//
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////end/////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////