/*********************************************************************************************
* Fichero:	pila_depuracion.c
* Descrip:	Funciones de manejo de la pila de depuracion
* Version:
* Autor1: Oscar Leon Barbed Perez 666137
* Autor2: Sergio Lazaro Magdalena 556030
*********************************************************************************************/

#include "pila_depuracion.h"

/* Variables globales */
int *stack_limit;
int *stack_pos;

void stack_init(void)
{
	stack_pos = 0x0c7ff5fc;
	stack_limit = 0x0c7ff500;		//Fijo


	//Check si memoria init
	while(stack_pos != stack_limit)
	{
		*stack_pos = 0;
		stack_pos--;
	}

}


//
// ---------------------
// |	timer2_lectura |
// ---------------------
// |	auxData		   |
// ---------------------
// |	idEvento	   |
// ---------------------
void push(int idEvento, int auxData)
{
	//check if limit has been reached
	if(stack_pos == stack_limit){
		stack_pos = 0x0c7ff5fc; //Cambiar por direccion del init
	}
	*stack_pos = idEvento;
	stack_pos--;
	*stack_pos = auxData;
	stack_pos--;
	*stack_pos = timer2_leer();
	stack_pos--;
}
