#include <xc.h>
#define persist_ __attribute__((section(".persist")))
#define persistArray_ __attribute__((section(".persistArray")))

#define HistoryLength 32

//Variables accesibles para el usuario (declaradas como extern)
extern float Capacidad;
extern float mVxV;
extern float mVxV_cal;
extern int32_t filterOut;

//Flags de estado
extern int8_t Balanza_flags;

#define F_Stable 0x01
#define F_StableEventTrigger 0x04
#define F_UnstableEventTrigger 0x10
#define F_OverloadEventTrigger 0x20
#define F_StartWeighing 0x80

//Funciones accesibles para el usuario

void setZero(void);                           //Sets the current value as zero.
void setReference(float Peso_de_referencia);  //Defines the linear function that represent the load cell using the weight given as parameter.
void setThreshold(float Filter_threshold);    //Sets the current value of filter's threshold.
void setTara(void);                           //Sets the current value as tare.
void setCapacity(float Capacity);             //Sets the value of Capacidad.
void setmVxV(float mVxV);                     //Sets the value of mVxV_cal.
void setFormat(uint8_t digits, uint8_t decimals, char fill_with); //Configure the representation of the weights as strings.
void startWeighing();
char* getPeso(void);                          //Gets the current value of the net weight.
char* getFilterIn(void);                      //Gets the filter in value of weight.
char* getFilterOut(void);                     //Gets the filter out value of weight.
char* getPesoBruto(void);                     //Gets the current value of the gross weight.
char* getTara(void);                          //Gets the current value of the tare weight.

//Funciones de inicialización y polling

void init_LoadCell(void);  //Initializes the correspondent variables for a correct work of the module.

void poll_LoadCell(void);  //Iteration when the module logic is implemented.

//Eventos

extern void eStable(void);                  //Is executed when the weight value is stable after a variation.
extern void eOverLoad(void);                //Is executed when the weight value is greater than the maximum capacity of the load cell.
//----------------------------------------------------

