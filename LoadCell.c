
/*==================[inclusions]=============================================*/
#include "inc/LoadCell.h"
#include "inc/ADS1231.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*==================[Variables definitions]==================================*/

//Variables accesibles para el usuario.
float persist_ Capacidad;
float persist_ mVxV_cal;
float mVxV;
int32_t filterOut;

//Variables no accesibles para el usuario.
char Peso_neto[16];                 //Current net weight value.
char Peso_bruto[16];                //Current gross weight value.
char Peso_tara[16];                 //Current tare value.

char filter_in[16];                 //
char filter_out[16];                //



float Peso_neto_f;                  //Current net weight value.
float Peso_bruto_f;                 //Current gross weight value.
int32_t nuevo_valor;

int32_t persist_ filterThreshold;   //Umbral del filtro de estabilizacion.
float persist_ Peso_tara_f;         //Current tare weight value.
float persist_ K;                   //Slope of the linear function which describes the load cell.
int32_t persist_ Cero;              //Intercept of the linear function which describes the load cell.
float persist_ Desviacion_cero;     //Is the simple variance of the measures in the cero point.
//int32_t persist_ Tara;              //Auxiliar term that modify the linear function which describes the load cell.
float persist_ Desviacion_tara;     //Is the simple variance of the measures in the tare point.
int32_t persist_ Corrimiento;       //Offset due to the deformation of the load cell.

int32_t Historial[HistoryLength];    //Circular FIFO used to eliminate the noise of the load cell measure.
int32_t ValorActual;                //Is the media of all values contains in the FIFO.
float Varianza;                     // Is the simple variance of the values contains in the FIFO.
int64_t Acumulador;                 //Is the sumatory of all values contains in the FIFO.

int8_t Indice = 0;                  //Position of the new value of the FIFO.

uint8_t persist_ digitos;
uint8_t persist_ decimales;
char persist_ relleno;

//static int32_t corrimiento = 0;
static int32_t pesoEstable = 0;

uint8_t Lecturas = 0;

int8_t Balanza_flags;

float getDevStd(void);

/*==================[Function implementations]===============================*/

void init_LoadCell(void)
{
#ifdef PORT_ADS1231_Dout
	init_ADS1231();
#endif
  Balanza_flags = 0;

  for (int i = 0; i < 32; i++)
  {
      Historial[i] = 0;
  }

  ValorActual = 0;
  Varianza = 0;

  Acumulador = 0;
  digitos = 5;
  decimales = 2;
  relleno = '0';

  filterThreshold = 50;

  K = 1.0;
  Cero = 0;
  Capacidad = 99999999999;
}

void setZero(void)
{
  if (Balanza_flags & 0x01)                 //If the measure is stable.
  {
    Cero = pesoEstable;
    //Tara = 0;
    Desviacion_tara = getDevStd();
    Peso_tara_f = 0;
    for (int i = 0; i < HistoryLength; i++)
    {
      Historial[i] = 0;
    }
    Balanza_flags &= 0b11111110;
    Indice = 0;
    Acumulador = 0;
    Lecturas = HistoryLength; //0;

  }
  else
  {
  }
}

void setReference(float Peso_de_referencia)
{
  if (Balanza_flags & 0x01)                               //If the measure is stable.
  {
    if ((pesoEstable - Cero) > 10)
    {
        K = Peso_de_referencia/(float)(pesoEstable - Cero);   //Determines the slope of the linear function that represent the load cell.
        Balanza_flags &= 0b11111110;
    }
  }
  else
  {
  }
}


void setThreshold(float Filter_threshold)
{
  filterThreshold =   (int32_t)(Filter_threshold / K);
}

void setTara(void)
{
  if (Balanza_flags & 0x01)                 //If the measure is stable.
  {
    //Tara = pesoEstable;
    //Desviacion_tara = getDevStd();
    Peso_tara_f = Peso_bruto_f; // (float) Tara * K;
  }
  else
  {
  }
}

void setCapacity(float Capacity)
{
  Capacidad = Capacity;
}

void setmVxV(float mVxV)
{
  mVxV_cal = mVxV;
}

