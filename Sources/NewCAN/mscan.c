#include <hidef.h> 
#include "Types.h"
#include "stdio.h"
#include "Regdef.h"
#include "mscan.h"

/***************************************************************************************
                 variable declaration	
****************************************************************************************/

unsigned char cammand;
unsigned long longAddr1=0;		// 地址
unsigned long longAddr2=0;		// 地址
unsigned char msgin[8];		    // CAN message in buffer

unsigned long g_mboxID;
unsigned char g_mboxData[3][8];// CAN2接收到的报文数据，每个报文8个字节
unsigned char length;
 
extern void ProcCanCmd(void);
extern void ProcessUDS(void);
//#pragma CODE_SEG  BOOT_SEG
//***************************************************************************************
//***************************************************************************************
void MSCAN0Init(void)
{
     
    CAN0BTR0 = 0x01;           // 500k波特率, 16M bus clock
    CAN0BTR1 = 0x1c; 
   
    //CAN0IDAC = 0x20; //ID 8位接收模式  // Disables all the Filters  
                               
    CAN0IDMR0 = 0xFF;
    CAN0IDMR1 = 0xFF;
    CAN0IDMR2 = 0xFF;
    CAN0IDMR3 = 0xFF;
    CAN0IDMR4 = 0xFF;
    CAN0IDMR5 = 0xFF;
    CAN0IDMR6 = 0xFF;
    CAN0IDMR7 = 0xFF;

    // Enable MSCAN and normal operation and select the bus clock as MSCAN clock source
    //CAN0CTL1 = 0xC0;
    // Enable MSCAN and normal operation and select the oscillator clock as MSCAN clock source

    CAN0CTL1 = 0x80;    //16M oscillator  as the bus clock 
    CAN0CTL0 = 0x00;// Active MSCAN Normal Operation

      
    while(CAN0CTL1&0x01);// Wait until the MSCAN operates normally 
    while(!(CAN0CTL0&0x10));// Wait until MSCAN is synchronized to the CAN bus
      
    CAN0RIER = 0x01;//Enable receiver interrupt所有CAN都中断接收
}
//***************************************************************************************
//Function Name	: MSCAN2Init
//Programmer		: R66193
//Date			: 2008.02.22
//Arguments		: none
//Returns			: none
//Notes			: This function initialize MSCAN module.		
//****************************************************************************************/
void MSCAN2Init(void) {  

    CAN2CTL1 = 0x80; 
    //CAN2CTL1 = 0x87;  // B 1000_0111
                      //   |||| ||||- read only: indicate initialize status
                      //   |||| |||-- read only: indicates sleep mode
                      //   |||| ||--- wake up on certain length of dominant pulse
                      //   |||| |---- automatic bus off recovery
                      //   ||||------ normal operation
                      //   |||------- loop back self test disable
                      //   ||-------- OSC clock source: 4M here
                      //   |--------- MSCAN enabled
 
    CAN2BTR0 = 0x43;  // BitRate = Fcanclk x (1+ TimeSegment1 + TimeSegment2) / Prescaler
    CAN2BTR1 = 0xE7;  // In this case:
                      //     Fcanclk=4M;      The crystal=4M
                      //     SycJumpWidth=2   CAN0BTR0[7:6]=SJW[1:0]=01
                      //     Prescaler=4,     CAN0BTR0[5:0]=BRP[5:0]=0x3
                      //     TSEG2=7,TSEG1=8, CAN0BTR1[6:4]=TSEG2[2:0]=110, CAN0BTR1[3:0]=TSEG2[3:0]=110

    CAN2IDMR0 = 0xFF; // no filter, define to accept all coming message
    CAN2IDMR1 = 0xFF;
    CAN2IDMR2 = 0xFF;
    CAN2IDMR3 = 0xFF;
    CAN2IDMR4 = 0xFF;
    CAN2IDMR5 = 0xFF;
    CAN2IDMR6 = 0xFF;
    CAN2IDMR7 = 0xFF;

    //CAN2CTL0_INITRQ = 0; // enable normal operation mode
    CAN2CTL0 &= 0xfe; 

    //while(CAN2CTL1_INITAK != 0);
    while((CAN2CTL1&0x01) != 0);

    //CAN2CTL0_TIME = 1;   // enable internal timer
    CAN2CTL0 |= 0x0c;
    //CAN2CTL0_WUPE = 1;   // WAKE UP enable

    CAN2RIER = 0x01;     // only enable receive buffer full interrupt
}


//**********************************************************************************
// MSCAN0SendMsg
// --------------------------------------------------------------------------------------
// MSCAN0 Send Message Routine
//**********************************************************************************
Bool MSCAN0SendMsg(struct can_msg msg)
{
    unsigned char n_tx_buf, i;
    unsigned long pp=0;
   
    if(!(CAN0CTL0&0x10))// Checks synchronization to CAN bus
        return(FALSE);
   
    n_tx_buf = 0;
   
    do
    {    
        // Looks for a free buffer
        n_tx_buf =  CAN0TFLG;
        CAN0TBSEL=n_tx_buf;
        n_tx_buf=CAN0TBSEL;
        i++;
        if(i>=200)
            return(FALSE);
    }while(!n_tx_buf); 
     
     
    CAN0TIDR0 = (unsigned char)(msg.id>>3);// Write standard Identifier
    CAN0TIDR1 = (unsigned char)(msg.id<<5);
    
    //pp = msg.id>>15;//Write Extended Identifier
    //CAN0TIDR0 = (unsigned char)(pp>>6);//Write Extended Identifier
    //CAN0TIDR1 = ((unsigned char)(msg.id>>13)&0xe0) |  0x18 | ((unsigned char)(msg.id>>15)&0x07);
    //CAN0TIDR2 = (unsigned char)(msg.id>>7);
    //CAN0TIDR3 = (unsigned char)(msg.id<<1);
  
    //if(msg.RTR)
      // RTR=Recessive
    //  CAN0TIDR1 |= 0x10;
        
    for(i = 0; i < 8; i++)// Write Data Segment
        *((&CAN0TDSR0)+i) = msg.data[i];
        
    CAN0TDLR = 8; // Write Data Length
      
    //CAN0TTBPR = msg.prty;// Write Priority
      
    CAN0TFLG = n_tx_buf;// Clear TXx Flag (buffer ready to transmission)
    
    return(TRUE);
    
}

