#include <SPI.h>
#include <HIB.h>
#include <timer_config.h>
#include <mcp_can_uib.h>
#include <mcp_can_uib_dfs.h>
#include "defComunes.h"
#include <Terminal.h>

/*********************
  Local macros
*********************/
 // Action macros

#define TRUE  1
#define FALSE 0
#define ACTION_NONE 6


/*********************
  Objects
*********************/
HIB hib;
MCP_CAN CAN;                                    
Terminal term;

/*********************
   Global variables
*********************/

//variables temperatura
uint16_t ADC_Ta = 0;
//variables para valvulas:
volatile char ActuacionVE='C' ;
volatile char ActuacionVS='C' ;
volatile char ActualVS = 'C';
volatile char ActualVE = 'C' ;
//variables para nivel:
volatile short int nivel = 2000;
volatile short int EH = 0;
volatile short int ActualizacionNivel = 0;
//variables para trama:
volatile unsigned char action = ACTION_NONE ;
char buff[80]; 
//variables timer5
volatile uint16_t PScaler = TIMER_PSCALER_FOR_10ms;
volatile uint16_t TimerValue = TIMER_TICKS_FOR_10ms;
volatile uint8_t SlotDuration = 20;
volatile uint8_t ValorPeriod = 10;
volatile bool CambioPeriodo = true;
//NODO B:

/************************************
   Prototypes of auxiliar functions
************************************/
void Print_inicio_LCD(){
 hib.lcdClear(); //Limipia la pantalla del LCD al principio del programa

  hib.lcdSetCursor(1, 0);
  hib.lcdPrint("VS:  ");

  hib.lcdSetCursor(0, 0);
  hib.lcdPrint("VE:  ");

  hib.lcdSetCursor(1, 6);
  hib.lcdPrint("Nv:     mm");
  
  hib.lcdSetCursor(0, 7);
  hib.lcdPrint("T:     ms");
}

void PrintLCD(){
  
    hib.lcdSetCursor(0, 3);
    sprintf(buff, "%c", ActualVE);
    hib.lcdPrint(buff);

    hib.lcdSetCursor(1, 3);
    sprintf(buff, "%c", ActualVS);
    hib.lcdPrint(buff);

    hib.lcdSetCursor(1, 10);
    sprintf(buff, "%i", nivel);
    hib.lcdPrint(buff);

    hib.lcdSetCursor(0, 12);
    sprintf(buff, "%i", ValorPeriod);
    hib.lcdPrint(buff);
}
void NewTimerValues(){
switch(ValorPeriod){
               case 5:
                PScaler =  TIMER_PSCALER_FOR_5ms;
                TimerValue =  TIMER_TICKS_FOR_5ms;
                 hib.lcdSetCursor(0, 7);
                 hib.lcdPrint("T:     ms");
                break;

              case 10:
                PScaler = TIMER_PSCALER_FOR_10ms;
                TimerValue = TIMER_TICKS_FOR_10ms;
                break;

              case 25:
                PScaler = TIMER_PSCALER_FOR_25ms;
                TimerValue = TIMER_TICKS_FOR_25ms;
                break;
               
              case 50:
                PScaler = TIMER_PSCALER_FOR_50ms;
                TimerValue = TIMER_TICKS_FOR_50ms;
                break;

                  default:
                   break;
}

  
}

