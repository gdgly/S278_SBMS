
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//---------------------- Pride Power------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//* Project Name       : S133
//* File Name          : Feedback.c
//* Author             : judy
//* Version            : V1.0.0
//* Start Date         : 2016.2.14
//* Description        : 该文件用于查表计算回馈电流当前值
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
#include "BMS20.h"
#include"FeedbackTable.h"
//*************************************************************************
//******************************************************************************
//* Function name:   linearizationRechargePower
//* Description:     最大允许回馈功率表，计算出当前的最大允许回馈电流和允许回馈的时间S
//* EntryParameter : 温度,SOC值
//* ReturnValue    : 最大允许脉冲回充电流
//******************************************************************************
float linearizationRechargePower(char tem,float soc) 
{
    float curr=0;
    unsigned char second=0;
    
    if((tem<=0)||(tem>=55)||(soc>=0.95)) 
    {
        curr=0;
        return curr;
    }
    
    if( (tem>0)&&(tem<=10))//(0,10] 
    {
        if(soc>0.8)
            curr =0;
        else if(soc>0.5) 
        {
            curr=120;
            second=5;
        } 
        else 
        {
            curr=120;
            second=10;
        }   
    }
    
    if( (tem>10)&&(tem<=20))//(10,20]
    {
        if(soc>0.8) 
        {
            curr =120;
            second=5;
        }
        else if(soc>0.5) 
        {
            curr=120;
            second=10;
        } 
        else 
        {
            curr=180;
            second=10;
        }
    }
    
    if( (tem>20)&&(tem<55)) //(20,55) 
    {
        if(soc>0.8) 
        {
            curr =120;
            second=10;
        }
        else if(soc>0.5) 
        {
            curr=180;
            second=10;
        } 
        else 
        {
            curr=240;
            second=10;
        }
    }
    
    pusleLimitSec = second;
    return curr; 
}

//******************************************************************************
//* Function name:   PulseRechargePowerAdjust
//* Description:     根据平均温度调整最大允许再生脉冲充电电流  给VCU  即制动能量回收
//* EntryParameter : tem, soc 平均温度，SOC值
//* ReturnValue    : fc 返回的回馈电流值  
//******************************************************************************
float PulseRechargePowerAdjust(char tem,float soc) 
{   
    static float current1=0; //回馈电流暂存值1
    static float current2=0; //回馈电流暂存值2;
    static unsigned int leftTime=0;
    static float fc=0;//返回的回馈电流值
    float BigCharCurrent = 0; 
    static unsigned char FirstTime=0;
    static unsigned char changeFlag=0;
    static unsigned int ii=0;
    static unsigned int jj=0;
    static unsigned char TimeoutFlag1=0;
    static unsigned char TimeoutFlag2=0;
    static unsigned char CloseFlag=0;
    
    //if(CloseFlag == 0)  // 
    //{ 
        BigCharCurrent = linearizationRechargePower(tem-40,soc);//第一次进入

        if(FirstTime==0) //第一次进入
        {
            fc=BigCharCurrent; 
            current1 = BigCharCurrent; 
            FirstTime=1;         
        }
        else//再次进入
        {
            if(changeFlag==0)//原值为0，所以current2为第二次进入该函数时的值
                {
                    current2=BigCharCurrent;
              
                }
            if(current1!=current2) //如果两者不等，说明第一与第二次不等
            {
                changeFlag=1;//设置变化标志位
                if(current1>current2) 
                {
                    fc=current1-0.16;// 每20ms变化0.5kw,7ms调用一次，21ms降低0.51KW
                    current1=fc;
                    if(current1<=current2)
                    {
                        
                        fc=current2;
                        changeFlag=0; 
                    }
                }
                else if(current1<current2) 
                {
                    fc=current1+0.16;// 每5ms变化0.05kw
                    current1=fc;
                    if(current1>=current2) 
                    {
                        
                        fc=current2;
                        changeFlag=0;
                    }
                }
            } 
            else 
            {
                changeFlag=0; 
            }
        
        } 
    
    if(-g_systemCurrent>BigCharCurrent)//如果实际电流大于给的最大反馈电流，则反馈电流设置为0
        return 0;  
    //}

 /*先放着  
  //if((-g_systemCurrent>=BigCharCurrent)&&(TimeoutFlag==0))
   if((-g_systemCurrent>=5)&&(TimeoutFlag1==0))//若是出现反馈电流
   {
      ii++;
      leftTime = pusleLimitSec*1000-7*ii;
      if(leftTime<=7)//若限制时间到了，则反馈电流设置为0
      {
          ii = 0;
          TimeoutFlag1 = 1;
      } 
   }
   if((TimeoutFlag1 == 1)&&(TimeoutFlag2 == 0))//如果限制时间到了，准备给反馈电流3秒 0值
   {
      CloseFlag = 1;
      fc = 0;
      jj++;
      if(jj>=400)//给0持续的时间为3
      {   
          TimeoutFlag2 = 1;
          jj = 0;
          CloseFlag = 0;//3秒后放开算反馈电流函数
      } 
   }
   
 */  
   return fc; 
}


/* 先一直给，没有时间限制，实车测试完后再定   
    if((-g_systemCurrent<=FBpower2)&&SecondOn == 0)
        {
            jj++;
            leftTime = pusleLimitSec*1000-7*jj;
            if(leftTime<=7)//若限制时间到了，则准备将电流逐步降低到0
            {
                jj = 0;
                SecondOn = 1;
            }
        } 
  
    if(SecondOn == 1)
        {
            ThirdOn = 1;
            fc = 0;
            ii++;
            if(ii>=400)//给0持续的时间为3
            {   
                ii = 0;    
                ThirdOn = 0;
                SecondOn = 0;
            }
        }
  */

//**************************************************************************************
//**************************************************************************************
//*****************************the end*****************************************************
//*****************************************************************************************
//***************************************************************************************