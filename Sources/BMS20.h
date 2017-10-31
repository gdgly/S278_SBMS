//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//---------------------- Pride Power------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//* Project Name       : C50ES_FP
//* File Name          : BMS.h
//* Author             : Judy
//* Version            : V1.0.0
//* Start Date         : 2011,05,26
//* Description        : 该文件作为系统头文件，对该项目的系统参数进行定义和对所有全局变量进行全局声明
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------每个项目必须要修改的参数---------------------------------------------------------
//----------------------------------------------------------------------------------------------------

///////////////////////////////////系统参数//////////////////////////////////
#define StoreAHState 0            //定义是否可以存储
#define BMU_NUMBER 5              // BMU个数
#define G_BMU_CIRCLE_FLAG 0x11111    //BMU个数
#define CELL_NUM       180         //电池节数
#define CELL_TEMP_NUM  30     //温度采集点个数
#define CELL_VOL_GROUP  60     //单体电压分组数/5个C箱分为180/3=60
#define CELL_TEMP_GROUP  5     //单体温度分组数/5个C箱分为30/6=5
#define C              240        //充电倍率
#define DubugMode      1          //发送状态机报文

//////////////////////////////////直流快充//////////////////////////////////
#define HIGHEST_ALLOWED_CHARGE_CV 3.65    //最大允许充电单体电压 V
#define HIGHEST_ALLOWED_CHARGE_V  657     //最大允许充电总电压 V   3.65*180=657
#define LOWEST_ALLOWED_CHARGE_V  594     //最低允许充电总电压 V   3.3*180=594

#define HIGHEST_ALLOWED_CHARGE_T 54       //最大允许充电和工作温度   54
#define LOWEST_ALLOWED_CHARGE_T 0       //最低允许充电温度   0
#define LOWEST_ALLOWED_WORK_T -20       //最低允许工作温度   -20

#define CHARGE_CUTDOWN_CV1 3.55           //第一次降流电压
#define CHARGE_CUTDOWN_CV2 3.6           //第二次降流电压
#define HIGHEST_ALLOWED_CHARGE_A  240.0   //直流最大允许充电电流 A
#define SYS_NOMINAL_AH 240                //系统额定容量 AH
#define SYS_NOMINAL_V  576                //系统额定电压 V//3.2*180= 576
#define SYS_KWH        138.2               //系统能量  //240*576=138240WH
#define LOWEST_ALLOWED_DISCHARGE_V  450   //最小允许充电总电压 V   2.5*180=450
#define CONSTANT_VOL_WORK 1               //恒压工作
#define CONSTANT_CURT_WORK 2              //恒流工作
#define CELL_TYPE 3                       //01铅酸电池;02镍氢电池;03:磷酸铁锂;04:锰酸铁锂;
                                          //05:钴酸锂;06:三元;07:聚合物;08:钛酸锂;FFH:其他
                                          
#define HIGHEST_VOL 6480 //CML充电机规格:最大输出能力  3.6*180*10
#define LOWEST_VOL 5760 //CML充电机规格:最小输出能力   3.2*186*10

#define CC2VOLHIGH 6.2  //CC2测量范围高
#define CC2VOLLOW  4.2  //CC2测量范围低
////////////////////////加热参数配置//////////////////////////////////////////////
#define HIGHEST_ALLOWED_HEAT_CHARGE_V 594   //最大允许加热充电总电压 V   3.3*180=594
#define HIGHEST_ALLOWED_HEAT_CHARGE_C 3     //最大允许加热充电电流 A
#define START_ALLOWED_PreHEAT_CHARGE_TEM -15   //开启预加热阀值
#define STOP_ALLOWED_PreHEAT_CHARGE_TEM -10    //关闭预加热阀值  
#define START_ALLOWED_HEAT_CHARGE_TEM 5        //开启加热阀值
#define STOP_ALLOWED_HEAT_CHARGE_TEM  10       //关闭加热阀值 


///////////////////////BMS最后保护的阀值///////////////////////
#define LOWEST_CELL_VOL  2.0       //放电最小单体电压
#define HIGHEST_CELL_VOL 3.85     //充电单体最大单体电压
#define HIGHEST_BATT_VOL 657       //最大总电压3.65*180 = 657
#define HIGHEST_TEM 55             //最高温度50