/************************************************
   Rutinas de servicio a la interrupcion / hooks
************************************************/
//CON ESTE TIMER SE CONTROLA EL AUGMENTO DE NIVEL
//Augmento del nivel, se simplifica puesto que al estar abiertas se llena 2mm cada 10ms
void timer5Hook (void){
  
nivel = nivel + EH; 

}
/*****************
  CAN handling
******************/
void MCP2515_ISR()
{
  // Var to save AVR reg
  char auxSREG;

  // Auxiliar local vars
  unsigned int canId;

  // Save the AVR Status Register by software
  // since the micro does not do it automatically
  auxSREG = SREG;
     
  // Handle reception interrupt
  if ( CAN.rxInterrupt() )
  {
    // Read received frame from buffer and CLEAR/RELEASE it
    CAN.readRxMsg();

    // Get CAN id of just readed frame
    canId = CAN.getRxMsgId();

    // Set action depending on canId
    switch (CAN.getRxMsgId())
    {   
                
      case ID_AB_Polling:
        action = ID_AB_Polling;
        break;

      case ID_Actua_VE:
        CAN.getRxMsgData(&ActuacionVE);
        ActualVE = ActuacionVE;
        break;

      case ID_Actua_VS:
        CAN.getRxMsgData(&ActuacionVS);
        ActualVS = ActuacionVS;
        break;
      case ID_CambioPeriodo:
        CAN.getRxMsgData(&SlotDuration);
        ValorPeriod = (SlotDuration)/2;
        CambioPeriodo = true;
        break;
        
      default:
        break; 
    }
	
  }

  // Restore the AVR Status Register by software
  // since the micro does not do it automatically
  SREG = auxSREG;
}





/*********************
  Setup
*********************/
void setup()
{
    // Init HIB
    hib.begin();

    // Init can ctrl : baudrate = 500k, normal mode, enable reception interrupt
    CAN.begin(CAN_500KBPS, MODE_NORMAL, true, false);
    Serial.println("CAN BUS Shield init ok!");

    // Set CAN interrupt handler address in the position of interrupt number 0
    // since the INT output signal of the CAN shield is connected to
    // the Mega 2560 pin num 2, which is associated to that interrupt number.
    attachInterrupt(0, MCP2515_ISR, FALLING); 
    hib.lcdTurnOnCursor();
    Print_inicio_LCD();
    PrintLCD();
}


/*********************
  Main loop
*********************/
void loop()
{
 hib.setUpTimer5(TIMER_TICKS_FOR_10ms, (tClkPreFactType) TIMER_PSCALER_FOR_10ms, timer5Hook);
 
  while(1)
  {
    
    switch (action)
    {//EN EL CASO DE RECIBIR TRAMA DE POLLING SE MANDA LOS VALORES ACTUALES 
      
        case ID_AB_Polling:
        while(CAN.checkPendingTransmission()!=CAN_OK){}
        CAN.sendMsgBufNonBlocking(ID_TramaCAN_VE, CAN_EXTID, 1,(uint8_t*)&ActualVE);
        while(CAN.checkPendingTransmission()!=CAN_OK){}
        CAN.sendMsgBufNonBlocking(ID_TramaCAN_VS, CAN_EXTID, 1,(uint8_t*)&ActualVS);
        while(CAN.checkPendingTransmission()!=CAN_OK){}
        CAN.sendMsgBufNonBlocking(ID_TramaCAN_Nivel, CAN_EXTID, 2,(uint8_t*)&nivel);
        action = ACTION_NONE;
        break;
      
        default:
        break;
      } // end switch

      //ACTUALIZACIÓN LEDS
      if(ActualVE == 'A'){
        
        hib.ledOn(0);
        }else{
        hib.ledOff(0);  
        
        }
      if(ActualVS == 'A'){
              
        hib.ledOn(1);
        }else{
        hib.ledOff(1);  
              
        }
       //ACTUALIZACIÓN DEL PERIODO DE MUESTREO
        if(CambioPeriodo == true){
        NewTimerValues();  
        hib.setUpTimer5(TimerValue,PScaler, timer5Hook);
        CambioPeriodo = false  ;
      }
       //CAMBIO DEL CRECIMIENTO DE NIVEL
      if(ActualVE == 'A' ){
        
        if(ActualVS == 'A'){
        EH = ValorPeriod*0.2; 
        }else{
        EH = ValorPeriod*0.4;
        }
      
      }else{

         if(ActualVS == 'A'){
        EH = -(ValorPeriod*0.2); 
        }else{
        EH = 0;
        }  
      }
       
      if(ActualizacionNivel == 30){
         hib.lcdSetCursor(1, 6);
         hib.lcdPrint("Nv:     mm"); 
         ActualizacionNivel = 0;
      }else{
        ActualizacionNivel++;
      }
      
      //ACTUALIZACIÓN LCD
      PrintLCD();
      
    } // end while
 

}
