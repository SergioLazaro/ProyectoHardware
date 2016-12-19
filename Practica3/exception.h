/*********************************************************************************************
* Fichero:	exception.h
* Descrip:	Cabeceras de manejo de excepciones
* Version:
* Autor1: Oscar Leon Barbed Perez 666137
* Autor2: Sergio Lazaro Magdalena 556030
*********************************************************************************************/

#ifndef EXCEPTION_
#define EXCEPTION_


/*--- variables globales del módulo ---*/
int exception;

/*--- declaracion de funciones visibles del módulo Exception.c/Exception.h ---*/
void Exception_init(void);

void Catch_exception(void);

void Catch_DAbort_exception(void);

#endif /* EXCEPTION_ */
