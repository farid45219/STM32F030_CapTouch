
#include "stm32f0xx.h"

void     CapTouch_Struct_Init(void);
void 	   CapTouch_Set_Key_Pin_Input(uint8_t current_channel);
void 	   CapTouch_Set_Key_Pin_Analog(uint8_t current_channel);
void 	   CapTouch_Set_Key_Pin_Output(uint8_t current_channel);
void 	   CapTouch_Set_Key_Pin_Output_Low(uint8_t current_channel);
void 	   CapTouch_Set_Key_Pin_Output_High(uint8_t current_channel);
void 	   CapTouch_ADC_Register_Config(void);
void 	   CapTouch_ADC_Wakeup(void);
void 	   CapTouch_ADC_Sleep(void);
uint16_t CapTouch_ADC_Sample(uint8_t channel);
void     CapTouch_Precharge_SH_Cap(uint8_t current_channel);
void     CapTouch_ADC_Sample1(uint8_t current_channel);
void     CapTouch_Precharge_Button(uint8_t current_channel);
void     CapTouch_ADC_Sample2(uint8_t current_channel);
void     CapTouch_Read_Delta(uint8_t current_channel);
void     CapTouch_Calibration(void);
void 	   CapTouch_Response(uint8_t current_channel);
void     CapTouch_Check_Trigger(uint8_t current_channel);
void     CapTouch_Check_Touch_Duration(uint8_t current_channel, uint32_t refernce_time);
void     CapTouch_Scan_Sensors(uint32_t refernce_time);
uint8_t  CapTouch_Short_Pressed(uint8_t current_channel);
uint8_t  CapTouch_Long_Pressed(uint8_t current_channel);
uint8_t  CapTouch_Constant_Pressed(uint8_t current_channel);
uint16_t CapTouch_Get_RawADC1(uint8_t current_channel);
uint16_t CapTouch_Get_RawADC2(uint8_t current_channel);
int      CapTouch_Get_RawDifferentialADC(uint8_t current_channel);
int 	   CapTouch_Get_Calibration_Value(uint8_t current_channel);
uint32_t CapTouch_Get_Touch_Duration(uint8_t current_channel);
uint8_t  CapTouch_Get_Touch_Response(uint8_t current_channel);
uint8_t  CapTouch_Get_Reference_Timer_Use_Flag(void);
uint8_t  CapTouch_Get_ErrorCode(uint8_t current_channel);
void 	   CapTouch_Get_Debug_Data(uint8_t current_channel, int *data_out);
void     CapTouch_Init(void);

