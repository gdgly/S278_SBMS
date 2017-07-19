/*-------------------------------------------------------------------------
File name: Process.c
Writen by Judy
Date: ,2010
Discription:
*/
#include "derivative.h" /* include peripheral declarations */
#include "BMS20.h" 
#include "mscan.h"
#include "bmn.h"
#include "dflash.h" 


unsigned char socadjflag=0;
char mbackcout=0;
char mbackcout1=0;
char mbackcout2=0;
unsigned char  flagf5; //温度<0度
unsigned char  flag7; //温度0-15度
unsigned char  flag25;//温度15-35度
unsigned char  flag45;//温度35-50度
unsigned char  flag60;//温度>50度
unsigned char  tempflag;//温度区间判断
unsigned char  feedflag=0; //温度0-15脉冲标志位
unsigned char  feedflag1=0; //温度15-35脉冲标志位
unsigned char  feedflag2=0;//温度35-50脉冲标志位
unsigned char  imflag=0; //温度0-15电流最大值脉冲标志
unsigned char  imflag1=0; //温度15-35电流最大值脉冲标志
unsigned char  imflag2=0;//温度35-50电流最大值脉冲标志
unsigned int  icount=0; //温度0-15电流计数
unsigned int  scount=0; //温度0-15脉冲S计数
unsigned int  icount1=0; //温度15-35电流计数
unsigned int  scount1=0; //温度15-35脉冲S计数
unsigned int  icount2=0; //温度35-50电流计数
unsigned int  scount2=0; //温度35-50脉冲S计数
unsigned char fastendflag=0;
unsigned char sendi1=0;
unsigned char sendi2=0;
unsigned char cellsendcount=0;
unsigned char cellsenddelay=0;
unsigned char imcount=1;
unsigned char imcount1=1;
unsigned char imcount2=1;


float g_socValue=0;  //实际SOC
float g_displaySocValue = 0;//该SOC值用于显示或发送给整车。是修正后的SOC
float g_energyOfUsed=0; //已用电池容量
float g_leftEnergy=0;    //剩余电池容量
float g_usefulCapacity;//可用电池容量
//unsigned int g_isolution = 1000; //绝缘电阻值
unsigned int g_isolationP_high;
unsigned int g_Rp=1000;
unsigned int g_Rn=1000;
unsigned int g_isolationP_low;
unsigned int g_isolationN_high;
unsigned int g_isolationN_low;
unsigned char g_isolationLever ;


unsigned char g_chargeStateFlag = 0; //0:充电 1:放电
unsigned char g_bmubalance = 0; //0:充电 1:放电
//unsigned char g_rechargerState;  // 0: ready  1:recharging 2: end recharge 3:recharge error
unsigned int g_chargerCurrent=0; //充电机当前充电电流
unsigned int g_chargerVoltage=0; //充电机当前充电电压
float g_uppestRechargeCurrent =0;//最大允许充电电流
float g_uppestDischargeCurrent =0;//最大允许放电电流
float g_feedbackChargeCurrent;//最大允许放电电流

/////
unsigned int g_isrTimes = 0; //中断次数
unsigned long g_BMU_CIRCLE_FLAG =0; //用于配置BMU个数
//
float g_systemCurrent=0;
float	g_highVoltageV1=0;
float g_highVoltageV2=0; //预充电继电器与预充电阻之间的电压
float g_highVoltageV3 =0;//预充电继电器另一边的电压

//**********M30经济型故障****************
unsigned int tmr_p2 =0;
unsigned int tmr_p3 =0;
unsigned char innerVoltFault =0;
unsigned char outVoltFault =0;
unsigned char currenFault =0;
unsigned char insulationFault =0;
unsigned char innerCanFault = 0;

unsigned char msdFault =0; //MSD故障标志
unsigned char relayNconnected =0; //负极继电器粘连
unsigned char relayNdisconnected =0;// 负极继电器断开故障
unsigned char relayPrechargeConnected =0; //预充继电器粘连
unsigned char  prechargeFinished =0;    //预充电完成标志位
unsigned char  bcuSelfcheckCounter =0; //BCU自检计数器
//*********************************************
unsigned char  fastend1=0;
unsigned char  fastend2=0;
unsigned char  fastend3=0;
unsigned char  fastend4=0;
unsigned char  fasterror11 =0;
unsigned char  fasterror12 =0;

unsigned char ChgeStat=0;//充电状态
unsigned char Htr2ChgeDly=0;  //加热到充电转换时间间隔
unsigned char ChgeAmpReq=0; //充电电流值
unsigned int counter_500ms=0;
unsigned char counter_500ms1=0;
unsigned char counter_50ms=0;
unsigned char counter_250ms=0;

unsigned long chargerID;
unsigned long vehicleID;

unsigned long g_mboxID; //CAN2 接收到的报文ID号，也就是BMU的报文ID
unsigned char g_mboxData[1][8];// CAN2接收到的报文数据，每个报文8个字节
unsigned char can2receiveIsReady;//can2处理数据中

unsigned int g_ParaEndFlag=0;
unsigned int g_endPowerOffFlag=0;

unsigned long sumAhCharge=0;
unsigned long sumAhDischarge=0;

unsigned long sumAhChargeOnce=0;
unsigned long sumAhDischargeOnce=0;

//************************************************************************
//************************************************************************
static void delay(unsigned int t) 
{ 
    while(t--); 
}

