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
#include "exception.h"
#include "sudoku_2016.h"

/*--- variables globales ---*/

/**
 * Asignacion variables aqui sino error de compilacion.
 * Errores si asignamos cuadricula en fichero .h
 */
CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS]__attribute__((aligned(4))) = {
	{0x8005,0x0000,0x0000,0x8003,0x0000,0x0000,0x0000,0x0000,0x0000,0,0,0,0,0,0,0},
	{0x0000,0x0000,0x0000,0x0000,0x8009,0x0000,0x0000,0x0000,0x8005,0,0,0,0,0,0,0},
	{0x0000,0x8009,0x8006,0x8007,0x0000,0x8005,0x0000,0x8003,0x0000,0,0,0,0,0,0,0},
	{0x0000,0x8008,0x0000,0x8009,0x0000,0x0000,0x8006,0x0000,0x0000,0,0,0,0,0,0,0},
	{0x0000,0x0000,0x8005,0x8008,0x8006,0x8001,0x8004,0x0000,0x0000,0,0,0,0,0,0,0},
	{0x0000,0x0000,0x8004,0x8002,0x0000,0x8003,0x0000,0x8007,0x0000,0,0,0,0,0,0,0},
	{0x0000,0x8007,0x0000,0x8005,0x0000,0x8009,0x8002,0x8006,0x0000,0,0,0,0,0,0,0},
	{0x8006,0x0000,0x0000,0x0000,0x8008,0x0000,0x0000,0x0000,0x0000,0,0,0,0,0,0,0},
	{0x0000,0x0000,0x0000,0x0000,0x0000,0x8002,0x0000,0x0000,0x8001,0,0,0,0,0,0,0}
};

uint16_t opcion = 0;

/*--- codigo de funciones ---*/

void Main(void)
{
	/* Inicializa controladores */
	sys_init();         // Inicializacion de la placa, interrupciones y puertos -> common/44blib.c
	timer_init();	    // Inicializacion del temporizador -> timer.c
	timer2_inicializar();
	Eint4567_init();	// inicializamos los pulsadores. Cada vez que se pulse se verá reflejado en el 8led ->button.c
	D8Led_init();       // inicializamos el 8led -> 8led.c

	//Init exceptions
	Exception_init();

	/* Valor inicial de los leds */
	led1_on();
	leds_off();

	long t1, t2;
	int test;
	test = 3;
	while (1)
	{

		if (test == 1)	//test timer2
		{
			timer2_empezar();
			t1 = timer2_leer();
			Delay(10000);
			t2 = timer2_leer();

		}
		else if(test == 2) //test Udef Exception
		{
			//Udef exception
			asm volatile(".word 0xe7f000f0\n");
		}
		else if(test == 3)	//test tiempos sudoku
		{
			// C - C
			char *ready = "hola";
			timer2_empezar();
			t1 = timer2_leer();
			sudoku9x9(cuadricula, ready);
			t2 = timer2_leer() - t1;

		}
	}
}
