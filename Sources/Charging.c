//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//---------------------- Pride Power------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//* Project Name       : 第二代移动充电车
//* File Name          : Charging.c
//* Author             : Judy
//* Version            : V1.0.0
//* Start Date         : 2014.5.7
//* Description        : 系统直流充电和交流充电过程
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
#include "BMS20.h" 
///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

static unsigned char sendi1=0;     //非全局变量
static unsigned char sendi2=0;      //非全局变量

unsigned char	m_chmmode=0x02; //全局变量
unsigned char fastendflag=0;  //全局变量


float	m_askvoltage=HIGHEST_ALLOWED_CHARGE_V;     //依据电池包充电曲线
float	m_askcurrent=20.0;      //全局变量，直流充电电流
float	big_askcurrent=150.0;      //全局变量，直流充电最大电流
float HeatCurt;
float	m_allowmaxvoltage=3.65;   //全局变量 ,目前没用到


/////////快充用,BMS停止充电机原因////////////////
unsigned char  fastend1=0;    //全局
unsigned char  fastend2=0;
unsigned char  fastend3=0;
unsigned char  fastend4=0;
/////////快充用,BMS停止充电机原因////////////////
unsigned char  ACEndState=0;//慢充停止标志位
/////////整车转发快充,BMS停止充电机原因////////////////
unsigned char  BMSToVCU655_1=0;    //全局
unsigned char  BMSToVCU655_2=0;    //全局
unsigned char  BMSToVCU655_3=0;    //全局
/////////整车转发快充,BMS停止充电机原因////////////////

/////////快充用，发送BSM，电池当前状态////////////////
unsigned char  fasterror11 =0;
unsigned char  fasterror12 =0x10;//允许充电，0 b 00 01 00 00
/////////快充用，发送BSM，电池当前状态////////////////
///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
float ChgeAmpReq=0; //交流充电电流值全局
unsigned char slowRechargeFinished=0;     //慢充结束标志
unsigned char ReqCtlToCHG =1;   //交流充电状态
 /////////////////////////////////////////////////////////////////
///////C11//////

unsigned char SelfState2 = 0;     //自检计数器变成2标志
unsigned char SelfState3 = 0;     //自检计数器变成3标志,收到后,不再发BRO
///////C11//////

unsigned int chargeTimes=0;   //充电次数
unsigned int chargeRemainderTime=65535;///s充电剩余时间

////超时时间标志//////
unsigned char CCSOverTime=0;//CCS超时时间
unsigned char CSTOverTime=0; //CST超时时间
unsigned char CSDOverTime=0; //CSD超时时间

////超时时间标志//////

////超时标志位////////
unsigned char OverTimeState=0;//超时标志

