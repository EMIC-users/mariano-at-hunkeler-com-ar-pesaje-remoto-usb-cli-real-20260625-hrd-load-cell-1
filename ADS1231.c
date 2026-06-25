
/*==================[inclusions]=============================================*/
#include "inc/ADS1231.h"
#include "inc/gpio.h"
#include "inc/system.h"  // Must be included before libpic30.h (defines FOSC and FCY)
#include <libpic30.h>    // For __delay_ms() and __delay_us()

/*==================[Variables definitions]==================================*/
uint8_t ADS123xFlag_adc=0;

void __attribute__((interrupt,auto_psv)) _INT1Interrupt(void)
{
	IFS1bits.INT1IF = 0; 
	IEC1bits.INT1IE = 0;
	
	//uint8_t flagPrimInt=0;//para filtrar la primer int
	unsigned char i;
	int32_t data32 = 0;
	HAL_GPIO_PinSet(ADS1231_Clk, GPIO_HIGH);
	
	data32 = data32 << 1;
	__delay_us(5);
  		HAL_GPIO_PinSet(ADS1231_Clk, GPIO_LOW);
	__delay_us(5);
	
	if (PORT_ADS1231_Dout)
		data32 =0xffffffff;

	for(i=0;i<16;i++)
	{
		HAL_GPIO_PinSet(ADS1231_Clk, GPIO_HIGH);
		data32 = data32 << 1;
		__delay_us(5);
   		HAL_GPIO_PinSet(ADS1231_Clk, GPIO_LOW);
		__delay_us(5);
		if (PORT_ADS1231_Dout)
			data32 |= 0x0001;
	}

	for(i=0;i<8;i++)
	{
		HAL_GPIO_PinSet(ADS1231_Clk, GPIO_HIGH);
		__delay_us(5);
   		HAL_GPIO_PinSet(ADS1231_Clk, GPIO_LOW);
		__delay_us(5);
	}
		
	nuevaLectura(data32);
	
	ADS123xFlag_adc = 1;
	
	IFS1bits.INT1IF = 0; 
	IEC1bits.INT1IE = 1;
	
	return;
}	

void init_ADS1231(void)
{
	HAL_GPIO_PinCfg(ADS1231_Pdwn, GPIO_OUTPUT);
	HAL_GPIO_PinCfg(ADS1231_Clk, GPIO_OUTPUT);
	HAL_GPIO_PinCfg(ADS1231_Speed, GPIO_OUTPUT);
	HAL_GPIO_PinCfg(ADS1231_Dout, GPIO_INPUT);
	
	HAL_GPIO_PinSet(ADS1231_Pdwn, GPIO_HIGH);
	HAL_GPIO_PinSet(ADS1231_Clk, GPIO_LOW);
#ifdef PORT_ADS1231_ON_Vcc
	HAL_GPIO_PinSet(ADS1231_ON_Vcc, GPIO_LOW);	
#endif

	/* Configure Input Functions (Table 10-2)) */
	__builtin_write_OSCCONL(OSCCON & 0xBF);
	_INT1R = RPIN_ADS1231_Dout;
	//SETEXT1;
	INTCON2bits.INT1EP = 1;
	IPC5bits.INT1IP1 = 1; // set interrupt priority
	IEC1bits.INT1IE = 1; // external interrupt enabled
	
	/* Lock Registers */
	__builtin_write_OSCCONL(OSCCON | 0x40);

	__delay_ms(50);

	ADS1231_speed(ADS1231_SPEED_10);
	
	__delay_ms(15);
}

void ADS1231_speed(uint8_t speed)
{
	HAL_GPIO_PinSet(ADS1231_Speed, speed);
}

