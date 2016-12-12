/*********************************************************************************************
* Fichero:		timer2.c
* Descrip:		funciones de control del timer2 del s3c44b0x
* Version:
* Autor1: Oscar Leon Barbed Perez 666137
* Autor2: Sergio Lazaro Magdalena 556030
*********************************************************************************************/

/*--- ficheros de cabecera ---*/
#include "44b.h"
#include "44blib.h"
#include "timer2.h"

/*--- variables globales ---*/
int timer2_num_int = 0;

/* declaración de función que es rutina de servicio de interrupción
 * https://gcc.gnu.org/onlinedocs/gcc/ARM-Function-Attributes.html */
void timer2_ISR(void) __attribute__((interrupt("IRQ")));

/*--- codigo de las funciones ---*/
void timer2_ISR(void)
{

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

	// Enmascara todas las lineas excepto Timer2, Timer0 y el bit global
	// (bits 26 y 13, BIT_GLOBAL y BIT_TIMER0 están definidos en 44b.h)
	// bit = 0 -> interrupcion disponible
	// bit = 1 -> interrupcion enmascarada
	rINTMSK = ~(BIT_GLOBAL | BIT_TIMER2 | BIT_TIMER0);
	/* Establece la rutina de servicio para TIMER0 */
	pISR_TIMER2 = (unsigned) timer2_ISR;

	/* Configura el Timer2 */
	//bits[15:8]
	rTCFG0 = (rTCFG0 & 0xFFFF00FF); // ajusta el preescalado

	//bits [11:8]
	//selecciona la entrada del mux que proporciona el reloj. La 00 corresponde a un divisor de 1/2.
	rTCFG1 = (rTCFG1 & 0xFFFFF0FF);
	rTCNTB2 = 65535;// valor inicial de cuenta (la cuenta es descendente)
	rTCMPB2 = 0;// valor de comparación
	/* establecer update=manual (bit 1) + inverter=on
	(¿? será inverter off un cero en el bit 2 pone el inverter en off)*/
	// update manual y autoreload.
	//timer2 bits [15:12]
	//bit 15 -> auto reload on/off
	//bit 14 -> output inverte on/off
	//bit 13 -> manual update on/off
	//bit 12 -> start/stop
	rTCON = rTCON | 0x2000;
	/* iniciar timer (bit 0) con auto-reload (bit 3) -> 1001 */
	rTCON = rTCON | 0x9000;
}

void timer2_empezar()
{
	timer2_num_int = 0;
	rTCNTO2 = rTCNTB2;
	//Volvemos a poner update manual para modificar
	rTCON = rTCON | 0x2000;
	//Volvemos al valor que nos interesa
	rTCON = rTCON & 0x9FFF;


}
long timer2_leer()
{
	return ((timer2_num_int*(rTCNTB2 - rTCMPB2) + (rTCNTB2 - rTCNTO2))) / 32;	//microsegundos
}