////超时标志位////////
/////////////////////////////////////////////////////////////////////////////
//******************************************************************************
//* Function name:   GetRequestCurrentDC
//* Description:     快充充电请求电流计算 C=120A
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
float GetRequestCurrentDC(void) 
{
    float DCAmp;
    
    if(st_heating==1) ///小于0度预加热,预加热
    {
        if(Tavg<=(0+45))
        {    
            m_askvoltage=594;//3.3v * 180=594
            DCAmp=0;
        }
    } 
    else//边充电边加热与只充电 
    {
        m_askvoltage = HIGHEST_ALLOWED_CHARGE_V;
        if(Tavg>=(55+40))  //如果温度大于等于55度[55,)
        {
            m_askvoltage=0;
            DCAmp=0;
        }
        else if(Tavg>=(50+40))//如果温度50~55度[50,55)
        {
            if(Can_g_socValue<0.8)
                DCAmp=0.2*C;////0.2c
            else
                DCAmp=0.1*C;////0.2c 
        }             
        else if(Tavg >=(45+40)) //如果温度45~50度[45,50)
        {
            if(Can_g_socValue<0.8)
                DCAmp=0.3*C;////0.2c
            else
                DCAmp=0.2*C;////0.2c
        }
        else if(Tavg >=(30+40)) //如果温度30~45度[30,45)
        {
            if(Can_g_socValue<0.8)
                DCAmp=0.6*C;////0.2c
            else
                DCAmp=0.3*C;////0.2c
        }
        else if(Tavg >=(20+40)) //如果温度20~30度[20,30)
        {
            if(Can_g_socValue<0.8)
                DCAmp=C;////0.2c
            else
                DCAmp=0.6*C;////0.2c
        }
        else if(Tavg >(15+40)) //如果温度15~20度(15,20)
        {
            if(Can_g_socValue<0.8)
                DCAmp=0.6*C;////0.2c
            else
                DCAmp=0.3*C;////0.2c
        }
        else if(Tavg >=(7+40))//如果温度7~15度[7,15]
        {
            if(Can_g_socValue<0.8)
                DCAmp=0.3*C;////0.2c
            else
                DCAmp=0.15*C;////0.2c
        }
        else if(Tavg >=(0+40))//[0~7)
            DCAmp=0.1*C;
        else //如果温度小于0度，则请求电压为0，请求电流为0
        {
            m_askvoltage=0;
            DCAmp=0;
        }   
    }
    return DCAmp;    
}
//**********************************************************************
//* Function name:   DCFaultDone
//* Description:     快充过程中故障的处理
//* EntryParameter : None
//* ReturnValue    : None
//**********************************************************************
void DCFaultDone(float Curt)
{
    
      
    if(CutDCChaCurt0.word!=0)
    {
        m_askcurrent=0; 
    }
    else if(CutDCChaCurt20.word!=0)
        m_askcurrent=0.2*Curt;
    else if(CutDCChaCurt50.word!=0)
        m_askcurrent=0.5*Curt;
    else
        m_askcurrent=Curt;    
    
    //////////////////////////////*断高压*/////////////////////////////
    

    if((Fault_Group3.Bit.F_Cell_Over_Voltage!=0)||(Fault_Group1.Bit.F_Battery_Over_Voltage == 1)||(Fault_Group1.Bit.F_Battery_Over_Voltage == 2))
    {
        fastendflag=1;
        fastend4|=0x04;//电压异常
        OffState=1;//请求下电
    } 
    else if(Fault_Group6.Bit.F_Over_Temp== 1)//若是出现3级温度过高故障
    {
        fastendflag=1;
        fastend3|=0x01;//电压异常
        OffState=1;//请求下电
    } 
    else if((Fault_Group1.Bit.F_Inner_Communiction==1)||(CutDCChaCurt0.word!=0))//若是出现3级内部通信故障
    {
        fastendflag=1;
        fastend3|=0x40;//其他故障
        OffState=1;//请求下电
    }  
    //////////////////////////////*断高压*/////////////////////////////
}

