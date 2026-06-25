#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "inc/userFncFile.h"
#include "inc/systemTimer.h"
#include "inc/led_Led1.h"
#include "inc/timer_api1.h"
#include "inc/LoadCell.h"
#include "inc/conversionFunctions.h"
#include "inc/EMICBus.h"
#include "inc/Persist.h"

void onReset()
{
    LEDs_Led1_blink(100, 500, 3);
    setFormat(8, 2, " ");
    setCapacity(100.0);
    setmVxV(2.0);
    startWeighing();
}


void eStable()
{
    pI2C("WEIGHT\t$s", getPeso());
}


void eOverLoad()
{
    pI2C("OVERLOAD\t$s", getPesoBruto());
}



