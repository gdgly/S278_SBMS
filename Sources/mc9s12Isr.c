//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//---------------------- Pride Power------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//* Project Name       : C50ES_FP
//* File Name          : mc9s12Isr.c
//* Author             : Judy
//* Version            : V1.0.0
//* Start Date         : 2011,05,26
//* Description        : 该文件处理系统所有的中断子程序，包括RTI中断、ADC中断、CAN接收中断（3路）
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
#include "BMS20.h"
//*******************************************
//*******************************************
float ac[12];
float ax[12];
//
struct can_msg *msg;
unsigned long g_mboxID;
unsigned char g_mboxData[1][8];// CAN2接收到的报文数据，每个报文8个字节

unsigned char CHMStep=0; //快充步骤

unsigned char Int_Flag =0;// 0x01:AD中断 0x02:can0中断 

unsigned char g_errorRecordRead_flag;//故障信息读取标志
unsigned char stateCode=0; //状态机编码

unsigned char TestState=0;//状态机进入到测试状态

long Priority = 6;

unsigned char DC_Start=0;//接收到0xaa后,开始闭合负极继电器

unsigned char BootState = 0;//boot程序

unsigned char TurnOnChangerRelay = 1;//闭合充电继电器

unsigned int DC_ChargeTime=0; //累计充电时间

unsigned char FlagBRMSend = 0; //BMS多帧报文标志位,0不发送,1发送
unsigned char BRMStep = 0; //BRM已发送标志位,0未发送或清零;1已发送

unsigned char FlagBCPSend = 0;//发送BCP后,收到充电机返回的消息,开始发送数据段
unsigned char BCPStep = 0;    //BCP中的步骤0:发送BCP的头;1:发送BCP的数据段

unsigned char FlagBCSSend = 0;//发送BCS后,收到充电机返回的消息,开始发送数据段
unsigned char BCSStep = 0;    //BCS中的步骤0:发送BCS的头;1:发送BCP的数据段


