
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//---------------------- Pride Power------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//* Project Name       : S133
//* File Name          : ProcessBMU.h
//* Author             : Judy
//* Version            : V1.0.0
//* Start Date         : 2015.7.13
//* Description        : 该文件是BMU处理函数头文件
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

#define SIX802_NUMBER 5    // 每个BMU对应的6802个数
#define CELL_NUMBER 12   //每个6802对应的单体个数
#define Tem_NUMBER 2   //每个6802对应的温度个数
#define SLAVE_NUMBER_BEIQI 8  //北汽要求的从板个数
///////////////////////////////////////////////////////////////////////////////////////////
//************ ProcessBMU.c ****************************************
extern unsigned char g_group;  //BMU1 组号， 从0~15
extern unsigned int g_singleCellVoltage[BMU_NUMBER][SIX802_NUMBER][CELL_NUMBER];  //BMU1号, 6802（组）号, 单体电池号
extern unsigned char g_singleCellTemperature[BMU_NUMBER][SIX802_NUMBER][Tem_NUMBER];// 2,5,2;
extern float g_highestCellVoltage;
extern float g_lowestCellVoltage;
extern float g_averageVoltage; //平均单体电压
extern float g_systemVoltage;      //系统电压=单体电压累加总电压
extern unsigned char g_highestTemperature;
extern unsigned char g_lowestTemperature;
extern unsigned char g_averageTemperature;
extern unsigned char Tavg;

extern unsigned char g_bmu2_number_v[BMU_NUMBER];
extern unsigned char g_cell_number_v[BMU_NUMBER][5];
extern unsigned char g_bmu2_number_t[BMU_NUMBER];
extern unsigned char g_cell_number_t[BMU_NUMBER][5];

extern unsigned char g_cellVol[CELL_VOL_GROUP][6];
extern unsigned char g_cellTemperature[CELL_TEMP_GROUP][6];

extern unsigned long g_circleFlag; //一个循环完成的标志
extern unsigned long g_configFlag;//收到配置信息标志，用于判断BMU个数，6805组数和单体个数

extern unsigned char LowVolNum;
extern unsigned char HighVolNum;
extern unsigned char HighTemNum;
extern unsigned char LowTemNum;

extern unsigned char highestCellVolNum;
extern unsigned char lowestCellVolNum;
extern unsigned char highestCellTempNum;
extern unsigned char lowestCellTempNum;

extern void BMU_initial(void);
extern void BMU_Processure(void);
extern unsigned char bmuProcess2(void);
//************************************************************************
//************************************************************************
//*************************the end****************************************
//************************************************************************
//************************************************************************