/*********************************************************************************************
* Fichero:		timer4.c
* Autores:		Oscar Leon Barbed Perez 666137 - Sergio Lazaro Magdalena 556030
* Descrip:		funciones de control del timer4 del s3c44b0x
* Version:
*********************************************************************************************/

/*--- ficheros de cabecera ---*/
#include "44b.h"
#include "44blib.h"
#include "timer4.h"

/*--- variables globales ---*/
enum state={"Idle","Sampling","ActivateIRQ"};
int timer4_wait_until=0;
int timer4_num_int = 0;
int switch_leds2 = 0;

/* declaración de función que es rutina de servicio de interrupción
 * https://gcc.gnu.org/onlinedocs/gcc/ARM-Function-Attributes.html */
void timer4_ISR(void) __attribute__((interrupt("IRQ")));

/*--- codigo de las funciones ---*/
void timer4_ISR(void)
{

	switch_leds2 = 1;

	/* Automata contra rebotes timer4 */
	if(timer4_wait_until>timer4_num_int){
		timer4_num_int++;
	}else{
		switch(state){
			case "Idle":
				if(){//Si se ha presionado el boton
					//Desactivar IRQ de botones
					timer4_wait_until=;//trp
					timer4_num_int=0;
					state="Sampling";
				}
			break;
			case "Sampling":
				if(){//Si el boton sigue presionado
					timer4_wait_until=;//50ms
					timer4_num_int=0;
				}else{
					timer4_wait_until=;//trd
					timer4_num_int=0;
					state="ActivateIRQ";
				}
			break;
			case "ActivateIRQ":
				//Activar IRQ de botones
			break;
		}
	}


	/* borrar bit en I_ISPC para desactivar la solicitud de interrupción*/
	rI_ISPC |= BIT_timer4; // BIT_timer4 está definido en 44b.h y pone un uno en el bit 15 que correponde al timer4
}

void timer4_inicializar(void)
{

	/* Configuraion controlador de interrupciones */
	rINTMOD = 0x0; // Configura las linas como de tipo IRQ
	rINTCON = 0x1; // Habilita int. vectorizadas y la linea IRQ (FIQ no)

	// Enmascara todas las lineas excepto timer4, Timer0 y el bit global 
	// (bits 26 y 13, BIT_GLOBAL y BIT_TIMER0 están definidos en 44b.h)
	// bit = 0 -> interrupcion disponible
	// bit = 1 -> interrupcion enmascarada
	rINTMSK = ~(BIT_GLOBAL | BIT_TIMER4 | BIT_TIMER2 | BIT_TIMER0); 
	/* Establece la rutina de servicio para TIMER0 */
	pISR_timer4 = (unsigned) timer4_ISR;

	/* Configura el timer4 */
	//bits[15:8]
	rTCFG0 = (rTCFG0 & 0xFF00FFFF); // ajusta el preescalado
	
	//bits [11:8]
	//selecciona la entrada del mux que proporciona el reloj. La 00 corresponde a un divisor de 1/2.
	rTCFG1 = (rTCFG1 & 0xFFF0FFFF);
	rTCNTB4 = 65535;// valor inicial de cuenta (la cuenta es descendente)
	rTCMPB4 = 12800;// valor de comparación
	/* establecer update=manual (bit 1) + inverter=on
	(¿? será inverter off un cero en el bit 2 pone el inverter en off)*/
	// update manual y autoreload.
	//timer4 bits [15:12]
	//bit 15 -> auto reload on/off
	//bit 14 -> output inverte on/off
	//bit 13 -> manual update on/off
	//bit 12 -> start/stop
	rTCON = rTCON | 0xA00000;
	/* iniciar timer (bit 0) con auto-reload (bit 3) -> 1001 */
	rTCON = rTCON | 0x900000;
}

void timer4_empezar()
{
	timer4_num_int = 0;
	rTCNTO2 = 0;
	//Volvemos a poner update manual para modificar
	rTCON = rTCON | 0x200000;
	//Volvemos al valor que nos interesa
	rTCON = rTCON | 0x900000;
	

}
long timer4_leer()
{
	return (timer4_num_int*(rTCNTB4 - rTCMPB4) + (rTCNTB4 - rTCNTO4));
}