void setFormat(uint8_t digits, uint8_t decimals, char fill_with)
{
  digitos = digits + decimals + 1;
  decimales = decimals;
  relleno = fill_with;
}


void startWeighing()
{
  Balanza_flags |= F_StartWeighing;
}

void nuevaLectura(int32_t adcValue)
{
  static int conta = 0;
  static int32_t signDiff = 0;
  int32_t viejo_valor;
  int32_t promedio;


  nuevo_valor = adcValue;

  //if (Lecturas < HistoryLength)
  //  Lecturas++;

  viejo_valor = Historial[Indice];              //Obtengo el valor mas antiguo ...
  Acumulador -= Historial[Indice];              //... y se lo resto al acumulador
  Acumulador += nuevo_valor;                    //Le sumo al acumulador el nuevo valor
  Historial[Indice] = nuevo_valor;

  Indice++;                                       // Se incrementa el indice de ll cola circular
  Indice %= HistoryLength;                        //.. en caso de ser necesario . Clamp the index between 0 and HistoryLength.

  filterOut = nuevo_valor - viejo_valor;              // Se extrae en filterOut la diferencia entre el nuevo y el mas antiguo valor.
  promedio = Acumulador / HistoryLength;

  if (Balanza_flags & F_StartWeighing)
  {
    Balanza_flags = 0;
    conta = 0;
  }


  if ((Balanza_flags & 0b00000001) && (abs(pesoEstable - promedio ) > filterThreshold ) )
  {
      Balanza_flags &= 0b11111110;                  // ...el peso actual no es estable
      Peso_bruto_f = (float)(nuevo_valor - Cero) * K;
      conta = 0;
  }


  else if ( abs(    filterOut         ) < filterThreshold &&
            abs(promedio - nuevo_valor) < filterThreshold &&
            abs(promedio - viejo_valor) < filterThreshold
           )           // Si la deferencia es menor que el  umbral ....
  {

       // Para detectar un maximo o minimo (en la medicion) - Se detecta un cambio de signo en la pendiente

      if ( (conta == 0) && (
           ( (promedio >= nuevo_valor) && (promedio >= viejo_valor) ) ||
           ( (promedio <= nuevo_valor) && (promedio <= viejo_valor) )   )
         )

       {

        conta = 1;


        // if (filterOut < 0)                                // En caso que la medicion está de-creciendo
        // {
        //   if (signDiff > 0)                               // ... y la ultima medicion inestable era creciente ...
        //   {
        //     conta = 1;                                    // ... Comienza el conteo de la estabilizacion.
        //   }
        // }
        // if (filterOut > 0)                                // En caso que la medicion está creciendo
        // {
        //   if  (signDiff < 0)                              // ... y la ultima medicion inestable era de-creciente ...
        //   {
        //       conta = 1;                                  // ... Comienza el conteo de la estabilizacion.
        //   }
        // }
        // else
        // {
        //   conta = 1;
        // }
        // Peso_bruto_f = (float)(nuevo_valor - Cero) * K;
      }


      else if (conta > 5)                                // ... durante cinco lecturas concecutivas ...
      {
        if ((Balanza_flags & 0b00000001) == 0)
        {
          Balanza_flags |= 0b00000001;                // ... se establece el peso actual como estable.
          pesoEstable = (promedio);
          Peso_bruto_f = (float)(pesoEstable - Cero) * K;
        }
      }
      else if (conta > 0)                             // Verifico que la variacion llego a un maximo o minimo.
      {
        conta++;
        Peso_bruto_f = (float)(nuevo_valor - Cero) * K;
      }
    }
  else                                            // Si la deferencia es MAYOR al umbral ....
  {
    Balanza_flags &= 0b11111110;                  // ...el peso actual no es estable
    signDiff = nuevo_valor - viejo_valor;         // Se guarda la diferencia entre el las lecturas mas nueva y mas vieja en el momento de variaciones rapidas
    Peso_bruto_f = (float)(nuevo_valor - Cero) * K;
    conta = 0;
  }



  Peso_neto_f = Peso_bruto_f - Peso_tara_f;              //Obtains the media of the values contains in the FIFO.

  mVxV = ( ValorActual * 62500 ) / ( 128 * 65536 / 16 );

}