//************************************************************************
//************************************************************************
void LoadParamter(void)
{	
  	Rd_Flash(ParaBaseAddr,(unsigned char* )g_sysPara,ParamNUMBER*4);  //读出所保存的系统参数
  	Rd_Flash(ParaBaseAddr,(unsigned char* )g_sysPara,ParamNUMBER*4);  
  	g_uppestRechargeCurrent = g_sysPara[PARA_HIGHEST_RECHARGE_CURRENT];  //最大充电允许电流
  	g_feedbackChargeCurrent = g_sysPara[PARA_HIGHEST_RECHARGE_CURRENT];
  	g_uppestDischargeCurrent = g_sysPara[PARA_HIGHEST_DISCHARGE_CURRENT];  //最大放电电允许电流
  	
    g_BMU_CIRCLE_FLAG = 0x0000011;	//4个BMU
	  /*
  	switch((unsigned int)g_sysPara[PARA_BMU_NUMBER])//根据BMU的个数，确定相应的配置信息，能够知道是否所有的单体数据已经发了一轮
  	{
  	  case 1:
  	    g_BMU_CIRCLE_FLAG = 0x0000001;
  	    break;
  	  case 2:
  	    g_BMU_CIRCLE_FLAG = 0x0000011;
  	    break; 
  	  case 3:
  	    g_BMU_CIRCLE_FLAG = 0x0000111;
  	    break;  
  	  case 4:
  	    g_BMU_CIRCLE_FLAG = 0x0001111;
  	    break; 
  	  case 5:
  	    g_BMU_CIRCLE_FLAG = 0x0011111;
  	    break;
  	  case 6:
  	    g_BMU_CIRCLE_FLAG = 0x0111111;
  	    break;  
  	  case 7:
  	    g_BMU_CIRCLE_FLAG = 0x1111111;
  	    break;      
  	  default:
  	    break;
    }  
    */

}
//************************************************************************
//************************************************************************
void StoreParameter(void)
{
	  //W25X_SectorErase(ParaBaseAddr);		
	  Wrt_Flash(ParaBaseAddr,(unsigned char* )g_sysPara,ParamNUMBER*4);  //重新写入系统参数
}
//************************************************************************
//************************************************************************
void StoreSocRealvalue(void)//
{
    g_sysPara[PARA_SOC_VALUE]=g_socValue;
    Wrt_Flash(ParaBaseAddr,(unsigned char* )g_sysPara,4);  //只存入新的SOC值，别的系统参数值不变
}
void StoreTotalAH(void)
{
     unsigned char AHbyte[8];
     
     //sumAhCharge =0;
     AHbyte[0]= (sumAhCharge&0xff000000)>>24;
     AHbyte[1]= (sumAhCharge&0x00ff0000)>>16;
     AHbyte[2]= (sumAhCharge&0x0000ff00)>>8;
     AHbyte[3]= sumAhCharge;
     //sumAhDischarge =0;
     AHbyte[4]= (sumAhDischarge&0xff000000)>>24;
     AHbyte[5]= (sumAhDischarge&0x00ff0000)>>16;
     AHbyte[6]= (sumAhDischarge&0x0000ff00)>>8;
     AHbyte[7]= sumAhDischarge;
     Wrt_Flash(SUM_AH_ADDRESS,AHbyte,8);  //保存总的充放电容量到AD161里 单位为A
     
}
void LoadTotalAH(void) 
{
     unsigned char AHbyte[8];
     
     Rd_Flash(SUM_AH_ADDRESS,AHbyte,8);
     sumAhCharge = (AHbyte[0]&0x000000ff)<<24; 
     sumAhCharge |= (AHbyte[1]&0x000000ff)<<16; 
     sumAhCharge |= (AHbyte[2]&0x000000ff)<<8; 
     sumAhCharge |= AHbyte[3];
     sumAhDischarge =0;
     sumAhDischarge = (AHbyte[4]&0x000000ff)<<24; 
     sumAhDischarge |= (AHbyte[5]&0x000000ff)<<16; 
     sumAhDischarge |= (AHbyte[6]&0x000000ff)<<8; 
     sumAhDischarge |= AHbyte[7];

}


