//---------------------- Pride Power------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//* Project Name       : S133
//* File Name          : MachineControl.c
//* Author             : judy
//* Version            : V1.0.0
//* Start Date         : 2016.2.14
//* Description        : ���ļ�����״̬ת���ж�
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
#include "BMS20.h" 
unsigned char HighVolPowerOff = 0;//��ѹֱ���µ�2.5s֮��
unsigned char OffState=0;//���͸�ѹ�µ�����

unsigned char ChangerINError = 2; //����ChanerIN�ź�ͻȻû��:0:��ChanerIN�ź�;1:��ChangerIN�ź�;2����ȷ��
unsigned char State177End=0;//��ͣ��177״̬,
unsigned char PowerOffError=0;//�г�ģʽ�µ����

//******************************************************************************
//* Function name:   stateCodeTransfer
//* Description:     ��ϵͳ�����ڼ�ʱ�̼�⵱ǰ�źţ���û�ź��ˣ���Ҫ�µ�
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void stateCodeTransfer(void) 
{
    static unsigned int Delay30=0;
    static unsigned int DisChangerDelay=0;
    static unsigned int Delay110=0;
    static unsigned int Delay170=0;
    static unsigned int Delay126=0;
    static unsigned int Delay186=0;
    
    if((OffState == 1)&&(g_BmsModeFlag == DISCHARGING)&&(VehicleSpeed<=8))
        PowerOffError = 1;//�г�ģʽ��Ҫ�µ�Ĺ��ϣ����ٽ���8km/h    
        
    
    if(State177End==1)//��177״̬�½�����State177End��1
    {
        stateCode=179;//����ģʽ
        return;
    }
    else if(TestState==1)
    {
        stateCode=177;//�رռ̵���״̬
        return;
    }
    /////////////////�г�//////////////////////
    if(g_BmsModeFlag == DISCHARGING)
    {
        if(stateCode==11)
        {   
            stateCode=12;
        }
        else if(stateCode==12)
        {  
            
            if((MSDError==1)||(N_RelayConnetError==1)||(acc_Connect == 0)||(PowerOffError == 1))
                stateCode=46;
            else if((VCU_Control.Bit.PowerOnOffReq == 1)&&(acc_Connect == 1)&&(bmsSelfcheckCounter==1))
                stateCode=14;
        }
        else if(stateCode==14)
        {
            
            if((acc_Connect == 0)||(PowerOffError == 1))
                stateCode=44;
            else if((acc_Connect == 1)&&(status_group3.Bit.St_N_Relay == 1))
                stateCode=17;
        }
        else if(stateCode==17)
        {
           if((N_RelayDisConError==1)||(P_RelayConError==1)||(acc_Connect == 0)||(PowerOffError == 1))
                stateCode=44;
           else if((acc_Connect == 1)&&(bmsSelfcheckCounter==2))
                stateCode=20;
        }
        else if (stateCode == 20)
        {
            if((acc_Connect == 0)||(P_RelayDisConError==1)||(PowerOffError == 1))
                stateCode =40;
            else if((bmsSelfcheckCounter==3)&&(acc_Connect == 1)
            &&(status_group3.Bit.St_P_Relay == 1)&&(VCU_PRelState.Bit.PState == 1))
                stateCode=30;
        }
        else if(stateCode==30)
        {
            if((PowerOffError == 1)||(VCU_PRelState.Bit.PState == 0)||((VCU_Control.Bit.PowerOnOffReq == 2)&&(VCU_PRelState.Bit.PState == 0))
               ||((VCU_ChgControl.Bit.downC_Switch == 1)&&(VCU_PRelState.Bit.PState == 0))||((plug_DC_Connect == 1)&&(VCU_PRelState.Bit.PState == 0)))
                stateCode=40;    
        } 
        else if(stateCode==40)//�Ͽ������̵���
        {
            if((bmsSelfcheckCounter == 2)&&(status_group3.Bit.St_P_Relay == 0))
                stateCode = 44;
        } 
        else if(stateCode==44)
        { 
            if(status_group3.Bit.St_N_Relay == 0)
                stateCode=46;
        } 
        else if((stateCode==46))
        {
            if((plug_DC_Connect==1)&&(bmsSelfcheckCounter==1)) //ֱ�����ǹ,��ճ������
            {
                //InitialSoc();//SOC���㷽ʽ�仯
                
                First_g_socValue=g_socValue;
                StoreAHSOC=g_socValue;
                Can_g_socValue_Start=Can_g_socValue;
                
                g_BmsModeFlag = FASTRECHARGING;
                status_group4.Bit.Mode_BMS_Work = 2;//���״̬ 
                stateCode = 141;
                bmsSelfcheckCounter = 0;
            }
            else if((VCU_ChgControl.Bit.downC_OK == 1)&&(VCU_ParkBrake.Bit.Parking_Brake == 1)//פ���źţ�������λ
                     &&(VCU_ChgControl.Bit.downC_Switch == 1)&&(bmsSelfcheckCounter==1))//�������ء���
            {
                InitialSoc();
                g_BmsModeFlag = RECHARGING;
                status_group4.Bit.Mode_BMS_Work = 2;//���״̬ 
                stateCode = 81;
                bmsSelfcheckCounter = 0;            
            }
            else if(acc_Connect==0)
            {
                
                if(bmsSelfcheckCounter==1)//�޹���
                    stateCode=47;
                else 
                {
                    DisChangerDelay++;
                    if(DisChangerDelay>=300) //��ʱ�ж�,��ֹbmsSelfcheckCounter�仯�����µ�����
                    {
                        DisChangerDelay=0;//�Լ������û����1,��ճ�����,���ŵ�
                        stateCode=47;
                    }
                }
                
            } 
            else
            {
              
                if(VCU_Control.Bit.PowerOnOffReq == 1)//�ϵ�
                    stateCode=11;
            }
        } 
        else if(stateCode==47)
        {
            bmsSelfcheckCounter=0;        
        }
    }
    ///////////////�ܵ繭��� //////////////////
    else if(g_BmsModeFlag == RECHARGING)
    {
        if(stateCode==81)
        {   
            if((status_group3.Bit.St_N_Relay == 0)&&(status_group3.Bit.St_P_Relay == 0))
                stateCode=82;
        }
        else if(stateCode==82)
        {  
            
            if((VCU_ChgControl.Bit.downC_OK == 0)||(VCU_ParkBrake.Bit.Parking_Brake == 0)//פ���źţ�������λ
              ||(VCU_ChgControl.Bit.downC_Switch == 0)||(OffState == 1)||(MSDError==1)||(N_RelayConnetError==1)) 
                stateCode=126;
            else if((VCU_ChgControl.Bit.downC_OK == 1)&&(VCU_ParkBrake.Bit.Parking_Brake == 1)//פ���źţ�������λ
              &&(VCU_ChgControl.Bit.downC_Switch == 1)&&(bmsSelfcheckCounter==1)) //��������
                stateCode=84;
           
        }
        else if(stateCode==84)
        {
            if((VCU_ChgControl.Bit.downC_OK == 0)||(VCU_ParkBrake.Bit.Parking_Brake == 0)//פ���źţ�������λ
              ||(VCU_ChgControl.Bit.downC_Switch == 0)||(OffState == 1)) //��������
                stateCode=124;
            else if((VCU_ChgControl.Bit.downC_OK == 1)&&(VCU_ParkBrake.Bit.Parking_Brake == 1)//פ���źţ�������λ
              &&(VCU_ChgControl.Bit.downC_Switch == 1)) //��������
                stateCode=87;
        }
        else if(stateCode==87)
        {

           if((VCU_ChgControl.Bit.downC_OK == 0)||(VCU_ParkBrake.Bit.Parking_Brake == 0)||(VCU_ChgControl.Bit.downC_Switch == 0)
             ||(OffState == 1)||(N_RelayDisConError == 1)||(CCHG_RelayConError = 1)) 
                stateCode=124;
           else if((VCU_ChgControl.Bit.downC_OK == 1)&&(VCU_ParkBrake.Bit.Parking_Brake == 1)//פ���źţ�������λ
             &&(VCU_ChgControl.Bit.downC_Switch == 1)&&(bmsSelfcheckCounter==2)) //��������
                stateCode=90;
        }
        else if (stateCode == 90)
        {
            if((VCU_ChgControl.Bit.downC_OK == 0)||(VCU_ParkBrake.Bit.Parking_Brake == 0)
              ||(VCU_ChgControl.Bit.downC_Switch == 0)||(OffState == 1)||(CCHG_RelayDisConError == 1)) 
                stateCode=120;
            else if((VCU_ChgControl.Bit.downC_OK == 1)&&(VCU_ParkBrake.Bit.Parking_Brake == 1)&&(status_group2.Bit.St_Ele_Relay == 2)
              &&(VCU_ChgControl.Bit.downC_Switch == 1)&&(VCU_ChgControl.Bit.Charge_Switch == 1)&&(bmsSelfcheckCounter==3)) 
                stateCode=110;
               
        }
        else if(stateCode==110)
        {
            if((slowRechargeFinished == 1)||(OffState == 1)||(plug_DC_Connect == 1)||(VCU_ChgControl.Bit.downC_OK == 0)
              ||(VCU_ParkBrake.Bit.Parking_Brake == 0)||(VCU_ChgControl.Bit.downC_Switch == 0))
                stateCode=120;    
        } 
        else if(stateCode==120)//�Ͽ��ܵ繭�̵���
        {
            if((bmsSelfcheckCounter == 2)&&(status_group2.Bit.St_Ele_Relay == 1))//�ܵ繭�̵����Ͽ�
                stateCode = 124;
        } 
        else if(stateCode==124)
        { 
            if(status_group2.Bit.St_Ele_Relay == 1)//ȷ���ܵ繭�̵����ѶϿ�
                stateCode=126;
        } 
        else if((stateCode==126))
        {
            if((plug_DC_Connect==1)&&(bmsSelfcheckCounter==1)) //ֱ�����ǹ,��ճ������
            {
                //InitialSoc();//SOC���㷽ʽ�仯
                
                First_g_socValue=g_socValue;
                StoreAHSOC=g_socValue;
                Can_g_socValue_Start=Can_g_socValue;
                
                g_BmsModeFlag = FASTRECHARGING;
                stateCode = 141;
                bmsSelfcheckCounter = 0;
            }
            else if((VCU_ChgControl.Bit.downC_OK == 1)&&(VCU_ParkBrake.Bit.Parking_Brake == 1)//פ���źţ�������λ
              &&(VCU_ChgControl.Bit.downC_Switch == 1)&&(bmsSelfcheckCounter==1)&&(OffState == 0)) //��������
                stateCode=81;
            else if(acc_Connect==0)
            {
                if(bmsSelfcheckCounter==1)//�޹���
                    stateCode=127;
                else 
                {
                    DisChangerDelay++;
                    if(DisChangerDelay>=300) //��ʱ�ж�,��ֹbmsSelfcheckCounter�仯�����µ�����
                    {
                        DisChangerDelay=0;//�Լ������û����1,��ճ�����,���ŵ�
                        stateCode=127;
                    }
                }
            }
            else if((acc_Connect == 1)&&(plug_DC_Connect == 0)&&(VCU_ChgControl.Bit.downC_OK == 0)&&(VCU_ParkBrake.Bit.Parking_Brake == 0)&&(VCU_ChgControl.Bit.downC_Switch == 0)) 
            {
                stateCode = 11;
                status_group4.Bit.Mode_BMS_Work = 1;//�ŵ�״̬ 
                bmsSelfcheckCounter = 0;
            } 
        } 
        else if(stateCode==127)
        {
            bmsSelfcheckCounter=0;        
        } 
    }
    ///////////////////////////���/////////////////////////////////
    else if(g_BmsModeFlag==FASTRECHARGING)
    {
        if(stateCode==141)
        {   
            if((status_group3.Bit.St_N_Relay == 0)&&(status_group3.Bit.St_P_Relay == 0))
                stateCode=142;
        }
        else if(stateCode==142)
        {  
            if((MSDError==1)||(N_RelayConnetError==1)||(plug_DC_Connect == 0)||(OffState == 1))
                stateCode=186;
            else if((plug_DC_Connect == 1)&&(bmsSelfcheckCounter==1))
                stateCode=144;
        }
        else if(stateCode==144)
        {
            if((plug_DC_Connect == 0)||(OffState == 1))
                stateCode=184;
            else if((plug_DC_Connect == 1)&&(status_group3.Bit.St_N_Relay == 1))
                stateCode=147;
        }
        else if(stateCode==147)
        {
           if((N_RelayDisConError==1)||(DCCHG_RelayConError==1)||(plug_DC_Connect == 0)||(OffState == 1))
                stateCode=184;
           else if((N_RelayDisConError==0)&&(DCCHG_RelayConError==0)&&(plug_DC_Connect == 1)&&(OffState == 0)&&(bmsSelfcheckCounter==2))
                stateCode=150;
        }
        else if (stateCode == 150)
        {
            if((plug_DC_Connect == 0)||(DCCHG_RelayDisConError==1)||(OffState == 1))
                stateCode =180;
            else if((bmsSelfcheckCounter==3)&&(plug_DC_Connect == 1)&&(status_group2.Bit.St_CHG_Connect == 2))
                stateCode=170;
        }
        else if(stateCode==170)
        {
            if((OffState == 1)||(plug_DC_Connect == 0))
                stateCode=180;    
        } 
        else if(stateCode==180)//�Ͽ������̵���
        {
            if((bmsSelfcheckCounter == 2)&&(status_group2.Bit.St_CHG_Connect == 1))
                stateCode = 184;
        } 
        else if(stateCode==184)
        { 
            if(status_group3.Bit.St_N_Relay == 0)
                stateCode=186;
        } 
        else if((stateCode==186))
        {
            if(bmsSelfcheckCounter==1)//�޹���
                stateCode=187;
        } 
        else if(stateCode==187)
        {
            bmsSelfcheckCounter=0;        
        }
    } 
}
//*************************************************************************
//**************************the end *********************************************
//************************************************************************************
//******************************************************************************************