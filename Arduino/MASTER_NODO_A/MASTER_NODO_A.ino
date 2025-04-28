/*********************
  Libraries
*********************/
#include <SPI.h>
#include <HIB.h>
#include <timer_config.h>
#include <mcp_can_uib.h>
#include <mcp_can_uib_dfs.h>
#include "defComunes.h"
#include <Terminal.h>

/*********************
  Objects
*********************/
HIB hib;
MCP_CAN CAN;       
Terminal term;


/*********************
   Macros
*********************/

#define TRUE  1
#define FALSE 0
#define NUM_TICKS 11
#define Modo_NORMAL 7
#define Modo_ALARMA 8
/*********************
   Global variables
*********************/
//variables timer5
volatile uint8_t TICK = NUM_TICKS - 1;
volatile bool Tick_Flag = false;
volatile uint16_t PScaler = TIMER_PSCALER_FOR_50ms;
volatile uint16_t TimerValue = TIMER_TICKS_FOR_50ms;
uint8_t SlotDuration = 50;
volatile bool CambioTimer = false;
//variables temperatura
volatile uint16_t ADC_Ta = 0;
unsigned char Modo = Modo_NORMAL ;
const short int TempMin = -250;
const short int TempMax = 250;                         
//variables para valvulas:
volatile char ActuacionVE='C';
volatile char ActuacionVS='C';
volatile char ActualVS='C';
volatile char ActualVE='C' ;
//variables para nivel:
volatile short int nivel = 0;
volatile short int consigna = 500;
volatile short int consignaMin = consigna-100;
volatile short int consignaMax = consigna+100;
volatile short int consigna_KeyPad = 500 ;
//Variables print
char buff[80]; 
//Variable interacción con Operador
volatile unsigned char KEYPAD_pressed = ' ' ;

/************************************
   Prototypes of auxiliar functions
************************************/

void print_inicio_LCD(){
hib.lcdClear(); //Limipia la pantalla del LCD al principio del programa

hib.lcdSetCursor(0, 0);
 hib.lcdPrint("VE: ");
 hib.lcdSetCursor(1, 0);
 hib.lcdPrint("VS: ");

 hib.lcdSetCursor(0, 3);
 hib.lcdPrint("C");
 hib.lcdSetCursor(1, 3);
 hib.lcdPrint("C");
 

 
 hib.lcdSetCursor(1, 7);
 hib.lcdPrint("Ta:              "); 

 

       
}
void print_inicio_UART(){
  term.clear();
  term.moveHome();
  Serial.println(" |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|");
  Serial.println(" |             Terminal Operador                            |");
  Serial.println(" |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|");
  Serial.println(" | Valor de consigna Operador:          mm                  |");
  Serial.println(" | Valor de consigna Objetivo:          mm                  |");
  Serial.println(" | Nivel actual del depósito:           mm                  |");
  Serial.println(" | Temperatura actual del depósito:        ºC               |");
  Serial.println(" | Estado VE:                                               |");
  Serial.println(" | Estado VS:                                               |");
  Serial.println(" | Alarma   :                                               |");
  Serial.println(" | Duración Slot  :     ms                                  |");
  Serial.println(" | Periodo Muestreo:    ms                                  |");
  Serial.println(" |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|");
  Serial.println(" | consigna OPERADOR: a-500mm b-750mm c-1000mm d-1250mm     |");
  Serial.println(" | Cambiar duración slot: h-10ms j-20ms k-50ms l-100ms      |");
  Serial.println(" |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|");
  
       
}

