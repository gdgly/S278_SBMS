//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//---------------------- Pride Power------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//* Project Name       : S133
//* File Name          : Vehicle.c
//* Author             : Sun Lixue
//* Version            : V1.0.0
//* Start Date         : 2011.6.7
//* Description        : 该文件根据项目通信协议发送相关的报文到整车或交流充电机
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
#include "BMS20.h"
#include "VehicleCAN.h"

unsigned char  RelayReq0 =0;


CAN450BYTE7      Can450Byte7;
CAN450BYTE0      Can450Byte0;

CAN554BYTE0      Can554Byte0;
CAN554BYTE1      Can554Byte1;
CAN554BYTE2      Can554Byte2;
CAN554BYTE3      Can554Byte3;
CAN554BYTE4      Can554Byte4;
CAN554BYTE5      Can554Byte5;
CAN554BYTE6      Can554Byte6;
CAN554BYTE7      Can554Byte7;


FAULT_GROUP1  Fault_Group1;
FAULT_GROUP2  Fault_Group2;
FAULT_GROUP3  Fault_Group3;
FAULT_GROUP4  Fault_Group4;
FAULT_GROUP5  Fault_Group5;
FAULT_GROUP6  Fault_Group6;
FAULT_GROUP7  Fault_Group7;


unsigned char  BmsCtlStat0 =0; //用于上位机监控，而不发给整车
unsigned char  SwithState=0;

float BiggestFeedbackPower=0;    //最大回馈功率
float BiggestDischargePower=0;   //最大放电功率 
float biggestFeedbackCurrent=0;  //最大允许回馈电流
float biggestDischargeCurrent=0; //最大允许放电电流
unsigned char pusleLimitSec=0;     //最大回馈电流限制时间


unsigned int DCLeftTime;
unsigned char st_heating;//动力电池加热状态：0未加热；1预加热中；2边充电边加热中
unsigned char St_heatManage = 0;//动力电池热管理状态：0没工作；1加热过程中；2cooling
unsigned char BattHeatSave;     //动力电池保温状态：0未保温，1保温
unsigned char BattPTCReq;        //动力电池加热PTC开启需求：0未需求；1已需求







unsigned char Box_Num_g_highestCellVoltage; //最高单体所在箱号
unsigned char Box_Posi_g_highestCellVoltage;//最高单体所在箱号位置

unsigned char Box_Num_g_lowestCellVoltage;  //最低单体所在箱号
unsigned char Box_Posi_g_lowestCellVoltage; //最低单体所在箱号位置

unsigned char Box_Num_g_highestTemperature;  //最高温度所在箱号
unsigned char Module_Num_g_highestTemperature;//最高温度所在模组

unsigned char State_Box_Online;               //箱子连接情况
unsigned char Box_Quantity;                   //箱子数量

unsigned char Box_Num_g_lowestTemperature;    //最低温度所在箱号
unsigned char Module_Posi_g_lowestTemperature;//最低温度所在模组

unsigned int Charge_Times;                    //充电次数
unsigned int Year_Battery_Manufacture = 2016;//电池生产年份
unsigned int Month_Battery_Manufacture = 6;//电池生产月份

unsigned long int chargeAH ;   //实际充电总安时
unsigned long int dischargeAH ;//实际放电总安时




//Fault_Group5.Bit.Reserve1_Bit = 0;
//Fault_Group7.Bit.Reserve2_Bit = 0;
//Fault_Group7.Bit.Reserve3_Bit = 0;
//Fault_Group7.Bit.Reserve4_Bit = 0;
//Fault_Group7.Bit.Reserve5_Bit = 0;
//Fault_Group7.Bit.Reserve6_Bit = 0;