unsigned char FireMessage[10]=0; //接收10个箱体的信息
//unsigned char InsRelayControl=0; //接收BMS板绝缘控制信息
//unsigned char Heat_P_Realy_Status = 0; // receive the heat paostive relay status from BMS master board, 0 = OFF, 1 = ON
BMS_SBMS_CTRL_CMD g_bms_sbms_ctrl_cmd;
//******************************************************************************
//* Function name:   SendMes
//* Description:     发送整车CAN报文
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void SendMes(void)
{
  
}
//******************************************************************************
//* Function name:   PIT0_ISR
//* Description:     10ms定时中断子程序,在此主要发送给充电机的0x650报文
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
#pragma CODE_SEG NON_BANKED
interrupt void PIT0_ISR(void)   //10ms中断一次
{   
    
    while (PITTF&0x01) 
        PITTF |= 0x01;        //写1 to clean timeout flag
    SendMes();
    if(BMUOK==1)
        TaskDC();//调用直流充电流程     
   	
}
#pragma CODE_SEG DEFAULT
//******************************************************************************
//* Function name:   RTI_ISR
//* Description:     1ms定时中断子程序,用于设置一些时间标志位以及对已使用电量进行积分
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
#pragma CODE_SEG NON_BANKED
interrupt void RTI_ISR(void)      //1ms中断一次
{ 
    static unsigned char numberCounter=0; 
    static unsigned char ledFlag=0;
    static unsigned char PIT_5ms_Counter0=0;       //5ms定时计数器 
    static unsigned char PIT_5ms_Counter1=3;       //5ms定时计数器 

    PIT_5ms_Counter0++;
    PIT_5ms_Counter1++;
    //turnOnLED0(); 	     // for debug 
    TaskRemarks();  //******
    if(PIT_5ms_Counter0>5) 
    {
        BMS_To_VCU_BasicMsg0();
        PIT_5ms_Counter0=0;
    }
    if(PIT_5ms_Counter1>8) 
    {
        BMS_To_VCU_BasicMsg1();
        PIT_5ms_Counter1=3;
    }
        
    if((g_systemCurrent>-500)&&(g_systemCurrent<500))  //防止负数溢出
        g_energyOfUsed =g_energyOfUsed+ g_systemCurrent*0.0011+0.0000042;// // 1m秒积分一次
    //turnOffLED0(); 	     // for debug 
    // clear RTIF bit 
    CRGFLG = 0x80; 
}
#pragma CODE_SEG DEFAULT
//******************************************************************************
//* Function name:   ADC0_ISR
//* Description:     ADC中断子程序,获取2个AD通道的电流  DHAB/S24
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
#pragma CODE_SEG NON_BANKED
interrupt void ADC0_ISR(void) 
{	
    static unsigned char ADcount=0;   
    ac[ADcount]=(((float)ATD0DR1-ADCLC)*5.0/4096-2.5)*250;//CUR_AD用于大电流通道  CUR_AD1  //S24
    ax[ADcount]=(((float)ATD0DR0-ADCLX)*5.0/4096-2.5)*250;//CUR_AD用于大电流通道  CUR_AD2  //S24
    //ax[ADcount]=(((float)ATD0DR0-ADCLX)*5.0/4096-2.5)*37.45;//X_AD  //用于小通道 CUR_AD2 电流
    ATD0STAT0_SCF=1;	//清除队列完成标志	                                                                                  
    ADcount++;	

    if(ADcount>=12)
    {
        ADcount =0;
        Int_Flag |= 0x01;
        turnOffADC();
    }	
}
#pragma CODE_SEG DEFAULT
//******************************************************************************
//* Function name:   CAN0_RECEIVE_ISR
//* Description:     CAN0接收中断子程序,在该项目中用于接收交流充电机发来的报文0x6d0
//*                  //车载 /外部CAN / 500bd  /标准帧格式
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
#pragma CODE_SEG NON_BANKED
interrupt void CAN0_RECEIVE_ISR(void)   //车载 /外部CAN / 500Hz
{
   unsigned char i,msgData[8];
   unsigned long msgCan0RxID;
   unsigned long pp=0;
   unsigned long pp1=0;
   unsigned int buffer = 0;
   unsigned int VCUSpeed = 0;

   DisableInterrupts;   
   
   if(!(CAN0RFLG&0x01)) // Checks for received messages
        return ;
   //msgCan0RxID = (unsigned int)(CAN0RXIDR0<<3) |       //standard
   //         (unsigned char)(CAN0RXIDR1>>5); 
   pp = (unsigned long)CAN0RXIDR0<<15;  
   pp1 = (unsigned long)((CAN0RXIDR1>>5)&0x07)<<15;                              
   msgCan0RxID = ((unsigned long)pp<<6) | //expanded
              (pp1<<3) |
              ((unsigned long)(CAN0RXIDR1&0x07)<<15)|
              ((unsigned long)CAN0RXIDR2<<7) | 
              ((unsigned long)CAN0RXIDR3>>1);
   
    for(i=0;i<8;i++) 
        msgData[i] = *((&CAN0RXDSR0)+i);  
                     
    switch(msgCan0RxID) 
    {   
        case 0x000c0125:
             g_bms_sbms_ctrl_cmd.InsRelayControl = msgData[0];
             g_bms_sbms_ctrl_cmd.Heat_P_Realy_Status = msgData[1];
             g_bms_sbms_ctrl_cmd.Kchg_N_ctrl_cmd = msgData[2];
             break;
     
        default:
            break;
    }
    CAN0RFLG = 0x01;   
    EnableInterrupts; 
}
#pragma CODE_SEG DEFAULT
//******************************************************************************
//* Function name:   CAN1_RECEIVE_ISR
//* Description:     CAN1接收中断子程序,用于快充通信  250bd /扩展帧模式                
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
#pragma CODE_SEG NON_BANKED
interrupt void CAN1_RECEIVE_ISR(void)  //充电
{
    unsigned long can1ReceiveID;
    unsigned char i,fChg2bmsbyte[8];
    unsigned long pp=0;
    unsigned long pp1=0;
    unsigned int VolLow=0;
    unsigned int VolHigh=0;
  
    if(!(CAN1RFLG&0x01))
    {
        CAN1RFLG = 0x01;     
        EnableInterrupts;
        return;
    }
    
    pp = (unsigned long)CAN1RXIDR0<<15;
    pp1 = (unsigned long)((CAN1RXIDR1>>5)&0x07)<<15; 
    
    can1ReceiveID =((unsigned long)pp<<6) | //expanded
              (pp1<<3) |
              ((unsigned long)(CAN1RXIDR1&0x07)<<15)|
              ((unsigned long)CAN1RXIDR2<<7) | 
              ((unsigned long)CAN1RXIDR3>>1);
    for(i=0;i<8;i++) 
        fChg2bmsbyte[i] = *((&CAN1RXDSR0)+i);
       
    switch(can1ReceiveID)
    {
        case 0x1826f456:
        //if((fChg2bmsbyte[0]==0x01)&&(fChg2bmsbyte[1]==0x01)&&(fChg2bmsbyte[2]==0))
        //{                    
            DCStartState=2;//新国标开始的标志 
            DC_Vesion = 2; //新国标
            if(CHMStep<=0x01)
                CHMStep=0x01;
        //}
            
            break;
        case 0x1801f456:
            CRMOverTimeBefore60s = 0;
            if( fChg2bmsbyte[0]==0x00)
            {              
                DCStartState=1;//老国标开始的标志                 
                CRMOverTimeBefore = 0;
                if(CHMStep<=0x01)                
                    CHMStep=0x01;
            }
            else if( fChg2bmsbyte[0]==0xaa)
            {
                CRMOverTime=0;
                DC_Start = 1;
                if(CHMStep<=0x02)
                    CHMStep=0x02;
                //TurnOn_INA2K();
                //SelfState2=1;
                            
            }            
            break;
            
        case 0x1cecf456:
           if((fChg2bmsbyte[0]==0x11)&&(fChg2bmsbyte[5]==0x00)&&(fChg2bmsbyte[6]==0x02)&&((fChg2bmsbyte[7]==0x00)))
               FlagBRMSend = 1; //标志位置1,开始发送多帧保证,并且不发送BRM
                //if((fChg2bmsbyte[0]==0x13)&&(fChg2bmsbyte[5]==0x00)&&(fChg2bmsbyte[6]==0x02)&&((fChg2bmsbyte[7]==0x00)))
                     //BRMStep = 0;//将标志位清零,可以从新发生BRM
                     
            if((fChg2bmsbyte[0]==0x11)&&(fChg2bmsbyte[5]==0x00)&&(fChg2bmsbyte[6]==0x06)&&((fChg2bmsbyte[7]==0x00)))
                 FlagBCPSend = 1;
            //if((fChg2bmsbyte[0]==0x13)&&(fChg2bmsbyte[5]==0x00)&&(fChg2bmsbyte[6]==0x06)&&((fChg2bmsbyte[7]==0x00)))
                 //BCPStep = 0;//将标志位清零,可以从新发生BRM
            
            if((fChg2bmsbyte[0]==0x11)&&(fChg2bmsbyte[5]==0x00)&&(fChg2bmsbyte[6]==0x11)&&((fChg2bmsbyte[7]==0x00)))
                 FlagBCSSend = 1;
            
            //if((fChg2bmsbyte[0]==0x13)&&(fChg2bmsbyte[5]==0x00)&&(fChg2bmsbyte[6]==0x11)&&((fChg2bmsbyte[7]==0x00)))
                 //BCSStep = 0;//将标志位清零,可以从新发生BRM
            
            break;
                
        case 0x1807f456:    
            //CHMStep=0x03;
            break;  
        case 0x1808f456:
            VolHigh = fChg2bmsbyte[1];
            VolHigh = VolHigh<<8;
            VolHigh = (VolHigh+fChg2bmsbyte[0]);

            VolLow = fChg2bmsbyte[3];
            VolLow = VolLow<<8;
            VolLow = (VolLow+fChg2bmsbyte[2]);
            if((VolLow>HIGHEST_VOL)||(VolHigh<LOWEST_VOL))
            {
                BROErrorAA=1;//不满足充电条件            
            } 
            else
            {
                BROErrorAA=0;//满足充电条件
            }
            if(CHMStep<=0x03)   
                CHMStep=0x03;
            break;  
        case 0x100af456:
            CROOverTime = 0;//收到CRO后清零
            if( fChg2bmsbyte[0]==0xaa)
            {
                if(CHMStep<=0x04)
                    CHMStep=0x04;
                SelfState3=1;
                CROOverTime60s = 0;//收到0xaa后清零           
            } 
            break; 
        case 0x1812f456:
            CCSOverTime=0;
                        
            DC_ChargeTime = fChg2bmsbyte[5];
            DC_ChargeTime = VolHigh<<8;
            DC_ChargeTime = (VolHigh+fChg2bmsbyte[4]);  
            if(CHMStep<=0x05)
                CHMStep=0x05;
            break;
        case 0x101af456:
            
            m_askcurrent=0;//请求电流为0 
            if(((fChg2bmsbyte[0])&(0x40)) == 0)
            {
                fastend1 = 0x40;
            }
            CSTOverTime=0;
            if(CHMStep<=0x06)    
                CHMStep=0x06;
            break; 
                  
        case 0x181df456:
            CSDOverTime=0;
                         
            if(CHMStep<=0x07)     
                CHMStep=0x07;
            break;
        case 0x081ff456:
            BEMStop = 1;
            if(CHMStep<=0x07)     
                CHMStep=0x07;
            break;
      default:
            break;
    }//end of swtich
    // Clear RXF flag (buffer ready to be read)
    CAN1RFLG = 0x01;        
}

