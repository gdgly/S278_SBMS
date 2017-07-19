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


CAN1818BYTE6     Can1818Byte6;
CAN1818BYTE7     Can1818Byte7;
CAN1818BYTE8     Can1818Byte8;

CAN181CBYTE3     Can181cByte3;
CAN181CBYTE5     Can181cByte5;

CANF100BYTE3     Canf100Byte3;


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

unsigned long int chargeAH ;   //实际充电总安时
unsigned long int dischargeAH ;//实际放电总安时

unsigned char LCellBoxNum=0;//最低电压对应的箱号
unsigned char LCellLocation=0;//最低单体电压箱内位置
unsigned char HCellBoxNum=0;//最高电压对应的箱号
unsigned char HCellLocation=0;//最高单体电压箱内位置

unsigned char LTemBoxNum=0;//最低温度对应的箱号
unsigned char LTemLocation=0;//最低温度箱内位置
unsigned char HTemBoxNum=0;//最高温度对应的箱号
unsigned char HTemLocation=0;//最高温度箱内位置


unsigned char Box1HTem=0;
unsigned char Box2HTem=0;
unsigned char Box3HTem=0;
unsigned char Box4HTem=0;
unsigned char Box5HTem=0;
unsigned char Box6HTem=0;
unsigned char Box1LTem=0;
unsigned char Box2LTem=0;
unsigned char Box3LTem=0;
unsigned char Box4LTem=0;
unsigned char Box5LTem=0;
unsigned char Box6LTem=0;

unsigned char ChaStartH=0;
unsigned char ChaStartM=0;
unsigned char ChaStartS=0;
unsigned char ChaOverH=0;
unsigned char ChaOverM=0;
unsigned char ChaOverS=0;
float ChangerVol=0;
float ChangerCurt=0;

//*************************************************************************************
//*************************************************************************************
//*****************以下是BMS给整车发消息子程序*****************************************
//*************************************************************************************
//******************************************************************************
//* Function name:   BMS1_transmit1818
//* Description:     整车报文,100ms
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void BMS1_transmit1818(void) 
{
    struct can_msg mg;
    unsigned int buff;
    unsigned char tt=100;
    
    
    mg.RTR= FALSE;  
    mg.len = 8;
    mg.prty = 0;

	  buff = g_highVoltageV1*10;	//电池总电压
	  mg.data[0]= buff;//低字节 
	  mg.data[1]= buff>>8;//高字节 
	  
	  buff = 10*g_systemCurrent-3200;	//总电流
	  mg.data[2]= buff;//低字节 
	  mg.data[3]= buff>>8;//高字节 

    if(g_socValue>=0.995) 
        buff=100;
    else 
        buff = (unsigned int)(g_socValue*100*2.5);
    mg.data[4] = buff; //SOC值
    
    mg.data[5] = Can1818Byte6.byte; //故障
    mg.data[6] = Can1818Byte7.byte; //故障
    mg.data[7] = Can1818Byte8.byte; //故障
    
    mg.id= 0x1818D0F3;
    while((!MSCAN0SendMsg(mg))&&(tt>0))
        tt--; 
}

//******************************************************************************
//* Function name:   BMS2_transmit1819
//* Description:     整车报文,100ms
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void BMS2_transmit1819(void) 
{
    struct can_msg mg;
    unsigned int buff;
    unsigned char life;
    unsigned char tt=100;
    
    life++;
    mg.RTR= FALSE;  
    mg.len = 8;
    mg.prty = 0;

	  buff = (unsigned int)(g_lowestCellVoltage*100);
	  mg.data[0]= (unsigned char)buff;//电池单体最低电压 低字节 
	  mg.data[1]= ((buff>>8)|(LCellBoxNum<<4));//电池单体最低电压 高字节
	  	  
	  //高字节4bit为箱号

	  buff = (unsigned int)(g_highestCellVoltage*1000);
	  mg.data[2]= buff;//电池单体最高电压 低字节
	  mg.data[3]= ((buff>>8)|(HCellBoxNum<<4));//电池单体最高电压高字节
	  
	  //高字节4bit为箱号
    
    mg.data[4] = g_lowestTemperature+40-40;  //最低温度
    mg.data[5] = g_highestTemperature+40-40; //最高温度
    mg.data[6] = 0;  //
    mg.data[7] =life; //
    
    mg.id= 0x1819D0F3;
    while((!MSCAN0SendMsg(mg))&&(tt>0))
        tt--; 
}


