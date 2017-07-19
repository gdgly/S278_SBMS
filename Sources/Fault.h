                             
//----------------------------------------------------------------------------------------------------
//---------------------- Pride Power------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//* Project Name       : S133
//* File Name          : Fault.h
//* Author             : Judy
//* Version            : V1.0.0
//* Start Date         : 2016.6.7
//* Description        : 故障定义
//----------------------------------------------------------------------------------------------------

extern unsigned char  g_caution_Flag_1;  //故障状态
extern unsigned char  g_caution_Flag_2;
extern unsigned char  g_caution_Flag_3;
extern unsigned char  g_caution_Flag_4;
extern unsigned int g_errorCounter;
extern unsigned char ACCha_Flag_BST;
extern unsigned char ACCOverTime;//交流充电机通信故障


///////////////////最大允许放电电流//////////////////////
typedef union{
	unsigned int word;
	struct {
	
		word F0_Battery_Under_Voltage1:1;   //
		word F1_Inner_Communiction:1;       //
		word F2_DisCharge_Over_Current1:1;	//
		word F3_Cell_Under_Voltage1:1;	   //
		word F4_I_Sener_Err:1;	          //
		word F5_Over_Temp1:1;             //
		word F6_Under_Temp1:1;            //
		word F715_Reverse:9;	
       		       
	}Bit;
}CUTDISCURT0;
extern CUTDISCURT0 CutDisCurt0; 

typedef union{
	unsigned int word;                             

	struct {
	
		word F0_Battery_Under_Voltage2:1;//
		word F1_DisCharge_Over_Current2:1;//	
		word F2_Cell_Under_Voltage2:1; //
		word F3_CellUnbalance2:1; //
		word F4_tempUnbalance2:1;//
		word F5_Over_Temp2:1; //
		  word F615_Reverse:10;	      	        		       
	}Bit;
}CUTDISCURT50;
extern CUTDISCURT50 CutDisCurt50;

typedef union{
	unsigned int word;
	struct {
	
		word F0_Battery_Under_Voltage3:1; //
		word F1_DisCharge_Over_Current3:1;//	
		word F2_Cell_Under_Voltage3:1;    //
		word F3_tempUnbalance3:1;         //
		word F4_Over_Temp3:1;             //
		  word F515_Reverse:11;	      	        		       
	}Bit;
}CUTDISCURT70;
extern CUTDISCURT70 CutDisCurt70;
///////////////////最大允许放电电流//////////////////////


///////////////////充电电流(回馈)//////////////////////

typedef union{
	unsigned int word;
	struct {
		
		word F0_Battery_Over_Voltage1:1;      //
	  word F1_Inner_Communiction:1;        //
		word F2_Charge_Over_Current1:1;      //
		word F3_Cell_Over_Voltage1:1;       //
		word F4_I_Sener_Err:1;             //
		word F5_Over_Temp1:1;	            //
		word F6_Under_Temp1:1;            //
		  word F715_Reverse:9;      		       
	}Bit;
}CUTCHACURT0;
extern CUTCHACURT0 CutChaCurt0;

typedef union{
	unsigned int word;
	struct {
	       
		word F0_Battery_Over_Voltage2:1; //
		word F1_Charge_Over_Current2:1;  //
	  word F2_Cell_Over_Voltage2:1;   //
	  word F3_tempUnbalance2:1;       //
	  word F4_Over_Temp2:1; //
	    word F515_Reverse:11;  	        		       
	}Bit;
}CUTCHACURT50;
extern CUTCHACURT50 CutChaCurt50;

typedef union{
	unsigned int word;
	struct {
	
		word F0_Battery_Over_Voltage3:1;//
		word F1_Charge_Over_Current3:1; //
	  word F2_Cell_Over_Voltage3:1;  //
	  word F3_tempUnbalance3:1;     //
	  word F4_Over_Temp3:1;     //
	    word F515_Reverse:11;  	        		       
	}Bit;
}CUTCHACURT70;
extern CUTCHACURT70 CutChaCurt70;
///////////////////充电电流（回馈）//////////////////////



///////////////////DC快充充电电流//////////////////////

typedef union{
	unsigned int word;
	struct {
	
		word F0_Battery_Over_Voltage21:1; //   
		word F1_Communication_With_Charger:1; //
		word F2_Inner_Communiction:1;         //
		word F3_Charge_Over_Current1:1;       //
		word F4_Cell_Over_Voltage321:1;       //
		word F5_I_Sener_Err:1;//
		word F6_Over_Temp1:1;	             //
		word F7_Under_Temp1:1;            //
		word F8_Low_Isolation1:1;	        //
		word F9_Charge_Count1:1;	        //
		word F915_Reverse:6;	       		       
	}Bit;
}CUTDCCHACURT0;
extern CUTDCCHACURT0 CutDCChaCurt0;

