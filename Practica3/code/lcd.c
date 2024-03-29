/*********************************************************************************************
 * File��	lcd.c
 * Author:	embest
 * Desc��	LCD control and display functions
 * History:
 *********************************************************************************************/

/*--- include files ---*/
#include "lcd.h"
#include "def.h"
#include "44b.h"
#include "44blib.h"

/*--- macro define ---*/
#define DMA_Byte  (0)
#define DMA_HW    (1)
#define DMA_Word  (2)
#define DW 		  DMA_Byte		//set ZDMA0 as half-word
/*--- extern variables ---*/
extern INT8U g_auc_Ascii6x8[];
extern INT8U g_auc_Ascii8x16[];

/*--- function code ---*/
/*********************************************************************************************
 * name:		Lcd_Init()
 * func:		Initialize LCD Controller
 * para:		none
 * ret:		none
 * modify:
 * comment:
 *********************************************************************************************/
void Lcd_Init(void) {
	rDITHMODE = 0x1223a;
	rDP1_2 = 0x5a5a;
	rDP4_7 = 0x366cd9b;
	rDP3_5 = 0xda5a7;
	rDP2_3 = 0xad7;
	rDP5_7 = 0xfeda5b7;
	rDP3_4 = 0xebd7;
	rDP4_5 = 0xebfd7;
	rDP6_7 = 0x7efdfbf;

	rLCDCON1 = (0) | (1 << 5) | (MVAL_USED << 7) | (0x0 << 8) | (0x0 << 10)
			| (CLKVAL_GREY16 << 12);
	rLCDCON2 = (LINEVAL) | (HOZVAL << 10) | (10 << 21);
	rLCDSADDR1 = (0x2 << 27)
			| (((LCD_ACTIVE_BUFFER >> 22) << 21) | M5D(LCD_ACTIVE_BUFFER>>1));
	rLCDSADDR2 = M5D(((LCD_ACTIVE_BUFFER+(SCR_XSIZE*LCD_YSIZE/2))>>1))
			| (MVAL << 21);
	rLCDSADDR3 = (LCD_XSIZE / 4) | (((SCR_XSIZE - LCD_XSIZE)/4)<<9 );
	// enable,4B_SNGL_SCAN,WDLY=8clk,WLH=8clk,
	rLCDCON1 = (1) | (1 << 5) | (MVAL_USED << 7) | (0x3 << 8) | (0x3 << 10)
			| (CLKVAL_GREY16 << 12);
	rBLUELUT = 0xfa40;
	//Enable LCD Logic and EL back-light.
	rPDATE = rPDATE & 0xae;
}

/*********************************************************************************************
 * name:		Lcd_Active_Clr()
 * func:		clear LCD screen
 * para:		none
 * ret:		none
 * modify:
 * comment:
 *********************************************************************************************/
void Lcd_Active_Clr(void) {
	INT32U i;
	INT32U *pDisp = (INT32U *) LCD_ACTIVE_BUFFER;

	for (i = 0; i < (SCR_XSIZE * SCR_YSIZE / 2 / 4); i++) {
		*pDisp++ = WHITE;
	}
}

/*********************************************************************************************
 * name:		Lcd_GetPixel()
 * func:		Get appointed point's color value
 * para:		usX,usY -- pot's X-Y coordinate
 * ret:		pot's color value
 * modify:
 * comment:
 *********************************************************************************************/INT8U LCD_GetPixel(
		INT16U usX, INT16U usY) {
	INT8U ucColor;

	ucColor = *((INT8U*) (LCD_VIRTUAL_BUFFER + usY * SCR_XSIZE / 2 + usX / 8 * 4
			+ 3 - (usX % 8) / 2));
	ucColor = (ucColor >> ((1 - (usX % 2)) * 4)) & 0x0f;
	return ucColor;
}

/*********************************************************************************************
 * name:		Lcd_Active_Clr()
 * func:		clear virtual screen
 * para:		none
 * ret:		none
 * modify:
 * comment:
 *********************************************************************************************/
void Lcd_Clr(void) {
	INT32U i;
	INT32U *pDisp = (INT32U *) LCD_VIRTUAL_BUFFER;

	for (i = 0; i < (SCR_XSIZE * SCR_YSIZE / 2 / 4); i++) {
		*pDisp++ = WHITE;
	}
}