//******************************************************************************
//* Function name:   BMS3_transmit181A
//* Description:     整车报文,100ms
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void BMS3_transmit181A(void) 
{
    struct can_msg mg;
    unsigned int buff;
    unsigned char life;
    unsigned char tt=100;
    
    life++;
    mg.RTR= FALSE;  
    mg.len = 8;
    mg.prty = 0;

	  buff = biggestDischargeCurrent*10+3200;	//最大允许放电电流
	  mg.data[0]= buff;//低字节 
	  mg.data[1]=buff>>8;//高字节 
	  
	  buff = biggestFeedbackCurrent*10+3200;	//最大允许充电电流
	  mg.data[2]= buff;// 低字节 
	  mg.data[3]= buff>>8;//高字节
	  
	  buff = C*10+3200;	//最大允许持续充电电流
	  mg.data[4]= buff;//低字节 
	  mg.data[5]=buff>>8;//高字节 
    
    buff = HIGHEST_ALLOWED_CHARGE_V*10;//最高允许充电总电压
    mg.data[6] = buff;  //
    mg.data[7] = buff>>8; //
    
    mg.id= 0x181AD0F3;
    while((!MSCAN0SendMsg(mg))&&(tt>0))
        tt--; 
}

//******************************************************************************
//* Function name:   BMS4_transmit181B
//* Description:     整车报文,100ms
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void BMS4_transmit181B(void) 
{
    struct can_msg mg;
    unsigned int buff;
    unsigned char tt=100;
    
    mg.RTR= FALSE;  
    mg.len = 8;
    mg.prty = 0;

    mg.data[0] = 0;  //保留
    buff = HIGHEST_ALLOWED_CHARGE_V*10;//最高允许放电总电压
    mg.data[1] = buff;  //
    mg.data[2] = buff>>8; // 
	  
	  buff = (-biggestFeedbackCurrent)*10+3200;	//最大允许回馈电流
	  mg.data[3]= buff;// 低字节 
	  mg.data[4]= buff>>8;//高字节
	  
    
    buff = HIGHEST_ALLOWED_CHARGE_CV*100;
    mg.data[5] = buff;  //
    mg.data[6] = buff>>8; //
    
    mg.data[7] = HIGHEST_ALLOWED_Tem+40;
    
    mg.id= 0x181BD0F3;
    while((!MSCAN0SendMsg(mg))&&(tt>0))
        tt--; 
}

//******************************************************************************
//* Function name:   BMS5_transmit181C
//* Description:     整车报文,100ms
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void BMS5_transmit181C(void) 
{
    struct can_msg mg;
    unsigned int buff;
    unsigned char tt=100;
    
    mg.RTR= FALSE;  
    mg.len = 8;
    mg.prty = 0;

    
    buff = LOWEST_ALLOWED_DISCHARGE_V*10;//最低允许放电总电压
    mg.data[0] = buff;  //
    mg.data[1] = buff>>8; // 
	  
	  mg.data[2]= Can181cByte3.byte;//连接器故障	  
	  mg.data[3] = 0;    
    mg.data[4] = Can181cByte5.byte;  //充电枪连接状态及故障处理方式
    mg.data[5] = 0;     
    mg.data[6] =0;
    mg.data[7] =0;
    
    mg.id= 0x181CD0F3;
    while((!MSCAN0SendMsg(mg))&&(tt>0))
        tt--; 
}


