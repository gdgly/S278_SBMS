//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//---------------------- Pride Power------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//* Project Name       : PHEV.DJ
//* File Name          : Machine.c
//* Author             : judy
//* Version            : V1.0.0
//* Start Date         : 2011.6.7
//* Description        : ���ļ�������Ŀ��״̬�����Դ���
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
#include "BMS20.h" 

//******************************************************************************
//******************************************************************************
//******************************************************************************

unsigned int tmr_p1 =0;
unsigned int tmr_p2 =0;
unsigned int tmr_p3 =0;
unsigned int tmr_p4 =0;
unsigned int tmr_p5 =0;
unsigned char prechargeFinished =0;      //Ԥ�����ɱ�־λ
unsigned char preChargeON =0;
unsigned char bmsSelfcheckCounter=0;
unsigned char acc_Connect=0;
unsigned char MSDError=0;                 //MSD����
unsigned char N_RelayConnetError=0;       //�����̵���ճ��
unsigned char N_RelayDisConError=0;       //�����̵�����·
unsigned char PreCha_RelayConError=0;     //Ԥ��̵���ճ��
unsigned char P_RelayConError=0;          //�����̵���ճ��
unsigned char PreCha_RelayDisConError=0;  //Ԥ��̵�����·
unsigned char P_RelayDisConError=0;  //�����̵�����·
unsigned char CCHG_RelayConError=0;  //�ܵ繭���̵���ճ��
unsigned char DCCHG_RelayConError=0;  //���̵���ճ��
unsigned char CCHG_RelayDisConError=0;  //�ܵ繭���̵�����·
unsigned char DCCHG_RelayDisConError=0;  //���̵�����·
unsigned char Cha_ResDisConError=0;       //Ԥ������·
unsigned char PreCha_Error=0;             //Ԥ���ʧ��
unsigned char SelfCheck = 0;              //BMS�Լ���: 2,δͨ��;1ͨ��(Ԥ��̵����Ƿ�ɹ��Ͽ�)
unsigned char RelayErrorPowerOff = 0;     //�̵����µ����         

//******************************************************************************
//* Function name:   VehiclePowerOffLogic
//* Description:     BMS�������ϣ��µ�
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void VehiclePowerOffLogic(void) 
{
    static unsigned char OffFlag=0;
    static unsigned char count=0;
    
    TurnOff_INZK();//�Ͽ������Ӵ��� 
    turnOffSW_Power();//�رյ�Դ���س��׶ϵ�
}
//******************************************************************************
//* Function name:   closeNegRelay
//* Description:     �պ������̵���
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void closeNegRelay(void) 
{
    TurnOn_INFK();                 //to pc
    delay(25000);                  //19ms
    delay(25000);                  //19ms
    //state_group1.Bit.St_N_Relay=1; //to vcu
    status_group3.Bit.St_N_Relay=1;//
}
//******************************************************************************
//* Function name:   openNegRelay
//* Description:     �Ͽ������̵���
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void openNegRelay(void) 
{
    TurnOff_INFK();//�Ͽ������̵���
    //state_group1.Bit.St_N_Relay=0;//to vcu
    delay(25000); //19ms
    delay(25000); //19ms
    status_group3.Bit.St_N_Relay=0;  

}
//******************************************************************************
//* Function name:   openPosRelay
//* Description:     �Ͽ������̵���
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void openPosRelay(void) 
{
    TurnOff_INZK();//�Ͽ������̵���
    delay(25000); //20ms
    //TurnOff_INBK();//�Ͽ�Ԥ���̵���
    //delay(25000); //20ms
    status_group3.Bit.St_P_Relay=0;//to vcu
}

