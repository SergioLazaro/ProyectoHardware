/*
 * exception
 *
 *  Created on: 03/11/2016
 *      Author: a556030
 */

#ifndef EXCEPTION_
#define EXCEPTION_


/*--- variables globales del módulo ---*/
int exception;

/*--- declaracion de funciones visibles del módulo Exception.c/Exception.h ---*/
void Exception_init(void);

void Catch_exception(void);

void Catch_DAbort_exception(void);

#endif /* EXCEPTION_ */