/*********************************************************************************************
 * name:		LcdClrRect()
 * func:		fill appointed area with appointed color
 * para:		usLeft,usTop,usRight,usBottom -- area's rectangle acme coordinate
 *			ucColor -- appointed color value
 * ret:		none
 * modify:
 * comment:	also as clear screen function
 *********************************************************************************************/
void LcdClrRect(INT16 usLeft, INT16 usTop, INT16 usRight, INT16 usBottom,
		INT8U ucColor) {
	INT16 i, k, l, m;

	INT32U ulColor = (ucColor << 28) | (ucColor << 24) | (ucColor << 20)
			| (ucColor << 16) | (ucColor << 12) | (ucColor << 8)
			| (ucColor << 4) | ucColor;

	i = k = l = m = 0;
	if ((usRight - usLeft) <= 8) {
		for (i = usTop; i <= usBottom; i++) {
			for (m = usLeft; m <= usRight; m++) {
				LCD_PutPixel(m, i, ucColor);
			}
		}
		return;
	}

	/* check borderline */
	if (0 == (usLeft % 8))
		k = usLeft;
	else {
		k = (usLeft / 8) * 8 + 8;
	}
	if (0 == (usRight % 8))
		l = usRight;
	else {
		l = (usRight / 8) * 8;
	}

	for (i = usTop; i <= usBottom; i++) {
		for (m = usLeft; m <= (k - 1); m++) {
			LCD_PutPixel(m, i, ucColor);
		}
		for (m = k; m < l; m += 8) {
			(*(INT32U*) (LCD_VIRTUAL_BUFFER + i * SCR_XSIZE / 2 + m / 2)) =
					ulColor;
		}
		for (m = l; m <= usRight; m++) {
			LCD_PutPixel(m, i, ucColor);
		}
	}
}

/*********************************************************************************************
 * name:		Lcd_Draw_Box()
 * func:		Draw rectangle with appointed color
 * para:		usLeft,usTop,usRight,usBottom -- rectangle's acme coordinate
 *			ucColor -- appointed color value
 * ret:		none
 * modify:
 * comment:
 *********************************************************************************************/
void Lcd_Draw_Box(INT16 usLeft, INT16 usTop, INT16 usRight, INT16 usBottom,
		INT8U ucColor) {
	Lcd_Draw_HLine(usLeft, usRight, usTop, ucColor, 1);
	Lcd_Draw_HLine(usLeft, usRight, usBottom, ucColor, 1);
	Lcd_Draw_VLine(usTop, usBottom, usLeft, ucColor, 1);
	Lcd_Draw_VLine(usTop, usBottom, usRight, ucColor, 1);
}

void Lcd_Draw_Box_grosor(INT16 usLeft, INT16 usTop, INT16 usRight,
		INT16 usBottom, INT8U ucColor, int grosor) {
	Lcd_Draw_HLine(usLeft, usRight, usTop, ucColor, grosor);
	Lcd_Draw_HLine(usLeft, usRight, usBottom, ucColor, grosor);
	Lcd_Draw_VLine(usTop, usBottom, usLeft, ucColor, grosor);
	Lcd_Draw_VLine(usTop, usBottom, usRight, ucColor, grosor);
}

/*********************************************************************************************
 * name:		Lcd_Draw_Line()
 * func:		Draw line with appointed color
 * para:		usX0,usY0 -- line's start point coordinate
 *			usX1,usY1 -- line's end point coordinate
 *			ucColor -- appointed color value
 *			usWidth -- line's width
 * ret:		none
 * modify:
 * comment:
 *********************************************************************************************/