//******************************************************************************
//* Function name:   GetTavg
//* Description:     ��ŵ繦�ʲο����¶�ȡֵ
//* EntryParameter : ����¶ȣ�����¶�
//* ReturnValue    : Tavg
//******************************************************************************
unsigned char GetTavg(unsigned char LowTem,unsigned char HighTem)  
{
    unsigned char Tavg1;

    if((LowTem>10+40)&&(HighTem>=55+40))
        Tavg1=HighTem;
    
    else if((LowTem>10+40)&&(HighTem<55+40)) //��������Ҫ�ſ�
        Tavg1=g_averageTemperature;
    
    else if(LowTem<=10+40) 
        Tavg1=LowTem;
    
    return Tavg1;
}
//******************************************************************************
//* Function name:   GetTavgProcess
//* Description:     ��ŵ繦�ʲο����¶�ȡֵ
//* EntryParameter : ����¶ȣ�����¶�
//* ReturnValue    : Tavg
//******************************************************************************
void GetTavgProcess(void)  
{
    static unsigned char timer1,timer2; 
    if(g_highestTemperature>45+40)//��������¶ȴ���45�ȳ���2S����ôƽ���¶�Ϊ����¶�
    {
        timer2=0;
        timer1++;
        if(timer1>7)//300ms*7=2100ms
        {
            Tavg=g_highestTemperature;
            timer1=0;
        }
    } 
    else if(g_highestTemperature<=45+40)//��������¶�С��45�ȳ���2S����ôƽ���¶�Ϊ����¶�
    {     
        timer1=0;
        timer2++;
        if(timer2>7) 
        {
            Tavg=g_lowestTemperature;
            timer2=0;
        } 
    }
}
//******************************************************************************
//* Function name:   HighVoltDetectPart1
//* Description:     ����state�����ϵ粽��1��״̬������12/17or46/126
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void HighVoltDetectPart1(void) 
{
    static unsigned char tt=0;
    static unsigned char pp=0;
    static unsigned char ww=0;

    if(tmr_p1<=350)//350ms�� ��ʱ����1ms�ж��������Ŀ������ʱ��׼ȷ�������������������������Ϊ��������Ӱ����ʱ��׼ȷ��
    {
        /*
        //��ѹ������⣻��Ϊ�ߵ�ƽ��˵����ѹ�����쳣����ѹ����״̬=1����ʾ�պ�,��������
        if(inputH_state()) 
        {
            hardware_error5.Bit.F1_Lock_Err = 1;
            RelayErrorPowerOff = 1;
        }
        else
        {          
            hardware_error5.Bit.F1_Lock_Err = 0;      
        }
        */
        //**************���MSD******************************//////////////////////// 
        if(g_highVoltageV1<100) 
        {                       
            tt++;   
            if(tt>=12)//60ms�����жϳ���
            {

                //to vcu
                RelayErrorPowerOff = 1;//�̵����µ����
                MSDError = 1;
                g_caution_Flag_4 |=0x01; //to PC
                bmsSelfcheckCounter=1;
                tt=13;
            }
        } 
        else   
        {
            tt=0;  
        }
        
        //********��������̵���ճ�� *****************////////////////////////////////// 
        /*if(g_highVoltageV2>=200) //(V2>=0.6*g_highVoltageV1))
        {
            pp++; 
            if(pp>=10)
            {
                /////////////////Ԥ���������ճ��///////////////////
                RelayErrorPowerOff = 1;//�̵����µ����
                state_group1.Bit.St_P_Relay=1;//to vcu
                P_RelayConError = 1;
                BmsCtlStat0 |=0x02;//�����̵���״̬�պ�    to pc
                g_caution_Flag_4 |=0x10; //to PCԤ�������ճ��
                hardware_error3.Bit.F2_PRa_Cir_Err = 1;//to VCU
                pp=11;
                /////////////////Ԥ���������ճ��///////////////////
            }

        } 
        else
        {
             pp=0;
        }
        */
        //********��������̵���ճ�� *****************////////////////////////////////// 
        if(g_highVoltageV2>=300) //(V2>=0.6*g_highVoltageV1))
        {
            pp++; 
            if(pp>=12)//60ms
            {
                /////////////////����ճ��///////////////////
                RelayErrorPowerOff = 1;//�̵����µ����
                BmsCtlStat0|=0x01;//�����̵���״̬�պ�
                g_caution_Flag_4 |=0x02; //to PC
                Error_Group3.Bit.F4_N_Con_Err = 1;//error to VCU
                N_RelayConnetError = 1;
                pp=13;
                /////////////////����ճ��///////////////////
            }

        } 
        else
        {
             pp=0;
        }
        
    }
    else  //��Ҫ350ms֮��
    {
        if((MSDError==0)&&(N_RelayConnetError==0))
        {
            bmsSelfcheckCounter=1; //����״̬ 
        } 
    }

}
//******************************************************************************
//* Function name:   HighVoltDetectPart2
//* Description:     ��ѹ��⣺״̬������17/87�����̵�����· ��Ԥ������·����
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void HighVoltDetectPart2(void)//Ԥ��̵����Ѿ��պ� 
{    
    
    static unsigned char NDisConnect_tt=0;
    static unsigned char PConnect_tt=0;
    static unsigned char CCHGConnect_tt=0;
    static unsigned char DCCHGConnect_tt=0;
    
    if((tmr_p2<=60)||(tmr_p2>=600)) //��ʱ20ms���ߴ���600ms,��������
        return;                                              
    /*if(g_highVoltageV2<=200)  /////������·///////????????????
    {
        if(g_highVoltageV3>=200)//???????????????
        {
            PConnect_tt++;
            if (PConnect_tt>=12)//�˲���ʱ60ms����ѹ�Ƿ��ܼ�ʱ�仯��
            {
                RelayErrorPowerOff = 1;//�̵����µ����
                g_caution_Flag_4 |=0x04; //to PC
                BmsCtlStat0 &=0xfe;//�����Ӵ���״̬�Ͽ�
                N_RelayDisConError = 1;  //�����̵�����·
                PConnect_tt = 0;
            }            
        }
        else//�޹���
        {
            bmsSelfcheckCounter=2; //����״̬ 
            delay(25000); //20ms
            delay(25000); //20ms
        }
    }
    else
    {
        Pre_tt++;
        if(Pre_tt>=6)
        {          
            RelayErrorPowerOff = 1;//�̵����µ����
            g_caution_Flag_4 |=0x20; //to PC
            PreCha_RelayDisConError = 1;
            state_group1.Bit.St_Pre_Relay=0;  //Ԥ��̵���״̬  for VCU                    
            TurnOff_INBK();//�Ͽ�Ԥ���̵���
            delay(25000); //20ms��ΪʲôҪ��ʱ��
            prechargeFinished =0;
            g_caution_Flag_4 |=0x80; //to PC
            Pre_tt = 0;
        }
    }
    */
    /////������·///////
    if(g_highVoltageV2<200)  
    {
        NDisConnect_tt++;
        if (NDisConnect_tt>=12)//�˲���ʱ60ms����ѹ�Ƿ��ܼ�ʱ�仯��
        {
            RelayErrorPowerOff = 1;//�̵����µ����
            g_caution_Flag_4 |=0x04; //to PC
            BmsCtlStat0 &=0xfe;//�����Ӵ���״̬�Ͽ�
            N_RelayDisConError = 1;  //�����̵�����·
            NDisConnect_tt = 13;
        }
    }
    else   
    {
        NDisConnect_tt=0;  
    }
    /////����ճ��///////
    
    if((g_highVoltageV3>200)&&(stateCode == 17))  
    {
        PConnect_tt++;
        if (PConnect_tt>=12)//�˲���ʱ60ms����ѹ�Ƿ��ܼ�ʱ�仯��
        {
            RelayErrorPowerOff = 1;//�̵����µ����
            g_caution_Flag_4 |=0x10; //to PC
            BmsCtlStat0 |=0x02;//�����̵���״̬�պ�
            Error_Group3.Bit.F2_P_Con_Err = 1;//error to VCU
            P_RelayConError = 1;//����ճ������
            PConnect_tt = 13;
        }
    }
    else   
    {
        PConnect_tt=0;  
    }
     /////�ܵ�����̵���ճ��///////
    if((g_highVoltageV2B>300)&&(stateCode == 87))  
    {
        CCHGConnect_tt++;
        if (CCHGConnect_tt>=12)//�˲���ʱ60ms����ѹ�Ƿ��ܼ�ʱ�仯��
        {
            RelayErrorPowerOff = 1;//�̵����µ����
            g_caution_Flag_2 |=0x20; //to PC
            Error_Group1.Bit.F3_Ele_Relay_Con = 1;//error to VCU
            CCHG_RelayConError = 1;//�ܵ繭�̵���ճ��
            CCHGConnect_tt = 13;
        }
    }
    else   
    {
        CCHGConnect_tt=0;  
    }
      /////���̵���ճ��///////
    if((g_highVoltageV3B>300)&&(stateCode == 147))  
    {
        DCCHGConnect_tt++;
        if (DCCHGConnect_tt>=12)//�˲���ʱ60ms����ѹ�Ƿ��ܼ�ʱ�仯��
        {
            RelayErrorPowerOff = 1;//�̵����µ����
            g_caution_Flag_3 |=0x80; //to PC
            Error_Group3.Bit.F5_DC_Con_Err = 1;//error to VCU
            DCCHG_RelayConError = 1;//���̵���ճ��
            DCCHGConnect_tt = 13;
        }
    }
    else   
    {
        DCCHGConnect_tt=0;  
    }
    if(tmr_p2>400)  //??????????????????
    {
        if((N_RelayDisConError==0)&&(((P_RelayConError==0)&&(stateCode == 17))
        ||((CCHG_RelayConError==0)&&(stateCode == 87))
        ||((DCCHG_RelayConError==0)&&(stateCode == 147))))
        {
            bmsSelfcheckCounter=2; //����״̬ 
        } 
    }
}
//******************************************************************************
//* Function name:   HighVoltDetectPart3
//* Description:     ����state�����ϵ粽��3��״̬������20/90
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void HighVoltDetectPart3(void) 
{      
    static unsigned char PDisConnect_tt=0;
    static unsigned char CCHGDisConnect_tt=0;
    static unsigned char DCHGDisConnect_tt=0;
    
    if(tmr_p3<=60)  //��ʱ60ms
       return;
    
    if(tmr_p3<=400) 
    {
        if((g_highVoltageV3 < 200)&&(stateCode == 20)) 
        {
            PDisConnect_tt ++;
            if (PDisConnect_tt>=12)//�˲���ʱ60ms����ѹ�Ƿ��ܼ�ʱ�仯��
            {
                RelayErrorPowerOff = 1;//�̵����µ����
                g_caution_Flag_4 |=0x40; //to PC
                BmsCtlStat0 &=0xfd;//�����̵���״̬�Ͽ�
                P_RelayDisConError = 1;//������·����
                status_group1.Bit.St_BMS = 3;//��ѹ�ϵ���Ч
                PDisConnect_tt = 13;
            }
            
        }
        else   
        {
            PDisConnect_tt=0;  
        }
        if((g_highVoltageV2B !=0)&&(g_highVoltageV2B < 200)&&(stateCode == 90)) 
        {
            CCHGDisConnect_tt ++;
            if (CCHGDisConnect_tt>=12)//�˲���ʱ60ms����ѹ�Ƿ��ܼ�ʱ�仯��
            {
                RelayErrorPowerOff = 1;//�̵����µ����
                g_caution_Flag_2 |=0x40; //to PC
                status_group1.Bit.St_BMS = 3;//��ѹ�ϵ���Ч
                CCHG_RelayDisConError = 1;//�ܵ繭�̵�����·����
                CCHGDisConnect_tt = 13;
            }
            
        }
        else   
        {
            CCHGDisConnect_tt=0;  
        }
        if((g_highVoltageV3B !=0)&&(g_highVoltageV3B < 200)&&(stateCode == 150)) 
        {
            DCHGDisConnect_tt ++;
            if (DCHGDisConnect_tt>=12)//�˲���ʱ60ms����ѹ�Ƿ��ܼ�ʱ�仯��
            {
                RelayErrorPowerOff = 1;//�̵����µ����
                g_caution_Flag_3 |=0x40; //to PC
                BmsCtlStat0 &=0xfd;//�����̵���״̬�Ͽ�
                DCCHG_RelayDisConError = 1;//���̵�����·����
                status_group1.Bit.St_BMS = 3;//��ѹ�ϵ���Ч
                DCHGDisConnect_tt = 13;
            }
            
        }
        else   
        {
            DCHGDisConnect_tt=0;  
        }
    } 
    else 
    {
        if(((P_RelayDisConError==0)&&(stateCode == 20))||((CCHG_RelayDisConError==0)&&(stateCode == 90))
        ||((DCCHG_RelayDisConError==0)&&(stateCode == 150))) 
        {
            bmsSelfcheckCounter = 3; //����״̬
            status_group1.Bit.St_BMS = 0;//��ѹ���� 
        }  
    }
        /*if(prechargeFinished==0) 
        {
            
            if((g_highVoltageV3 <= 100)&&(g_highVoltageV2>=200))//��ֹ���ض�·��������ض�·��V3���С,���V2ҲС�����ⲿ��· 
            {            
                tt++;
                if(tt>=10)//�˲�50ms 
                {  
                    RelayErrorPowerOff = 1;//�̵����µ����
                    tt=11;
                    state_group1.Bit.St_N_Relay=0;  //to vcu
                    g_caution_Flag_4 |=0x04;        //to PC
                    BmsCtlStat0 &= 0xfe;            //�����̵���״̬   open    to pc                   
                    TurnOff_INBK();                 //�Ͽ�Ԥ���̵���
                    state_group1.Bit.St_Pre_Relay=0;  //Ԥ��̵���״̬  for VCU
                    delay(25000);                   //20ms��ΪʲôҪ��ʱ
                    N_RelayDisConError = 1;         //�����̵�����·
                }
                                         
            } 
            else 
            {   
                tt=0;
                if((g_highVoltageV3>g_highVoltageV1*0.95)&&(prechargeFinished==0))//û�й��� 
                {
                    TurnOn_INZK();//�պ������̵���
                    state_group1.Bit.St_P_Relay=1;//to vcu
                    delay(25000); //20ms
                    delay(25000); //20ms
                    TurnOff_INBK();//�Ͽ�Ԥ���̵���
                    state_group1.Bit.St_Pre_Relay=0;  //Ԥ��̵���״̬  for VCU                    
                    prechargeFinished =1;  //Ԥ����ɱ�־
                    state_group4.Bit.St_Precharge=2;  //Ԥ������    for VCU
                    bmsSelfcheckCounter=3; 
                   
                } 
                
            } 
        }*/
        
    /*} 
    else if(tmr_p3>1000) //��ʱ100ms //�ϵ�ɹ���100ms�ټ���ѹ�������ж������̵����Ƿ��ж�·��
    {
        if(prechargeFinished==0)//Ԥ���δ��� 
        {
            RelayErrorPowerOff = 1;//�̵����µ����
            prechargeFinished =0;  //Ԥ���δ���
            g_caution_Flag_4 |=0x80; //to PC
            TurnOff_INBK();//�Ͽ�Ԥ���̵���
            state_group1.Bit.St_Pre_Relay=0;  //Ԥ��̵���״̬  for VCU
            PreCha_Error = 1; //Ԥ���δ���
            tmr_p3 =1010; // ֻҪ����400����    
        }
    }
    */               
}
//******************************************************************************
//* Function name:   PreRelayConnectTest
//* Description:     Ԥ��̵���ճ�����ϣ���State=13,83,143ʱ��⣬�Ͽ���,���Ƿ�ճ��
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void PreRelayConnectTest(void)
{
    static unsigned char pred=0;
    if(tmr_p4<60)
        return;
    
    if(g_highVoltageV2>=200) 
    {
        pred++;
        if(pred>=12) 
        {
            RelayErrorPowerOff = 1;//�̵����µ����
            PreCha_RelayConError = 1;
    //        state_group1.Bit.St_Pre_Relay=1;  //Ԥ��̵���״̬  for VCU
            BmsCtlStat0 |=0x08;//Ԥ��̵���״̬�պ�    to pc
            g_caution_Flag_4 |=0x10; //to PCԤ�������ճ��
            pred=13; 
            SelfCheck = 2; 
        }   
    }
    else
    {
        SelfCheck = 1;
        pred=0;  
    }
    

}
//******************************************************************************
//* Function name:   PRelayConnectTest
//* Description:     �����̵���ճ�����ϣ���State=46,126,186ʱ���
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void PRelayConnectTest(void)
{
    static unsigned char pd=0;
    if(tmr_p5<60)
        return;
    
    if(g_highVoltageV2>=100) 
    {
        pd++;
        if(pd>=12) 
        {
            RelayErrorPowerOff = 1;//�̵����µ����
            P_RelayConError = 1;
  //          state_group1.Bit.St_P_Relay=1;//to vcu
            BmsCtlStat0 |=0x02;//�����̵���״̬�պ�    to pc
            g_caution_Flag_4 |=0x10; //to PCԤ�������ճ��
            //hardware_error3.Bit.F3_NRa_Cir_Err = 1;//to VCU
            //hardware_error3.Bit.F2_PRa_Cir_Err = 1;//to VCU
            pd=13; 
             
        }   
    }
    else
    {
        bmsSelfcheckCounter=1;//û�й��ϣ��Լ������
   //     state_group1.Bit.St_P_Relay=0;//to vcu�����̵���
    //    state_group1.Bit.St_N_Relay=0;//to vcu�����̵���
         
        pd=0;  
    }
    

}
//***********************************************************************
//************************************************************************
//*************************the end*************************************
//************************************************************************