#pragma CODE_SEG DEFAULT
//******************************************************************************
//* Function name:   CAN2_RECEIVE_ISR
//* Description:     CAN2接收中断子程序,用于与BMU和上位机进行内部报文通信
//*                  内部/ 250Bd /扩展帧 
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
#pragma CODE_SEG NON_BANKED
interrupt void CAN2_RECEIVE_ISR(void)   //内部  BMU   250Hz
{
    unsigned char i,j;
    unsigned int temper;
    unsigned long pp=0;
    unsigned long pp1=0;
    unsigned long DBuffer[1];
    unsigned int DBuffer1[1];
    unsigned int buffer = 0;
    unsigned int V2B = 0;
    unsigned int V3B = 0;
    unsigned int HeatCurrent = 0; 
    //unsigned long int DBuffer[1];
    // Checks for received messages
    if(!(CAN2RFLG&0x01))
    {
        CAN2RFLG = 0x01;     
        EnableInterrupts;
        return;
    }  
    
    
    pp = (unsigned long)CAN2RXIDR0<<15;  
    pp1 = (unsigned long)((CAN2RXIDR1>>5)&0x07)<<15;                              
    g_mboxID = ((unsigned long)pp<<6) | //expanded
               (pp1<<3) |
               ((unsigned long)(CAN2RXIDR1&0x07)<<15)|
               ((unsigned long)CAN2RXIDR2<<7) | 
               ((unsigned long)CAN2RXIDR3>>1);
   
    for(i=0;i<8;i++)
        g_mboxData[0][i]= *((&CAN2RXDSR0)+i);
  
    switch(g_mboxID)//
    {
        ///////
        case 0x18B0F601://接收创为灭火器信息 再通过整车CAN发出去
            FireMessage[0]= (g_mboxData[0][7]&0x00ff)>>4;  //取bit4-7   
            break;
            
        case 0x18B0F602://接收创为灭火器信息 再通过整车CAN发出去
            FireMessage[1]= (g_mboxData[0][7]&0x00ff)>>4;  //取bit4-7 
            break;
            
        case 0x18B0F603://接收创为灭火器信息 再通过整车CAN发出去
            FireMessage[2]= (g_mboxData[0][7]&0x00ff)>>4;  //取bit4-7    
            break;
            
        case 0x18B0F604://接收创为灭火器信息 再通过整车CAN发出去
            FireMessage[3]= (g_mboxData[0][7]&0x00ff)>>4;  //取bit4-7    
            break;
            
        case 0x18B0F605://接收创为灭火器信息 再通过整车CAN发出去
            FireMessage[4]= (g_mboxData[0][7]&0x00ff)>>4;  //取bit4-7   
            break;
            
        case 0x18B0F606://接收创为灭火器信息 再通过整车CAN发出去
            FireMessage[5]= (g_mboxData[0][7]&0x00ff)>>4;  //取bit4-7 
            break;
            
            
        case 0x18B0F607://接收创为灭火器信息 再通过整车CAN发出去
            FireMessage[6]= (g_mboxData[0][7]&0x00ff)>>4;  //取bit4-7    
            break;
            
        case 0x18B0F608://接收创为灭火器信息 再通过整车CAN发出去
            FireMessage[7]= (g_mboxData[0][7]&0x00ff)>>4;  //取bit4-7    
            break;
            
        case 0x18B0F609://接收创为灭火器信息 再通过整车CAN发出去
            FireMessage[8]= (g_mboxData[0][7]&0x00ff)>>4;  //取bit4-7    
            break;
            
        case 0x18B0F60a://接收创为灭火器信息 再通过整车CAN发出去
            FireMessage[9]= (g_mboxData[0][7]&0x00ff)>>4;  //取bit4-7    
            break;
 
      
        /*case 0xC06D0:
            ACTem1 = g_mboxData[0][3];
            ACTem2 = g_mboxData[0][4];
            if(input5_state()==0) 
            {    
                CC2VOL = g_mboxData[0][7];
                CC2VOL = (CC2VOL << 8);       
                CC2VOL = (CC2VOL+g_mboxData[0][6]);
                DC_CC2Count=1;
            }
            break;
            */
       /* case 0x18ff01ab:
            V2B = g_mboxData[0][1];
            g_highVoltageV2B = V2B*0.02;
            V3B = g_mboxData[2][3];
            g_highVoltageV3B = V3B*0.02;
            HeatCurrent = g_mboxData[4][5];
            g_HeatCurrent = (HeatCurrent*0.02+400); 
            break;
        */
        /*case 0xC01EE3F:
            if((g_mboxData[0][0]==0x55)&&(g_mboxData[0][1]==0xAA))
            {
                TestState=1;//测试标志位置1,进入测试状态机177
                g_BmsModeFlag = TESTSTATE;//调试模式
            }
            //InTestToPC();
            break;
        case 0xC01EE27: //上下电调试
            if(g_mboxData[0][2]==0x55)//正极继电器闭合
            {
                //闭合总正    
        	      TurnOn_INZK();//吸合正极接触器
        	 //     state_group1.Bit.St_P_Relay=1;//to vcu
        	      
            }              
            if(g_mboxData[0][2]==0xaa)  
            { 
                TurnOff_INZK();//断开正极接触器
          //      state_group1.Bit.St_P_Relay=1;//to vcu  
                                          
            }
            bmsToPcInfo552(); //发送继电器状态，完成，BMS保持当前状态  
            break;

        case 0xC01EE3a: //预充
            if(g_mboxData[0][2]==0X55) 
            {
                   
        	      TurnOn_INBK();//吸合预充接触器
        	       
        	      
            }
            if(g_mboxData[0][2]==0xAA)  
            { 
                TurnOff_INBK();//断开预充接触器
                
            }        
            break;
        case 0xC01EE3b://加热
            if(g_mboxData[0][2]==0x55) 
            {   
        	      TurnOn_INHK();//吸合加热接触器        	      
        	      
            }
            if(g_mboxData[0][2]==0xAA)  
            { 
                TurnOff_INHK();//断开加热接触器
                
            } 
            break;
        case 0xC01EE3d: //充电
            if(g_mboxData[0][2]==0x55) 
            {   
        	      TurnOn_INA2K();//吸合充电接触器
        	      
        	      
            }
            if(g_mboxData[0][2]==0xAA)  
            { 
                TurnOff_INA2K();//断开充电接触器
                
            } 
            break;        
        case 0xC01EE3e: //负极继电器控制0xC01EE3a         
            if(g_mboxData[0][2]==0x55) 
            {
                //闭合总负    
        	      TurnOn_INFK();//吸合负极接触器
        	 //     state_group1.Bit.St_N_Relay=1;//to vcu 
        	      
            }
            if(g_mboxData[0][2]==0xaa)  
            { 
                TurnOff_INFK();//断开负极接触器
         //       state_group1.Bit.St_N_Relay=0;//to vcu
                
            }
            break;
        
        case 0xC0777: //清除充放电容量
            DBuffer[0]=0;
            DBuffer1[0]=0;
            DisableInterrupts;
            Write24c64_Byte(AH_CHARGE_ADDR,(unsigned char* )DBuffer,4);
            Write24c64_Byte(AH_DISCHARGE_ADDR,(unsigned char* )DBuffer,4);
            Write24c64_Byte(ChangerTime_ADDR,(unsigned char* )DBuffer1,2);
            DisableInterrupts;
            break;
       case 0xC0888:
       
            ReadChargeDischargeAH();//读取数据
            //StoreChargeTime();
            ReadAHRecord();//发送数据0xC0AAA
                       
            break;
            
        case 0xC0999:
            //StoreChargeTime();
            break;
            */
        case 0x18FF0A00:         
            BMNVPNflag.Bit.flag0=1;
            if(g_mboxData[0][0]==25) 
            { 
                g_mboxData[0][0]=0;
                BMNVPNflag.Bit.flag4=1;
            }
            if(g_mboxData[0][0]==35) 
            { 
                g_mboxData[0][0]=0;
                BMNVPNflag.Bit.flag6=1;
            } 
            Task13_Vpn_Value_Calibrate();       //校准总电压       
            break;
        case  0x18FF0A01:
            Vpn_K1 = (((unsigned int)g_mboxData[0][0]&0x00ff)<<8) | g_mboxData[0][1];
            Vpn_B1 = (((unsigned int)g_mboxData[0][2]&0x00ff)<<8) | g_mboxData[0][3];
            KB_DATA_SAVE();
            break;
        case  0x18FF0A02:
            Vpn_K2 = (((unsigned int)g_mboxData[0][0]&0x00ff)<<8) | g_mboxData[0][1];
            Vpn_B2 = (((unsigned int)g_mboxData[0][2]&0x00ff)<<8) | g_mboxData[0][3];
            KB_DATA_SAVE();
            break;
        case  0x18FF0A03:
            Vpn_K3 = (((unsigned int)g_mboxData[0][0]&0x00ff)<<8) | g_mboxData[0][1];
            Vpn_B3 = (((unsigned int)g_mboxData[0][2]&0x00ff)<<8) | g_mboxData[0][3];
            KB_DATA_SAVE();
            break;
        
       /* case 0x18ff6200:  //收到读取故障信息的命令
            g_errorRecordRead_flag=1;  
            break;
        case 0x18ff6201: //清除所有故障信息
            g_errorCounter=0;
            temper = g_errorCounter;
            Write24c64_Byte(ERROR_COUNT_ADDR,(unsigned char *)&temper,2);//更新故障计数器 
            temper = 0;
            Read24c64_Byte(ERROR_COUNT_ADDR,(unsigned char *)&temper,2);
            g_errorCounter = temper;
            break;
         */
        case 0x0a0218fb://进入BOOT程序
            DFlash_Erase_Sector(0x0000); //必须先擦除
            DataBuffer[0] = 0xaaaa;
            DFlash_Write_Word(0x0000);//写入boot标志0xaaaa,当boot程序读到这个标志，则执行bootloader下载程序，反之，则执行应用程序                 
            COPCTL = 0x07;        //enable watchdog
            ARMCOP = 0x00;        //feed 0 to watch dog for reset 
            BootState = 1;
            break;
        //case 0x0c14ee00:      //读取时钟
        //case 0xc01ee14:       //设置时钟
        //case 0xC01ee15:       //SOC设置
        //case 0xC01ef15:       //设置显示SOC
        //case 0xC01ee16:       //剩余容量
        //case 0xC01ee17:        //BMU个数
        //case 0xC01ee18:        //软件版本号
        case 0xC01EE28:         //接收系统参数修正信息from 上位机
        case 0xC01EE29:         //接收系统参数修正信息from 上位机
        case 0xC01EE2a:         //接收系统参数修正信息from 上位机
        case 0xC01EE2b:         //接收系统参数修正信息from 上位机
        case 0xC01EE26:
            //ParameterSetting();
            break;
        default:
            Int_Flag |= 0x08;
            //BMU_Processure();
            break;  
    }//end of switch      	 		
    // Clear RXF flag (buffer ready to be read)
    CAN2RFLG = 0x01;       
  
}
#pragma CODE_SEG DEFAULT

//*******************************************
//*******************************************