


#include "stm32f0xx.h"

#define    CAPTOUCH_GPIO                  GPIOA
#define    CAPTOUCH_EXT_GND_ADC_CHANNEL   7
#define    CAPTOUCH_VREF_INT_ADC_CHANNEL  17
#define    CAPTOUCH_TOTAL_TOUCH_KEY       2
#define    CAPTOUCH_THRESHOLD             100
#define    CAPTOUCH_SHORT_PRESS_TIME_MIN  1   
#define    CAPTOUCH_SHORT_PRESS_TIME_MAX  20   
#define    CAPTOUCH_LONG_PRESS_TIME       100 

typedef struct captouch_t{
  uint8_t  Channel;
  uint8_t  Result[CAPTOUCH_TOTAL_TOUCH_KEY];
  int      CalibrationValue[CAPTOUCH_TOTAL_TOUCH_KEY];
  uint16_t RawADC1[CAPTOUCH_TOTAL_TOUCH_KEY];
  uint16_t RawADC2[CAPTOUCH_TOTAL_TOUCH_KEY];
  int      RawDiffADC[CAPTOUCH_TOTAL_TOUCH_KEY];
  int      DiffThreshold[CAPTOUCH_TOTAL_TOUCH_KEY];
  uint8_t  TriggerMethod[CAPTOUCH_TOTAL_TOUCH_KEY];
  uint8_t  LastTouchState[CAPTOUCH_TOTAL_TOUCH_KEY];
  uint32_t TouchDuration[CAPTOUCH_TOTAL_TOUCH_KEY];
  uint32_t TouchDurationTemp[CAPTOUCH_TOTAL_TOUCH_KEY];
  uint8_t  LongTouchFlag[CAPTOUCH_TOTAL_TOUCH_KEY];
  uint8_t  UsingReferenceTimer;
  uint8_t  ErrorCode[CAPTOUCH_TOTAL_TOUCH_KEY];
}captouch_t;
  

captouch_t CapTouch;
  
  
void CapTouch_Struct_Init(void){
  CapTouch.Channel=0;
  for(uint8_t i=0;i<CAPTOUCH_TOTAL_TOUCH_KEY;i++){
  CapTouch.Result[i]=0;
  CapTouch.CalibrationValue[i]=0;
  CapTouch.RawADC1[i]=0;
  CapTouch.RawADC2[i]=0;
  CapTouch.RawDiffADC[i]=0;
  CapTouch.DiffThreshold[i]=CAPTOUCH_THRESHOLD;
  CapTouch.TriggerMethod[i]=0;
  CapTouch.LastTouchState[i]=0;
  CapTouch.TouchDuration[i]=0;
  CapTouch.TouchDurationTemp[i]=0;
  CapTouch.LongTouchFlag[i]=0;
  CapTouch.UsingReferenceTimer=0;
  CapTouch.ErrorCode[i]=0;
  }
}

void CapTouch_Set_Key_Pin_Input(uint8_t current_channel){
  CAPTOUCH_GPIO->MODER&=~( (1<<(current_channel*2)) | (1<<((current_channel*2)+1)) );
}

void CapTouch_Set_Key_Pin_Analog(uint8_t current_channel){
  CAPTOUCH_GPIO->MODER|=( (1<<(current_channel*2)) | (1<<((current_channel*2)+1)) );
}

void CapTouch_Set_Key_Pin_Output(uint8_t current_channel){
  CAPTOUCH_GPIO->MODER&=~(1<<((current_channel*2)+1));
	CAPTOUCH_GPIO->MODER|=(1<<(current_channel*2));
}

void CapTouch_Set_Key_Pin_Output_Low(uint8_t current_channel){
  CAPTOUCH_GPIO->ODR&=~(1<<current_channel);
}

void CapTouch_Set_Key_Pin_Output_High(uint8_t current_channel){
  CAPTOUCH_GPIO->ODR|=(1<<current_channel);
}

void CapTouch_ADC_Register_Config(void){
  RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
  RCC->CR2 |= RCC_CR2_HSI14ON;
  while ((RCC->CR2 & RCC_CR2_HSI14RDY) == 0);
	if ((ADC1->CR & ADC_CR_ADEN) != 0){
		ADC1->CR |= ADC_CR_ADDIS;
  }
	while ((ADC1->CR & ADC_CR_ADEN) != 0);
  ADC1->CFGR2&=~(ADC_CFGR2_CKMODE_0|ADC_CFGR2_CKMODE_1);
  ADC1->CR |= ADC_CR_ADCAL;
  while ((ADC1->CR & ADC_CR_ADCAL) != 0);
  if ((ADC1->ISR & ADC_ISR_ADRDY) != 0){
    ADC1->ISR |= ADC_ISR_ADRDY;
  }
  ADC1->CR |= ADC_CR_ADEN;
  while ((ADC1->ISR & ADC_ISR_ADRDY) == 0);
	ADC->CCR|=ADC_CCR_VREFEN;
	ADC1->SMPR=0x07;
	ADC1->CHSELR=(1<<17);
	
	RCC->AHBENR|=RCC_AHBENR_GPIOAEN;
}