void Lcd_Draw_Line(INT16 usX0, INT16 usY0, INT16 usX1, INT16 usY1,
		INT8U ucColor, INT16U usWidth) {
	INT16 usDx;
	INT16 usDy;
	INT16 y_sign;
	INT16 x_sign;
	INT16 decision;
	INT16 wCurx, wCury, wNextx, wNexty, wpy, wpx;

	if (usY0 == usY1) {
		Lcd_Draw_HLine(usX0, usX1, usY0, ucColor, usWidth);
		return;
	}
	if (usX0 == usX1) {
		Lcd_Draw_VLine(usY0, usY1, usX0, ucColor, usWidth);
		return;
	}
	usDx = abs(usX0 - usX1);
	usDy = abs(usY0 - usY1);
	if (((usDx >= usDy && (usX0 > usX1)) || ((usDy > usDx) && (usY0 > usY1)))) {
		GUISWAP(usX1, usX0);
		GUISWAP(usY1, usY0);
	}
	y_sign = (usY1 - usY0) / usDy;
	x_sign = (usX1 - usX0) / usDx;

	if (usDx >= usDy) {
		for (wCurx = usX0, wCury = usY0, wNextx = usX1, wNexty = usY1, decision =
				(usDx >> 1); wCurx <= wNextx; wCurx++, wNextx--, decision +=
				usDy) {
			if (decision >= usDx) {
				decision -= usDx;
				wCury += y_sign;
				wNexty -= y_sign;
			}
			for (wpy = wCury - usWidth / 2; wpy <= wCury + usWidth / 2; wpy++) {
				LCD_PutPixel(wCurx, wpy, ucColor);
			}

			for (wpy = wNexty - usWidth / 2; wpy <= wNexty + usWidth / 2;
					wpy++) {
				LCD_PutPixel(wNextx, wpy, ucColor);
			}
		}
	} else {
		for (wCurx = usX0, wCury = usY0, wNextx = usX1, wNexty = usY1, decision =
				(usDy >> 1); wCury <= wNexty; wCury++, wNexty--, decision +=
				usDx) {
			if (decision >= usDy) {
				decision -= usDy;
				wCurx += x_sign;
				wNextx -= x_sign;
			}
			for (wpx = wCurx - usWidth / 2; wpx <= wCurx + usWidth / 2; wpx++) {
				LCD_PutPixel(wpx, wCury, ucColor);
			}

			for (wpx = wNextx - usWidth / 2; wpx <= wNextx + usWidth / 2;
					wpx++) {
				LCD_PutPixel(wpx, wNexty, ucColor);
			}
		}
	}
}

/*********************************************************************************************
 * name:		Lcd_Draw_HLine()
 * func:		Draw horizontal line with appointed color
 * para:		usX0,usY0 -- line's start point coordinate
 *			usX1 -- line's end point X-coordinate
 *			ucColor -- appointed color value
 *			usWidth -- line's width
 * ret:		none
 * modify:
 * comment:
 *********************************************************************************************/
void Lcd_Draw_HLine(INT16 usX0, INT16 usX1, INT16 usY0, INT8U ucColor,
		INT16U usWidth) {
	INT16 usLen;

	if (usX1 < usX0) {
		GUISWAP(usX1, usX0);
	}

	while ((usWidth--) > 0) {
		usLen = usX1 - usX0 + 1;
		while ((usLen--) > 0) {
			LCD_PutPixel(usX0 + usLen, usY0, ucColor);
		}
		usY0++;
	}
}

/*********************************************************************************************
 * name:		Lcd_Draw_VLine()
 * func:		Draw vertical line with appointed color
 * para:		usX0,usY0 -- line's start point coordinate
 *			usY1 -- line's end point Y-coordinate
 *			ucColor -- appointed color value
 *			usWidth -- line's width
 * ret:		none
 * modify:
 * comment:
 *********************************************************************************************/
void Lcd_Draw_VLine(INT16 usY0, INT16 usY1, INT16 usX0, INT8U ucColor,
		INT16U usWidth) {
	INT16 usLen;

	if (usY1 < usY0) {
		GUISWAP(usY1, usY0);
	}

	while ((usWidth--) > 0) {
		usLen = usY1 - usY0 + 1;
		while ((usLen--) > 0) {
			LCD_PutPixel(usX0, usY0 + usLen, ucColor);
		}
		usX0++;
	}
}

/*********************************************************************************************
 * name:		Lcd_DspAscII6x8()
 * func:		display 6x8 ASCII character string
 * para:		usX0,usY0 -- ASCII character string's start point coordinate
 *			ForeColor -- appointed color value
 *			pucChar   -- ASCII character string
 * ret:		none
 * modify:
 * comment:
 *********************************************************************************************/