//*************************************************************************************
//*************************************************************************************
//*****************以下是BMS给整车发消息子程序*****************************************
//*************************************************************************************
//******************************************************************************
//* Function name:   BMS_To_VCU_BatMsg1
//* Description:     Message1
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void BMS_To_VCU_BatMsg1(void) 
{
    struct can_msg mg;
    unsigned char tt=100;
    unsigned int buff;
    static unsigned char life=0;
    
    mg.RTR= FALSE;  
    mg.len = 8;
    mg.prty = 0;
    
    life++;
    
    if(g_socValue>=0.995) 
        buff = 100;
    else 
        buff = (unsigned int)(g_socValue*100*2.5);
    mg.data[0] = buff; //SOC值    
    
	  buff = (unsigned int)(g_highVoltageV1*50);	//电池总电压
	  mg.data[1]= buff;//低字节 
	  mg.data[2]= buff>>8;//高字节 
	  
	  buff = (unsigned int)((g_systemCurrent+3200)*10);	//总电流
	  mg.data[3] = buff;//低字节 
	  mg.data[4] = buff>>8;//高字节 
	  	  
	  buff = (unsigned int)(biggestDischargeCurrent*0.2);	//最大允许放电电流
	  mg.data[5] = buff;//持续的
	  mg.data[6] = buff;//30s的，一样的 
	 
	  mg.data[7] = life;//
    
    mg.id= 0x18F201F3;
    while((!MSCAN0SendMsg(mg))&&(tt>0))
        tt--; 
}
//******************************************************************************
//* Function name:   BMS_To_VCU_BatMsg2
//* Description:     Message2
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void BMS_To_VCU_BatMsg2(void) 
{
    struct can_msg mg;
    unsigned char tt=100;
    unsigned int buff;
    
    mg.RTR= FALSE;  
    mg.len = 8;
    mg.prty = 0;
    
    buff = (unsigned int)((biggestFeedbackCurrent+1000)*0.2);	//最大允许回馈电流
    mg.data[0] = buff;  //两个一样
    mg.data[1] = buff;  //两个一样
    
    mg.data[2] = dischargeAH&0x000f;
    mg.data[3] = (dischargeAH>>8)&0x000f;
    mg.data[4] = (dischargeAH>>16)&0x000f;    
   
    mg.data[5] = chargeAH&0x000f;
    mg.data[6] = (chargeAH>>8)&0x000f;
    mg.data[7] = (chargeAH>>16)&0x000f;        
    
    mg.id= 0x18F202F3;
    while((!MSCAN0SendMsg(mg))&&(tt>0))
        tt--; 
}
//******************************************************************************
//* Function name:   BMS_To_VCU_Fault
//* Description:     故障和状态信息
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void BMS_To_VCU_Fault(void)  
{
    struct can_msg mg;
    unsigned int buff;
    unsigned char tt=100;
    
    mg.RTR= FALSE;  
    mg.len = 8;
    mg.prty = 0;

	  mg.data[0] = Fault_Group1.byte;
	  mg.data[1] = Fault_Group2.byte;
	  mg.data[2] = Fault_Group3.byte; 
	  mg.data[3] = Fault_Group4.byte;
    mg.data[4] = 0xff;
    mg.data[5] = Fault_Group5.byte;
    mg.data[6] = Fault_Group6.byte;
    mg.data[7] = Fault_Group7.byte;
    
    mg.id= 0x18F205F3;
    while((!MSCAN0SendMsg(mg))&&(tt>0))
        tt--; 
}
//******************************************************************************
//* Function name:   BMS_To_VCU_CellMsg1
//* Description:     Message1
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void BMS_To_VCU_CellExtremumMsg1(void) 
{
    struct can_msg mg;
    unsigned char tt=100;
    unsigned int buff;
    
    mg.RTR = FALSE;  
    mg.len = 8;
    mg.prty = 0;
    

	  buff = (unsigned int)(g_highestCellVoltage*1000);
	  mg.data[0] = buff;   //电池单体最高电压 低字节
	  mg.data[1] = buff>>8;//电池单体最高电压 高字节
	  mg.data[2] = Box_Num_g_highestCellVoltage;
	  mg.data[3] = Box_Posi_g_highestCellVoltage;

	  buff = (unsigned int)(g_lowestCellVoltage*1000);
	  mg.data[4] = (unsigned char)buff;//电池单体最低电压 低字节 
	  mg.data[5] = buff>>8;//电池单体最低电压 高字节
	  mg.data[6] = Box_Num_g_lowestCellVoltage;
	  mg.data[7] = Box_Posi_g_lowestCellVoltage;	  	  

    mg.id= 0x18F206F3;
    while((!MSCAN0SendMsg(mg))&&(tt>0))
        tt--; 
}

//******************************************************************************
//* Function name:   BMS_To_VCU_CellMsg2
//* Description:     Message2
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void BMS_To_VCU_CellExtremumMsg2(void) 
{
    struct can_msg mg;
    unsigned char tt=100;

    
    mg.RTR= FALSE;  
    mg.len = 8;
    mg.prty = 0;
    


	  mg.data[0] = g_highestTemperature+40;   //电池单体最高温度
	  mg.data[1] = Box_Num_g_highestTemperature;//电池单体最高温度所在箱号
	  mg.data[2] = Module_Num_g_highestTemperature; //电池单体最高温度所在模组编号
	 
	  mg.data[3] = g_lowestTemperature+40;//电池单体最低温度
	  mg.data[4] = Box_Num_g_lowestTemperature; //电池单体最低温度所在箱号
	  mg.data[5] = Module_Posi_g_lowestTemperature; //电池单体最低温度所在模组编号
	 
	  mg.data[6] = State_Box_Online&0x3f; 
	  mg.data[7] = Box_Quantity;	  	  

    mg.id= 0x18F207F3;
    while((!MSCAN0SendMsg(mg))&&(tt>0))
        tt--; 
}

