 
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//---------------------- Pride Power------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//* Project Name       : C50ES_FP
//* File Name          : Charging.h
//* Author             : Judy
//* Version            : V1.0.0
//* Start Date         : 2015.7.13
//* Description        : 该文件是对CAN发送的报文进行BIT定义
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//**************Charging.h**************************************************************
extern unsigned char slowRechargeFinished;
extern float	m_askvoltage;
extern float	m_askcurrent;
extern float  big_askcurrent;
extern float HeatCurt;
extern float	m_allowmaxvoltage;
extern unsigned char  fastend1;
extern unsigned char  fastend2;
extern unsigned char  fastend3;
extern unsigned char  fastend4;
extern unsigned char  ACEndState;//慢充停止标志位
extern unsigned char fastendflag;
extern unsigned char  BMSToVCU655_1;    //全局
extern unsigned char  BMSToVCU655_2;    //全局
extern unsigned char  BMSToVCU655_3;    //全局
extern unsigned char SelfState2;     //自检计数器变成2标志
extern unsigned char SelfState3;     //自检计数器变成3标志
extern unsigned char  fasterror11;
extern unsigned char  fasterror12;
extern unsigned int  chargeTimes;
extern unsigned int chargeRemainderTime;
extern unsigned char	m_chmmode;
extern unsigned char CCSOverTime;//快充超时判断
extern unsigned char CSTOverTime;
extern unsigned char CSDOverTime; //CSD超时时间
extern unsigned char OverTimeState;

extern float ChgeAmpReq;                //交流充电电流值全局
extern unsigned char  ReqCtlToCHG;      //动力电池充电请求：0待机；2恒流
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
extern float GetRequestCurrentDC(void);
extern void TaskRechargeDC(void);
extern float GetRequestCurrentAC(void);
extern void TaskRechargeAC(void);
//***************************************************************************
//****************************************************************************
//**************************************************************************
//*************************************************************************