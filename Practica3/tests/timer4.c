/*********************************************************************************************
* Fichero:		timer4.c
* Descrip:		funciones de control del timer4 del s3c44b0x
* Version:
* Autor1: Oscar Leon Barbed Perez 666137
* Autor2: Sergio Lazaro Magdalena 556030
*********************************************************************************************/

/*--- ficheros de cabecera ---*/
#include "44b.h"
#include "44blib.h"
#include "timer4.h"
#include "button.h"
#include "pila_depuracion.h"

/*--- variables globales ---*/
int state[state_length] = {Idle, Sampling, ButtonReleased, ActivateIRQ, End};

/* declaración de función que es rutina de servicio de interrupción
 * https://gcc.gnu.org/onlinedocs/gcc/ARM-Function-Attributes.html */
void timer4_ISR(void) __attribute__((interrupt("IRQ")));

/*--- codigo de las funciones ---*/
void timer4_ISR(void)
{
	timer4_num_int ++;
	automata_timer();
	/* borrar bit en I_ISPC para desactivar la solicitud de interrupción*/
	rI_ISPC |= BIT_TIMER4; // BIT_timer4 está definido en 44b.h y pone un uno en el bit 15 que correponde al timer4
}

void automata_timer(void)
{

	/* Automata contra rebotes timer4 */
	if(timer4_wait_until <= timer4_leer()){
		//push(11,which_int_timer);
		switch(status_timer){
			case 0:	//Idle
				/*
				 * 0x40 -> boton derecho pulsado
				 * 0x80 -> boton izquierdo pulsado
				 */
				primeros_rebotes();
				break;
			case 1:	//Sampling
				mantiene_pulsado();
				break;
			case 2: 
				button_released();
				break;
			case 3: //ActivateIRQ
				activar_irq();
				break;
			default:
				break;
		}
	}
}

void primeros_rebotes(void){
	if(getWhichInt() == 0x40 || getWhichInt() == 0x80){//Hay pulsacion
		//push(7,rPDATG);
		timer4_wait_until = 80;//trp (en ms)
		//timer4_num_int = 0;
		timer4_empezar();
		status_timer = state[Sampling];
	}
}

void mantiene_pulsado(void){
	which_int_timer = rPDATG & 0xc0;
	if(which_int_timer == getWhichInt()) //Si el boton sigue presionado
	{
		timer4_wait_until=50;	//50ms
		timer4_empezar();
		//timer4_num_int=0;
	}
	else{							//Se ha soltado el boton
		status_timer = state[ButtonReleased];
		timer4_empezar();//Leon: Creo que esto no hace falta(creo que te hara esperar 50ms otra vez)				
	}
}

void button_released(void){
	timer4_wait_until=120;//trd (en ms)
	//timer4_num_int=0;
	timer4_empezar();
	status_timer = state[ActivateIRQ];
}

void activar_irq(void){
	setWhichInt(0);
	//Activamos interrupciones de los botones
	rINTMSK &= ~(BIT_EINT4567);
	
	//Leon: status_timer = state[End];
	
	//Reactivar interrupciones boton
	//rEXTINTPND = 0xf;
	/* Por precaucion, se vuelven a borrar los bits de INTPND y EXTINTPND */
	//rI_ISPC |= (BIT_EINT4567);
	
}

void restart_status_timer4(void){
	status_timer = state[Idle];
}

void timer4_inicializar(void)
{
	/* Inicializamos variables globales*/
	timer4_wait_until = 0;
	timer4_num_int = 0;
	status_timer = 0;
	which_int_timer = 0;
	push(10,0);

	/* Configuraion controlador de interrupciones */
	rINTMOD = 0x0; // Configura las linas como de tipo IRQ
	rINTCON = 0x1; // Habilita int. vectorizadas y la linea IRQ (FIQ no)

	// Enmascara todas las lineas excepto timer4, Timer0 y el bit global
	// (bits 26 y 13, BIT_GLOBAL y BIT_TIMER0 están definidos en 44b.h)
	// bit = 0 -> interrupcion disponible
	// bit = 1 -> interrupcion enmascarada
	rINTMSK    = ~(BIT_GLOBAL | BIT_TIMER4 | BIT_TIMER2 | BIT_TIMER0); // Enmascara todas las lineas excepto...
	/* Establece la rutina de servicio para TIMER0 */
	pISR_TIMER4 = (unsigned) timer4_ISR;

	/* Configura el timer4 */
	//bits[15:8]
	rTCFG0 = (rTCFG0 & 0xFF00FFFF); // ajusta el preescalado

	//bits [11:8]
	//selecciona la entrada del mux que proporciona el reloj. La 00 corresponde a un divisor de 1/2.
	rTCFG1 = (rTCFG1 & 0xFFF0FFFF);
	rTCNTB4 = 65535;// valor inicial de cuenta (la cuenta es descendente)
	rTCMPB4 = 0;// valor de comparación
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

	tiempo_tick = 0.03125;	// 0.03125 us
	tiempo_interrupcion = tiempo_tick * rTCNTB4;
}

void timer4_empezar(void)
{
	//status_timer = state[Idle];
	timer4_num_int = 0;
	//rTCNTO4 = rTCNTB4;
	//Volvemos a poner update manual para modificar
	rTCON |= 0x200000;
	//Volvemos al valor que nos interesa
	rTCON &= 0x9FFFFF;
}
long timer4_leer(void)
{
	return (timer4_num_int * tiempo_interrupcion + ((rTCNTB4 - rTCNTO4) * tiempo_tick)) / 1000; //ms //Leon: Creo que faltaban unos parentesis.
	//return (timer4_num_int*(rTCNTB4 - rTCMPB4) + (rTCNTB4 - rTCNTO4)) / 32000;
}
