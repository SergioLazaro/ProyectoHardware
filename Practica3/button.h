/*********************************************************************************************
* Fichero:	button.h
* Descrip:	Funciones de manejo de los pulsadores (EINT6-7)
* Version:
* Autor1: Oscar Leon Barbed Perez 666137
* Autor2: Sergio Lazaro Magdalena 556030
*********************************************************************************************/
#ifndef _BUTTON_H_
#define _BUTTON_H_

volatile int which_int;		//Variable global para indicar estado del boton
volatile int boton_pulsado;	//Variable global boton para main

/*--- declaracion de funciones visibles del módulo button.c/button.h ---*/
void Eint4567_init(void);

/* Getters y Setters*/
void setBotonPulsado(int value);
int getBotonPulsado(void);

void setWhichInt(int value);
int getWhichInt(void);

#endif /* _BUTTON_H_ */
