// Define microcontroller type

// I2C configuration (using I2C2)

// ADS1231 Weight sensor pins
#define TRIS_ADS1231_Dout	                _TRISB11
#define PORT_ADS1231_Dout 	                _RB11
#define LAT_ADS1231_Dout 	                _LATB11
#define ODC_ADS1231_Dout	                _ODB11
#define PIN_ADS1231_Dout 	                _RB11
#define RPOUT_ADS1231_Dout		            RPOR5bits.RP11R
#define RPIN_ADS1231_Dout		            11
#define CN_ADS1231_Dout		                15
#define TRIS_ADS1231_Clk	                _TRISB12
#define PORT_ADS1231_Clk 	                _RB12
#define LAT_ADS1231_Clk 	                _LATB12
#define ODC_ADS1231_Clk	                _ODB12
#define PIN_ADS1231_Clk 	                _RB12
#define RPOUT_ADS1231_Clk		            RPOR6bits.RP12R
#define RPIN_ADS1231_Clk		            12
#define CN_ADS1231_Clk		                14
#define ADC_value_ADS1231_Clk              Buffer_entradas[12]
#define HAL_SetAnalog_ADS1231_Clk()        {_PCFG12=0;\
                                        adc_addAnalogChannel(12);}

#define TRIS_ADS1231_Pdwn	                _TRISB13
#define PORT_ADS1231_Pdwn 	                _RB13
#define LAT_ADS1231_Pdwn 	                _LATB13
#define ODC_ADS1231_Pdwn	                _ODB13
#define PIN_ADS1231_Pdwn 	                _RB13
#define RPOUT_ADS1231_Pdwn		            RPOR6bits.RP13R
#define RPIN_ADS1231_Pdwn		            13
#define CN_ADS1231_Pdwn		                13
#define ADC_value_ADS1231_Pdwn              Buffer_entradas[11] 
#define HAL_SetAnalog_ADS1231_Pdwn()        {_PCFG11=0;\
                                        adc_addAnalogChannel(11);}

#define TRIS_ADS1231_Speed	                _TRISB7
#define PORT_ADS1231_Speed 	                _RB7
#define PIN_ADS1231_Speed 	                _RB7
#define LAT_ADS1231_Speed 	                _LATB7
#define ODC_ADS1231_Speed	                _ODB7
#define RPOUT_ADS1231_Speed		            RPOR3bits.RP7R
#define RPIN_ADS1231_Speed		            7
#define CN_ADS1231_Speed		                23
#define TRIS_ADS1231_ON_Vcc	                _TRISB15
#define PORT_ADS1231_ON_Vcc 	                _RB15
#define LAT_ADS1231_ON_Vcc 	                _LATB15
#define ODC_ADS1231_ON_Vcc	                _ODB15
#define PIN_ADS1231_ON_Vcc 	                _RB15
#define RPOUT_ADS1231_ON_Vcc	                RPOR7bits.RP15R
#define RPIN_ADS1231_ON_Vcc	                15
#define CN_ADS1231_ON_Vcc		                11
#define ADC_value_ADS1231_ON_Vcc              Buffer_entradas[9] 
#define HAL_SetAnalog_ADS1231_ON_Vcc()        {_PCFG9=0;\
                                        adc_addAnalogChannel(9);}

// LED
#define TRIS_Led1	                _TRISB6
#define PIN_Led1 	                _RB6
#define LAT_Led1 	                _LATB6
#define ODC_Led1	                _ODB6
#define RPOUT_Led1		            RPOR3bits.RP6R
#define RPIN_Led1		            6
#define CN_Led1		                24

// Board name definition
