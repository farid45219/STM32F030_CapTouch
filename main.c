
#include "stm32f0xx.h"
#include "captouch.h"

int main(void){
	
	CapTouch_Init();
	

	while(1){
		CapTouch_Scan_Sensors();
		
		int data_out;
		CapTouch_Get_Debug_Data(0, data_out);
		for(uint8_t i=0; i<data_out[0]; i++){
			UART_PRint_Text("Raw1");
			UART_PRint_Number(data_out[1]);
			UART_PRint_Text("Raw2");
			UART_PRint_Number(data_out[2]);
			UART_PRint_Text("Diff");
			UART_PRint_Number(data_out[3]);
			UART_PRint_Text("Calib");
			UART_PRint_Number(data_out[4]);
			UART_PRint_Text("Duration");
			UART_PRint_Number(data_out[5]);
			UART_PRint_Text("Touched");
			UART_PRint_Number(data_out[6]);
		}
		//some delay
	}
}