void Lcd_DspAscII6x8(INT16U usX0, INT16U usY0, INT8U ForeColor, INT8U* pucChar) {
	INT32U i, j;
	INT8U ucTemp;

	while (*pucChar != 0) {
		for (i = 0; i < 8; i++) {
			ucTemp = g_auc_Ascii6x8[(*pucChar) * 8 + i];
			for (j = 0; j < 8; j++) {
				if ((ucTemp & (0x80 >> j)) != 0) {
					LCD_PutPixel(usX0 + i, usY0 + 8 - j, (INT8U)ForeColor);
				}
			}
		}
		usX0 += XWIDTH;
		pucChar++;
	}
}
/*********************************************************************************************
* name:		Lcd_DspAscII8x16()
* func:		display 8x16 ASCII character string
* para:		usX0,usY0 -- ASCII character string's start point coordinate
*			ForeColor -- appointed color value
*			pucChar   -- ASCII character string
* ret:		none
* modify:
* comment:
*********************************************************************************************/
void Lcd_DspAscII8x16(INT16U x0, INT16U y0, INT8U ForeColor, INT8U * s)
{
	INT16 i,j,k,x,y,xx;
	INT8U qm;
	INT32U ulOffset;
	INT8 ywbuf[16],temp[2];

	for( i = 0; i < strlen((const char*)s); i++ )
	{
		if( (INT8U)*(s+i) >= 161 )
		{
			temp[0] = *(s + i);
			temp[1] = '\0';
        	return;
		}
		else
		{
			qm = *(s+i);
			ulOffset = (INT32U)(qm) * 16;		//Here to be changed tomorrow
        	for( j = 0; j < 16; j ++ )
			{
				ywbuf[j] = g_auc_Ascii8x16[ulOffset + j];
            }

            for( y = 0; y < 16; y++ )
            {
            	for( x = 0; x < 8; x++ )
               	{
                	k = x % 8;
                	if( ywbuf[y]  & (0x80 >> k) )
			       	{
			       		xx = x0 + x + i*8;
			       		LCD_PutPixel(xx, y + y0, (INT8U)ForeColor);
			       	}
			   	}
            }
		}
	}
}


/*********************************************************************************************
 * name:		ReverseLine()
 * func:		Reverse display some lines
 * para:		ulHeight -- line's height
 *			ulY -- line's Y-coordinate
 * ret:		none
 * modify:
 * comment:
 *********************************************************************************************/
void ReverseLine(INT32U ulHeight, INT32U ulY) {
	INT32U i, j, temp;

	for (i = 0; i < ulHeight; i++) {
		for (j = 0; j < (SCR_XSIZE / 4 / 2); j++) {
			temp = *(INT32U*) (LCD_VIRTUAL_BUFFER + (ulY + i) * SCR_XSIZE / 2
					+ j * 4);
			temp ^= 0xFFFFFFFF;
			*(INT32U*) (LCD_VIRTUAL_BUFFER + (ulY + i) * SCR_XSIZE / 2 + j * 4) =
					temp;
		}
	}
}

/*********************************************************************************************
 * name:		Zdma0Done()
 * func:		LCD dma interrupt handle function
 * para:		none
 * ret:		none
 * modify:
 * comment:
 *********************************************************************************************/
static INT8U ucZdma0Done = 1; //When DMA is finish,ucZdma0Done is cleared to Zero
void Zdma0Done(void) {
	rI_ISPC |= BIT_ZDMA0;	    //clear pending
	ucZdma0Done = 0;
}

/*********************************************************************************************
 * name:		Lcd_Dma_Trans()
 * func:		dma transport virtual LCD screen to LCD actual screen
 * para:		none
 * ret:		none
 * modify:
 * comment:
 *********************************************************************************************/