//************************************************************************
///////////根据平均温度调整最大允许充电电流  给充电机
//************************************************************************
void BigRechargeCurrentAdjust(void) //
{
    g_uppestRechargeCurrent =  g_sysPara[PARA_HIGHEST_RECHARGE_CURRENT];
    if((g_averageTemperature-40)>=50) 
        g_uppestRechargeCurrent = 0;
    else if((g_averageTemperature-40)>=45)
        g_uppestRechargeCurrent = g_usefulCapacity*0.2;
    else if((g_averageTemperature-40)>=20)
        g_uppestRechargeCurrent = g_usefulCapacity*0.5;
    else if((g_averageTemperature-40)>=10)
        g_uppestRechargeCurrent = g_usefulCapacity*0.3;
    else if((g_averageTemperature-40)>=5)
        g_uppestRechargeCurrent = g_usefulCapacity*0.2;
    else if((g_averageTemperature-40)>=0)
        g_uppestRechargeCurrent = g_usefulCapacity*0.1;
    else
        g_uppestRechargeCurrent =0;
}
//*************************************************************************
void FeedbackCurrentAdjust(void)
{
    float m_ccurrent;
    
    //该部分处理原因为，防止温度在临界区域时，温度来回跳变，导致脉冲回馈电流异常跳变 
    if(g_averageTemperature>=40) 
        flag7=1;
    else 
        flagf5=1;
    if(g_averageTemperature>=55) 
        flag25=1;
    if(g_averageTemperature>=75) 
        flag45=1;
    if(g_averageTemperature>=90) 
        flag60=1;
    //////温度回落情况，可能不会出现该现象	
	  if(g_averageTemperature<39)
	      flag7=0;
	  if(g_averageTemperature<54)
	      flag25=0;
	  if(g_averageTemperature<74)
	      flag45=0;
		
	  if(flag60==1) 
	  {
	      m_ccurrent=0;
	      tempflag=5;
	  }
	  else if(flag45==1) 
	  {
	      m_ccurrent=t45m;
	      tempflag=4;
	  }
	  else if(flag25==1) 
	  {
	      m_ccurrent=t25m;
	      tempflag=3;
	  }
	  else if(flag7==1) 
	  {
	      m_ccurrent=t7m;
	      tempflag=2;
	  }
	  else if(flagf5==1) 
	  {
	      m_ccurrent=0;
	      tempflag=1;
	  }

  	if(tempflag==2)   //处理0-15度区间
	  {	
		    if(g_systemCurrent<=(0-t7l))//大于持续电流值10S
		    {
			      scount++;
			      if(scount <=99) 
			          feedflag=0; //10S=100ms*100
			      else if(scount >99) 
			          feedflag=1;//反馈脉冲标准至1	
		    }
		    if(feedflag==0)//无反馈脉冲标准
		    {
			      if(g_systemCurrent>(0-t7l))//如果回馈电流终止或低于当前温度下可接受的最大持续回馈电流时
			          scount=0;
		    }
		    if(g_systemCurrent<=(0-t7m))//大于最大值2S
		    {
			      if(++mbackcout>=20) //2S=100ms*20	
			      {
				        imflag=1;//0-15度区间电流最大值标志
			      }
		    }
    }
	  else if(tempflag==3)  //处理15-35度区间
	  {	
		    if(g_systemCurrent<=(0-t25l))//大于持续电流值30S
		    {
			      scount1++;
			      if(scount1<=280) 
			          feedflag1=0;
			      else if(scount1>280) 
			          feedflag1=1;	
		    }
		    if(feedflag1==0)
		    {
			      if(g_systemCurrent>(0-t25l))
			          scount1=0;
		    }

		    if(g_systemCurrent<=(0-t25m))//大于最大值2S
		    {
			      if(++mbackcout1>=20)	
			      {
				        imflag1=1;
			      }
		    }
    }
	  else if(tempflag==4)
	  {	
		    if(g_systemCurrent<=(0-t45l))//大于持续电流值10S
		    {
			      scount2++;
			      if(scount2<=99) feedflag2=0;
			      else if(scount2>99) feedflag2=1;	
		    }
		    if(feedflag2==0)
		    {
			      if(g_systemCurrent>(0-t45l))
			      scount2=0;
		    }		
		    if(g_systemCurrent<=(0-t45m))//大于最大值2S
		    {
			      if(++mbackcout2>=20)	
			      {
				        imflag2=1;
			      }
		    }
    }
    /////单脉冲处理
	  if(feedflag==1)//如果已存在反馈
	  {
		    icount++;
		    if(g_systemCurrent<-0.1)// 若还存在反馈电流
		    {
			      if(icount<=50) 
			          m_ccurrent=t7m-(t7m-t7l)*icount/50;
			      else  
			          m_ccurrent=t7l;
		    }
	  }
	  else if(feedflag1==1)  //确保第一个回馈脉冲结束，才能开始
	  {
		    icount1++;
		    if(g_systemCurrent<-0.1)
		    {
			      if(icount1<=50) 
			          m_ccurrent=t25m-(t25m-t25l)*icount1/50;
			      else  
			          m_ccurrent=t25l;
		    }
	  }
	  else if(feedflag2==1)  //确保第一个回馈脉冲结束，才能开始
	  {
		    icount2++;
		    if(g_systemCurrent<-0.1)
		    {
			      if(icount2<=50) 
			          m_ccurrent=t45m-(t45m-t45l)*icount2/50;
			      else  
			          m_ccurrent=t45l;
		    }
	  }
    /////END单脉冲处理
	  /////////////////超最大值脉冲处理
	  if(imflag==1)
	  {
		    m_ccurrent=t7m-imcount;
			  if(imcount++>30)
			      imcount=30;
			  if(m_ccurrent<t7l)
				    m_ccurrent=t7l;
	  } 
	  else if(imflag1==1)
	  {
			  m_ccurrent=t25m-imcount1*2.5;
			  if(imcount1++>30)
			      imcount1=30;
			  if(m_ccurrent<t25l)
				    m_ccurrent=t25l;
	  }
	  else if(imflag2==1)
	  {
			  m_ccurrent=t45m-imcount2*0.7;
			  if(imcount2++>30)
			      imcount2=30;
			  if(m_ccurrent<t45l)
				    m_ccurrent=t45l;	
	  } 
	  else
	  {
	      imcount=1;
	      imcount1=1;
	      imcount2=1;
	  }

    //////////END 最大值脉冲
	  if(g_systemCurrent>=-0.1)//无反馈电流后，计数器清零，处于未反馈状态。
	  {
  	    feedflag=0;	
  	    imflag=0;
  	    feedflag1=0;	
  	    imflag1=0;	    
  		  scount=0;
  		  icount=0;
  		  feedflag2=0;	
  		  imflag2=0;		  
  		  scount2=0;
  		  icount2=0;
  		  scount1=0;
  		  icount1=0;
  		  mbackcout2=0;
  		  mbackcout1=0; 
  		  mbackcout=0;
  		  
  		  imcount=1;
  		  imcount1=1;
  		  imcount2=1;
  		  if(tempflag==2) 
  		      m_ccurrent=t7m;
  		  else if(tempflag==3) 
  		      m_ccurrent=t25m;
  		  else if(tempflag==4) 
  		      m_ccurrent=t45m;
    }
    
    if((g_socValue>=0.9)||(g_highVoltageV1>=400)||(g_highestCellVoltage>=4.3)) //处理SOC>=90%时，不允许反馈电流优先级最高。
	  {
    		m_ccurrent=0;
    		feedflag=0;	
    		imflag=0;
    		feedflag1=0;	
    		imflag1=0;
    		scount=0;
    		icount=0;
    		feedflag2=0;	
    		imflag2=0;
    		scount2=0;
    		icount2=0;
    		scount1=0;
    		icount1=0;
    		mbackcout2=0;
    		mbackcout1=0; 
    		mbackcout=0;
    		imcount=1;
    		imcount1=1;
    		imcount2=1;
	  }

    g_feedbackChargeCurrent=  m_ccurrent;
    //g_sysPara[PARA_HIGHEST_RECHARGE_CURRENT]=  m_ccurrent;
}
//************************************************************************
//************************************************************************
void BigDischargeCurrentAdjust(void)
{
    float tem =0;
    
    if(g_averageTemperature==0)
        return;
    
    if(g_socValue<0.1) 
    {
        g_uppestDischargeCurrent =10;
        return;
    }
    
    if(g_averageTemperature<40-30)
        g_uppestDischargeCurrent =0;
    
    if(40-30<=g_averageTemperature<40-20)
        g_uppestDischargeCurrent =10;    
    
    if(40-20<=g_averageTemperature<=40+45) 
    {
        if(g_socValue>=0.3) 
        {          
            g_uppestDischargeCurrent = 50+g_socValue*100 + 5*(float)(g_averageTemperature-40);
            if(g_uppestDischargeCurrent>=200)
                g_uppestDischargeCurrent = 200;            
            if(g_uppestDischargeCurrent<=40)
                g_uppestDischargeCurrent = 40;
        } 
        else //if(0.1<g_socValue<0.3)
        {
            tem = 80+ 5*(float)(g_averageTemperature-40);
            if(tem<204)
                g_uppestDischargeCurrent= tem - (30-g_socValue*100)*10;
            else
                g_uppestDischargeCurrent= 200 - (30-g_socValue*100)*10;
            if(g_uppestDischargeCurrent<40) 
                g_uppestDischargeCurrent=40;
        }            
    } 
    else if(40+45<g_averageTemperature<=40+50)
    {
         if(g_socValue>=0.3) 
         {
             g_uppestDischargeCurrent=200- ((float)g_averageTemperature-40-45)*4;
         }
         else //if(0.1<g_socValue<0.3)
         {
             g_uppestDischargeCurrent=200- ((float)g_averageTemperature-40-45)*4 - (30-g_socValue*100)*4;
         }
    }
    else if(40+50<g_averageTemperature<=40+55) 
    {
         if(g_socValue>=0.3) 
         {
             g_uppestDischargeCurrent= 180 - ((float)g_averageTemperature-40-50)*20;
         }
         else //if(0.1<g_socValue<0.3)
         {
             g_uppestDischargeCurrent= 180 - ((float)g_averageTemperature-40-50)*20 - (30-g_socValue*100)*4 ;
         }
         if(g_uppestDischargeCurrent<40) 
               g_uppestDischargeCurrent=40;
         
    } 
    else if(g_averageTemperature>40+55) 
    {
         g_uppestDischargeCurrent=10;
    }
    
}
//************************************************************************
///////////根据平均温度调整最大允许再生脉冲充电电流  给VCU
//************************************************************************
float PulseRechargeCurrentAdjust(void) 
{
   float current;
   
   current =  0;
   if(g_socValue>0.98)
       return current;
   if(g_averageTemperature<= 35) //-5℃
       return current;   
   else if(g_averageTemperature<=40) 
       if((g_socValue>=0.75)&&(g_socValue<=0.85)) 
           current =  g_sysPara[PARA_WHOLE_CAPACITY]; // 1c/10s
   else if(g_averageTemperature<=50)
       if((g_socValue>=0.75)&&(g_socValue<=0.95)) 
           current =  g_sysPara[PARA_WHOLE_CAPACITY]; //1c/10s
   else if(g_averageTemperature<=63) //
   {    
       if((g_socValue>=0.75)&&(g_socValue<=0.85)) 
           current =  g_sysPara[PARA_WHOLE_CAPACITY]*1.5;  //1.5c
       if((g_socValue>0.85)&&(g_socValue<=0.98)) 
           current =  g_sysPara[PARA_WHOLE_CAPACITY]; //1c/10s
   } 
   else if(g_averageTemperature<=90) 
   {
       if((g_socValue>=0.75)&&(g_socValue<=0.85)) 
           current =  g_sysPara[PARA_WHOLE_CAPACITY]*2; // 2c
       if((g_socValue>0.85)&&(g_socValue<=0.95)) 
           current =  g_sysPara[PARA_WHOLE_CAPACITY]*1.5; //1.5
       if((g_socValue>0.95)&&(g_socValue<=0.98)) 
           current =  g_sysPara[PARA_WHOLE_CAPACITY];  //1c
   }
   
   return (current);
}