//**********************************************************************
//* Function name:   TaskRechargeDC
//* Description:     快充流程，大约每10ms调用一次该函数
//* EntryParameter : None
//* ReturnValue    : None
//**********************************************************************
void TaskRechargeDC(void)
{
    static unsigned char counter_250ms1=0;
    static unsigned char CHMStep1=0;
    
    static unsigned char counter_250ms2=0;
    static unsigned char CHMStep2=0;
    
    static unsigned char counter_250ms3=0;
    static unsigned char CHMStep3=0;
    
    static unsigned char counter_250ms4=0;
    static unsigned char CHMStep4_5=0;
    static unsigned char counter_50ms=0;
    
    static unsigned char counter_250ms5=0;
    static unsigned char CHMStep5=0;
    
    static unsigned char counter_250ms6=0;
    static unsigned char CHMStep6=0;
    
    
    static unsigned char DCFinish=0;
    static unsigned char timer1S=0;//单体电压到达后,停止充电条件判断延时
    static unsigned char timer2S=0;//第一次降电流延时
    static unsigned char timer3S=0;//第二次降电流延时
    static unsigned char ChanceCurt=0;//充电末端,电流变化后不回跳,
    static unsigned char ChangeTimeState=0;//充电结束后自进行一次加法
    float curr=0;
    ///////test//////////////
    if(g_BmsModeFlag != FASTRECHARGING)
        return;
    
    if(g_lowestTemperature == 0) //温度检测有问题时不能充电
        return;
    if((g_highestTemperature>=95)||((g_lowestTemperature<40)&&(st_heating!=1)))//40+50，40+0
    {
        fastend3|=0x01; //Temperature，中止充电
        BMSToVCU655_3|=0x01;
        fastendflag=1; //快充停止充电标志  
    } 
    else 
    {     
        fastend3&=0xfe;
        BMSToVCU655_3 &= 0xfe;
    }    
   
    //if(st_heating==1)  //预加热阶段(平均温度小于0度)，温度大于5度后，才能进行边加热边充电
        //return;
  
    curr = GetRequestCurrentDC(); 
    curr= curr+HeatCurt; 
    DCFaultDone(curr);//直流充电故障处理
    
    if(((g_highestCellVoltage>=3.5)&&(g_highestCellVoltage<3.55))||(ChanceCurt==1))//电流为计数出来的一半
    {
        if(ChanceCurt!=2)
        {          
            timer2S++;
            if(timer2S>=80)//10*80=800ms
            {    
                timer2S=81;                      
                if(m_askcurrent>48)
                    m_askcurrent=48;
                else if(m_askcurrent==0)
                    m_askcurrent=0;
                ChanceCurt=1;
            }
        }
    } 
    else 
    {
        timer2S=0;
    }
    //////////////电流分步控制及SOC修正,单体电压
    if((g_highestCellVoltage>=3.55)&&(g_highestCellVoltage<3.65)||(ChanceCurt==2))//电流为计数出来的一半
    {
        timer3S++;
        if(timer3S>=80)//10*80=800ms
        {    
            timer3S=81;
            if(m_askcurrent==0)
                m_askcurrent=0;
            else if(m_askcurrent>=24)      
                m_askcurrent=24;
            ChanceCurt=2;
        }
    } 
    else
    {      
        timer3S=0;
    }

	  if(g_highestCellVoltage>=3.65)  //3.65V停止充电
    {
        timer1S++;
        if(timer1S>=100)//10ms*100=1000ms
        {          
            if(DCFinish==0)
            {
              
                SocRechargeAdjust();
                AdjustQ1Value();
        	      StoreParameter();
        	      DCFinish=1;
    	      
            }   
            fastendflag=1;
            fastend1|=0x10;//单体电压达到目标值，请求中止充电
            timer1S=103; 
                      
        }
    } 
    else 
    {
        timer1S=0; 
    }


    

	  ///*******///////快充发送消息 
	  if(CHMStep)
		{     
    	  if(CHMStep==0x01)      //1801f456 00,多帧报文，不是250ms，报文间隔10ms
    	  {

    	      counter_250ms1++;
    	      if(counter_250ms1>=23)
    	      {
        	      if(DCStartState==2)//新国标开始
                    cpuToCHMBHM();
    	          CHMStep1=0;
    	          counter_250ms1=0;
    	      }    
    	      if(CHMStep1==0)
    	      { 
    			      if(DCStartState==1)//如果已经接收到旧国标开始
    			      {
        			      sendi1++;
        			      if(sendi1%7==1)
        			          cpuToCHMBRM();
        			      else if(sendi1%7==2)
        			          cpuToCHMBRMDATA1();
        			      else if(sendi1%7==3)
        			          cpuToCHMBRMDATA2();
        			      else if(sendi1%7==4)
        			          cpuToCHMBRMDATA3();
        			      else if(sendi1%7==5)
        			          cpuToCHMBRMDATA4();
        			      else if(sendi1%7==6)
        			          cpuToCHMBRMDATA5();
        			      else if(sendi1%7==0)
        			      {  
        			          sendi1=0;
        			          cpuToCHMBRMDATA6();
        			          CHMStep1=1;
        			      }
        			  }
    	      }
    	  }
    		if(CHMStep==0x02)   //1801f456 aa
			  {
			      counter_250ms2++;
			      if(counter_250ms2>50)//500ms
			      {
			        
			          CHMStep2=0;
			          counter_250ms2=0;
			      }
			      if(CHMStep2==0)
			      { 
    			      sendi2++;
    			      if(sendi2%3==1)
    			          cpuToCHMBCP();
    			      if(sendi2%3==2)
    			          cpuToCHMBCPDATA1();
    			      else if(sendi2%3==0)
    			      {
    			          sendi2=0;
    			          cpuToCHMBCPDATA2();
    			          CHMStep2=1;
    			      }
    			      sendi1=0;//清除第一组计数
			      }
			  }
			  if(CHMStep==0x03)  //1808f456 aa 250ms
  		  {
           
            counter_250ms3++;
            if(CHMStep3==0)
            {
                if(SelfState3==0)
          			    cpuToCHMBRO();  //100956f4 aa
                CHMStep3=1;
            }
            if(counter_250ms3>19)//确保第一帧数据发出，不必在延时小于250ms
            {
              
  			        counter_250ms3=0;
  			        CHMStep3=0;
  			        
            }
  		  }
    		if((CHMStep==0x04)||(CHMStep==0x05)) //50ms周期
        {
            CCSOverTime++;
            if(CCSOverTime>150)//10*150=1.5S
            {
                OverTimeState=1;//超时标志位置1                
                CHMStep=0x06;
                BEMError3|=0x01;
                
            }
            if(counter_50ms%4==0)//50ms 
            {			            
                cpuToCHMBCL();    // voltage request and current request
                counter_50ms=0;
            }
            counter_50ms++;
  		      counter_250ms4++;      //250ms
  		      if(counter_250ms4>19)
  		      {
  		        
  		          CHMStep4_5=0;
  		          counter_250ms4=0;
  		      }
  		      if(CHMStep4_5==0)
  		      {			        
    			      sendi1++;
    			      if(sendi1%3==1)
    			          cpuToCHMBCS();
    			      else if(sendi1%3==2)
    			      {
    			          cpuToCHMBCSDATA1();
    			      }
    			      else if(sendi1%3==0)
    			      {
    			          sendi1=0;
    			          cpuToCHMBCSDATA2();
    			          CHMStep4_5=1;
    			      }
  		      }
  		      if(CHMStep==0x05) 
  		      {
      		      counter_250ms5++;
                if(CHMStep5==0)
                {
                    cpuToCHMBSM();
                    CHMStep5=1;
                }
                if(counter_250ms5>19)//确保第一帧数据发出，不必在延时小于250ms
                {
                  
      			        counter_250ms5=0;
      			        CHMStep5=0;
      			        
                }
            }
  		  }	  
			  if((CHMStep==0x06)||((CHMStep==0x05)&&(fastendflag==1)))//10ms发送一次,开始发的时候	
    		{
    			  //如果收到地面充电机充电截止报文或者单体电压、总电压超过保护值
			      cpuToCHMBST();
			      m_askcurrent=0;//请求电流为0
			      if((StoreAHState==1)&&(ChangeTimeState==0))//记录充电次数1次
			      {
                Charge_Times++;
                StoreChargeTime();//存储充电次数
                ChangeTimeState=1;
			      }
			      CSTOverTime++;//10ms*200
			      if(CSTOverTime>200)//防止收不到CST报文 
			      {
    			      CHMStep=0x06;
    			      OverTimeState=1;
    			      BEMError3|=0x02;
    			      //Fault_Group6.Bit.F_Communication_With_Charger=1;
			      }			       							      			      			      			      			         		
        		if(CHMStep==0x06)   
    			  {
                SocRechargeAdjust();// SOC adjust
                if(CHMStep6==0)
                {                  
                    if(OverTimeState==1)
            			  {
            			      cpuToCHMBEM();
            			  }
                    cpuToCHMBSD();  // report BMS status: the highest and the lowest cell voltage, temperature and SOC.
                    CHMStep6=1;
                }
                counter_250ms6++;
    			      if(counter_250ms6>19)
    			      {
      			        counter_250ms6=0;
      			        CHMStep6=0;			                       
    			      }
    			      CSDOverTime++;
    			      if(CSDOverTime>=200)//接收不到充电机统计报文时,延时2s下电
    			      {
    			          CHMStep=0x07;
    			          OverTimeState=1;
    			          BEMError4|=0x01;    			          
    			      }    			      
    			      
    			  }
			  }        
			  if(CHMStep==0x07) //快充充电结束 
			  {
			      if(OverTimeState==1)//已经超时
			          cpuToCHMBEM();
            OffState=1;//状态机由170跳转40的标志位
            DC_ChangeOver=1;//直流充电完成标志
           
			  }
												
		}/////////END快充发送消息
		
}
//******************************************************************************
//* Function name:   GetRequestCurrentAC
//* Description:     充电请求电流计算
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
float GetRequestCurrentAC(void) 
{
    float CurrentAValue;
    
  
    if((Tavg<(0+45))&&(st_heating==1)) ///小于0度预加热
    {
        m_askvoltage=376;//3.3v * 114
        CurrentAValue=0;
    } 
    else if(Tavg<(10+40)) //////大于等于0度小于10度
    {
        if(BCU_AC_CC_State==2) //220欧
            CurrentAValue = 12;// 0.1c  
        if(BCU_AC_CC_State==1) //680欧 
            CurrentAValue = 10;
        //m_askvoltage=417;
        m_askvoltage=403;
    }
        
    else if(Tavg<(55+40)) //////大于等于10度小于等于50度
    {
        if(BCU_AC_CC_State==2) //220欧
            CurrentAValue = 20;// 0.1c
        if(BCU_AC_CC_State==1) //680欧 
            CurrentAValue = 10;
        //m_askvoltage=417;
        m_askvoltage=403;
    }       
    else
    {
        m_askvoltage=0;
        CurrentAValue=0;
    }
    
    return CurrentAValue;         
}
//**********************************************************************
//* Function name:   ACFaultDone
//* Description:     慢充过程中故障的处理
//* EntryParameter : None
//* ReturnValue    : None
//**********************************************************************
void ACFaultDone(float Curt)
{
    
    //////////////////////////////*降电流*/////////////////////////////
    /*
    if((Can08f0Byte0.Bit.F1_batteryOV == 1)||(Can08f0Byte3.Bit.F2_overTemp== 1))//若是出现1级电池包过压故障,2级温度过高故障，则降电流
    {
        ChgeAmpReq =  Curt*0.5;      
    }
    else
    {
        ChgeAmpReq= Curt;
    }
  
    
    //////////////////////////////降电流/////////////////////////////
        
    
    //////////////////////////////断高压/////////////////////////////
    /
    if(((Can08f0Byte0.byte&0x80)!=0)||((Can08f0Byte2.byte&0x84)!=0)||((Can08f0Byte4.byte&0x84)!=0)||((Can08f0Byte5.byte&0x21)!=0))
    {
        ACEndState=1;//慢充停止标志位
        OffState=1;//请求下电
    }
    */ 
    //////////////////////////////*断高压*/////////////////////////////
}