void Lcd_Dma_Trans(void) {
	INT8U err;

	ucZdma0Done = 1;
	//#define LCD_VIRTUAL_BUFFER	(0xc400000)
	//#define LCD_ACTIVE_BUFFER	(LCD_VIRTUAL_BUFFER+(SCR_XSIZE*SCR_YSIZE/2))	//DMA ON
	//#define LCD_ACTIVE_BUFFER	LCD_VIRTUAL_BUFFER								//DMA OFF
	//#define LCD_BUF_SIZE		(SCR_XSIZE*SCR_YSIZE/2)
	//So the Lcd Buffer Low area is from LCD_VIRTUAL_BUFFER to (LCD_ACTIVE_BUFFER+(SCR_XSIZE*SCR_YSIZE/2))
	rNCACHBE1 = (((unsigned) (LCD_ACTIVE_BUFFER) >> 12) << 16)
			| ((unsigned) (LCD_VIRTUAL_BUFFER) >> 12);
	rZDISRC0 = (DW << 30) | (1 << 28) | LCD_VIRTUAL_BUFFER; // inc
	rZDIDES0 = (2 << 30) | (1 << 28) | LCD_ACTIVE_BUFFER; // inc

	rZDICNT0 = (2 << 28) | (1 << 26) | (LCD_BUF_SIZE) | (3 << 22) | (0 << 20);

	//                      |            |            |             |            |---->0 = Disable DMA
	//                      |            |            |             |------------>Int. whenever transferred
	//                      |            |            |-------------------->Write time on the fly
	//                      |            |---------------------------->Block(4-word) transfer mode
	//                      |------------------------------------>whole service
	//reEnable ZDMA transfer
	rZDICNT0 |= (1 << 20);		//after ES3
	rZDCON0 = 0x1; // start!!!

	Delay(500);
	//while(ucZdma0Done);		//wait for DMA finish
}

/*********************************************************************************************
 * name:		Lcd_Test()
 * func:		LCD test function
 * para:		none
 * ret:		none
 * modify:
 * comment:
 *********************************************************************************************/
void Lcd_Test(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS]) {
	/* initial LCD controller */
	//Lcd_Init();
	/* clear screen */
	Lcd_Clr();
	Lcd_Active_Clr();
	/* draw rectangle pattern */
	Lcd_print_sudoku(cuadricula);

	Lcd_Dma_Trans();
	Delay(100);
}

void Lcd_print_info(void) {
	//Mostramos informacion
	// - Introducir fila A para salir
	// - Tiempo de calculo
	// - Tiempo transcurrido
	Lcd_DspAscII6x8(0, 0, BLACK, "T. transcurrido:\0");
	Lcd_DspAscII6x8(150, 0, BLACK, "s\0");
	Lcd_DspAscII6x8(160, 0, BLACK, "T. calculo:\0");
	Lcd_DspAscII6x8(290, 0, BLACK, "0 \0");
	Lcd_DspAscII6x8(300, 0, BLACK, "us\0");
	Lcd_DspAscII6x8(0, 15, BLACK, "Introducir fila A para salir\0");
}