typedef union{
	unsigned int word;
	struct {
		
		word F0_Charge_Over_Current2:1; //
	  word F1_tempUnbalance2:1;       //
	  word F2_Over_Temp2:1;          //
	  word F3_Low_Isolation2:1;             //
	    word F4F15_Reverse:12;   	        		       
	}Bit;                                
}CUTDCCHACURT50;
extern CUTDCCHACURT50 CutDCChaCurt50;

typedef union{
	unsigned int word;
	struct {
		
		word F0_Battery_Over_Voltage3:1; //
	  word F1_Charge_Over_Current3:1;  //
	  word F2_tempUnbalance3:1;      //
	  word F3_Over_Temp3:1;      //
	  word F4_Low_Isolation3:1;  //
	    word F515_Reverse:11;   	        		       
	}Bit;
}CUTDCCHACURT70;
extern CUTDCCHACURT70 CutDCChaCurt70;

///////////////////DC快充充电电流//////////////////////



///////////////////AC慢充充电电流//////////////////////

typedef union{
	unsigned int word;
	struct {
	
		word F0_Battery_Over_Voltage21:1; //    
		word F1_Communication_With_Charger:1;// 
		word F2_Inner_Communiction:1;        //
		word F3_Charge_Over_Current1:1;      //
		word F4_Cell_Over_Voltage321:1;	    //
		word F5_I_Sener_Err:1;              //
		word F6_Over_Temp1:1;               //
		word F7_Under_Temp1:1;	            //
		word F8_Low_Isolation1:1;           //
		word F9_Charge_Count1:1;	        //
		word F915_Reverse:6;		       		       
	}Bit;
}CUTACCHACURT0;
extern CUTACCHACURT0 CutACChaCurt0;

typedef union{
	unsigned int word;
	struct {
		word F0_Charge_Over_Current2:1;   //
		word F1_tempUnbalance2:1;         //
	  word F2_Over_Temp2:1;             //
	  word F3_Low_Isolation2:1;             //
	    word F415_Reverse:12;    	        		       
	}Bit;
}CUTACCHACURT50;
extern CUTACCHACURT50 CutACChaCurt50;

typedef union{
	unsigned int word;
	struct {
		word F0_Battery_Over_Voltage3:1;//
		word F1_Charge_Over_Current3:1; //
	  word F2_tempUnbalance3:1;       //
	  word F3_Over_Temp3:1;      //  
	  word F4_Low_Isolation3:1; //
	    word F515_Reverse:11;    	        		       
	}Bit;
}CUTACCHACURT70;
extern CUTACCHACURT70 CutACChaCurt70;
///////////////////AC慢充充电电流//////////////////////





