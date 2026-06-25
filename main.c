#include <xc.h>
#include "inc/systemConfig.h"

#include "inc/systemTimer.h"
#include "inc/led_Led1.h"
#include "inc/timer_api1.h"
#include "inc/LoadCell.h"
#include "inc/conversionFunctions.h"
#include "inc/EMICBus.h"
#include "inc/Persist.h"
#include "inc/system.h"
#include "inc/userFncFile.h"

#include "system.c"

int main(void)
{
	initSystem();
	systemTimeInit();
	LEDs_Led1_init();
	init_LoadCell();
	EMICBus_init();
	onReset();
	do
	{
		LEDs_Led1_poll();
		poll_LoadCell();
		poll_EMICBus();
	}
	while(1);
}