//************************************************************************
////////放出容量温度修正//////////////////////////////////////////////
//************************************************************************
float TemperatureAdjustForCap(float capacity,unsigned char tem) 
{
    float x,y;
    unsigned char t;        
    
    x= capacity;
    return x;
   /* 
    y=0;
    t= tem;
    
    if(t>17+40)
       y=x;
    else if(t>=12+40)
       y=0.986*x-0.083;
    else if(t>=7+40)
       y=0.949*x-0.039;
    else if(t>=3+40)
       y=0.912*x+0.133;
    else if(t>=37)
       y=0.869*x-0.849;
    else if(t>=32)
       y=0.833*x-2.3;
    else if(t>=27)
       y=0.761*x-3.273;
    else if(t>=22)
       y=0.651*x-2.772;
    else
       y=0.579*x-3.341;
    
    return y;
   */ 


}
//************************************************************************
// 根据平均温度、单体最高电压、充电电流和单体平均电压对SOC进行调整
//************************************************************************
void SocRechargeAdjust(void)//充电SOC修正子程序
{
      /*
      if(g_averageTemperature<40)  //如果平均温度小于10摄氏度，则不需要修正。50对应10℃
      //if(g_averageTemperature>=20+40)  //如果平均温度小于20摄氏度，则不需要修正。
          return;
      
      //if(g_highestCellVoltage<=3.60)  //如果单体电压小于3.6 则不修正
      if(g_highestCellVoltage<4.15)  //如果单体电压小4.2 则不修正
          return;
      
      if(g_averageVoltage<3.95) //averageVoltage under 3.95V Don't need adjust
          return;
      else if((g_averageVoltage>=3.95)&&(g_averageVoltage<4.2))
      {
           if(socadjflag==0)
           {
               g_socValue=(148*g_averageVoltage-515+0.45*(g_averageTemperature-63))*0.01;
               socadjflag=1;
           }
           if(g_socValue>=1.0)
               g_socValue=0.99;
           g_leftEnergy = g_socValue*(g_usefulCapacity*3600);
           g_energyOfUsed =0; 
      }
     */   
      //if((-g_systemCurrent) >= g_sysPara[PARA_WHOLE_CAPACITY]*1)  //如果充电电流大于1c,则不修正  PARA_WHOLE_CAPACITY
      //  return;
  /*    
      if((-g_systemCurrent) >=g_sysPara[PARA_WHOLE_CAPACITY]*0.6)  //
      {
          if(g_averageVoltage>=3.50) 
          {
              if(g_socValue<0.99)
                  g_socValue=0.99;
              
          }           
          else if(g_averageVoltage>=3.45) 
          {
              if(g_socValue<0.98)
                  g_socValue=0.98;  
                      
          } 
          else if(g_averageVoltage>=3.40) 
          {
              if(g_socValue<0.8)
                  g_socValue=0.8; 
              else if(g_socValue>0.98)
                  g_socValue=0.98;               
                      
          } 
          else 
          {
               if(g_socValue>0.8)
                  g_socValue=0.8; 
                       
          }          
      }  // end if 0.6 到1c 
      else if((-g_systemCurrent) >= g_sysPara[PARA_WHOLE_CAPACITY]*0.4)  //
      {
          if(g_averageVoltage>=3.50) 
          {
              if(g_socValue<0.99)
                  g_socValue=0.99;
              
          }           
          else if(g_averageVoltage>=3.45) 
          {
              if(g_socValue<0.98)
                  g_socValue=0.98;  
                       
          } 
          else if(g_averageVoltage>=3.40) 
          {
              if(g_socValue<0.9)
                  g_socValue=0.9; 
              else if(g_socValue>0.98)
                  g_socValue=0.98;               
                    
          } 
          else 
          {
               if(g_socValue>0.9)
                  g_socValue=0.9; 
                       
          } 
      
      } // end if 0.4 到0.6c 
      else if((-g_systemCurrent) >= g_sysPara[PARA_WHOLE_CAPACITY]*0.2)  //
      {
          if(g_averageVoltage>=3.50) 
          {
              if(g_socValue<0.99)
                  g_socValue=0.99;
              
          }           
          else if(g_averageVoltage>=3.45) 
          {
              if(g_socValue<0.98)
                  g_socValue=0.98;  
                       
          } 
          else if(g_averageVoltage>=3.40) 
          {
              if(g_socValue<0.95)
                  g_socValue=0.95; 
              else if(g_socValue>0.98)
                  g_socValue=0.98;               
                      
          } 
          else 
          {
               if(g_socValue>0.95)
                  g_socValue=0.95; 
                     
          }       
      }  // end if 0.2 到0.4c
      else//
      {
          if(g_averageVoltage>=3.45) 
          {
              if(g_socValue<0.99)
                  g_socValue=0.99;      
          } 
          else if(g_averageVoltage>=3.42) 
          {
              if(g_socValue<0.98)
                  g_socValue=0.98; 
              else if(g_socValue>0.99)
                  g_socValue=0.99;                         
          } 
          else if(g_averageVoltage>=3.40) 
          {
               if(g_socValue<0.96)
                  g_socValue=0.96; 
               else if(g_socValue>0.98)
                  g_socValue=0.98; 
               
          } 
          else  if(g_averageVoltage>=3.38) 
          {
                if(g_socValue<0.93)
                  g_socValue=0.93; 
               else if(g_socValue>0.96)
                  g_socValue=0.96; 
          } 
          else 
          {
              if(g_socValue>=0.93)
                 g_socValue>=0.93; 
          }
          
      } // end if 0 到0.2C
      g_leftEnergy = g_socValue*(g_usefulCapacity*3600);
      g_energyOfUsed =0;
     */ 
}
//************************************************************************
//************************************************************************
float SocOCVAdjust(float vocv)
{
    float x=0;
    float y2=0;
    
    x=vocv;
    if(LITHIUM) 
    {
        if(x>=3.662)
            y2=-0.719*x*x+6.797*x-14.82;
        else if(x>=3.450)        
            y2=8.606*x*x-59.35*x+102.3;
        else
            y2=0;
        return(y2*100);
    }

    if(x>=3.669)
        y2=-104.29*x*x+926.07*x-1943.4;
    else if(x>=3.567)
        y2=920.89*x*x-6362.4*x+10997.2;
    else if(x>=3.378)
        y2=78.74*x-265.9;
    else
        y2=0;
    if(y2<0) 
        y2=0;
    else if(y2>100) 
        y2=100;
    return(y2) ;
}
//************************************************************************
//************************************************************************
void SocProcessure(void)
{
    
    static float ahCharge1A=0;
    static float ahDischarge1A=0;
    float ft=0,ca=0;
	  unsigned char i=0;
	  	  	
	  turnOnADC(); //打开ADC中断
	  delay(100);
	  if((Int_Flag&0x01)==0x01)  
        ADC_Processure();
	  delay(100);
	
	  if(g_isrTimes>=750)  // 750ms 检测一次"秒"位是否有变化了
	  {
   	    g_isrTimes = 0;
		
		    DisableInterrupts;
		    I2CReadDate();  //读取系统时间				
		    //实时时间保存
		  
        //for(i=SYS_REALTIME_SECOND;i<SYS_REALTIME_YEAR+1;i++)
        //    g_storageSysVariable[i]= CurrentTime[i-10];          
        EnableInterrupts;
		
		    if(CurrentTime[0]!=g_oldTime[0])   //每秒钟计算一次SOC的值
		    {
				    g_oldTime[0] = CurrentTime[0];//秒
				    isrTimer0count_100ms = 1;
				    //fast
				    //if((g_chargeStateFlag ==RECHARGING)||(g_chargeStateFlag ==FASTRECHARGING))//充电(快或慢)
				    if(g_systemCurrent<=0)// //充电
				    {
					      if(g_leftEnergy<=g_usefulCapacity*3600)//如果容量未满
					      {
						        ft = g_leftEnergy-g_energyOfUsed;
						        g_socValue = ft/(g_usefulCapacity*3600);
						        g_displaySocValue = g_socValue;	
						        					        						        					
					      }					
					      if(g_highestCellVoltage>4.30) // 强制断开高压
				        {					  
					          TurnOff_INZK();//断开正极继电器
					          BmsCtlStat0 &=0xfd;//正极继电器状态断开
					          BmsCtlStat =0;//正极继电器状态断开
					          //下电
		                turnOffSW_Power();//关闭电源开关彻底断电
				        }	
				    }
				    //if(g_chargeStateFlag == DISCHARGING)//放电
				    if(g_systemCurrent>0)// //放电
				    {
					      if(g_leftEnergy>=1.0)
					      {
						        ft = g_leftEnergy-g_energyOfUsed;
						        g_displaySocValue= ft/(g_usefulCapacity*3600);
						        ca=TemperatureAdjustForCap(ft,g_averageTemperature);
						        g_socValue = ca/(g_usefulCapacity*3600);						        
						        				
					      }
					      //if((g_lowestCellVoltage<3.0)&&(error11&0x02==0x02))     // 强制断开高压
					      if(g_lowestCellVoltage<2.5)     // 强制断开高压
				        {					  
					          TurnOff_INZK();//断开正极继电器	
					          BmsCtlStat0 &=0xfd;//正极继电器状态断开
					          BmsCtlStat =0;//正极继电器状态断开
					          //下电
		                turnOffSW_Power();//关闭电源开关彻底断电
				         }	
				     } //end of 放电
						 			
				     if(g_socValue>1.0)	
				     {
					       g_socValue = 1.0;
					       g_leftEnergy = g_socValue*TemperatureAdjustForCap((g_usefulCapacity*3600),g_averageTemperature);
					       g_energyOfUsed =0;
				     }   
			
				     if(0>=g_socValue)
				     {								
					       g_socValue = 0;
				 	       g_leftEnergy = 0;
					       g_energyOfUsed =0;
				     }			
		         StoreSocRealvalue();//每秒保存SOC值
		         			    
        } //end of 每秒钟
				
		    if(CurrentTime[1]!=g_oldTime[1])// 每分钟SOC值赋给系统参数，作为保存或通信
		    {
			      g_oldTime[1] = CurrentTime[1];//分
			      g_sysPara[PARA_SOC_VALUE] = g_socValue;
			      g_leftEnergy=g_leftEnergy-g_energyOfUsed;
			      if(g_systemCurrent<=0)// //充电
			      {
			          //计算总充电电AH数
						    ahCharge1A += g_energyOfUsed/3600;
				        if(ahCharge1A>=1.0) 
				        {
				            sumAhCharge += (unsigned int)ahCharge1A;
				            sumAhChargeOnce += (unsigned int)ahCharge1A;
						        ahCharge1A =0;
				        }		
			      
			      }
			      if(g_systemCurrent>0)// //放电
			      {
			          //计算总放电AH数
				        ahDischarge1A += g_energyOfUsed/3600;
				        if(ahDischarge1A>=1.0) 
				        {
				            sumAhDischarge += (unsigned int)ahDischarge1A;
				            sumAhDischargeOnce += (unsigned int)ahDischarge1A;
						        ahDischarge1A =0;
				        }
			      
			      }
			      
			      g_energyOfUsed =0;
			      //保存状态数据
			      //if((CurrentTime[4]!=0)||(CurrentTime[5]!=0)) // 日期读取正常再存储
			      StoreTotalAH();
		        StoreSysVariable();//每分钟保存SOC值和故障信息		  
		        sendBMSIDtoBMU(); //发送BMS版本ID号给BMU
			      sendRealtimeToBMU(); //发送BMS系统时间给BMU
        }
        
    }//end of 每750ms
    

}
//************************************************************************
//************************************************************************
void ADC_Processure(void)
{
    unsigned char i;
	  float tmp2=0,tmp3=0;
	  float sc=0,sx=0;
	
	  Int_Flag &= 0xfe;//清中断标志
	    
	  tmp2 = ac[0];
		tmp3 = ax[0];			
		for(i=0;i<11;i++)  //取最大值
		{			
				if(tmp2<ac[i+1])
					  tmp2= ac[i+1];	
				if(tmp3<ax[i+1])
					  tmp3= ax[i+1];	
				sc += ac[i];
				sx += ax[i];								
	  }
    sc += ac[11];  //求总和
	  sx += ax[11];
		sc = sc-tmp2; //去掉一个最大值
		sx = sx-tmp3;
			
		tmp2 = ac[0];
		tmp3 = ax[0];			
		for(i=0;i<11;i++) //取最小值
		{			
			  if(tmp2>ac[i+1])
					  tmp2= ac[i+1];	
				if(tmp3>ax[i+1])
					  tmp3= ax[i+1];			
		}
	  sc = sc-tmp2;  //去掉一个最小值
		sx = sx-tmp3;
		//
		sc = sc/10;//取平均值
		sx = sx/10;
		//
		if((sc > 26.0)||(sc<-26.0))//电流大于26A时，用大电流通道 电流互感器型号：DHABs18
		{
			  g_systemCurrent = sc;					
		} 
		else
			  g_systemCurrent = sx;
			
		if(g_systemCurrent<-400)  //防止负数溢出
        g_systemCurrent = 0;			  
}
//************************************************************************************
//************************************************************************************
void ParameterSetting(void)//
{      
    unsigned char i=0;
    unsigned char SetTime[7];  
     
    switch(g_mboxID)//
    {            
        case 0x0c14ee00:
            sendRealtimeToBMU();
            break;
        case 0xC01ee18:
            sendBMSSFVersionToPC();
            break;
        case 0xc01ee14:         //标定系统时间
            //设置时钟
		        for(i=0;i<7;i++)
		            SetTime[i]=*((&CAN2RXDSR0)+i);//秒//分//时//星期//日//月//年
		        I2CWriteDate(SetTime);  //设置初始时间
            break;
        case 0xC01ee15:
            g_sysPara[PARA_SOC_VALUE]= (float)(*((&CAN2RXDSR0)+2))/100;    // SOC
            g_socValue = g_sysPara[PARA_SOC_VALUE];
            g_leftEnergy = g_socValue*(g_usefulCapacity*3600);  //剩余容量
	          g_energyOfUsed = 0;                          //本次开机已用容量	                
	          StoreParameter();
	          //paraChangedFlag = 1;
            break;
        case 0xC01ee16:
            g_sysPara[PARA_WHOLE_CAPACITY]= *((&CAN2RXDSR0)+2);
	          g_usefulCapacity =  g_sysPara[PARA_WHOLE_CAPACITY];//可用容量初始化
	          StoreParameter();
	          //paraChangedFlag = 1;
            break;
        case 0xc01ee17:     //配置BMU个数
            g_sysPara[PARA_BMU_NUMBER]= *((&CAN2RXDSR0)+2);
            switch((unsigned int)g_sysPara[PARA_BMU_NUMBER])//根据BMU的个数，确定相应的配置信息，能够知道是否所有的单体数据已经发了一轮
            {
                case 1:
                  	g_BMU_CIRCLE_FLAG = 0x0000001;
                  	break;
                case 2:
                  	g_BMU_CIRCLE_FLAG = 0x0000011;
                  	break; 
                case 3:
                  	g_BMU_CIRCLE_FLAG = 0x0000111;
                  	break;  
                case 4:
                  	g_BMU_CIRCLE_FLAG = 0x0001111;
                  	break; 
                case 5:
                  	g_BMU_CIRCLE_FLAG = 0x0011111;
                  	break;
                case 6:
                  	g_BMU_CIRCLE_FLAG = 0x0111111;
                  	break;  
                case 7:
                  	g_BMU_CIRCLE_FLAG = 0x1111111;
                  	break;      
                default:
                  	break;
            }
            StoreParameter();
            //paraChangedFlag = 1;
            break;
        case 0xC01EE26://读取系统内部参数
            if(*((&CAN0RXDSR0)+0)==0x66)
                paraReadFlag=1;
            break;            
        case 0xC01EE28:         //接收系统参数修正信息from 上位机
            g_sysPara[PARA_O_WHOLE_VOLTAGE]=  (*((&CAN2RXDSR0)+0)&0x00ff)<<8 | (*((&CAN2RXDSR0)+1)&0x00ff);//总电压过高阈值
            g_sysPara[PARA_L_WHOLE_VOLTAGE]=  (*((&CAN2RXDSR0)+2)&0x00ff)<<8 | (*((&CAN2RXDSR0)+3)&0x00ff);//总电压过低阈值
            g_sysPara[PARA_O_CELL_VOLTAGE] = *((&CAN2RXDSR0)+4)*0.01-1.75;//过压值
            g_sysPara[PARA_L_CELL_VOLTAGE] = *((&CAN2RXDSR0)+5)*0.01-1.75;//欠压值
            g_sysPara[PARA_O_TEMPERATURE] =  *((&CAN2RXDSR0)+6)-40; //过温值
            g_sysPara[PARA_L_TEMPERATURE] =  *((&CAN2RXDSR0)+7)-40; //温度过低值                    
            StoreParameter();
            //paraChangedFlag = 1;
            break;
        case 0xC01EE29:         //接收系统参数修正信息from 上位机
            g_sysPara[PARA_HIGHEST_RECHARGE_CURRENT] =  *((&CAN2RXDSR0)+0);//最大允许充电电流值  
            g_sysPara[PARA_O_SOC_VALUE] =  *((&CAN2RXDSR0)+4); //SOC过高
            g_sysPara[PARA_L_SOC_VALUE] =  *((&CAN2RXDSR0)+5); //SOC过低  
            g_sysPara[PARA_L_ISOLATION_RESISTANCE] =  *((&CAN2RXDSR0)+6); //绝缘电阻过低值
            g_sysPara[PARA_O_CONSISTENCY_V] =  *((&CAN2RXDSR0)+7);//电压一致性差 
            StoreParameter();
            //paraChangedFlag = 1;
            break;
        case 0xC01EE2a:         //接收系统参数修正信息from 上位机
            g_sysPara[PARA_BALANCE_ON_VOLTAGE] = *((&CAN2RXDSR0)+0)*0.01-1.75;//均衡开启电压
            g_sysPara[PARA_BALANCE_OFF_VOLTAGE] = *((&CAN2RXDSR0)+1)*0.01-1.75;//均衡截止电压
            g_sysPara[PARA_DIFFERENCE_SINGLE_V] =  *((&CAN2RXDSR0)+2);//均衡压差
            StoreParameter();
            //paraChangedFlag = 1;
            break;
        case 0xC01EE2b:         //接收系统参数修正信息from 上位机
            break;                      
        default:
            break;                  
      }//end of switch      

}
//**********************************************************************
//**********************************************************************
void FastRecharge_Processure(void)
{
 

}
//**********************************************************************
//**********************************************************************
void Recharge_Processure(void)
{
    
}
//************************************************************************
//************************************************************************
void VehiclePowerOnLogic(void)
{
    static unsigned char times=0;
    static unsigned char delaycounter =0;
    static unsigned char PowerOnFlag=0;
    static unsigned char closedPrecharge=0;
    static unsigned char openPrecharge=0;
    static unsigned char closedZKrelay=0; 
    static unsigned char closedFKrelay=0;  
      
    if((RelayReq == 0x03)||(RelayReq == 0x02)) //若已经处于保持当前状态或者下电状态，则返回
        return;
      
    //等待212命令
    if(times<=3) 
        RelayReq=0;     //系统开始自检，发送 00 给 VCU     id=452   //先发3次0x00  表示自检                                                                      
    else 
    {      
        //RelayReq = 0x01;//请求高压上电 发0x01
        RelayReq = 0x00;//请求高压上电 发0x01
        RelayReq0 = 0x02;// for PC
    }
      
    times++;
    if(times>100)
        times=100; //确保值不会过大
    if((Vcu2bms_212_byte0==0x01)&&(PowerOnFlag==0)) //收到高压上电指令
    {                        	
    	  Vcu2bms_212_byte0=0;
    	  PowerOnFlag =1;
    	  RelayReq = 0x01;//请求高压上电 发0x01    	    
    	  TurnOn_INFK();//闭合总负      
        BmsCtlStat =1;//负极继电器状态闭合 
        BmsCtlStat0 |=0x02;// 
        closedFKrelay =1;          
    } 
    if(closedFKrelay)
    {
        delaycounter++;
        if(delaycounter>=15) //5*20ms=100ms  
        {
            delaycounter =0; 
            closedFKrelay =0;   	    
    	      TurnOn_INBK();//闭合预充电继电器
    	      PrechargeRelayStat =1;//预充电继电器状态置位
    	      closedPrecharge =1;
        }
    }
    if(closedPrecharge)  //如果发了闭合继电器的命令，那么开始计时。
    {        
        delaycounter++;
    	  if(delaycounter>=15) //5*20ms=100ms
    	  {    	             	    
            //g_highVoltageV2 = 250;
        	  if((g_highVoltageV1-g_highVoltageV2)>30) //92cell*2.7V=248.4  //若动力电池电压达到要求
        	  {
        	      if(delaycounter>=100)  //如果2秒内电压还达不到要求；汇报故障
        	      {
        	          delaycounter =0;
        	          TurnOff_INBK();//断开预充电继电器
                    PrechargeRelayStat =0;//预充电继电器状态
                    TurnOff_INFK();//断开负极接触器
                    BmsCtlStat &=0x0;//负极继电器状态断开
                    BmsCtlStat0 &=0xfd;//负极继电器状态断开
                    closedPrecharge =0;
                    return;        	                   	       
        	      }        	   
        	  } 
        	  else 
        	  {        	    
        	       delaycounter =0;
        	       closedPrecharge =0;
        	       TurnOn_INZK();//吸合正极接触器//闭合总正 
        	       BmsCtlStat |=1;//正极继电器状态闭合
        	       BmsCtlStat0 |=0x02;//正极继电器状态闭合 
        	       closedZKrelay =1;    
        	   }
    	   }
    }
    if(closedZKrelay)//如果已经闭合了主正 
    {
        delaycounter++;
        if(delaycounter>=15) //5*20ms=100ms
        {
            delaycounter = 0;
            closedZKrelay =0;
            TurnOff_INBK();//断开预充电继电器
            PrechargeRelayStat =0;//预充电继电器状态
            openPrecharge=1;
        }  
        	  
    }  
    if(openPrecharge) 
    {
        delaycounter++;
    	  if(delaycounter>=15) //5*20ms=100ms
    	  {
    	      //g_highVoltageV2 = 230;
    	      if((g_highVoltageV1-g_highVoltageV2)>30) 
    	      {
        	      if(delaycounter>=100)  //如果2秒内电压还达不到要求；汇报故障
        	      {
        	          TurnOff_INZK();//断开总正电继电器
                    BmsCtlStat =0;//正极继电器状态断开
                    BmsCtlStat0 &=0xfe;//正极继电器状态断开                         
                    TurnOff_INFK();//断开负极接触器
                    BmsCtlStat &=0x0;//负极继电器状态断开
                    BmsCtlStat0 &=0xfd;//负极继电器状态断开                         
                    openPrecharge = 0;
                    delaycounter =0;
                        return;        	                   	       
        	      }    	         
    	      } 
    	      else 
    	      {    	            
        	      delaycounter = 0;
        	      openPrecharge = 0;
        	      PrechargeStat =1;
        	      RelayReq = 0x03;//保持当前状态 
        	      RelayReq0 = 0x03;//保持当前状态 
    	      }
    	  }//end of 100ms
    }
    	        
    /*
    if(inputP_state())  //延迟400ms后检查继电器闭合状态   
    {
        bmsToVcuInfo450();  //发送BMS故障代码
    }	 
    else 
    {    	      
        BmsCtlStat =1;//正极继电器状态闭合
        BmsCtlStat0 |=0x02;//正极继电器状态闭合 
        RelayReq = 0x03;//保持当前状态  
    }
    */      
} 
//************************************************************************
//************************************************************************
void VehiclePowerOffLogic(void) 
{
    if((Vcu2bms_212_byte0 == 0x02)&&(RelayReq==0x03)&&(!g_endPowerOffFlag)) //高压下电
    {
        Vcu2bms_212_byte0 =0;         
        TurnOff_INZK();//断开正极接触器  
        BmsCtlStat =0;//正极继电器状态断开
        BmsCtlStat0 &=0xfe;//正极继电器状态断开
         
        delay(1000);
        TurnOff_INFK();//断开负极接触器
        BmsCtlStat &=0x0;//负极继电器状态断开
        BmsCtlStat0 &=0xfd;//负极继电器状态断开
                 
        RelayReq =0x03;//正负继电器已经断开
        //bmsToVcuInfo450(); //发送继电器状态，完成，BMS保持当前状态
        g_endPowerOffFlag=1;
         
        //下电
		    turnOffSW_Power();//关闭电源开关彻底断电
      
    }

}
void HighVoltDetectPart1(void) 
{
    if(g_highVoltageV1==0) //检测MSD   g_highVoltageV1=V1 ;   g_highVoltageV2=V2
    {      
        msdFault =1;
        return;
    }
    if(g_highVoltageV2 >= g_highVoltageV1*0.5) //判断负极继电器粘连
    {
        relayNconnected =1;
        return;
    }
        
}
void HighVoltDetectPart2(void) 
{    
    unsigned char flag1,flag2;
    
    if(tmr_p2>=10 && tmr_p2<=80)
        if(g_highVoltageV2 <= g_highVoltageV1*0.4)
            flag1=1;
    if(tmr_p2 >= 120&& tmr_p2 <= 150)  
        if(g_highVoltageV2 >= g_highVoltageV1*0.8)
            flag2=1;       
    if((flag1+flag2) == 2) 
    {      
        relayPreState =2; //预充继电器粘连
        return;
    }
    
    if(tmr_p2 >= 150) 
    {      
        if(g_highVoltageV2 <= g_highVoltageV1*0.2) 
        {
            relayNdisconnected =1; //负极继电器断开故障
        } 
        else
            relayNdisconnected =0; //负极继电器无故障
        
        if((g_highVoltageV2 >= g_highVoltageV1*0.4) && (g_highVoltageV2 <= g_highVoltageV1*0.8))
            relayPreResState =1;  //预充电电阻断开故障
    }
}
void HighVoltDetectPart3(void) 
{      
    static preChargeON =0;
    static tt=0;
    
    if(!preChargeON) 
    {
        preChargeON=1;
        TurnOn_INBK();//闭合预充电继电器 
        delaysss(25000); //20ms
        PrechargeRelayStat =1;//预充电继电器状态置位
    }
    while(tmr_p3<=20); 
    while((tmr_p3 >20)&&(tmr_p3<40))
        if(g_highVoltageV2 <= g_highVoltageV1*0.8 ) 
        {            
            tt++;
              if(tt==10) 
              {  
                   relayPreState =0; 
                   tt=11;
                   break;
              }
                                 
        } 
        else 
        {
            relayPreState =1;    //预充电继电器断开故障                         
        }
        
    if(relayPreState) 
    {
         TurnOff_INBK();//断开预充电继电器
         delay(25000); //20ms
         PrechargeRelayStat =0; //清除预充电标志位
         prechargeFinished =0;
    }

     delaysss(65000); //20ms
     delaysss(65000); //20ms
    //if((tmr_p3>80)&&(tmr_p3<=280)&&(!relayPreState))  
     if((tmr_p3>80)&&(tmr_p3<=380))
         if(relayPreState==0) 
            if(g_highVoltageV2 >= g_highVoltageV1*0.8) 
            {
                TurnOn_INZK();//闭合主正继电器
                delay(25000); //20ms
                TurnOff_INBK();//断开预充电继电器
                BmsCtlStat =1;//正极继电器状态闭合标志位
                BmsCtlStat0 |=0x02;//正极继电器状态闭合    to pc 
                delay(25000); //20ms 
                PrechargeRelayStat =0; //清除预充电标志位
                prechargeFinished =1;
                tmr_p3 =381; //只运行一次
            }
 
    if(tmr_p3>380)
        if(g_highVoltageV2 <= g_highVoltageV1*0.8) 
        {
            TurnOff_INBK();//断开预充电继电器
            PrechargeRelayStat =0; //清除预充电标志位
            prechargeFinished =0;
        }
   
}

//************************************************************************
//************************************************************************
//************************************************************************
//************************************************************************
/*-------------------------------------------------------
---------------------------------------------------------*/





