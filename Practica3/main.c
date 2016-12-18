/*********************************************************************************************
* Fichero:	main.c
* Descrip:	punto de entrada de C
* Version:  <P4-ARM.timer-leds>
* Autor1: Oscar Leon Barbed Perez 666137
* Autor2: Sergio Lazaro Magdalena 556030
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
#include "pila_depuracion.h"
#include "timer4.h"
#include "tp.h"
#include "lcd.h"

/*--- variables globales ---*/

CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS]__attribute__((aligned(16))) = {
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

int status;
int fila;
int columna;
int valor;
int int_count;

#define PLACA ;		//CONDICION

/* Declaracion de funciones*/
void automata(void);
void iniciar_placa(void);

/* Declaracion funciones estado del automata */
void esperar_confirmacion_fila(int button);
void seleccionar_valor_fila(int button);
void esperar_confirmacion_columna(int button);
void seleccionar_valor_columna(int button);
void seleccionar_valor_celda(int button);
extern void Lcd_Test();
/*--- codigo de funciones ---*/


void iniciar_placa(void)
{
	/* Inicializa controladores */
	sys_init();         // Inicializacion de la placa, interrupciones y puertos -> common/44blib.c

	timer_init();	    // Inicializacion del temporizador
	D8Led_init(); 		// inicializamos el 8led

	timer2_inicializar();
	timer2_empezar();
	timer4_inicializar();

	Eint4567_init();	// inicializamos los pulsadores. Cada vez que se pulse se verá reflejado en el 8led

	Lcd_Init();
	Lcd_Test(cuadricula);

	TS_init();
	//char yn;
	int estoy_zoom;
	estoy_zoom = 0;
	while(1)
	 {
	   //yn = Uart_Getch();
		if(estoy_zoom && activar_zoom){	//Estoy en zoom y salgo
			Lcd_Test(cuadricula);
			estoy_zoom = 0;
			activar_zoom = 0;
		}
		else if(activar_zoom){	//Entro en zoom
			Lcd_zoom_region(cuadricula, x_elegida, y_elegida);
			activar_zoom = 0;
			estoy_zoom = 1;
		}
		Lcd_print_tiempo_total(timer0_leer());
	   //if(yn == 0x52) TS_Test();// R to reset the XY REC
	   //else break;
	 }

	TS_close();
	//Init exceptions
	Exception_init();
	//Init stack
	stack_init();
}

void Main(void)
{
	/**
	 * CON O2 NO HAY DELAYS
	 */

	sudoku9x9(cuadricula,"A");

	valor = 0;
	fila = 0;
	columna = 0;
	int_count = 0;
	status = 0;

	int test;	//Variable empleada para comprobar las distintas
				//etapas de la practica

#ifdef PLACA
	iniciar_placa();
	test = 0;	//Opcion para tests
	long t1, t2;

	//Init variables globales automata boton
	//int status, fila, columna, valor, test, int_count;

	if(test == 0){ //ejecución normal
		automata();
	}

	else if (test == 1)	//test timer2
	{
		timer2_empezar();
		t1 = timer2_leer();
		Delay(10000);
		t2 = timer2_leer();

	}
	else if(test == 2) //test Udef Exception
	{

		/*--- instrucciones para prueba de excepciones ---*/
		//Udef exception
		//asm volatile(".word 0xe7f000f0\n");

		//__asm__("mov pc,lr");

		//SWI exception
		//__asm__("swi 8");

		//__asm__("mov r4,#0x00000001");
		//__asm__("ldr r4,[r4]");

		//DAbort exception
		//Usar puntero antes de inicializacion

		//PAbort exception
		//Salto a una direccion que no contiene una instruccion
	}
	else if(test == 3)	//test tiempos sudoku
	{
		char *ready = "hola";
		//timer2_empezar();
		t1 = timer2_leer();
		sudoku9x9(cuadricula, ready);	//Actualmente ejecuta ARM - ARM
		t2 = timer2_leer() - t1;

	}
	else if(test == 4)	//test pila depuracion
	{
		int i;
		for(i = 0; i < 40; i++)
		{
			push(12,13);

		}
	}

#else

	//fila = 6, columna = 5, valor = 5 -> valor correcto
	sudoku_candidatos_modificar(cuadricula, 5, 4, 5);

	//fila = 6, columna = 5, valor = 0 -> borrar valor
	sudoku_candidatos_modificar(cuadricula, 5, 4, 0);

	//fila = 6, columna = 5, valor = 2 -> valor erroneo -> propagar errores
	sudoku_candidatos_modificar(cuadricula, 5, 4, 2);

	//fila = 6, columna = 6, valor = 2 -> dos valores erroneos -> propagar errores
	sudoku_candidatos_modificar(cuadricula, 5, 6, 2);

	//fila = 6, columna = 5, valor = 5 -> valor correcto sobre incorrecto -> propagar
	sudoku_candidatos_modificar(cuadricula, 5, 4, 5);


	/**
	*	SOLUCION SUDOKU
	*
	*	---------------------------------
	*	5 , 1 , 7 | 3 , 2 , 8 | 9 , 4 , 6 
	*	8 , 2 , 3 | 4 , 9 , 6 | 7 , 1 , 5 
	*	4 , 9 , 6 | 7 , 1 , 5 | 8 , 3 , 2
	*	---------------------------------
	*	2 , 8 , 1 | 9 , 4 , 7 | 6 , 5 , 3
	*	7 , 3 , 5 | 8 , 6 , 1 | 4 , 2 , 9
	*	9 , 6 , 4 | 2 , 5 , 3 | 1 , 7 , 8
	*	---------------------------------
	*	1 , 7 , 8 | 5 , 3 , 9 | 2 , 6 , 4
	*	6 , 5 , 2 | 1 , 8 , 4 | 3 , 9 , 7
	*	3 , 4 , 9 | 6 , 7 , 2 | 5 , 8 , 1
	*	---------------------------------
	*/

#endif

}

/*********************************************************************
 *
 * FUNCIONES ESTADOS AUTOMATA JUEGO
 *********************************************************************/

/**
 * Estado inicial. Pone F en 8led y espera confirmacion
 * Boton derecho -> confirma
 */
void esperar_confirmacion_fila(int button)
{
	D8Led_symbol(0xF);
	push(14,button);
	if(button == 0x80)
	{
		status = 1;
		int_count = 1;
		D8Led_symbol(int_count);
		setBotonPulsado(0);
	}
}

/**
 * Estado seleccion valor fila.
 * Boton izquierda -> incrementa
 * Boton derecho -> confirma
 */
void seleccionar_valor_fila(int button)
{
	push(10,button);
	if(button == 0x80)
	{
		if(int_count == 9)
		{
			int_count = 0;
		}
		int_count++; // incrementamos el contador

		D8Led_symbol(int_count);	//Display valor actual
		setBotonPulsado(0);
	}
	else if(button == 0x40)
	{
		fila = int_count;
		D8Led_symbol(0xC);
		status = 2;
		setBotonPulsado(0);
	}
}

/**
 * Estado espera confirmacion columna.
 * Boton derecho -> confirma
 */
void esperar_confirmacion_columna(int button)
{
	if(button == 0x80)
	{
		status = 3;
		int_count = 1;
		D8Led_symbol(int_count);
		setBotonPulsado(0);
	}
}
/**
 * Estado seleccion valor columna.
 * Boton izquierda -> incrementa
 * Boton derecho -> confirma
 * Si la celda elegida es una pista no pide valor
 */
void seleccionar_valor_columna(int button)
{
	if(button == 0x80)
	{
		if(int_count == 9)
		{
			int_count = 0;
		}
		int_count++; // incrementamos el contador

		D8Led_symbol(int_count);
		setBotonPulsado(0);
	}
	else if(button == 0x40)
	{
		columna = int_count;
		if(!comprobar_celda_pista(cuadricula[fila-1][columna-1])){
			int_count = 0;
			D8Led_symbol(int_count);
			status = 4;
		}
		else{
			D8Led_symbol(14);	//Mostrar E
			Delay(10000);
			status = 0;
		}
		setBotonPulsado(0);
	}
}

/**
 * Estado seleccion valor celda.
 * Boton izquierda -> incrementa [0:9]
 * Boton derecho -> confirma
 */
void seleccionar_valor_celda(int button)
{
	if(button == 0x80)
	{
		if(int_count == 9)
		{
			int_count = -1;
		}
		int_count++; // incrementamos el contador

		D8Led_symbol(int_count);
		setBotonPulsado(0);
	}
	else if(button == 0x40)
	{
		valor = int_count;
		sudoku_candidatos_modificar(cuadricula, fila - 1, columna - 1 , valor);
		D8Led_symbol(11);	//Display 0 para test
		//Llamada a propagar
		Delay(10000);		//Delay para ver el 0
		status = 0;
		setBotonPulsado(0);
	}
}

void automata()
{
	while(1)
	{
		switch (status)
		{
			case 0:
				esperar_confirmacion_fila(getBotonPulsado());
				break;
			case 1:
				seleccionar_valor_fila(getBotonPulsado());
				break;
			case 2:
				esperar_confirmacion_columna(getBotonPulsado());
				break;
			case 3:
				seleccionar_valor_columna(getBotonPulsado());
				break;
			case 4:
				seleccionar_valor_celda(getBotonPulsado());
				break;
		}

	}


}