void Lcd_zoom_region(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS], int x, int y) {

	int index_fila, index_col, i, j, valorCelda;//Indice para posicionar cuadricula
	int posregx, posregy, maxposregx, maxposregy, incrposregx, incrposregy;
	CELDA auxCuadricula;

	index_col = comprobar_region_x(x);	//Eje x
	index_fila = comprobar_region_y(y);	//Eje y

	/* clear screen */
	Lcd_Clr();
	Lcd_Active_Clr();

	//Pintar region en grande
	posregx = 10;
	posregy = 38;
	maxposregx = 307;
	maxposregy = 201;
	incrposregx = 99;
	incrposregy = 67;

	/*	Print cuadricula principal */
	Lcd_Draw_Box_grosor(posregx, posregy, posregx + 3 * incrposregx,
			posregy + 3 * incrposregy - 1, BLACK, 2);

	/* Print celdas con 2 porque antes pintamos cuadricula */
	Lcd_print_celdas(posregx, posregy, maxposregx, maxposregy, incrposregx,
			incrposregy, 2, 0);

	//Pintar indices region
	Lcd_print_indexes_zoom(posregx, posregy, incrposregx, incrposregy,
			index_col, index_fila);

	//Iteracion en region
	for (i = 0; i < 3; i++)	//columnas
			{
		for (j = 0; j < 3; j++)	//filas
		{
			//celda leer valor
			auxCuadricula = cuadricula[index_fila + j][index_col + i];//posicion region click
			valorCelda = celda_leer_valor(auxCuadricula);
			if(!comprobar_celda_error(auxCuadricula)){		//Celda normal
				if (valorCelda > 0) {
					//Pintar valor
					if(comprobar_celda_pista(auxCuadricula)){	//Si es pista, pintamos cuadrado negro
						Lcd_Draw_Box(posregx + (i * incrposregx) + 5, posregy + (j * incrposregy) + 5,
							posregx + (i + 1) * incrposregx - 5, posregy + (j + 1) * incrposregy - 5,
							BLACK);
					}

					Lcd_DspAscII8x16(posregx + (i * incrposregx + (incrposregx / 2)),
							posregy + (j * incrposregy + (incrposregy / 2)), BLACK,
							get_string_from_integer(valorCelda - 1));
				}
				else {
					//Pintar candidatos
					Lcd_print_candidatos(auxCuadricula, posregx + (i * incrposregx), posregy + (j * incrposregy), incrposregx, incrposregy, 1);
				}
			}
			else{	//Error
				//Clear celda en negro
				LcdClrRect(posregx + (i * incrposregx), posregy + (j * incrposregy), posregx + ((i + 1) * incrposregx), posregy + ((j + 1) * incrposregy), BLACK);
				//Print valor en blanco
				if(comprobar_celda_pista(auxCuadricula)){	//Si es pista, pintamos cuadrado blanco
					Lcd_Draw_Box(posregx + (i * incrposregx) + 5, posregy + (j * incrposregy) + 5,
						posregx + (i + 1) * incrposregx - 5, posregy + (j + 1) * incrposregy - 5,
						WHITE);
				}
				Lcd_DspAscII8x16(posregx + (i * incrposregx + (incrposregx / 2)),
							posregy + (j * incrposregy + (incrposregy / 2)), WHITE,
							get_string_from_integer(valorCelda - 1));
			}

		}
	}

	//Mostramos informacion
	Lcd_print_info();

	Lcd_Dma_Trans();
}

int comprobar_region_x(int x) {
	int xreg1, xreg2, xreg3, index;

	xreg1 = 112;
	xreg2 = 214;
	xreg3 = 316;

	index = 0;	//region 0

	if (x > xreg1) {
		if (x > xreg2) {
			index = 6;	//region 2
		} else {
			index = 3;	//region 1
		}
	}
	return index;
}

int comprobar_region_y(int y) {
	int yreg1, yreg2, yreg3, index;

	yreg1 = 104;
	yreg2 = 170;
	yreg3 = 236;

	index = 0;	//region 0

	if (y > yreg1) {
		if (y > yreg2) {
			index = 6;	//region 2
		} else {
			index = 3;	//region 1
		}
	}
	return index;
}

void Lcd_pantalla_inicial(void) {
	/* clear screen */
	Lcd_Clr();
	Lcd_Active_Clr();

	LCD_display_centrado(0, BLACK, "Instrucciones para jugar:\0");//Display string centrado
	Lcd_DspAscII6x8(0, 20, BLACK, "- Resuelva el sudoku siguiendo las reglas del mismo.\0");
	Lcd_DspAscII6x8(0, 45, BLACK, "- Introduzca fila, columna y valor con pulsadores.\0");
	Lcd_DspAscII6x8(0, 70, BLACK, "- Pulse boton izquierdo para incrementar.\0");
	Lcd_DspAscII6x8(0, 95, BLACK, "- Pulse boton derecho para confirmar.\0");
	Lcd_DspAscII6x8(0, 120, BLACK, "- Pulse en una region para hacer zoom.\0");
	//Display string en un cuadrado
	Lcd_Draw_Box(10, 190, 310, 225, BLACK);
	LCD_display_centrado(200, BLACK, "Toque la pantalla para jugar.\0");
	Lcd_Dma_Trans();
}

void Lcd_pantalla_final(int time){
	/* clear screen */
	LcdClrRect(0, 0, 320, 25, WHITE);
	Lcd_DspAscII6x8(0, 15, BLACK, "Fin de la partida. Pulsar boton para reiniciar.\0");
	Lcd_print_tiempo_total(time);
	Lcd_DspAscII6x8(0, 0, BLACK, "T. total partida:\0");
	Lcd_DspAscII6x8(154, 0, BLACK, "segundos\0");
	Lcd_Dma_Trans();
}

