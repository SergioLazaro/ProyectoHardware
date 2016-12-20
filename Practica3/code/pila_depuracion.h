/*********************************************************************************************
* Fichero:	pila_depuracion.h
* Descrip:	Cabeceras pila de depuracion
* Version:
* Autor1: Oscar Leon Barbed Perez 666137
* Autor2: Sergio Lazaro Magdalena 556030
*********************************************************************************************/

#include "timer2.h"


/* Variables globales */
int *stack_limit;
int *stack_pos;

void stack_init(void);

void push(int idEvento, int auxData);
