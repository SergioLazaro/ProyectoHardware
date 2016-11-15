#include "pila_depuracion.h"

/* Variables globales */
int *stack_limit;
int *stack_pos;

void stack_init(void)
{
	stack_pos = 0xFFFFFFFF; //cambiar @pos
	stack_limit = 0xFFFFFFFF; //cambiar @limit

	//iniciar stack pos
	//limpiar memoria (opcional) -> util para comprobar init
}


//
// ---------------------
// |	timer2_lectura |
// ---------------------
// |	auxData		   |
// ---------------------
// |	idEvento	   |
// ---------------------
void push_debug(int idEvento, int auxData)
{
	//check if limit has been reached
	if(stack_pos == stack_limit){
		stack_pos = 0xFFFFFFFF; //Cambiar por direccion del init
	}
	*stack_pos = idEvento;
	stack_pos = stack_pos + 4;
	*stack_pos = auxData;
	stack_pos = stack_pos + 4;
	*stack_pos = timer2_leer();
	stack_pos = stack_pos + 4;
}