void CapTouch_ADC_Wakeup(void){
  
}

void CapTouch_ADC_Sleep(void){
  
}

uint16_t CapTouch_ADC_Sample(uint8_t channel){
	ADC1->CHSELR=(1<<channel);
  ADC1->CR|=ADC_CR_ADSTART;
	while((ADC1->CR & ADC_CR_ADSTART)!=0)ADC;
	return ADC1->DR;
}

void CapTouch_Precharge_SH_Cap(uint8_t current_channel){
  CapTouch_Set_Key_Pin_Output(current_channel);
  CapTouch_Set_Key_Pin_Output_Low(current_channel);
  CapTouch_ADC_Wakeup();
  CapTouch_ADC_Sample(CAPTOUCH_VREF_INT_ADC_CHANNEL);
  CapTouch_ADC_Sample(CAPTOUCH_VREF_INT_ADC_CHANNEL);
}

void CapTouch_ADC_Sample1(uint8_t current_channel){
  CapTouch_Set_Key_Pin_Analog(current_channel);
  CapTouch.RawADC1[current_channel]=CapTouch_ADC_Sample(current_channel);
}

void CapTouch_Precharge_Button(uint8_t current_channel){
  CapTouch_Set_Key_Pin_Output(current_channel);
  CapTouch_Set_Key_Pin_Output_High(current_channel);
	CapTouch_Set_Key_Pin_Analog(CAPTOUCH_EXT_GND_ADC_CHANNEL);
  CapTouch_ADC_Sample(CAPTOUCH_EXT_GND_ADC_CHANNEL);
	CapTouch_ADC_Sample(CAPTOUCH_EXT_GND_ADC_CHANNEL);
}

void CapTouch_ADC_Sample2(uint8_t current_channel){
  CapTouch_Set_Key_Pin_Analog(current_channel);
  CapTouch.RawADC2[current_channel]=CapTouch_ADC_Sample(current_channel);
  CapTouch_ADC_Sleep();
}

void CapTouch_Read_Delta(uint8_t current_channel){
  CapTouch_Precharge_SH_Cap(current_channel);
  CapTouch_ADC_Sample1(current_channel);
  CapTouch_Precharge_Button(current_channel);
  CapTouch_ADC_Sample2(current_channel);
  int delta=CapTouch.RawADC1[current_channel];
  delta-=CapTouch.RawADC2[current_channel];
  CapTouch.RawDiffADC[current_channel]=delta;
}

void CapTouch_Calibration(void){
for(uint8_t key=0;key<CAPTOUCH_TOTAL_TOUCH_KEY;key++){
  int32_t sum=0;
  for(int i=0;i<64;i++){
    CapTouch_Read_Delta(key);
  sum+=CapTouch.RawDiffADC[key];
  }
  sum>>=6;
  CapTouch.CalibrationValue[key]=sum-CapTouch.DiffThreshold[key];
  }
}


void CapTouch_Response(uint8_t current_channel){
  CapTouch_Read_Delta(current_channel);
  if(CapTouch.CalibrationValue[current_channel]>CapTouch.RawDiffADC[current_channel]){
    CapTouch.Result[current_channel]=1;
  }else{
    CapTouch.Result[current_channel]=0;
  }
}

void CapTouch_Check_Trigger(uint8_t current_channel){
  uint8_t current_state=0;
  CapTouch_Response(current_channel);
  current_state=CapTouch.Result[current_channel];
    if((CapTouch.LastTouchState[current_channel]==0) && (current_state==1)){
        CapTouch.LongTouchFlag[current_channel]=0;     //Rising Edge
        CapTouch.TriggerMethod[current_channel]=0x01;
    }else if((CapTouch.LastTouchState[current_channel]==1) && (current_state==0)){
        CapTouch.TriggerMethod[current_channel]=0x04;  //Falling Edge
    }else if((CapTouch.LastTouchState[current_channel]==1) && (current_state==1)){
        CapTouch.TriggerMethod[current_channel]=0x02;  //Constant Touched
    }else{
        CapTouch.TriggerMethod[current_channel]=0x00;  //No Touch Event Occured
    }
    CapTouch.LastTouchState[current_channel]=current_state;
}


