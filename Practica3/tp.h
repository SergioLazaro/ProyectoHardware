/*********************************************************************************************
* File��	tp.H
* Author:	embest	
* Desc��	Touch Screen define file
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
unsigned int  Vx, Vy;
unsigned int  Xmax;
unsigned int  Ymax;
unsigned int  Xmin;
unsigned int  Ymin;
unsigned int x_elegida;
unsigned int y_elegida;
unsigned int activar_zoom;

void TS_Test(void);
void TS_init(void);
void TSInt(void);
void TS_close(void);
void Lcd_TC(void);
void DesignREC(ULONG tx, ULONG ty);
void Check_Sel(void);
//int get_activar_zoom(void);
