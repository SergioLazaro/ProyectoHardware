/*********************************************************************************************
* Fichero:		timer4.h
* Descrip:		cabecera timer4 del s3c44b0x
* Version:
* Autor1: Oscar Leon Barbed Perez 666137
* Autor2: Sergio Lazaro Magdalena 556030
*********************************************************************************************/

#ifndef _TIMER4_H_
#define _TIMER4_H_

/*--- variables visibles del módulo timer4.c/timer4.h ---*/
enum state {
	Idle = 0,
	Sampling = 1,
	ButtonReleased = 2,
	ActivateIRQ = 3,
	End = 4,
	state_length = 5
};

int status_timer;
int timer4_wait_until;
int timer4_num_int;
int which_int_timer;
double tiempo_tick;
double tiempo_interrupcion;

/*--- declaracion de funciones visibles timer4 ---*/
void timer4_inicializar(void);
void timer4_empezar(void);
long timer4_leer(void);
void automata_timer(void);

/*--- declaracion funciones estado automamta ---*/
void primeros_rebotes(void);
void mantiene_pulsado(void);
void button_released(void);
void activar_irq(void);
void restart_status_timer4(void);

#endif /* _TIMER4_H_ */