void printUART(){

    term.move(4, 36);
  sprintf(buff, "    ");
  term.println(buff);

  term.move(4, 36);
  sprintf(buff, "%i ", consigna_KeyPad);
  term.println(buff);

  term.move(5, 36);
  sprintf(buff, "    ");
  term.println(buff);
  
 term.move(5, 36);
  sprintf(buff, "%i", consigna);
  term.println(buff);
  term.move(6, 36);
  sprintf(buff, "    ");
  term.println(buff);
  
  term.move(6, 36);
  sprintf(buff, "%u", nivel);
  term.println(buff);


   term.move(7, 38);
  sprintf(buff, "       ");
  term.println(buff);
  
  term.move(7, 38);
  sprintf(buff, "%i", ADC_Ta);
  term.println(buff);


  term.move(8, 15);
  sprintf(buff, " %c ", ActuacionVE);
  term.println(buff);

  term.move(9, 15); 
  sprintf(buff, " %c ", ActuacionVS);
  term.println(buff);


  if(Modo == Modo_NORMAL){
  term.move(10, 15); 
  sprintf(buff, " ESTADO NORMAL     ");
  term.println(buff);
  
  }else{
  term.move(10, 15); 
  sprintf(buff, " ESTADO DE ALARMA  ");
  term.println(buff);      
  }


  term.move(11, 20);
  sprintf(buff, "    ");
  term.println(buff);
  
  term.move(11, 21);
  sprintf(buff, "%i", SlotDuration);
  term.println(buff);
  
   term.move(12, 20);
  sprintf(buff, "    ");
  term.println(buff);
  
  term.move(12, 21);
  sprintf(buff, "%i", (SlotDuration/2));
  term.println(buff);
}


void Funcion_Keypad(){
     KEYPAD_pressed = term.getChar(false);
          // mientras no se reciba nada de teclado tendremos false
          // cuando se reciba algo por teclado tendremos true
          
          if (KEYPAD_pressed != term.NO_CHAR_RX_UART) {

            switch (KEYPAD_pressed) {

              // Cambio Consigna Nivel si el operador lo solicita:
              
              case 'a':
                consigna_KeyPad = 500;
                break;
              case 'b':
                consigna_KeyPad = 750;
                break;
              case 'c':
                consigna_KeyPad = 1000;
                break;
              case 'd':
                consigna_KeyPad = 1250;
                break;
              // Cambio tiempo de paso si el operador lo solicita:
                
              case 'h':
                PScaler =  TIMER_PSCALER_FOR_10ms;
                TimerValue =  TIMER_TICKS_FOR_10ms;
                SlotDuration = 10;
                CambioTimer = true;
                break;

              case 'j':
                PScaler = TIMER_PSCALER_FOR_20ms;
                TimerValue = TIMER_TICKS_FOR_20ms;
                SlotDuration = 20;
                CambioTimer = true;
                break;

              case 'k':
                PScaler = TIMER_PSCALER_FOR_50ms;
                TimerValue = TIMER_TICKS_FOR_50ms;
                SlotDuration = 50;
                CambioTimer = true;
                break;
               
              case 'l':
                PScaler = TIMER_PSCALER_FOR_100ms;
                TimerValue = TIMER_TICKS_FOR_100ms;
                SlotDuration = 100;
                CambioTimer = true;
                break;
            }
            
          consigna = consigna_KeyPad;
          consignaMin = consigna-100;
          consignaMax = consigna+100;
         }

         
}
void print_LCD_Ta(){
  hib.lcdSetCursor(1, 7);
  hib.lcdPrint("Ta:              "); 
  hib.lcdSetCursor(1, 10);
  sprintf(buff,"%i",ADC_Ta);
  hib.lcdPrint(buff);    
}



/****************************************
   Rutinas de servicio a la interrupcion
****************************************/

void MCP2515_ISR()
{

    char auxSREG;
    auxSREG = SREG;
    
  if (CAN.rxInterrupt())
  {
    CAN.readRxMsg();
   if(CAN.getRxMsgId()==ID_TramaCAN_VE){
    CAN.getRxMsgData((uint8_t*)&ActualVE);
   }
   if(CAN.getRxMsgId()==ID_TramaCAN_VS){
    CAN.getRxMsgData((uint8_t*)&ActualVS);
   }
   if(CAN.getRxMsgId()==ID_TramaCAN_Nivel){
    CAN.getRxMsgData((uint8_t*)&nivel);
   }
  }
  
  SREG = auxSREG;
}



/*****************
  Timer 5 handling
******************/

void timer5Hook (void){
  if(CambioTimer == true){
     CambioTimer = false;
     hib.setUpTimer5(TimerValue,PScaler, timer5Hook);
     }
     TICK = (TICK + 1) % NUM_TICKS;
     Tick_Flag = true;
}