//******************************************************************************
//* Function name:   BMS6_transmit181D
//* Description:     整车报文,100ms
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void BMS6_transmit181D(void) 
{
    struct can_msg mg;
    unsigned int buff;
    unsigned char tt=100;
    
    mg.RTR= FALSE;  
    mg.len = 8;
    mg.prty = 0;

    
    mg.data[0] = 0;  
    mg.data[1] = 0;  
	  
	  mg.data[2] = 0;	  
	  mg.data[3] = 0;    
    mg.data[4] = 0;
    mg.data[5] = 0;     
    mg.data[6] = 0;
    mg.data[7] = 0;
    
    mg.id= 0x181DD0F3;
    while((!MSCAN0SendMsg(mg))&&(tt>0))
        tt--; 
}
//******************************************************************************
//* Function name:   BMS7_transmit18F2
//* Description:     整车报文(报警信息)100ms
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void BMS7_transmit18F2(void) 
{
    struct can_msg mg;
    unsigned int buff;
    unsigned char tt=100;
    
    mg.RTR= FALSE;  
    mg.len = 8;
    mg.prty = 0;

    
    mg.data[0] = 0;  
    mg.data[1] = 0;  
	  
	  mg.data[2] = 0; 
	  mg.data[3] = 0;    
    mg.data[4] = 0;
    mg.data[5] = 0;     
    mg.data[6] = 0;
    mg.data[7] = 0;
    
    mg.id= 0x18F214F3;
    while((!MSCAN0SendMsg(mg))&&(tt>0))
        tt--; 
}

//******************************************************************************
//* Function name:   BMS_transmitFF2A
//* Description:     整车报文(电池组相关报文),1000ms
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void BMS_transmitFF2A(void) 
{
    struct can_msg mg;
    unsigned int buff;
    unsigned char tt=100;
    
    mg.RTR= FALSE;  
    mg.len = 8;
    mg.prty = 0;

       
    mg.data[0] = HCellBoxNum; //单体最高电压箱号
    mg.data[1] = HCellLocation; //单体最高电压箱内位置 
	  
	  mg.data[2] = LCellBoxNum;//单体最低电压箱号	  
	  mg.data[3] = LCellLocation;//单体最低电压箱内位置     
    mg.data[4] = HTemBoxNum;//温度最高箱号
    mg.data[5] = HTemLocation;//温度最高箱内位置     
    mg.data[6] = LTemBoxNum;//温度最低箱号
    mg.data[7] = LTemLocation;//温度最低箱内位置 
    
    mg.id= 0x18FF2AF4;
    while((!MSCAN0SendMsg(mg))&&(tt>0))
        tt--; 
}
//******************************************************************************
//* Function name:   BMS_transmitFF2C
//* Description:     整车报文(电池组相关报文),1000ms
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void BMS_transmitFF2C(void) 
{
    struct can_msg mg;
    unsigned int buff;
    unsigned char tt=100;
    
    mg.RTR= FALSE;  
    mg.len = 8;
    mg.prty = 0;

    buff=2.8*100;   
    mg.data[0] = buff; //单体低阀值(1级)
    mg.data[1] = buff>>8; //
	  
	  buff=3.65*100;
	  mg.data[2] = buff;//单体高阀值(1级)	  
	  mg.data[3] = buff>>8;//  
	    
    mg.data[4] = -20+40;//温度低阀值(1级)
    
    mg.data[5] = 45+40;//温度高阀值(1级)
      
    mg.data[6] = 0;//保留
    mg.data[7] = 0;//保留 
    
    mg.id= 0x18FF2CF4;
    while((!MSCAN0SendMsg(mg))&&(tt>0))
        tt--; 
}