float getDevStd(void)
{
  uint64_t accum = 0;
  for (int i = 0; i < Lecturas; i++)
    accum += pow((Historial[i] - pesoEstable),2);

  return sqrt((float)accum/(float)(Lecturas-1));
}

void calcularCorrimiento(void)
{

}

char* getPeso(void)
{
  //static float PesoNetoBkp = -999999999999.0;
  //char* i;
  //if (Peso_neto_f != PesoNetoBkp)
  //{
  if (relleno == '0')
  {
    sprintf(Peso_neto, "%0*.*f", digitos, decimales, (double) Peso_neto_f);
  }
  else
  {
    sprintf(Peso_neto, "%*.*f", digitos, decimales, (double) Peso_neto_f);
  }

//    sprintf(Peso_neto, "%5.0f", Peso_neto_f);
    //i = Peso_neto;
    //do
    //{
    //  if (*i == ' ')
    //    *i = relleno;
    //  i++;
    //} while (*i == ' ' );
  //}
  return Peso_neto;
}

char* getFilterIn(void)
{
  float aux = (float)(nuevo_valor - Cero) * K;

  if (relleno == '0')
  {
    sprintf(filter_in, "%0*.*f", digitos, decimales,(double) aux);
  }
  else
  {
    sprintf(filter_in, "%*.*f", digitos, decimales, (double) aux);
  }

  return filter_in;
}

char* getFilterOut(void)
{
  float aux = (float)(Acumulador / HistoryLength - Cero) * K;
  if (relleno == '0')
  {
    sprintf(filter_out, "%0*.*f", digitos, decimales,(double) aux);
  }
  else
  {
    sprintf(filter_out, "%*.*f", digitos, decimales,(double) aux);
  }
  return filter_out;
}


char* getPesoBruto(void)
{

  if (relleno == '0')
  {
    sprintf(Peso_bruto, "%0*.*f", digitos, decimales, (double)Peso_bruto_f);
  }
  else
  {
    sprintf(Peso_bruto, "%*.*f", digitos, decimales, (double)Peso_bruto_f);
  }


  return Peso_bruto;
}

char* getTara(void)
{
  static float PesoTaraBkp = -999999999999.0;
  char* i;
  if (Peso_tara_f != PesoTaraBkp)
  {
    sprintf(Peso_tara, "%*.*f", digitos, decimales, (double)Peso_tara_f);

    i = Peso_tara;
    do
    {
      if (*i == ' ')
        *i = relleno;
      i++;
    } while (*i == ' ' );
  }
  return Peso_tara;
}

void poll_LoadCell(void)
{

  if ((Balanza_flags & F_Stable))            //If the measure is stable.
  {
    if (!(Balanza_flags & F_StableEventTrigger))
    {
      //if ((((pesoEstable - Tara) & 0x00FFFFFF) <= (int32_t)(4 * Desviacion_tara)))  //If the measure can considerate near to zero.
      if (Peso_neto_f == 0)  //If the measure can considerate near to zero.
      {
      }
      else
      {
        eStable();                    //Executes the stable event.
      }
      if (Peso_bruto_f < Capacidad)
      {
        Balanza_flags |= F_StableEventTrigger;        //Stable event trigger.
        Balanza_flags &= 0x0F;                        //Clears the triggers.
      }
      else
      {
        Balanza_flags |= F_StableEventTrigger;        //Stable event trigger.
        Balanza_flags &= 0x2F;                        //Clears the triggers, except the overload trigger.
      }

    }
  }
  else
  {
    if (!(Balanza_flags & F_UnstableEventTrigger))    //If the measure is unstable
    {
      Balanza_flags = F_UnstableEventTrigger;        //Unstable event trigger.
    }
  }

  if ((Peso_bruto_f >= Capacidad) && !(Balanza_flags & F_OverloadEventTrigger))   //If the measure is greater that the maximun capacity of the load cell.
  {
    Balanza_flags |= F_OverloadEventTrigger;          //Overload event trigger.
    eOverLoad();                    //Execute the overload event.
  }
}

