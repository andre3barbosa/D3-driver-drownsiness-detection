#include "utils.h"
#include <linux/module.h>

void SetGPIOFunction(struct GpioRegisters *s_pGpioRegisters, int GPIO, int functionCode) {
	int registerIndex = GPIO / 10;  
	int bit = (GPIO % 10) * 3;  

	unsigned oldValue = s_pGpioRegisters->GPFSEL[registerIndex];
	unsigned mask = 0b111 << bit;

	s_pGpioRegisters->GPFSEL[registerIndex] = (oldValue & ~mask) | ((functionCode << bit) & mask);
}

void SetGPIOOutputValue(struct GpioRegisters *s_pGpioRegisters, int GPIO, bool outputValue) {
		 
	if (outputValue){
		s_pGpioRegisters->GPSET[GPIO / 32] = (1 << (GPIO % 32));
	}	
	else{
		s_pGpioRegisters->GPCLR[GPIO / 32] = (1 << (GPIO % 32));
	}
}