//******************************************************************************
//* Function name:   TaskRechargeAC
//* Description:     与车载充电机通讯，对电池进行慢充
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void TaskRechargeAC(void)
{
    static unsigned char Step = 0x00;
    static unsigned char time0=0,ChanceAC=0;
    static unsigned char chargeFinish=0;
    static unsigned char ii = 0;
    float curr=0;
    
    
    /*
    if(chargeFaultState!=0)// 充电机故障状态,chargeFaultState接收充电机发送过来的充电机故障状态 
        Step=0xFF;
    */
    if(chargeWorkState==0x01)//充电机控制状态=关闭，则停止充电
        Step=0xFF;
    
    if((g_highestCellVoltage>=3.70)&&(slowRechargeFinished==0)) //单体大于3.65且慢充未完成，不允许再充电。。防止反复在那充电
        Step=0xFF;
    if(st_heating!=1) //若是0表示温度大于10度；2表示温度大于0度，1是温度在0下
    {
      if(g_highestTemperature > (55+40)||(g_lowestTemperature < (0+40))) //t>=55 or t<=0
            Step=0xFF;
    }
    else
    {
        if(g_highestTemperature > (55+40))
            Step=0xFF;
    }
    
    if((!slowRechargeFinished)&&(ReqCtlToCHG==0x00)) //如果充电未完成
    {

        curr=GetRequestCurrentAC();
        curr = curr+HeatCurt;
        if((g_systemCurrent<=-3)&&(g_socValue<0.992))
            chargeRemainderTime=(unsigned int)((g_realNominalCap*(1-g_socValue)*60)/(-g_systemCurrent));
        else if(g_socValue>=0.992)
            chargeRemainderTime=2;     
        chargeRemainderTime=chargeRemainderTime*60;
  
    }
    ACFaultDone(curr);
    if(ACEndState==1)//故障停止标志位
        Step = 0xff;    
    switch(Step)
    {
        
        case 0://初始
            ReqCtlToCHG = 0x0;//void
            ChgeAmpReq = 0;
            Step = 1;
            
        case 1://Chge Step1
        
            ReqCtlToCHG = 0x0;//请求充电 
            Step = 2;
            break;
            
        case 2://Chge Step2
            ReqCtlToCHG = 0x0;//请求充电
            if(g_highestCellVoltage >=3.55)  //大于等于3.55开始降电流
            Step = 3;
            break;
            
        case 3://Chge Step3
            ReqCtlToCHG = 0x0;//请求充电

            if(g_highestCellVoltage >=3.65)
            {
                time0++;
                ChgeAmpReq=0.5*curr;
                if(time0>=2) //延时1s后，当确实大于3.65后才下电
                {
                  
                    Step = 4;
                    ChgeAmpReq=0;
                    time0=3;
                    break;  
                }
            } 
            else
            {
                time0 = 0;
                ChgeAmpReq=0.5*curr;//
            }

            break;  
        case 4://Chge Step4 
                
                chargeFinish=1;
                ReqCtlToCHG = 0x01;//停机状态
                ChgeAmpReq=0;
                Step = 0xFF;
                
            break;  
        case 0xFF://err or finish
        
                slowRechargeFinished=1;//交流充电完成
                SocRechargeAdjust();  ////修正SOC
                ChgeAmpReq=0;//充电电流请求为0
                ReqCtlToCHG = 0x01;  //void 
                //stateCode=40;
                OffState=1;//状态机由110跳转40的标志位
            break;
        default:
          break;        
      }//switch(Step)
      
}
//************************************************************************
//************************************************************************
//**************************the end******************************************
//************************************************************************