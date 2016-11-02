/*********************************************************************************************
* Fichero:		timer2.c
* Autores:		Oscar Leon Barbed Perez 666137 - Sergio Lazaro Magdalena 556030
* Descrip:		funciones de control del timer2 del s3c44b0x
* Version:
*********************************************************************************************/

/*--- ficheros de cabecera ---*/
#include "44b.h"
#include "44blib.h"
#include "timer2.h"

/*--- variables globales ---*/
int timer2_num_int = 0;
int switch_leds2 = 0;

/* declaración de función que es rutina de servicio de interrupción
 * https://gcc.gnu.org/onlinedocs/gcc/ARM-Function-Attributes.html */
void timer2_ISR(void) __attribute__((interrupt("IRQ")));

/*--- codigo de las funciones ---*/
void timer2_ISR(void)
{

	switch_leds2 = 1;

	/* Incremento numero de interrupciones timer2 */
	timer2_num_int++;

	/* borrar bit en I_ISPC para desactivar la solicitud de interrupción*/
	rI_ISPC |= BIT_TIMER2; // BIT_TIMER2 está definido en 44b.h y pone un uno en el bit 15 que correponde al Timer2
}

void timer2_inicializar(void)
{

	/* Configuraion controlador de interrupciones */
	rINTMOD = 0x0; // Configura las linas como de tipo IRQ
	rINTCON = 0x1; // Habilita int. vectorizadas y la linea IRQ (FIQ no)
	rINTMSK = ~(BIT_GLOBAL | BIT_TIMER2); // Enmascara todas las lineas excepto Timer2 y el bit global (bits 26 y 13, BIT_GLOBAL y BIT_TIMER0 están definidos en 44b.h)

	/* Establece la rutina de servicio para TIMER0 */
	pISR_TIMER2 = (unsigned) timer2_ISR;

	/* Configura el Timer2 */
	rTCFG0 = 0xff00; // ajusta el preescalado para TIMER2
	rTCFG1 = 0x0; // selecciona la entrada del mux que proporciona el reloj. La 00 corresponde a un divisor de 1/2.
	rTCNTB2 = 65535;// valor inicial de cuenta (la cuenta es descendente)
	rTCMPB2 = 12800;// valor de comparación
	/* establecer update=manual (bit 1) + inverter=on (¿? será inverter off un cero en el bit 2 pone el inverter en off)*/
	rTCON = 0x2000;
	/* iniciar timer (bit 0) con auto-reload (bit 3)*/
	rTCON = 0x09000;
}

void timer2_empezar()
{
	timer2_num_int = 0;
	rTCNTO2 = 0;
}
float timer2_leer()
{
	return (timer2_num_int*(rTCNTB2 - rTCMPB2) + rTCNTO2);
}

