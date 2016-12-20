/*********************************************************************************************
* Fichero:	button.c
* Descrip:	Funciones de manejo de los pulsadores (EINT6-7)
* Version:
* Autor1: Oscar Leon Barbed Perez 666137
* Autor2: Sergio Lazaro Magdalena 556030
*********************************************************************************************/

/*--- ficheros de cabecera ---*/
#include "button.h"
#include "8led.h"
#include "44blib.h"
#include "44b.h"
#include "def.h"
#include "timer4.h"

/* Declaracion variables globales
 *
 * VARIABLES GLOBALES CON VOLATILE SI SE EMPLEAN EN ISR
 * VOLATILE PERMITE MODIFICAR VARIABLE EN OTRO PROGRAMA
 * O HARDWARE
 *
 * */

/* declaración de función que es rutina de servicio de interrupción
 * https://gcc.gnu.org/onlinedocs/gcc/ARM-Function-Attributes.html */
void Eint4567_ISR(void) __attribute__((interrupt("IRQ")));

/*--- codigo de funciones ---*/
void Eint4567_ISR(void)
{


	/* Identificar la interrupcion (hay dos pulsadores)*/
	which_int = rPDATG & 0xc0;
	boton_pulsado = which_int;
	restart_status_timer4();	//Necesario tras añadir estado END
	//rEXTINTPND = 0;
	rINTMSK |= BIT_EINT4567;
	//rINTMSK    = ~(BIT_GLOBAL | BIT_EINT4567 | BIT_TIMER4 | BIT_TIMER2 | BIT_TIMER0);
	//iniciar timer aqui
	timer4_empezar();

	//D8Led_symbol(int_count & 0x000f); // sacamos el valor por pantalla (módulo 16)

	/* Finalizar ISR */
	rEXTINTPND = 0xf;				// borra los bits en EXTINTPND
	rI_ISPC   |= BIT_EINT4567;		// borra el bit pendiente en INTPND
}

void Eint4567_init(void)
{
	/* Configuracion del controlador de interrupciones. Estos registros están definidos en 44b.h */
	rI_ISPC    = 0x3ffffff;	// Borra INTPND escribiendo 1s en I_ISPC
	rEXTINTPND = 0xf;       // Borra EXTINTPND escribiendo 1s en el propio registro
	rINTMOD    = 0x0;		// Configura las linas como de tipo IRQ
	rINTCON    = 0x1;	    // Habilita int. vectorizadas y la linea IRQ (FIQ no)
	rINTMSK    = ~(BIT_GLOBAL | BIT_TIMER4 | BIT_TIMER2 | BIT_TIMER0 | BIT_EINT4567); // Enmascara todas las lineas excepto...

	/* Establece la rutina de servicio para Eint4567 */
	pISR_EINT4567 = (int) Eint4567_ISR;

	/* Configuracion del puerto G */
	rPCONG  = 0xffff;        		// Establece la funcion de los pines (EINT0-7)
	rPUPG   = 0x0;                  // Habilita el "pull up" del puerto
	rEXTINT = rEXTINT | 0x22222222;   // Configura las lineas de int. como de flanco de bajada

	/* Por precaucion, se vuelven a borrar los bits de INTPND y EXTINTPND */
	rI_ISPC    |= (BIT_EINT4567);
	rEXTINTPND = 0xf;

	which_int = 0;
	boton_pulsado = 0;
}

void setBotonPulsado(int value)
{
	boton_pulsado = value;
}

int getBotonPulsado()
{
	return boton_pulsado;
}

void setWhichInt(int value)
{
	which_int = value;
}

int getWhichInt()
{
	return which_int;
}
