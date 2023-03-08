

#include "stm32f0xx.h"

typedef struct timebase_t{
  volatile uint8_t  Status;
  volatile uint16_t SubSecondsToSecondsFactor;
  uint32_t          LastTimeSample;
  volatile uint32_t SubSeconds;
  volatile uint32_t Seconds;
}timebase_t;

timebase_t Timebase;

void Timebase_Struct_Init(void){
  Timebase.Status=0;
  Timebase.SubSecondsToSecondsFactor=1;
  Timebase.LastTimeSample=0;
  Timebase.SubSeconds=0;
  Timebase.Seconds=0;
}


void Timebase_Timer_Config(uint32_t isr_firing_frequency_Hz){
  uint32_t ocr_val=F_CPU/isr_firing_frequency_Hz;
  ocr_val/=1024;
  if(ocr_val>255){
    ocr_val=255;
  }
  cli();
  TCCR0B = (1<<CS00)|(1<<CS02) ;
  TCCR0A = (1<<WGM01) ;
  TIMSK0 = (1<<OCIE0A);
  TIFR0  = 0x07;
  OCR0A  = ocr_val;
  Timebase.SubSecondsToSecondsFactor=isr_firing_frequency_Hz;
  sei();
}


void Timebase_Init(uint32_t isr_update_frequency_hz){
  Timebase_Struct_Init();
  Timebase_Timer_Config(isr_update_frequency_hz);
}

void Timebase_Wait_Unit_Time(void){
  while(Timebase.LastTimeSample==Timebase.SubSeconds);
  Timebase.LastTimeSample=Timebase.SubSeconds;
}

uint8_t Timebase_Total_Tokens_Executing(void){
  return (Timebase.Status & 0x3F);
}

void Timebase_Add_Token(void){
  Timebase.Status+=1;
  if( Timebase_Total_Tokens_Executing() > 63){
    Timebase.Status|=63;
  }
}

void Timebase_Remove_Token(void){
  if( Timebase_Total_Tokens_Executing() > 0){
    Timebase.Status-=1;
  }
}

void Timebase_Remove_All_Tokens(void){
  Timebase.Status&=0xC0;
}

void Timebase_Set_Status_Busy(void){
  Timebase.Status|=(1<<7);
}

void Timebase_Clear_Status_Busy(void){
  Timebase.Status&=~(1<<7);
}

uint8_t Timebase_Check_Busy_Status(void){
  if(Timebase.Status & (1<<7)){
    return 1;
  }else{
    return 0;
  }
}

void Timebase_Set_Timer_Reset_Request_Status(void){
  Timebase.Status|=(1<<6);
}

void Timebase_Clear_Timer_Reset_Request_Status(void){
  Timebase.Status&=~(1<<6);
}

uint8_t Timebase_Check_Timer_Reset_Request_Status(void){
  if(Timebase.Status & (1<<6)){
    return 1;
  }else{
    return 0;
  }
}

uint32_t Timebase_Get_SubSeconds(void){
  Timebase_Add_Token();
  return Timebase.SubSeconds;
}

uint32_t Timebase_Get_Seconds(void){
  Timebase_Add_Token();
  return Timebase.Seconds;
}

void Timebase_Set_SubSeconds(uint32_t value){
  Timebase.SubSeconds=value;
}

void Timebase_Set_Seconds(uint32_t value){
  Timebase.Seconds=value;
}

void Timebase_Reset_SubSeconds(void){
  Timebase.SubSeconds=0;
}

void Timebase_Reset_Seconds(void){
  Timebase.Seconds=0;
}

void Timebase_Safely_Reset_Timer(void){
  if(Timebase_Check_Timer_Reset_Request_Status() && (Timebase_Total_Tokens_Executing()==0)){
    Timebase_Set_Status_Busy();
    Timebase_Reset_SubSeconds();
    Timebase_Remove_All_Tokens();
    Timebase_Clear_Timer_Reset_Request_Status();
    Timebase_Clear_Status_Busy();
  }
}

void Timebase_ISR_Executables(void){
  Timebase.SubSeconds++;
  if((Timebase.SubSeconds%Timebase.SubSecondsToSecondsFactor)==0){
    Timebase.Seconds++;
    if(Timebase.SubSeconds>86400000){
      Timebase_Set_Timer_Reset_Request_Status(); 
    }
  }
}


ISR(TIMER0_COMPA_vect){
  Timebase_ISR_Executables();
}