//******************************************************************************
//* Function name:   BMS_To_VCU_CellVoltage
//* Description:     发送所有单体电压
//* EntryParameter : number
//* ReturnValue    : None
//******************************************************************************
/*
void BMS_To_VCU_CellVoltage(unsigned char number)  //TO 车载：CAN0
{
	struct can_msg mg;
	//unsigned long buff;
	unsigned int buff;
	unsigned char i;
	
	if(number >16)
		return;
		
  mg.RTR= FALSE;  
  mg.len = 8;
  mg.prty = 0;
	
	for(i=0;i<8;i++)
	  mg.data[i]= g_cellToVehicleVoltage[number][i];//8N+i号电池单体电压 

	buff = 0x0100+(unsigned long)number;
			
  mg.id=buff;    
	MSCAN0SendMsg(mg);

}
//******************************************************************************
//* Function name:   BMS_To_VCU_CellTem
//* Description:     发送所有电池温度
//* EntryParameter : number
//* ReturnValue    : None
//******************************************************************************
void BMS_To_VCU_CellTem(unsigned char number)//动力电池管理信息报文18 ID：0x1C1227EE  
{
	struct can_msg mg;
	unsigned long buff;
	unsigned char i;
	
	if(number >16)
		return;
	
	mg.RTR= FALSE;  
  mg.len = 8;
  mg.prty = 0;
	
	for(i=0;i<8;i++)
	  mg.data[i]= g_cellTemperature[number][i];//8N+i号电池单体温度

	buff = 0x010f+(unsigned long)number;
			
	mg.id=buff;
	MSCAN0SendMsg(mg);
	
}
*/
//******************************************************************************
//* Function name:   BMS_To_VCU_PackMsg
//* Description:     电池pack信息
//* EntryParameter : number
//* ReturnValue    : None
//******************************************************************************

void BMS_To_VCU_PackMsg(unsigned char number)  //TO 车载：CAN0
{
	struct can_msg mg;
	unsigned char tt=100;
	unsigned int buff;
			
  mg.RTR= FALSE;  
  mg.len = 8;
  mg.prty = 0;
  mg.data[0] = 0x03;    //电池厂家
  buff = (unsigned int)(m_sysah*0.2);//动力电池系统额定容量
	mg.data[1] = (unsigned char)buff; 
  
  buff = (unsigned int)(m_sysratevoltage*0.2);//364.8V
	mg.data[2] = (unsigned char)buff;//动力电池系统额定电压 
  
  buff = Charge_Times;//
	mg.data[3] = (unsigned char)buff;//动力电池系统额定电压     
	mg.data[4] = Charge_Times>>8; 
	mg.data[5] = 0x10|Month_Battery_Manufacture; //电池材料和生产月份
  
  mg.data[6] = Year_Battery_Manufacture-2000;   	

	mg.data[7] = BMS_SW_Version[7];
	
  mg.id= 0x18F240F3;
  while((!MSCAN0SendMsg(mg))&&(tt>0))
      tt--; 

}
//******************************************************************************
//* Function name:   BMS_To_VCU_ChargeMsg
//* Description:     发送充电时候的系统电流、系统电压、单体极值
//* EntryParameter : number
//* ReturnValue    : None
//******************************************************************************
void BMS_To_VCU_ChargeMsg(void) 
{   
    struct can_msg mg;
    unsigned int buff;
    unsigned char tt=100;
    
    mg.RTR= FALSE;  
    mg.len = 8;
    mg.prty = 0;        
 

    buff = (unsigned int)((g_systemCurrent+3200)*10); //系统电流
    mg.data[0]= buff; //低
    mg.data[1]= buff>>8; //高   
  
    buff = (unsigned int)(g_highVoltageV1*50);        //系统电压
    mg.data[2]=buff;//
    mg.data[3]=buff>>8;
    
    buff = (unsigned int)(g_highestCellVoltage*1000); //单体最高电压
    mg.data[4]=buff;    
    mg.data[5]= buff>>8;
    
    buff = (unsigned int)(g_lowestCellVoltage*1000);  //单体最高电压
    mg.data[6]=buff;    
    mg.data[7]= buff>>8;   

            
    mg.id = 0x18F241F3;
    while((!MSCAN0SendMsg(mg))&&(tt>0))
        tt--;
}

void bmsToPcTestCar(void)
{
    struct can_msg mg;
    unsigned int buff;
    unsigned char tt=100;
    
    mg.RTR= FALSE;  
    mg.len = 8;
    mg.prty = 0;
    	
	  mg.data[0]= 0xA0; 
  	mg.data[1]= 0;

	  mg.data[2]= 0; 
	  mg.data[3]= 0; 
	  
	  mg.data[4]= 0;
	  mg.data[5]= 0;
	  
	  mg.data[6]= 0;
	  mg.data[7]= 0;

	  mg.id= 0x000c0453; 
    while((!MSCAN0SendMsg(mg))&&(tt>0))
        tt--;
	  
}
//***********************************************************************
//************************************************************************
//*************************the end*************************************
//************************************************************************