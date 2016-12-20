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
	ActivateIRQ = 2,
	End = 3,
	state_length = 4
};

int status_timer;
int timer4_wait_until;
int timer4_num_int;
int which_int_timer;

/*--- declaracion de funciones visibles timer4 ---*/
void timer4_inicializar(void);
void timer4_empezar(void);
long timer4_leer(void);
void automata_timer(void);


#endif /* _TIMER4_H_ */