/***************************************************************************************
Function Name	: MSCAN2Trans
Programmer		: R66193
Date			: 2008.02.22
Arguments		: none
Returns			: none
Notes			: This function sends message onto the CAN bus.		
****************************************************************************************/
/*
void MSCAN2Trans(byte * msgout)
{
    unsigned char cansel;
 
    CAN2TBSEL = CAN2TFLG;       //select one available transmit buffer
    cansel = CAN2TBSEL;
 
    CAN2TXIDR0 = 0x11;          //filling extended ID
    CAN2TXIDR1 = 0xff;			
    CAN2TXIDR2 = 0xff;
    CAN2TXIDR3 = 0xfe;
    
    CAN2TXDSR0 = *(msgout+0);   //filling data to be transmit
    CAN2TXDSR1 = *(msgout+1);
    CAN2TXDSR2 = *(msgout+2);
    CAN2TXDSR3 = *(msgout+3);
    CAN2TXDSR4 = *(msgout+4);
    CAN2TXDSR5 = *(msgout+5);
    CAN2TXDSR6 = *(msgout+6);
    CAN2TXDSR7 = *(msgout+7);
    CAN2TXDLR = 8;              //filling transmit data length
    CAN2TXTBPR = 0;             //set highest priority
 
    CAN2TFLG = cansel;  //clearing TxE to lauch transmission
}

*/ 
//#pragma CODE_SEG  DEFAULT_ROM
//#pragma CODE_SEG __NEAR_SEG NON_BANKED
//***************************************************************************************
//***************************************************************************************
//Function Name	: MSCAN0Rec
//Programmer		: R66193
//Date			: 2014.12.23
//Arguments		: none
//Returns			: 1/0
//Notes			: This function receives message from the CAN bus.		
//****************************************************************************************/
interrupt void  MSCAN0Rec(void) 
{ 
    unsigned char i;
    unsigned long msgCan0RxID;
    
    DisableInterrupts;
    
    if(!(CAN0RFLG&0x01)) // Checks for received messages
        return ;
    
    msgCan0RxID = (unsigned int)(CAN0RXIDR0<<3) |       //standard
                  (unsigned char)(CAN0RXIDR1>>5); 
    for(i=0;i<8;i++)
        g_mboxData[0][i]= *((&CAN0RXDSR0)+i); 
    switch(msgCan0RxID) 
    {
        case 0x7e1:
        case 0x7df:
            ProcessUDS();
            break;
        default:
            break; 
    }  
    
    CAN0RFLG = 0x01;  
    EnableInterrupts;    

}
//***************************************************************************************
//Function Name	: MSCAN2Rec
//Programmer		: R66193
//Date			: 2008.02.22
//Arguments		: none
//Returns			: 1/0
//Notes			: This function receives message from the CAN bus.		
//****************************************************************************************/
interrupt void  MSCAN2Rec(void) 
{ 
    /*
    unsigned long  g_mboxID=0;
    unsigned char *p;
    unsigned long pp=0;
    unsigned long pp1=0;
    
    DisableInterrupts;
     
    PTP &= 0xfb;     //输出PP2=0;    灭灯 
    
    pp = (unsigned long)CAN2RXIDR0<<15;  
    pp1 = (unsigned long)((CAN2RXIDR1>>5)&0x07)<<15;                              
    g_mboxID = ((unsigned long)pp<<6) | //expanded
               (pp1<<3) |
               ((unsigned long)(CAN2RXIDR1&0x07)<<15)|
               ((unsigned long)CAN2RXIDR2<<7) | 
               ((unsigned long)CAN2RXIDR3>>1);
    
    switch(g_mboxID) 
    {
    
        case 0x0a0218ef: //该ID表示擦除         
            cammand = CAN2RXDSR0;               
            ProcCanCmd();              
            break;
        case 0x0a0218f0:         
            p= (unsigned char *)(&longAddr1);
            *p++ = 0;
            *p++ = CAN2RXDSR1;
            *p++ = CAN2RXDSR2;
            *p++ = CAN2RXDSR3;        
            
            *(msgin+0) = CAN2RXDSR4;
            *(msgin+1) = CAN2RXDSR5;
            *(msgin+2) = CAN2RXDSR6;
            *(msgin+3) = CAN2RXDSR7;
            break;
       case 0x0a0218f1:        
            p= (unsigned char *)(&longAddr2);
            *p++ = 0;
            *p++ = CAN2RXDSR1;
            *p++ = CAN2RXDSR2;
            *p++ = CAN2RXDSR3;  
                    
            if(longAddr2-longAddr1==4) 
            {               
                if(longAddr2==0x7ffffc)
                 longAddr1=0x7FEFF8;            
                cammand = CAN2RXDSR0;
                *(msgin+4) = CAN2RXDSR4;
                *(msgin+5) = CAN2RXDSR5;
                *(msgin+6) = CAN2RXDSR6;
                *(msgin+7) = CAN2RXDSR7;
                ProcCanCmd();
            }
            break;
        default:
            break;
    
    }
 
    CAN2RFLG = 0x01;  //clearing RXF to free the RxFG    
    EnableInterrupts; 
    */ 
}