void Lcd_print_sudoku(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS]) {

	/* Creacion de variables */
	int x, y, xmax, ymax;	//Posiciones
	int incrx, incry;

	/* Inicializacion de variables */
	x = 10;		//Margen de 11 pixeles a la izquierda
	y = 38;		//Margen de 39 pixeles arriba
	xmax = 308;
	ymax = 200;
	incrx = 34;	//xmax 308 -> incremento x = 34
	incry = 22;	//ymax 200 -> increment y = 22

	/* Mostrar sudoku */
	//Pintar cuadro principal
	Lcd_Draw_Box_grosor(x, y, x + xmax, y + ymax, BLACK, 2);//Print cuadricula principal

	//Pintar divisiones de celdas
	Lcd_print_celdas(x, y, xmax, ymax, incrx, incry, 8, 1);

	//Pintar numeros
	Lcd_print_indexes(x, y, incrx, incry);

	//Pintar elementos
	Lcd_print_info_celda(cuadricula, x, y, incrx, incry);

	//Mostramos informacion
	Lcd_print_info();
}

void Lcd_print_celdas(int x, int y, int xmax, int ymax, int incrx, int incry,
		int iteraciones, int modificarGrosor) {
	int i, grosor;	//Iterador

	i = 0;
	grosor = 1;
	for (i = 0; i < iteraciones; i++) {
		if ((i == 2 || i == 5) && modificarGrosor) {
			grosor = 2;
		}
		Lcd_Draw_VLine(y, y + ymax, x + ((i + 1) * incrx), BLACK, grosor);
		Lcd_Draw_HLine(x, x + xmax, y + ((i + 1) * incry), BLACK, grosor);
		grosor = 1;
	}
}

/* FUNCION PARA LLAMAR CON FUNCION ZOOM SUDOKU*/
void Lcd_print_indexes_zoom(int x, int y, int incrx, int incry, int incrindex_x,
		int incrindex_y) {
	int i, posx, posy, correcty, correctx;
	posx = x + (incrx / 2);
	posy = y + (incry / 2);
	correcty = 5;	//En pixeles
	correctx = 3;	//En pixeles
	for (i = 0; i < 3; i++) {
		Lcd_DspAscII6x8(posx + (i * incrx) - correctx, posy - (incry / 2) - 9,
				BLACK, get_string_from_integer(i + incrindex_x));	//Eje x
		Lcd_DspAscII6x8(0, (posy + (i * incry)) - correcty, BLACK,
				get_string_from_integer(i + incrindex_y));	//Eje y
	}
}

/* FUNCION PARA LLAMAR CON PRINT SUDOKU*/
void Lcd_print_indexes(int x, int y, int incrx, int incry) {
	int i, posx, posy, correcty, correctx;
	posx = x + (incrx / 2);
	posy = y + (incry / 2);
	correcty = 5;	//En pixeles
	correctx = 3;	//En pixeles
	for (i = 0; i < NUM_FILAS; i++) {
		Lcd_DspAscII6x8(posx + (i * incrx) - correctx, posy - incry, BLACK,
				get_string_from_integer(i));	//Eje x
		Lcd_DspAscII6x8(0, (posy + (i * incry)) - correcty, BLACK,
				get_string_from_integer(i));	//Eje y
	}
}

void Lcd_print_info_celda(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS], int x,
		int y, int incrx, int incry) {
	int i, j, valorCelda, posx, posy, correcty, correctx;
	posx = x + (incrx / 2);
	posy = y + (incry / 2);
	correcty = 5;	//En pixeles
	correctx = 3;	//En pixeles
	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			valorCelda = celda_leer_valor(cuadricula[i][j]);
			if(valorCelda > 0)
			{
				if(comprobar_celda_error(cuadricula[i][j])){	//print error
					LcdClrRect(x + (j * incrx), y + (i * incry), x + (j + 1) * incrx,
						y + (i + 1) * incry, BLACK);

					if (comprobar_celda_pista(cuadricula[i][j]))	//Print cuadrado pista
					{
						Lcd_Draw_Box(x + (j * incrx) + 3, y + (i * incry) + 3,
								x + (j + 1) * incrx - 2, y + (i + 1) * incry - 2,
								WHITE);
					}


					Lcd_DspAscII8x16(posx + (j * incrx) - correctx,
							posy + (i * incry) - correcty, WHITE,
							get_string_from_integer(valorCelda - 1));
				}
				else{
					LcdClrRect(x + (j * incrx) + 2, y + (i * incry) + 2, x + (j + 1) * incrx - 2,
						y + (i + 1) * incry - 2, WHITE);

					if (comprobar_celda_pista(cuadricula[i][j]))	//Print cuadrado pista
					{
						Lcd_Draw_Box(x + (j * incrx) + 3, y + (i * incry) + 3,
								x + (j + 1) * incrx - 2, y + (i + 1) * incry - 2,
								BLACK);
					}


					Lcd_DspAscII8x16(posx + (j * incrx) - correctx,
							posy + (i * incry) - correcty, BLACK,
							get_string_from_integer(valorCelda - 1));
				}
			}
			else {	//Print candidatos
				Lcd_print_candidatos(cuadricula[i][j], x + (j * incrx),
						y + (i * incry), incrx, incry, 0);
			}
		}
	}
}

