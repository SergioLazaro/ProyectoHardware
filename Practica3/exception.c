/*********************************************************************************************
* Fichero:	exception.c
* Descrip:	Funciones de manejo de excepciones
* Version:
* Autor1: Oscar Leon Barbed Perez 666137
* Autor2: Sergio Lazaro Magdalena 556030
*********************************************************************************************/

#include "44b.h"
#include "44blib.h"
#include "exception.h"
#include "8led.h"
#include <stdlib.h>

int exception = 0;

void Exception_init(void)
{
	//MODIFICAR UNSIGNED POR UNSIGNED *
	pISR_UNDEF = (unsigned)Catch_exception;
	pISR_SWI = (unsigned)Catch_exception;
	pISR_PABORT	= (unsigned)Catch_exception;
	pISR_DABORT	= (unsigned)Catch_DAbort_exception; //Llamar a otra funcion para identificar tipo ABORT
}

/*
 * http://osnet.cs.nchu.edu.tw/powpoint/Embedded94_1/Chapter%207%20ARM%20Exceptions.pdf
 * http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.faqs/ka3540.html
 * http://lioncash.github.io/ARMBook/apsr_and_cpsr_flags.html
 */

void Catch_DAbort_exception(void){
	int code;
	int* instr;
	exception = 1;

	//Check cpsr en variable C -> code
	//Check instruccion que provoca excepcion -> instr
	asm volatile("MRS %0, CPSR /* get current program status */\n\t"
				 "AND %0, %0, #0x1f \n\t"
				 "MOV %1, lr /* get instruction catched */\n\t"
				 "SUB %1, %1, #4 \n\t"
				 : "=r" (code), "=r" (instr));


	if(code == 23){
		while (1) {
			D8Led_symbol(16);
			Delay(1000);
			D8Led_symbol(14);	//MOSTRAR E
			Delay(1000);
		}
	}
}

void Catch_exception(void)
{
	int code;
	int instr;
	int show;
	exception = 1;	//Activamos indicador excepciones

	//Init variables
	code = 0;
	instr = 0;
	//Obtener tipo de excepcion y la instrucción causante.

	//Check cpsr en variable C
	asm volatile("MRS %0, CPSR /* get current program status */\n\t"
				 "AND %0, %0, #0x1f \n\t"
				 "MOV %1, lr /* get instruction catched */\n\t"
				 "SUB %1, %1, #4 \n\t"
				 : "=r" (code), "=r" (instr));

	//switch/if-else para gestionar el tipo de excepcion
	if(code == 19)	//SW Int.
	{
		show = 10;
	}
	else if(code == 23) //Abort
	{
		show = 12;
	}
	else if(code == 27) // Udef
	{
		show = 13;
	}

	while (1) {
		D8Led_symbol(16);
		Delay(1000);
		D8Led_symbol(show);	//MOSTRAR CODIGO EXCEPCION
		Delay(1000);
	}
}