//////////////////////BMS功率变化速度//////////////////////////
#define POWERSPEED  0.5       //功率的变化速度

//////////////////////高压等级:BMS绝缘计算采样电阻值//////////////////////////
#define RESVALUE  0.2       //采样电阻为0.2K

//////////////////////BMS标定高压时的配置电压//////////////////////////
#define HIGHVOL_19    350       //发送0x19时对应的电压
#define HIGHVOL_23    550      //发送0x23时对应的电压

#include <hidef.h>      /* common defines and macros */
#include <string.h>
#include "derivative.h"      /* derivative-specific definitions */
//底层头文件
#include  "mc9s12gpio.h"
#include  "74hc595.h"
#include  "adc.h"
#include  "bmn.h"
#include  "mscan.h"
#include  "RTI.h"
#include  "sd2405.h"
#include  "dflash.h"
#include  "ProcessBMU.h"
#include  "current.h"
#include  "SocAHintegral.h"
#include  "Record.h"
#include  "Storage.h"
#include  "M95M02.h"
#include  "Connect.h"
//应用层头文件
#include  "Machine.h"
#include  "VehicleCAN.h"
#include  "InnerCAN.h"
#include  "FastChargeCAN.h"
#include  "SOF.h"
#include  "feedBack.h"
#include  "SocAdjust.h"
#include  "Heat.h"
#include  "MachineControl.h"
//#include "BatteryOverVoltage1_types.h"
#include "Fault.h"
#include "24LC64.h"
#include "InitBMS.h"
#include "mc9s12Isr.h"
#include "ParameterSetting.h"
#include "Q1Q2calculate.h"
#include "Task.h"
#include "DC_ChangerProcess.h"
#include "AC_ChangerProcess.h"
#include "ads1015.h"
#include "lookup_Table_Tem.h"
//#include "SOF_20161129_qingnian.h"
//#include "SOFsustain_20161129_qingnian.h"
////#include "feedback_20161129_qingnian.h"
//#include "feedbacksustain_20161129_qingnian.h"
#include "Q1.h"

//#include "Isolation.h"
//*******************************************
//*******************************************
#define  uchar unsigned char
#define  uint  unsigned int
#define  ulong unsigned long
#define  HIGH 0
#define  LOW 1

// 任务结构
typedef struct _TASK_COMPONENTS
{
    uchar Run;                 // 程序运行标记：0-不运行，1运行
    uint Timer;                // 计时器
    uint ItvTime;              // 任务运行间隔时间
    void (*TaskHook)(void);    // 要运行的任务函数
} TASK_COMPONENTS;       // 任务定义
/////////
//************************************************************************
//************************************************************************
// 任务清单
typedef enum _TASK_LIST
{
    TASK_CURRENT,
    //TASK_SOC,
    //TASK_BMU,            
    TASK_VOLTAGE,             
    TASK_INSULATION,             
    //TASK_REPORT2PC,
    //TASK_STAT_MACHINE,
    //TASK_FAULT,
    //TASK_RECORD,
    //TASK_DC_RECHARGE,
    //GetTavgProcess()
    TASK_T_CTRL_PROCESS,
    Task_KchgN_CTRL_PROCESS,
    TASKS_MAX      // 总的可供分配的定时任务数目                                     
} TASK_LIST;
////////
enum slaveNumber
{
    S0,
    S1,
    S2,
    S3,
    S4,
    S5,
    S6,
    S7
};
//*******************************************
//*******************************************
enum parameter_list
{
    PARA_SOC_VALUE,    //SOC实际值
    //PARA_DISCHARGE_RATIO,//过电流
    PARA_POSRELAY_PSTATE,//总正继电器粘连故障  relayPstate
    //PARA_SINGLE_CAPACITY,  //单体电池容量AH
    PARA_PRECHARGE_RELAY_PSTATE,//预充继电器粘连故障
    PARA_WHOLE_CAPACITY,//电池组总容量
    PARA_BMU_NUMBER,//  电池数量

    PARA_DIFFERENCE_SINGLE_V, // 单体电压均衡值
    PARA_DIFFERENCE_SINGLE_T, // 电池温度均衡值
    PARA_BALANCE_ON_VOLTAGE, //均衡开启电压值
    PARA_BALANCE_OFF_VOLTAGE,//均衡关闭电压值
    PARA_O_SOC_VALUE,// soc高