///////////////////最大允许放电电流//////////////////////
/*typedef union{
	unsigned int word;
	struct {
	
		word F0_Battery_Under_Voltage2:1;
		word F1_DisCharge_Over_Current2:1;
		word F2_DisCharge_Over_Current3:1;	
		word F3_Low_Isolation2:1;	
		word F4_Cell_Under_Voltage2:1;	
		word F5_Over_Temp2:1;
		word F6_MSD:1;
		word F7_Inner_Communiction:1;//内部通信故障
		word F815_Reverse:8;	
       		       
	}Bit;
}CUTDISCURT0;
extern CUTDISCURT0 CutDisCurt0; 

typedef union{
	unsigned int word;
	struct {
	
		word F0_Battery_Under_Voltage1:1;
		word F1_DisCharge_Over_Current1:1;	
		word F2_Cell_Under_Voltage1:1;
		word F3_Over_Temp1:1;
		  word F415_Reverse:12;	      	        		       
	}Bit;
}CUTDISCURT50;
extern CUTDISCURT50 CutDisCurt50;

typedef union{
	unsigned int word;
	struct {
	
		word F0_Battery_Under_Voltage1:1;
		word F1_DisCharge_Over_Current1:1;	
		word F2_Cell_Under_Voltage1:1;
		word F3_Over_Temp1:1;
		  word F415_Reverse:12;	      	        		       
	}Bit;
}CUTDISCURT70;
extern CUTDISCURT70 CutDisCurt70;
///////////////////最大允许放电电流//////////////////////


///////////////////充电电流(回馈)//////////////////////

typedef union{
	unsigned int word;
	struct {
		
		word F0_Battery_Over_Voltage1:1;
	  word F1_Battery_Over_Voltage3:1;  
		word F2_Charge_Over_Current2:1;
		word F3_Charge_Over_Current3:1;
		word F4_Low_Isolation2:1;
		word F5_Cell_Over_Voltage2:1;	
		word F6_Cell_Over_Voltage3:1;  
		word F7_Over_Temp2:1;
		word F8_Inner_Communiction:1;//内部通信故障
		  word F915_Reverse:7;      		       
	}Bit;
}CUTCHACURT0;
extern CUTCHACURT0 CutChaCurt0;

typedef union{
	unsigned int word;
	struct {
	       
		word F0_Battery_Over_Voltage2:1;
		word F1_Charge_Over_Current1:1;
	  word F2_Cell_Over_Voltage1:1; 
	  word F3_Over_Temp1:1;
	    word F415_Reverse:12;  	        		       
	}Bit;
}CUTCHACURT50;
extern CUTCHACURT50 CutChaCurt50;

typedef union{
	unsigned int word;
	struct {
	
		word F0_Battery_Over_Voltage3:1;
		word F1_Charge_Over_Current1:1;
	  word F2_Cell_Over_Voltage1:1; 
	  word F3_Over_Temp1:1;
	    word F415_Reverse:12;  	        		       
	}Bit;
}CUTCHACURT70;
extern CUTCHACURT70 CutChaCurt70;
///////////////////充电电流（回馈）//////////////////////



///////////////////DC快充充电电流//////////////////////

typedef union{
	unsigned int word;
	struct {
	
		word F0_Battery_Over_Voltage21:1;    
		word F1_Battery_Over_Voltage3:1; 
		word F2_Charge_Over_Current2:1;
		word F3_Charge_Over_Current3:1;
		word F4_Low_Isolation2:1;
		word F5_Cell_Under_Voltage:1;
		word F6_Cell_Over_Voltage2:1;	
		word F7_Cell_Over_Voltage3:1;
		word F8_Over_Temp2:1;	
		word F9_Under_Temp1:1;
		word F10_Charge_Count:1;
		word F11_MSD:1;
		word F12_Communication_With_Charger:1;
	  word F13_Inner_Communiction:1;
		word F1415_Reverse:2;	       		       
	}Bit;
}CUTDCCHACURT0;
extern CUTDCCHACURT0 CutDCChaCurt0;

typedef union{
	unsigned int word;
	struct {
		
		word F0_Battery_Over_Voltage1:1;
	  word F1_Charge_Over_Current1:1; 
	  word F2_Over_Temp1:1;
	    word F3F15_Reverse:13;   	        		       
	}Bit;
}CUTDCCHACURT50;
extern CUTDCCHACURT50 CutDCChaCurt50;

typedef union{
	unsigned int word;
	struct {
		
		word F0_Battery_Over_Voltage3:1;
	  word F1_Charge_Over_Current1:1; 
	  word F2_Over_Temp1:1;
	    word F3F15_Reverse:13;   	        		       
	}Bit;
}CUTDCCHACURT70;
extern CUTDCCHACURT70 CutDCChaCurt70;

///////////////////DC快充充电电流//////////////////////



///////////////////AC慢充充电电流//////////////////////

typedef union{
	unsigned int word;
	struct {
	
		word F0_Battery_Over_Voltage21:1;     
		word F1_Battery_Over_Voltage3:1; 
		word F2_Charge_Over_Current2:1;
		word F3_Charge_Over_Current3:1;
		word F4_Low_Isolation2:1;	
		word F5_Cell_Over_Voltage2:1;
		word F6_Cell_Over_Voltage3:1;
		word F7_Over_Temp2:1;	
		word F8_Under_Temp1:1;
		word F9_Charge_Count:1;
		word F10_MSD:1;
	  word F11_Communication_With_Charger:1;
	  word F12_Inner_Communiction:1;                       
		word F1315_Reverse:3;		       		       
	}Bit;
}CUTACCHACURT0;
extern CUTACCHACURT0 CutACChaCurt0;

typedef union{
	unsigned int word;
	struct {
		word F0_Battery_Over_Voltage1:1;
		word F1_Charge_Over_Current1:1;
	  word F2_Over_Temp1:1;
	    word F315_Reverse:13;    	        		       
	}Bit;
}CUTACCHACURT50;
extern CUTACCHACURT50 CutACChaCurt50;

typedef union{
	unsigned int word;
	struct {
		word F0_Battery_Over_Voltage3:1;
		word F1_Charge_Over_Current1:1;
	  word F2_Over_Temp1:1;
	    word F315_Reverse:13;    	        		       
	}Bit;
}CUTACCHACURT70;
extern CUTACCHACURT70 CutACChaCurt70;
///////////////////AC慢充充电电流//////////////////////
*/
extern Bool errorCurrSensorIniatial(void);
extern unsigned char TaskFaultProcess(void);
extern void CarFaultDone(void);


