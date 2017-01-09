/*********************************************************************************************
* File£º	tp.H
* Author:	embest	
* Desc£º	Touch Screen define file
* History:	
*********************************************************************************************/
#include "def.h"
#include "44b.h"
#include "44blib.h"
#ifndef __TP_H__
#define __TP_H__

#endif /*__TP_H__*/

/*--- global  variables ---*/
volatile int CheckTSP,oneTouch;
volatile unsigned int  Vx, Vy;
volatile unsigned int  Xmax;
volatile unsigned int  Ymax;
volatile unsigned int  Xmin;
volatile unsigned int  Ymin;
volatile unsigned int x_elegida;
volatile unsigned int y_elegida;
volatile unsigned int activar_zoom;
volatile unsigned int estado_zoom;

void TS_Test(void);
void TS_init(void);
void TSInt(void);
void TS_close(void);
void Lcd_TC(void);
void DesignREC(ULONG tx, ULONG ty);
void Check_Sel(void);
//int get_activar_zoom(void);