    PARA_OO_SOC_VALUE,// soc过高
    PARA_L_SOC_VALUE,//soc低
    PARA_LL_SOC_VALUE,//soc过低
    PARA_O_WHOLE_VOLTAGE, // 总电压值高
    PARA_OO_WHOLE_VOLTAGE, // 总电压值极高

    PARA_L_WHOLE_VOLTAGE, // 总电压值低
    PARA_LL_WHOLE_VOLTAGE, // 总电压值极低
    PARA_O_CELL_VOLTAGE, // 单体电压高（过压值）
    PARA_OO_CELL_VOLTAGE, // 单体电压极高
    PARA_L_CELL_VOLTAGE,//  单体电压低(欠压值)

    PARA_LL_CELL_VOLTAGE,//  单体电压极低
    PARA_O_TEMPERATURE,//过温
    PARA_OO_TEMPERATURE,//温度极高
    PARA_L_TEMPERATURE, // 温度低
    PARA_LL_TEMPERATURE, // 温度极低

    PARA_O_CONSISTENCY_V, //电压一致性差
    PARA_OO_CONSISTENCY_V,//电压一致性极差
    PARA_O_CONSISTENCY_T, //温度一致性差
    PARA_OO_CONSISTENCY_T,//温度一致性极差
    PARA_CURRENT,//   过电流值

    PARA_HIGHEST_RECHARGE_VOLTAGE,//允许最高充电端电压
    PARA_HIGHEST_RECHARGE_CURRENT,//最大允许充电电流 
    PARA_HIGHEST_RECHARGE_CURRENT_TIME,//允许最大充电电流时间
    PARA_HIGHEST_DISCHARGE_CURRENT,//最大允许放电电流 
    PARA_HIGHEST_CHARGE_CURRENT_TIME,//允许放电电流最大电流时间

    PARA_L_ISOLATION_RESISTANCE, //  绝缘电阻低值
    PARA_LL_ISOLATION_RESISTANCE, //  绝缘电阻过低值

    PARA_ENDFLAG
};
enum storage_list 
{
    INDEX,
    TOTAL_VOLTAGE,
    TOTAL_CURRENT,
    PARA_SOC_DISPLAY, // SOC值
    SYS_CONTACTORS_STATE,  //总正总负继电器状态

    PARA_ERROR_LEVER,//故障等级值  1:1级故障  2:2级故障
    CAUTION_FLAG_1,
    CAUTION_FLAG_2,
    CAUTION_FLAG_3,
    CAUTION_FLAG_4,

    SYS_REALTIME_SECOND,
    SYS_REALTIME_MINUTE,
    SYS_REALTIME_HOUR,
    SYS_REALTIME_WEEK,
    SYS_REALTIME_DAY,

    SYS_REALTIME_MONTH,
    SYS_REALTIME_YEAR,
    CELL_MAX_TEM,
    CELL_MIN_TEM,
    CELL_AVERAGE_TEM,

    CELL_MAX_VOLTAGE,
    CELL_MIN_VOLTAGE,
    CELL_AVERAGE_VOLTAGE,
    SYS_INSULATION_P,
    SYS_INSULATION_N,
    
    VOLT_K1,
    VOLT_B1,
    VOLT_K2,
    VOLT_B2,
    VOLT_K3,
    
    VOLT_B3,
    VERIFICATION  //校验
};

enum storage_cell_vol_list 
{  
    CELL_VOLTAGE_0,
    CELL_VOLTAGE_1,
    CELL_VOLTAGE_2,
    CELL_VOLTAGE_3,
    CELL_VOLTAGE_4,
    CELL_VOLTAGE_5,
    CELL_VOLTAGE_6,
    CELL_VOLTAGE_7,
    CELL_VOLTAGE_8,
    CELL_VOLTAGE_9,

    CELL_VOLTAGE_10,
    CELL_VOLTAGE_11,
    CELL_VOLTAGE_12,
    CELL_VOLTAGE_13,
    CELL_VOLTAGE_14,
    CELL_VOLTAGE_15,
    CELL_VOLTAGE_16,
    CELL_VOLTAGE_17,
    CELL_VOLTAGE_18,
    CELL_VOLTAGE_19,