/*****************
  adc_hook
******************/


void adc_hook(uint16_t new_adc_value){
    
    ADC_Ta=map((float)new_adc_value, 0, 1023, -511, 512);   //Lectura y traducción en la escala pedida
          
}


/*********************
  Setup
*********************/
void setup()
{
    // Init HIB
    hib.begin();
    term.begin(115200);
    
    
    // Init can ctrl : baudrate = 500k, normal mode, disable reception&transmission interrupts
    CAN.begin(CAN_500KBPS, MODE_NORMAL, true, false);
    //Habilitamos interrupcion
    attachInterrupt(0, MCP2515_ISR, FALLING);
    //Habilitamos la lectura del ADC mediante timer [Tiempo Muestreo temperatura]
    hib.adcSetTimerDriven(TIMER_TICKS_FOR_10ms, TIMER_PSCALER_FOR_10ms,adc_hook); 
    
    hib.lcdTurnOnCursor();
    print_inicio_LCD();
    print_inicio_UART();
     
}


/*********************
  Main loop
*********************/
void loop()
{
  // Init timer 5 to expire each 50 ms default
   hib.setUpTimer5(TimerValue,PScaler, timer5Hook);

  while(true){

    if(Tick_Flag)
    {
      Tick_Flag = false;
      
      switch (TICK)
      {
        case 0://[TRAMA POLING]
          while(CAN.checkPendingTransmission()!=CAN_OK){}
          CAN.sendMsgBufNonBlocking(ID_AB_Polling, CAN_EXTID, 0, 0); 
        break;
          
        case 4://[CALCULO NUEVOS VALORES VE y VS]
        
        if(ADC_Ta> 250 && ADC_Ta < -250){
          Modo = Modo_ALARMA;
          }else{
            Modo = Modo_NORMAL;
            }
            
        if(Modo == Modo_NORMAL){    

          ActuacionVS = 'A';
          hib.lcdSetCursor(1, 3);
          hib.lcdPrint("A");
          
            if(nivel<consignaMin){
              
              ActuacionVE = 'A';
             hib.lcdSetCursor(0, 3);
             hib.lcdPrint("A");
             
            }else if(nivel>consignaMax){
              
             ActuacionVE = 'C';
             hib.lcdSetCursor(0, 3);
             hib.lcdPrint("C");
             
            }else{ 
               
            ActuacionVE = ActualVE;
            
            }
            
        }else if(Modo == Modo_ALARMA) {
         ActuacionVE = 'C'; ActuacionVS = 'C';
            
             hib.lcdSetCursor(0, 3);
             hib.lcdPrint("C");
             hib.lcdSetCursor(1, 3);
             hib.lcdPrint("C");
        }

           
  
          break;

        case 5://[TRANSMISION ACTUACION]

        while(CAN.checkPendingTransmission()!=CAN_OK){}
        CAN.sendMsgBufNonBlocking(ID_Actua_VE, CAN_EXTID, 1, &ActuacionVE);

        while(CAN.checkPendingTransmission()!=CAN_OK){}
        CAN.sendMsgBufNonBlocking(ID_Actua_VS, CAN_EXTID, 1,&ActuacionVS);
        
        break;
        
        case 6://[ACTUALIZACIÓN UART]
        
        printUART();
         
        break;
        
        case 10:
        //Enviamos el cambio de Timer5
       
        
        Funcion_Keypad();
        print_LCD_Ta();

        if(CambioTimer == true){
        while(CAN.checkPendingTransmission()!=CAN_OK){}
        CAN.sendMsgBufNonBlocking(ID_CambioPeriodo, CAN_EXTID, 2,(uint8_t*)&SlotDuration);
        }
         
        break;
        
        default:
        break;


        
          
      } // end switch
       
      

    } // end if

  }// end while

}



////// IMPORTANTE :
// cambio de válvulas para bajar deposito y tal
//Esta bien lo de los leds o hay q hacerlo en los dos nodos
//las valvulas hay q 
//cambiar el muestreo la clave es pasar por parametro el tiempo como un valor y calcularlo en el nodo B