void Lcd_print_candidatos(CELDA celda, int posx, int posy, int incrx, int incry,
		int zoom) {

	int i, value, contx, conty, tamx, tamy;
	contx = 1;
	conty = 1;
	tamx = incrx / 5;
	if (incrx % 5 != 0)
		tamx++;
	tamy = incry / 5;
	if (incry % 5 != 0)
		tamy++;
	if(zoom)
	{
		Lcd_DspAscII6x8(posx + tamx, posy + tamy, BLACK, "Candidatos:\0");	//Display string
	}
	for (i = 0; i < NUM_FILAS; i++) {
		value = celda & (1 << (i + 4));
		if (i == 3 || i == 6)	//Reset x al final
		{
			contx = 1;
			conty++;
		}
		if (value > 0) {
			if (zoom)	//Hay zoom
			{
				Lcd_DspAscII6x8(posx + ((contx) * tamx) + 5, posy + ((conty+1) * tamy),
						BLACK, get_string_from_integer(i));
			} else {	//No hay zoom

				Lcd_print_candidato_actual(posx + contx * tamx,
						posy + conty * tamy, tamx, tamy);
			}
		}
		contx++;
	}
}

void Lcd_print_candidato_actual(int posx, int posy, int incrx, int incry) {
	int i;
	for (i = 0; i < incry - 2; i++) {
		Lcd_Draw_HLine(posx, posx + incrx - 2, posy + i, BLACK, 1);
	}
}

void Lcd_print_tiempo_total(int num){
	int auxNum, i;
	//char *value;

	LcdClrRect(100, 0,149, 8, WHITE);	//Clear time

	auxNum = num;
	//value = "";
	i = 0;
	while(auxNum > 0){
		Lcd_DspAscII6x8(140 - i*6, 0, BLACK, get_string_from_integer(auxNum%10 - 1));
		auxNum = auxNum / 10;
		i++;
	}
	Lcd_Dma_Trans();
}

void Lcd_print_tiempo_calculo(long num){
	long auxNum;
	int i;
	//char *value;

	LcdClrRect(240, 0, 295, 8, WHITE);	//Clear time

	auxNum = num;
	//value = "";
	i = 0;
	while(auxNum > 0){
		Lcd_DspAscII6x8(264 - i*6, 0, BLACK, get_string_from_integer((int) auxNum%10 - 1));
		auxNum = auxNum / 10;
		i++;
	}

	Lcd_Dma_Trans();
}

char* get_string_from_integer(int num) {
	char* value;
	switch (num) {
	case -1:
		value = "0\0";
		break;
	case 0:
		value = "1\0";
		break;
	case 1:
		value = "2\0";
		break;
	case 2:
		value = "3\0";
		break;
	case 3:
		value = "4\0";
		break;
	case 4:
		value = "5\0";
		break;
	case 5:
		value = "6\0";
		break;
	case 6:
		value = "7\0";
		break;
	case 7:
		value = "8\0";
		break;
	case 8:
		value = "9\0";
		break;
	}
	return value;
}

void LCD_display_centrado(int y, int color, char *string) {

	int length = strlen(string);
	Lcd_DspAscII6x8((LCD_XSIZE - (length * 8)) / 2, y, color, string);
}