    CELL_VOLTAGE_20,
    CELL_VOLTAGE_21,
    CELL_VOLTAGE_22,
    CELL_VOLTAGE_23,
    CELL_VOLTAGE_24,
    CELL_VOLTAGE_25,
    CELL_VOLTAGE_26,
    CELL_VOLTAGE_27,
    CELL_VOLTAGE_28,
    CELL_VOLTAGE_29,

    CELL_VOLTAGE_30,
    CELL_VOLTAGE_31,
    CELL_VOLTAGE_32,
    CELL_VOLTAGE_33,
    CELL_VOLTAGE_34,
    CELL_VOLTAGE_35,
    CELL_VOLTAGE_36,
    CELL_VOLTAGE_37,
    CELL_VOLTAGE_38,
    CELL_VOLTAGE_39,

    CELL_VOLTAGE_40,
    CELL_VOLTAGE_41,
    CELL_VOLTAGE_42,
    CELL_VOLTAGE_43,
    CELL_VOLTAGE_44,
    CELL_VOLTAGE_45,
    CELL_VOLTAGE_46,
    CELL_VOLTAGE_47,
    CELL_VOLTAGE_48,
    CELL_VOLTAGE_49,

    CELL_VOLTAGE_50,
    CELL_VOLTAGE_51,
    CELL_VOLTAGE_52,
    CELL_VOLTAGE_53,
    CELL_VOLTAGE_54,
    CELL_VOLTAGE_55,
    CELL_VOLTAGE_56,
    CELL_VOLTAGE_57,
    CELL_VOLTAGE_58,
    CELL_VOLTAGE_59,

    CELL_VOLTAGE_60,
    CELL_VOLTAGE_61,
    CELL_VOLTAGE_62,
    CELL_VOLTAGE_63,
    CELL_VOLTAGE_64,
    CELL_VOLTAGE_65,
    CELL_VOLTAGE_66,
    CELL_VOLTAGE_67,
    CELL_VOLTAGE_68,
    CELL_VOLTAGE_69,

    CELL_VOLTAGE_70,
    CELL_VOLTAGE_71,
    CELL_VOLTAGE_72,
    CELL_VOLTAGE_73,
    CELL_VOLTAGE_74,
    CELL_VOLTAGE_75,
    CELL_VOLTAGE_76,
    CELL_VOLTAGE_77,
    CELL_VOLTAGE_78,
    CELL_VOLTAGE_79,

    CELL_VOLTAGE_80,
    CELL_VOLTAGE_81,
    CELL_VOLTAGE_82,
    CELL_VOLTAGE_83,
    CELL_VOLTAGE_84,
    CELL_VOLTAGE_85,
    CELL_VOLTAGE_86,
    CELL_VOLTAGE_87,
    CELL_VOLTAGE_88,
    CELL_VOLTAGE_89,

    CELL_VOLTAGE_90,
    CELL_VOLTAGE_91,
    CELL_VOLTAGE_92,
    CELL_VOLTAGE_93,
    CELL_VOLTAGE_94,
    CELL_VOLTAGE_95,
    CELL_VOLTAGE_96,
    CELL_VOLTAGE_97,
    CELL_VOLTAGE_98,
    CELL_VOLTAGE_99,

    CELL_VOLTAGE_100,
    CELL_VOLTAGE_101,
    CELL_VOLTAGE_102,
    CELL_VOLTAGE_103,
    CELL_VOLTAGE_104,
    CELL_VOLTAGE_105,
    CELL_VOLTAGE_106,
    CELL_VOLTAGE_107,
    CELL_VOLTAGE_108,
    CELL_VOLTAGE_109,

    CELL_VOLTAGE_110,
    CELL_VOLTAGE_111,
    CELL_VOLTAGE_112,
    CELL_VOLTAGE_113,
    CELL_VOLTAGE_114,
    CELL_VOLTAGE_115,
    CELL_VOLTAGE_116,
    CELL_VOLTAGE_117,
    CELL_VOLTAGE_118,
    CELL_VOLTAGE_119,
    
    
    CELL_VOLTAGE_120,
    CELL_VOLTAGE_121,
    CELL_VOLTAGE_122,
    CELL_VOLTAGE_123,
    CELL_VOLTAGE_124,
    CELL_VOLTAGE_125,
    CELL_VOLTAGE_126,
    CELL_VOLTAGE_127,
    CELL_VOLTAGE_128,
    CELL_VOLTAGE_129,
    
