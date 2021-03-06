//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//---------------------- Pride Power------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//* Project Name       : s133
//* File Name          : current.c
//* Author             : Judy
//* Version            : V1.0.0
//* Start Date         : 2011,05,26
//* Description        : 根据ADC得到的值，计算出系统电流
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
#include "derivative.h" /* include peripheral declarations */
#include "BMS20.h" 

float g_systemCurrent;
float g_systemCurrent_2;
//******************************************************************************
//* Function name:   CurrentCalculation
//* Description:     将AD值转换成电流值
//* EntryParameter : None
//* ReturnValue    : None
//******************************************************************************
void CurrentCalculation(float *current1, float *current2)
{
    unsigned char i;
    float tmp2=0,tmp3=0;
    float sc=0,sx=0;
//    float current=0;

    //if((Int_Flag&0x01)==0x01) //采了32次AD值后才计算
    {
        Int_Flag &= 0xfe;//清中断标志
        PITINTE  &=0xfe; ///disable the PIT Ch0 interrupt 

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
        
        sc = sc/10;//取平均值
        sx = sx/10;
        
        *current1 = sc;
        if((*current1 >= -1) && (*current1 <= 1)){
            *current1 = 0;
        }
        
        if(*current1 >= 400.0){
            *current1 = 400.0;
        }
        else if(*current1 <= -400.0){
            *current1 = -400.0;
        }
        
        *current2 = sx;
        if((*current2 >= -1) && (*current2 <= 1)){
            *current2 = 0;
        }
        
        if(*current2 >= 400.0){
            *current2 = 400.0;
        }
        else if(*current2 <= -400.0){
            *current2 = -400.0;
        }
        
        /*
        if((sc <= 26.0)&&(sc>=-26.0))//电流-26A~26A时，电流通道 电流互感器型号：DHABs18
        {
            current =sx;   	          
        }
        else 
            current = sc;//因为霍尔传感器接反了 所以加负号 
    	          	  
        if((current>=-1)&&(current<=1)) //若接霍尔，但没电流输入，可能检出的电流在此范围内，视电流=0
            current=0;
        */
        PITINTE  |=0x01; ///Enable the PIT Ch0 interrupt
    }
    //return current;
}

//**********************************************
//*************************************************************************
//***************************************************************************
//******************************************************************************