//******************************************************************************
//* Function name:   BMS_transmitFF2D
//* Description:     整车报文(电池组相关报文),1000ms
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void BMS_transmitFF2D(void) 
{
    struct can_msg mg;
    unsigned int buff;
    unsigned char t;
    unsigned char tt=100;
    
    mg.RTR= FALSE;  
    mg.len = 8;
    mg.prty = 0;

       
    mg.data[0] = BOX_NUMBER; //电池箱个数
    mg.data[1] = ALL_CELL_NUMBER; //电池串数
	  
	  mg.data[2] = ALL_Tem_NUMBER;//温度总个数	  
	  mg.data[3] = 0;//
	  
	  buff = SYS_NOMINAL_V*10;//额定总电压     
    mg.data[4] = buff;
    mg.data[5] = buff>>8;
          
    mg.data[6] = SYS_KWH*2/3;//额定能量
    
    mg.data[7] = g_socValue*C*g_highVoltageV1*3/2000;//剩余能量soc*240*V1/1000*分辨率
        
    mg.id= 0x18FF2DF4;
    while((!MSCAN0SendMsg(mg))&&(tt>0))
        tt--; 
}
//******************************************************************************
//* Function name:   BMS_transmitFF2E
//* Description:     整车报文(电池组相关报文),1000ms
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void BMS_transmitFF2E(void) 
{
    struct can_msg mg;
    unsigned int buff;
    unsigned char t;
    unsigned char tt=100;
    
    mg.RTR= FALSE;  
    mg.len = 8;
    mg.prty = 0;

       
    mg.data[0] = BOX1_CELL_NUMBER; // 电箱1单体个数
    mg.data[1] = BOX2_CELL_NUMBER; // 电箱2单体个数
	  
	  mg.data[2] = BOX3_CELL_NUMBER; // 电箱3单体个数	  
	  mg.data[3] = BOX4_CELL_NUMBER; // 电箱4单体个数;//
	      
    mg.data[4] = BOX5_CELL_NUMBER; // 电箱5单体个数
    mg.data[5] = BOX6_CELL_NUMBER; // 电箱6单体个数
          
    mg.data[6] = 0;//保留
    mg.data[7] = 0;//保留
        
    mg.id= 0x18FF2EF4;
    while((!MSCAN0SendMsg(mg))&&(tt>0))
        tt--; 
}
//******************************************************************************
//* Function name:   BMS_transmitFF30
//* Description:     整车报文(电池组相关报文),1000ms
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void BMS_transmitFF30(void) 
{
    struct can_msg mg;
    unsigned int buff;
    unsigned char t;
    unsigned char tt=100;
    
    mg.RTR= FALSE;  
    mg.len = 8;
    mg.prty = 0;

       
    mg.data[0] = BOX1_TEM_NUMBER; // 电箱1温度个数
    mg.data[1] = BOX2_TEM_NUMBER; // 电箱2温度个数
	  
	  mg.data[2] = BOX3_TEM_NUMBER; // 电箱3温度个数  
	  mg.data[3] = BOX4_TEM_NUMBER; // 电箱4温度个数
	      
    mg.data[4] = BOX5_TEM_NUMBER; // 电箱5温度个数
    mg.data[5] = BOX6_TEM_NUMBER; // 电箱6温度个数
          
    mg.data[6] = 0;//保留
    mg.data[7] = 0;//保留
        
    mg.id= 0x18FF30F4;
    while((!MSCAN0SendMsg(mg))&&(tt>0))
        tt--; 
}
//******************************************************************************
//* Function name:   BMS_transmitF100
//* Description:     整车报文(电池组相关报文),1000ms
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void BMS_transmitF100(void) 
{
    struct can_msg mg;
    unsigned int buff;
    unsigned char t;
    unsigned char tt=100;
    
    mg.RTR= FALSE;  
    mg.len = 8;
    mg.prty = 0;

    t=(0x24|PARA_NUM);    
    mg.data[0] = t; // 电箱1温度个数
    mg.data[1] = 0; // 保留
	  
	  mg.data[2] = Canf100Byte3.byte;
	  mg.data[3] = 0;
	      
    mg.data[4] = 0;
    mg.data[5] = 0;
          
    mg.data[6] = 0;//实际容量(AH)
    mg.data[7] = 0;//额定容量(AH)
                                                    
    mg.id= 0x18F100F4;
    while((!MSCAN0SendMsg(mg))&&(tt>0))
        tt--; 
}
//******************************************************************************
//* Function name:   BMS_transmitFF33
//* Description:     整车报文(电池组相关报文),1000ms
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void BMS_transmitFF33(void) 
{
    struct can_msg mg;
    unsigned int buff;
    unsigned char t;
    unsigned char tt=100;
    
    mg.RTR= FALSE;  
    mg.len = 8;
    mg.prty = 0;
    
    mg.data[0] = Box1HTem; // 电箱1最高温度
    mg.data[1] = Box1LTem; // 电箱1最低温度
	  
	  mg.data[2] = Box2HTem; // 电箱2最高温度
	  mg.data[3] = Box2LTem; // 电箱2最低温度
	      
    mg.data[4] = Box3HTem; // 电箱3最高温度
    mg.data[5] = Box3LTem; // 电箱3最低温度
          
    mg.data[6] = Box4HTem; // 电箱4最高温度
    mg.data[7] = Box4LTem; // 电箱4最低温度
        
    mg.id= 0x18FF33F4;
    while((!MSCAN0SendMsg(mg))&&(tt>0))
        tt--; 
}
//******************************************************************************
//* Function name:   BMS_transmitFF34
//* Description:     整车报文(电池组相关报文),1000ms
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void BMS_transmitFF34(void) 
{
    struct can_msg mg;
    unsigned int buff;
    unsigned char t;
    unsigned char tt=100;
    
    mg.RTR= FALSE;  
    mg.len = 8;
    mg.prty = 0;
    
    mg.data[0] = Box5HTem; // 电箱5最高温度
    mg.data[1] = Box5LTem; // 电箱5最低温度
	  
	  mg.data[2] = Box6HTem; // 电箱6最高温度
	  mg.data[3] = Box6LTem; // 电箱6最低温度
	      
    mg.data[4] = 0;
    mg.data[5] = 0;
          
    mg.data[6] = 0;
    mg.data[7] = 0;
        
    mg.id= 0x18FF34F4;
    while((!MSCAN0SendMsg(mg))&&(tt>0))
        tt--; 
}
//******************************************************************************
//* Function name:   BMS_transmitFF50
//* Description:     整车报文(电池组相关报文),500ms
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void BMS_transmitFF50(void) 
{
    struct can_msg mg;
    unsigned int buff;
    unsigned char t;
    unsigned char tt=100;
    
    mg.RTR= FALSE;  
    mg.len = 8;
    mg.prty = 0;
    
    mg.data[0] = ChaStartH; // 充电开始时间(时)BCD码
    mg.data[1] = ChaStartM; // 充电开始时间(分)BCD码	  
	  mg.data[2] = ChaStartS; // 充电开始时间(秒)BCD码
	  
	  mg.data[3] = ChaOverH; // 充电结束时间(时)BCD码	      
    mg.data[4] = ChaOverM; // 充电结束时间(分)BCD码
    mg.data[5] = ChaOverS; // 充电结束时间(秒)BCD码
    
    //充电电压
    buff=ChangerVol*10;//快充充电机输出电压      
    mg.data[6] = buff;
    mg.data[7] = buff>>8;
        
    mg.id= 0x18FF50F4;
    while((!MSCAN0SendMsg(mg))&&(tt>0))
        tt--; 
}

//******************************************************************************
//* Function name:   BMS_transmitFF51
//* Description:     整车报文(电池组相关报文),500ms
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void BMS_transmitFF51(void) 
{
    struct can_msg mg;
    unsigned int buff;
    unsigned char t;
    unsigned char tt=100;
    
    mg.RTR= FALSE;  
    mg.len = 8;
    mg.prty = 0;
    
    mg.data[0] = ChaStartH; // 充电开始时间(时)BCD码
    mg.data[1] = ChaStartM; // 充电开始时间(分)BCD码	  
	  mg.data[2] = ChaStartS; // 充电开始时间(秒)BCD码
	  
	  mg.data[3] = ChaOverH; // 充电结束时间(时)BCD码	      
    mg.data[4] = ChaOverM; // 充电结束时间(分)BCD码
    mg.data[5] = ChaOverS; // 充电结束时间(秒)BCD码
    
    //充电电压
    buff=ChangerCurt*10;//快充充电机输出电压      
    mg.data[6] = buff;
    mg.data[7] = buff>>8;
        
    mg.id= 0x18FF51F4;
    while((!MSCAN0SendMsg(mg))&&(tt>0))
        tt--; 
}
//***********************************************************************
//************************************************************************
//*************************the end*************************************
//************************************************************************