void CapTouch_Check_Touch_Duration(uint8_t current_channel, uint32_t refernce_time){
  uint32_t duration=0;
  CapTouch_Check_Trigger(current_channel);
  if(CapTouch.TriggerMethod[current_channel]==0x01){
    CapTouch.UsingReferenceTimer|=(1<<current_channel);
    CapTouch.TouchDurationTemp[current_channel]=refernce_time;
  }else if(CapTouch.TriggerMethod[current_channel]==0x04 || CapTouch.TriggerMethod[current_channel]==0x02){
    duration=refernce_time;
    duration-=CapTouch.TouchDurationTemp[current_channel];
    if(duration>0){
      CapTouch.TouchDuration[current_channel]=duration;
    }
    if(CapTouch.TriggerMethod[current_channel]==0x04){
      CapTouch.TouchDurationTemp[current_channel]=refernce_time;
      CapTouch.UsingReferenceTimer&=~(1<<current_channel);
    }
  }
  else{
    CapTouch.TouchDuration[current_channel]=0;
    CapTouch.TouchDurationTemp[current_channel]=0;
  }
}


void CapTouch_Scan_Sensors(uint32_t refernce_time){
  for(uint8_t channel=0;channel<CAPTOUCH_TOTAL_TOUCH_KEY;channel++){
    CapTouch_Check_Touch_Duration(channel, refernce_time);
  }
}

uint8_t CapTouch_Short_Pressed(uint8_t current_channel){
  uint8_t sts=0;
  if((CapTouch.TriggerMethod[current_channel]==0x04) && (CapTouch.TouchDuration[current_channel]>CAPTOUCH_SHORT_PRESS_TIME_MIN) && (CapTouch.TouchDuration[current_channel]<CAPTOUCH_SHORT_PRESS_TIME_MAX)){
    sts=1;
  }
  return sts;
}

uint8_t CapTouch_Long_Pressed(uint8_t current_channel){
  uint8_t sts=0;
  if((CapTouch.TouchDuration[current_channel]>CAPTOUCH_LONG_PRESS_TIME) && (CapTouch.LongTouchFlag[current_channel]==0)){
    CapTouch.LongTouchFlag[current_channel]=1;
    sts=1;
  }
  return sts;
}

uint8_t CapTouch_Constant_Pressed(uint8_t current_channel){
  uint8_t sts=0;
  if((CapTouch.TriggerMethod[current_channel]==0x02) && (CapTouch.TouchDuration[current_channel]>CAPTOUCH_SHORT_PRESS_TIME_MIN)){
    sts=1;
  }
  return sts;
}

uint16_t CapTouch_Get_RawADC1(uint8_t current_channel){
  return CapTouch.RawADC1[current_channel];
}

uint16_t CapTouch_Get_RawADC2(uint8_t current_channel){
  return CapTouch.RawADC2[current_channel];
}

int CapTouch_Get_RawDifferentialADC(uint8_t current_channel){
  return CapTouch.RawDiffADC[current_channel];
}

int CapTouch_Get_Calibration_Value(uint8_t current_channel){
  return CapTouch.CalibrationValue[current_channel];
}

uint32_t CapTouch_Get_Touch_Duration(uint8_t current_channel){
  return CapTouch.TouchDuration[current_channel];
}

uint8_t CapTouch_Get_Touch_Response(uint8_t current_channel){
  return CapTouch.Result[current_channel];
}

uint8_t CapTouch_Get_Reference_Timer_Use_Flag(void){
  if(CapTouch.UsingReferenceTimer){
    return 1;
  }else{
    return 0;
  }
}

uint8_t CapTouch_Get_ErrorCode(uint8_t current_channel){
  return CapTouch.ErrorCode[current_channel];
}

void CapTouch_Get_Debug_Data(uint8_t current_channel, int *data_out){
  data_out[0]=7;
  data_out[1]=CapTouch_Get_RawADC1(current_channel);
  data_out[2]=CapTouch_Get_RawADC2(current_channel);
  data_out[3]=CapTouch_Get_RawDifferentialADC(current_channel);
  data_out[4]=CapTouch_Get_Calibration_Value(current_channel);
  data_out[5]=CapTouch_Get_Touch_Duration(current_channel);
  data_out[6]=CapTouch_Get_Touch_Response(current_channel);
  data_out[7]=CapTouch_Get_Reference_Timer_Use_Flag();
}

void CapTouch_Init(void){
  CapTouch_Struct_Init();
  CapTouch_ADC_Register_Config();
  CapTouch_Calibration();
}

