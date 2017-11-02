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
//* Description        : ���ļ�����ϵͳ���е��ж��ӳ��򣬰���RTI�жϡ�ADC�жϡ�CAN�����жϣ�3·��
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
unsigned char g_mboxData[1][8];// CAN2���յ��ı������ݣ�ÿ������8���ֽ�

unsigned char CHMStep=0; //��䲽��

unsigned char Int_Flag =0;// 0x01:AD�ж� 0x02:can0�ж� 

unsigned char g_errorRecordRead_flag;//������Ϣ��ȡ��־
unsigned char stateCode=0; //״̬������

unsigned char TestState=0;//״̬�����뵽����״̬

long Priority = 6;

unsigned char DC_Start=0;//���յ�0xaa��,��ʼ�պϸ����̵���

unsigned char BootState = 0;//boot����

unsigned char TurnOnChangerRelay = 1;//�պϳ��̵���

unsigned int DC_ChargeTime=0; //�ۼƳ��ʱ��

unsigned char FlagBRMSend = 0; //BMS��֡���ı�־λ,0������,1����
unsigned char BRMStep = 0; //BRM�ѷ��ͱ�־λ,0δ���ͻ�����;1�ѷ���

unsigned char FlagBCPSend = 0;//����BCP��,�յ��������ص���Ϣ,��ʼ�������ݶ�
unsigned char BCPStep = 0;    //BCP�еĲ���0:����BCP��ͷ;1:����BCP�����ݶ�

unsigned char FlagBCSSend = 0;//����BCS��,�յ��������ص���Ϣ,��ʼ�������ݶ�
unsigned char BCSStep = 0;    //BCS�еĲ���0:����BCS��ͷ;1:����BCP�����ݶ�

BMS_SBMS_CTRL_CMD g_bms_sbms_ctrl_cmd;
FireMsg g_FireMsg[10]; //����10������Ļ��ֱ���װ�õ���Ϣ
//******************************************************************************
//* Function name:   SendMes
//* Description:     ��������CAN����
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void SendMes(void)
{
  
}
//******************************************************************************
//* Function name:   PIT0_ISR
//* Description:     10ms��ʱ�ж��ӳ���,�ڴ���Ҫ���͸�������0x650����
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
#pragma CODE_SEG NON_BANKED
interrupt void PIT0_ISR(void)   //10ms�ж�һ��
{   
    
    while (PITTF&0x01) 
        PITTF |= 0x01;        //д1 to clean timeout flag
    SendMes();
    if(BMUOK==1)
        TaskDC();//����ֱ���������     
   	
}
#pragma CODE_SEG DEFAULT
//******************************************************************************
//* Function name:   RTI_ISR
//* Description:     1ms��ʱ�ж��ӳ���,��������һЩʱ���־λ�Լ�����ʹ�õ������л���
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
#pragma CODE_SEG NON_BANKED
interrupt void RTI_ISR(void)      //1ms�ж�һ��
{ 
    static unsigned char numberCounter=0; 
    static unsigned char ledFlag=0;
    static unsigned char PIT_5ms_Counter0=0;       //5ms��ʱ������ 
    static unsigned char PIT_5ms_Counter1=3;       //5ms��ʱ������ 

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
        
    if((g_systemCurrent>-500)&&(g_systemCurrent<500))  //��ֹ�������
        g_energyOfUsed =g_energyOfUsed+ g_systemCurrent*0.0011+0.0000042;// // 1m�����һ��
    //turnOffLED0(); 	     // for debug 
    // clear RTIF bit 
    CRGFLG = 0x80; 
}
#pragma CODE_SEG DEFAULT
//******************************************************************************
//* Function name:   ADC0_ISR
//* Description:     ADC�ж��ӳ���,��ȡ2��ADͨ���ĵ���  DHAB/S24
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
#pragma CODE_SEG NON_BANKED
interrupt void ADC0_ISR(void) 
{	
    static unsigned char ADcount=0;   
    ac[ADcount]=(((float)ATD0DR1-ADCLC)*5.0/4096-2.5)*250;//CUR_AD���ڴ����ͨ��  CUR_AD1  //S24
    ax[ADcount]=(((float)ATD0DR0-ADCLX)*5.0/4096-2.5)*250;//CUR_AD���ڴ����ͨ��  CUR_AD2  //S24
    //ax[ADcount]=(((float)ATD0DR0-ADCLX)*5.0/4096-2.5)*37.45;//X_AD  //����Сͨ�� CUR_AD2 ����
    ATD0STAT0_SCF=1;	//���������ɱ�־	                                                                                  
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
//* Description:     CAN0�����ж��ӳ���,�ڸ���Ŀ�����ڽ��ս������������ı���0x6d0
//*                  //���� /�ⲿCAN / 500bd  /��׼֡��ʽ
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
#pragma CODE_SEG NON_BANKED
interrupt void CAN0_RECEIVE_ISR(void)   //���� /�ⲿCAN / 500Hz
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
//* Description:     CAN1�����ж��ӳ���,���ڿ��ͨ��  250bd /��չ֡ģʽ                
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
#pragma CODE_SEG NON_BANKED
interrupt void CAN1_RECEIVE_ISR(void)  //���
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
            DCStartState=2;//�¹��꿪ʼ�ı�־ 
            DC_Vesion = 2; //�¹���
            if(CHMStep<=0x01)
                CHMStep=0x01;
        //}
            
            break;
        case 0x1801f456:
            CRMOverTimeBefore60s = 0;
            if( fChg2bmsbyte[0]==0x00)
            {              
                DCStartState=1;//�Ϲ��꿪ʼ�ı�־                 
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
               FlagBRMSend = 1; //��־λ��1,��ʼ���Ͷ�֡��֤,���Ҳ�����BRM
                //if((fChg2bmsbyte[0]==0x13)&&(fChg2bmsbyte[5]==0x00)&&(fChg2bmsbyte[6]==0x02)&&((fChg2bmsbyte[7]==0x00)))
                     //BRMStep = 0;//����־λ����,���Դ��·���BRM
                     
            if((fChg2bmsbyte[0]==0x11)&&(fChg2bmsbyte[5]==0x00)&&(fChg2bmsbyte[6]==0x06)&&((fChg2bmsbyte[7]==0x00)))
                 FlagBCPSend = 1;
            //if((fChg2bmsbyte[0]==0x13)&&(fChg2bmsbyte[5]==0x00)&&(fChg2bmsbyte[6]==0x06)&&((fChg2bmsbyte[7]==0x00)))
                 //BCPStep = 0;//����־λ����,���Դ��·���BRM
            
            if((fChg2bmsbyte[0]==0x11)&&(fChg2bmsbyte[5]==0x00)&&(fChg2bmsbyte[6]==0x11)&&((fChg2bmsbyte[7]==0x00)))
                 FlagBCSSend = 1;
            
            //if((fChg2bmsbyte[0]==0x13)&&(fChg2bmsbyte[5]==0x00)&&(fChg2bmsbyte[6]==0x11)&&((fChg2bmsbyte[7]==0x00)))
                 //BCSStep = 0;//����־λ����,���Դ��·���BRM
            
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
                BROErrorAA=1;//������������            
            } 
            else
            {
                BROErrorAA=0;//����������
            }
            if(CHMStep<=0x03)   
                CHMStep=0x03;
            break;  
        case 0x100af456:
            CROOverTime = 0;//�յ�CRO������
            if( fChg2bmsbyte[0]==0xaa)
            {
                if(CHMStep<=0x04)
                    CHMStep=0x04;
                SelfState3=1;
                CROOverTime60s = 0;//�յ�0xaa������           
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
            
            m_askcurrent=0;//�������Ϊ0 
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
//* Description:     CAN2�����ж��ӳ���,������BMU����λ�������ڲ�����ͨ��
//*                  �ڲ�/ 250Bd /��չ֡ 
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
#pragma CODE_SEG NON_BANKED
interrupt void CAN2_RECEIVE_ISR(void)   //�ڲ�  BMU   250Hz
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
   
    for(i=0;i<8;i++) {
        g_mboxData[0][i]= *((&CAN2RXDSR0)+i);
    }
    
    switch(g_mboxID)
    {
        case 0x18FF5A51://�����Ϻ������������Ϣ ��ͨ������CAN����ȥ
            for(i=0;i<8;i++){
                g_FireMsg[0].data[i] = g_mboxData[0][i];
            }
            break;
            
        case 0x18FF5A52://�����Ϻ������������Ϣ ��ͨ������CAN����ȥ
            for(i=0;i<8;i++){
                g_FireMsg[1].data[i] = g_mboxData[0][i];
            }
            break;
            
        case 0x18FF5A53://�����Ϻ������������Ϣ ��ͨ������CAN����ȥ
            for(i=0;i<8;i++){
                g_FireMsg[2].data[i] = g_mboxData[0][i];
            }
            break;
            
        case 0x18FF5A54://�����Ϻ������������Ϣ ��ͨ������CAN����ȥ
            for(i=0;i<8;i++){
                g_FireMsg[3].data[i] = g_mboxData[0][i];
            }
            break;
            
        case 0x18FF5A55://�����Ϻ������������Ϣ ��ͨ������CAN����ȥ
            for(i=0;i<8;i++){
                g_FireMsg[4].data[i] = g_mboxData[0][i];
            }
            break;
            
        case 0x18FF5A56://�����Ϻ������������Ϣ ��ͨ������CAN����ȥ
            for(i=0;i<8;i++){
                g_FireMsg[5].data[i] = g_mboxData[0][i];
            }
            break;
            
            
        case 0x18FF5A57://�����Ϻ������������Ϣ ��ͨ������CAN����ȥ
            for(i=0;i<8;i++){
                g_FireMsg[6].data[i] = g_mboxData[0][i];
            }
            break;
            
        case 0x18FF5A58://�����Ϻ������������Ϣ ��ͨ������CAN����ȥ
            for(i=0;i<8;i++){
                g_FireMsg[7].data[i] = g_mboxData[0][i];
            }
            break;
            
        case 0x18FF5A59://�����Ϻ������������Ϣ ��ͨ������CAN����ȥ
            for(i=0;i<8;i++){
                g_FireMsg[8].data[i] = g_mboxData[0][i];
            }
            break;
            
        case 0x18FF5A5a://�����Ϻ������������Ϣ ��ͨ������CAN����ȥ
            for(i=0;i<8;i++){
                g_FireMsg[9].data[i] = g_mboxData[0][i];
            }
            break;
            
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
            Task13_Vpn_Value_Calibrate();       //У׼�ܵ�ѹ       
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
        
       /* case 0x18ff6200:  //�յ���ȡ������Ϣ������
            g_errorRecordRead_flag=1;  
            break;
        case 0x18ff6201: //������й�����Ϣ
            g_errorCounter=0;
            temper = g_errorCounter;
            Write24c64_Byte(ERROR_COUNT_ADDR,(unsigned char *)&temper,2);//���¹��ϼ����� 
            temper = 0;
            Read24c64_Byte(ERROR_COUNT_ADDR,(unsigned char *)&temper,2);
            g_errorCounter = temper;
            break;
         */
        case 0x0a0218fb://����BOOT����
            DFlash_Erase_Sector(0x0000); //�����Ȳ���
            DataBuffer[0] = 0xaaaa;
            DFlash_Write_Word(0x0000);//д��boot��־0xaaaa,��boot������������־����ִ��bootloader���س��򣬷�֮����ִ��Ӧ�ó���                 
            COPCTL = 0x07;        //enable watchdog
            ARMCOP = 0x00;        //feed 0 to watch dog for reset 
            BootState = 1;
            break;
        //case 0x0c14ee00:      //��ȡʱ��
        //case 0xc01ee14:       //����ʱ��
        //case 0xC01ee15:       //SOC����
        //case 0xC01ef15:       //������ʾSOC
        //case 0xC01ee16:       //ʣ������
        //case 0xC01ee17:        //BMU����
        //case 0xC01ee18:        //����汾��
        case 0xC01EE28:         //����ϵͳ����������Ϣfrom ��λ��
        case 0xC01EE29:         //����ϵͳ����������Ϣfrom ��λ��
        case 0xC01EE2a:         //����ϵͳ����������Ϣfrom ��λ��
        case 0xC01EE2b:         //����ϵͳ����������Ϣfrom ��λ��
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