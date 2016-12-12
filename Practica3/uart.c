/*********************************************************************************************
* Fichero:	uart.c
* Autor:	
* Descrip:	funciones de UART
* Version: <P5-ARM-A>	
*********************************************************************************************/

/*--- ficheros de cabecera ---*/
#include "44b.h"
#include "uart.h"
//#include <stdarg.h>

/*--- definición de constantes ---*/
#define KEY_BUFLEN 100
#define UART_INT

/*--- variables globales ---*/
static unsigned char keyBuf[KEY_BUFLEN]; // buffer de recepción
volatile static int keyBufRdPt=0;
volatile static int keyBufWrPt=0;

static char *uart0TxStr;		// puntero al buffer de envío

/*--- prototipos de funciones ---*/
void Uart0_RxInt(void)       __attribute__ ((interrupt ("IRQ")));
void Uart0_TxInt(void)       __attribute__ ((interrupt ("IRQ")));


/*--- implementación de las funciones ---*/
void Uart_Config(void)
{
#ifdef UART_INT
    keyBufRdPt = keyBufWrPt=0;	          // iniciar punteros al comienzo del buffer	   
    rUCON0 = 0x205;					      // configurar UART TX:nivel/int, RX:flanco/int
    pISR_UTXD0 = (unsigned)Uart0_TxInt;   // establecer ISR para UTXD0 
    pISR_URXD0 = (unsigned)Uart0_RxInt;   // establecer ISR para URDX0
    rINTMSK &= ~(BIT_GLOBAL|BIT_URXD0);   // desenmascarar URXD0
#endif 
}

inline void Uart_TxEmpty(void)
{
    while (!(rUTRSTAT0 & 0x4)); 	     // esperar a que el shifter de TX se vacie
}


char Uart_Getch(void)
{

#ifdef UART_INT 
    if (keyBufRdPt == KEY_BUFLEN)      // buffer circular
	   keyBufRdPt=0;

    while (keyBufWrPt == keyBufRdPt);  // esperar a que haya caracteres nuevos
    return keyBuf[keyBufRdPt++];	   // devolver el caracter
#else
    while (!(rUTRSTAT0 & 0x1));        // esperar a que el buffer contenga datos
	return RdURXH0();		   		   // devolver el caracter  
#endif
}

void Uart_SendByte(int data)
{
    char localBuf[2] = {'\0','\0'};
    
#ifdef UART_INT
    localBuf[0] = (char) data;	       // escribir caracter en el buffer
	uart0TxStr = localBuf;	           // actualizar puntero buffer
	rINTMSK &= ~(BIT_GLOBAL|BIT_UTXD0); // habilitar interrupción UTXD0
    while (uart0TxStr);	   			   // esperar a que se haya recorrido el buffer	
#else
    if(data == '\n')		
	{
	   while (!(rUTRSTAT0 & 0x2));     // esperar a que THR se vacie
	   WrUTXH0('\r');			       // escribir retorno de carro (utilizar macro)
	}
	while (!(rUTRSTAT0 & 0x2)); 	   // esperar a que THR se vacie
	Delay(10);					       // por precaución esperar 1 ms (hyper_terminal es lento)
	WrUTXH0(data);				       // escribir data (utilizar macro)
#endif
}

void Uart_SendString(char *pt)
{
#ifdef UART_INT
    uart0TxStr = pt;					// actualizar puntero al buffer
    rINTMSK &= ~(BIT_GLOBAL|BIT_UTXD0); // habilitar interrupcion UTXD0
    while (uart0TxStr);        			// esperar a que se haya recorrido el buffer
#else
    while (*pt)						    // mandar byte a byte hasta completar string
	Uart_SendByte(*pt++);
#endif
}
/*
void Uart_Printf(char *fmt,...)
{
    va_list ap;
    char string[256];

    va_start(ap,fmt);
    vsprintf(string,fmt,ap);
    Uart_SendString(string);
    va_end(ap);
}*/

void Uart0_TxInt(void)
{
    if(*uart0TxStr != '\0')     // mas caracteres que enviar?
    {
	   if(*uart0TxStr == '\n') 
	   {
	      WrUTXH0(CR_char);		// escribir retorno de carro
	      Uart_TxEmpty();
	   } 
	   WrUTXH0(*uart0TxStr++);  // escribir caracter y actualizar puntero
    }
    else						// fin de la cadena
    {
	   rINTMSK|=BIT_UTXD0;		// enmascarar UTXD0
	   uart0TxStr = 0;			// borrar puntero 
    }
    rI_ISPC=BIT_UTXD0;		    // finalizar interrupción
}

void Uart0_RxInt(void)
{
    
    keyBuf[keyBufWrPt++]=RdURXH0(); // leer byte e insertar en el buffer
     
    if(keyBufWrPt==KEY_BUFLEN) 		// mantener comportamiento circular 
		keyBufWrPt=0;
	
	rI_ISPC=BIT_URXD0;		        // finalizar interrupción
}
