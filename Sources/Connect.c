//---------------------- Pride Power------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//* Project Name       : S133
//* File Name          : Connect.c
//* Author             : judy
//* Version            : V1.0.0
//* Start Date         : 2016.2.14
//* Description        : ���ļ������ж�ϵͳ��ǰ���ӵ��Ƿŵ磬���仹�ǿ��״̬
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
#include "BMS20.h"
//******************************************************************************
//* Function name:   bmsModeAwake
//* Description:     �ϵ绽���ź��жϣ��ó�ϵͳ����ģʽ
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void bmsModeAwake(void) 
{
    long timedelay = 6500000;
    if(input5_state()==0)//CC2״̬���,IN5==0,���ǹ�Ѳ�;IN5==1,���ǹû�в�
    {
        while(input6_state())
            _FEED_COP(); //��charge_in�źţ����            
        if((CC2VOL<=CC2VOLHIGH)&&(CC2VOL>=CC2VOLLOW))
        {
            g_BmsModeFlag = FASTRECHARGING; //���ģʽ   
            plug_DC_Connect=1;
            status_group4.Bit.Mode_BMS_Work = 2;//���״̬ 
            status_group3.Bit.St_CHG_Mode=1;//���ģʽ����DC���

            stateCode=141;      
        } 
        //else
        //{
        //    stateCode=187;
        //    plug_DC_Connect=0;
        //}
           
        
    } 
    else //���IN5==0��˵�����ǹ�Ѿ����ӣ����ģʽ
    {
        
        if(VCU_ChgControl.Bit.downC_Switch == 1)
        {
            g_BmsModeFlag = RECHARGING;//�����ܵ繭���ģʽ
            status_group4.Bit.Mode_BMS_Work = 2;//���״̬ 
            status_group3.Bit.St_CHG_Mode=2;//���ģʽ����AC���

            stateCode = 81;
            
        } 
        else if(input4_state()==0)
        {
            g_BmsModeFlag = DISCHARGING;//�ŵ�ģʽ
            acc_Connect=1;   //ON�ź�
            status_group4.Bit.Mode_BMS_Work = 1;//�ŵ�״̬ 
            stateCode=11; 
        }
    }

}
//******************************************************************************
//* Function name:   SignalOnJudge
//* Description:     ������,�ж��Ƿ��в�ǹ�ź�
//* EntryParameter : None
//* ReturnValue    : None
//* ע��           : ������ֱ��ʱ�޷���ChangerIN�ж��ź�,ֱ����CC2Ϊ�ź�,�����ѱ���CC��״̬Ϊ�ź�;
//                 : ����⵽��ǹ�źź�(ֱ������),��Ҫ�ȴ�ChangerIN�źź�,���ܽ��г���ϵ�                      
//******************************************************************************
void SignalOnOffJudge(void) 
{
    static unsigned char KEY_Connect=0;    //Կ���ź���
    static unsigned char AC_ConnectIN=0;   //����ChangerIN�����ź���
    static unsigned char AC_Connect=0;     //�������ǹ�����ź���
    static unsigned char DC_Connect=0;     //ֱ�����ǹ�����ź���
    static unsigned char KEY_DisConnect=0; //Կ���ź���
    static unsigned char AC_DisConnectIN=0;//����ChangerIN�����ź���
    static unsigned char AC_DisConnect=0;  //�������ǹ�����ź���
    static unsigned char DC_DisConnect=0;  //ֱ�����ǹ�����ź���
    long timedelay = 6500000;
    if( BootState == 1) //Boot
    {
        turnOffSW_Power();//close�ܵ�Դ����
        //state_group4.Bit.Mode_BMS_Run = 3;//Boot To VCU
    } 
    else //��boot
    {
      
      ////////////////////////////ֱ�����ǹ���////////////////////////
        if(input5_state()==0)//���ǹ����
        {
          ////////////////////ֱ�����ǹCC2��ǹ���/////////////////////
            //TurnOn_CC2();//12Vϵͳ  
            DC_Connect++; //ֱ����ǹ��ʱ���,����������⵽200*5msʱ��ſ���
            DC_DisConnect = 0;
            if(DC_Connect>=100)
            {               
                /*while((DC_CC2Count==0)&&(timedelay>0))
                {
                    timedelay--;   
                    _FEED_COP(); //��charge_in�źţ����;
                }*/   
                if((CC2VOL<=CC2VOLHIGH)&&(CC2VOL>=CC2VOLLOW))//״̬ת������״̬ά��
                {
                    plug_DC_Connect=1;
                    //state_group1.Bit.St_Charge_connection = 1;//To VCU
                    //state_group4.Bit.Mode_BMS_Run = 2;//���״̬ To VCU
                    status_group4.Bit.Mode_BMS_Work = 2;//���״̬ 
                    DC_Connect=0;      
                } 
                else //���ٷ�Χ�����µ�
                {
                    
                    if(g_BmsModeFlag == DISCHARGING)//������г�ģʽ�³�磬���Ҳ����������������CC2��ֱ���µ�
                    {
                        RelayErrorPowerOff = 1;
                        plug_DC_Connect=0;
                    } 
                    else//���г�״̬�����״̬��)��CC2�������費��
                    {                      
                        fastendflag=1;
                        fastend2|=0x40;//�������������
                    }
                }
;
                
            }
        
        }
        else //CC2���ź�
        {
         ////////////////////ֱ�����ǹCC2��ǹ���/////////////////////   
            DC_Connect=0;
            DC_DisConnect++;//ֱ����ǹ��ʱ���,����������⵽200*5msʱ��ſ���
            if(DC_DisConnect>=100)
            {
                plug_DC_Connect=0;
                if(g_BmsModeFlag == FASTRECHARGING)
                {                  
                    fastendflag=1;
                    fastend2|=0x40;//�������������
                }
                //if(plug_AC_CP_Connect == 0)
                 //   state_group1.Bit.St_Charge_connection = 0;//To VCU
                DC_DisConnect=0;
            }
            
            //////////////////���CC���/////////////////////////////
            if(AC_ChangerState.Bit.AC_CCState==1)
            {
                AC_Connect++; //������ǹ��ʱ���,����������⵽200*5msʱ��ſ���
                AC_DisConnect = 0;
                if(AC_Connect>=100)
                {              
                    plug_AC_CP_Connect = 1;
                    //state_group1.Bit.St_Charge_connection = 1;//To VCU
                    //state_group4.Bit.Mode_BMS_Run = 2;//���״̬ To VCU
                    AC_Connect = 0;
                }
                
           
            } 
            else
            {              
                AC_Connect = 0;
                AC_DisConnect++;//������ǹ��ʱ���,����������⵽200*5msʱ��ſ���
                if(AC_DisConnect>=100)
                {
                    plug_AC_CP_Connect = 0;
               //     state_group1.Bit.St_Charge_connection = 0;//To VCU
                    AC_DisConnect = 0;
                } 
                ////////////////////ACCԿ�׿���ON���/////////////////////  
                if(input4_state()==0)//���ACC���ź�,��On�ź���Ч
                {
                    KEY_Connect++;
                    KEY_DisConnect = 0;
                    if(KEY_Connect>=100)
                    {                  
                        acc_Connect = 1;    //ON��
                        //state_group1.Bit.St_Charge_connection = 0;//To VCU
                        //state_group4.Bit.Mode_BMS_Run = 1;//�ŵ�״̬ To VCU
                        KEY_Connect = 0;
                    } 
                }
                else
                {
                ////////////////////ACCԿ�׿���OFF���/////////////////////
                    KEY_Connect = 0;
                    KEY_DisConnect++;
                    if(KEY_DisConnect>=100)
                    {
                        acc_Connect = 0;    //OFF��
                       // state_group1.Bit.St_Charge_connection = 0;//To VCU
                       // state_group4.Bit.Mode_BMS_Run = 0;//���״̬ To VCU
                        KEY_DisConnect = 0;                
                    }
                
                }
            } 
      
        }    
        ////////////////////ChangerIN���/////////////////////     
        if(input6_state()==1)
        {
            AC_ConnectIN = 0;
            AC_DisConnectIN++;//������ǹ��ʱ���,����������⵽200*5msʱ��ſ���
            if(AC_DisConnectIN>=100)
            {
                ChangerINError = 1;
                AC_DisConnectIN = 0;
            }
                
        } 
        else 
        {
        
            AC_DisConnectIN = 0;
            AC_ConnectIN++;//������ǹ��ʱ���,����������⵽200*5msʱ��ſ���
            if(AC_ConnectIN>=100)
            {
                ChangerINError = 0;
                AC_ConnectIN = 0;
            }        
        
        }
        
  
    }
}
//*************************************************************************
//***************************************************************************
//*******************************************************************************
//**********************************************************************************
//************************************************************************************