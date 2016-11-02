/*********************************************************************************************
* Fichero:	main.c
* Autor:
* Descrip:	punto de entrada de C
* Version:  <P4-ARM.timer-leds>
*********************************************************************************************/

/*--- ficheros de cabecera ---*/
#include "8led.h"
#include "button.h"
#include "led.h"
#include "timer.h"
#include "44blib.h"
#include "44b.h"
#include "timer2.h"

/*--- variables globales ---*/


/*--- codigo de funciones ---*/
void Main(void)
{
	/* Inicializa controladores */
	/* Default init code */
	sys_init();         // Inicializacion de la placa, interrupciones y puertos -> common/44blib.c
	timer_init();	    // Inicializacion del temporizador -> timer.c
	Eint4567_init();	// inicializamos los pulsadores. Cada vez que se pulse se verá reflejado en el 8led ->button.c
	D8Led_init();       // inicializamos el 8led -> 8led.c
	
	/* Valor inicial de los leds */
	leds_off();
	led1_on();

	/* Our init code */
	timer2_inicializar();
	timer2_empezar();


	

	while (1)
	{
		/* Cambia los leds con cada interrupcion del temporizador */
		if (switch_leds == 1)
		{
			leds_switch();
			switch_leds = 0;
		}
	}
}
