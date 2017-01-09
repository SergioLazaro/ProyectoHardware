/*********************************************************************************************
* Fichero:		timer.h
* Autor:
* Descrip:		funciones de control del timer0 del s3c44b0x
* Version:
*********************************************************************************************/

#ifndef _TIMER_H_
#define _TIMER_H_

/*--- variables visibles del módulo timer.c/timer.h ---*/
int timer0_num_int;

/*--- declaracion de funciones visibles del módulo timer.c/timer.h ---*/
void timer_init(void);
void timer0_empezar(void);
long timer0_leer(void);

#endif /* _TIMER_H_ */
