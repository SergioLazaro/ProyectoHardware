/*********************************************************************************************
* Fichero:		timer2.h
* Descrip:		funciones de control del timer2 del s3c44b0x
* Version:
* Autor1: Oscar Leon Barbed Perez 666137
* Autor2: Sergio Lazaro Magdalena 556030
*********************************************************************************************/

#ifndef _TIMER2_H_
#define _TIMER2_H_

/*--- variables visibles del módulo timer2.c/timer2.h ---*/
int timer2_num_int;

/*--- declaracion de funciones visibles timer2 ---*/
void timer2_inicializar(void);
void timer2_empezar(void);
long timer2_leer(void);

#endif /* _TIMER2_H_ */