    CELL_VOLTAGE_130,
    CELL_VOLTAGE_131,
    CELL_VOLTAGE_132,
    CELL_VOLTAGE_133,
    CELL_VOLTAGE_134,
    CELL_VOLTAGE_135,
    CELL_VOLTAGE_136,
    CELL_VOLTAGE_137,
    CELL_VOLTAGE_138,
    CELL_VOLTAGE_139,
    
    CELL_VOLTAGE_140,
    CELL_VOLTAGE_141,
    CELL_VOLTAGE_142,
    CELL_VOLTAGE_143,
    CELL_VOLTAGE_144,
    CELL_VOLTAGE_145,
    CELL_VOLTAGE_146,
    CELL_VOLTAGE_147,
    CELL_VOLTAGE_148,
    CELL_VOLTAGE_149,
    
    CELL_VOLTAGE_150,
    CELL_VOLTAGE_151,
    CELL_VOLTAGE_152,
    CELL_VOLTAGE_153,
    CELL_VOLTAGE_154,
    CELL_VOLTAGE_155,
    CELL_VOLTAGE_156,
    CELL_VOLTAGE_157,
    CELL_VOLTAGE_158,
    CELL_VOLTAGE_159,
    
    CELL_VOLTAGE_160,
    CELL_VOLTAGE_161,
    CELL_VOLTAGE_162,
    CELL_VOLTAGE_163,
    CELL_VOLTAGE_164,
    CELL_VOLTAGE_165,
    CELL_VOLTAGE_166,
    CELL_VOLTAGE_167,
    CELL_VOLTAGE_168,
    CELL_VOLTAGE_169,
    
    CELL_VOLTAGE_170,
    CELL_VOLTAGE_171,
    CELL_VOLTAGE_172,
    CELL_VOLTAGE_173,
    CELL_VOLTAGE_174,
    CELL_VOLTAGE_175,
    CELL_VOLTAGE_176,
    CELL_VOLTAGE_177,
    CELL_VOLTAGE_178,
    CELL_VOLTAGE_179,
    
    CELL_VOLTAGE_180,
    CELL_VOLTAGE_181,
    CELL_VOLTAGE_182,
    CELL_VOLTAGE_183,
    CELL_VOLTAGE_184,
    CELL_VOLTAGE_185,
    CELL_VOLTAGE_186,
    CELL_VOLTAGE_187,
    CELL_VOLTAGE_188,
    CELL_VOLTAGE_189,
    
    CELL_VOLTAGE_190,
    CELL_VOLTAGE_191,
    CELL_VOLTAGE_192,
    CELL_VOLTAGE_193,
    CELL_VOLTAGE_194,
    CELL_VOLTAGE_195,
    CELL_VOLTAGE_196,
    CELL_VOLTAGE_197,
    CELL_VOLTAGE_198,
    CELL_VOLTAGE_199
    
};
//    
#define LITHIUM 1     //0:ATL电池                                               
#define DEVICE_SERIAL_NUMBER 0x11    //	部件序列号 1byte
#define HARDWARE_VERSION 0x01    //	硬件版本 低4位有效   
#define SOFTWARE_VERSION 0x01   // 	软件版本 低4位有效
#define COMMUNICATION_VERSION 0x01  // 通讯协议版本 低4位有效
//
//
//#define ParamNUMBER 38
#define ENDFLAG 96

//
#define TURN_ON 0
#define TURN_OFF 1
//
#define DISCHARGING 0 //放电
#define RECHARGING 2  //充电
#define FASTRECHARGING 1  //快充
#define TESTSTATE 4  //调试模式


//************ main.c ****************************************
extern unsigned char BMS_SW_Version[8];
extern unsigned char BMUOK;
 //***********************************************************************************
//*******************************the end**********************************************
//************************************************************************************
///////////////////////////////////////////////////////////////////////////////////
 /////////////////////////////////////////////////////////////////////////////////////


