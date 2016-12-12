/*********************************************************************************************
 * File£º	lcd.c
 * Author:	embest
 * Desc£º	LCD control and display functions
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
	Lcd_Init();
	/* clear screen */
	Lcd_Clr();
	Lcd_Active_Clr();
	/* draw rectangle pattern */
	Lcd_print_sudoku(cuadricula);

	Lcd_Dma_Trans();
	Delay(100);
}

void Lcd_zoom_region(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS], int x, int y)
{
	int posx, posy, i, j, valorCelda;	//Indice para posicionar cuadricula
	int posregx, posregy, maxposregx, maxposregy;
	CELDA auxCuadricula;

	posx = comprobar_region_x(x);
	posy = comprobar_region_y(y);

	auxCuadricula = cuadricula[posx][posy];	//posicion region click

	/* clear screen */
	Lcd_Clr();
	Lcd_Active_Clr();
	//Pintar region en grande
	posregx = 10;
	posregy = 38;
	maxposregx = 307;
	maxporegy = 239;
	incrposregx = 99;
	incrposregy = 67;

	Lcd_print_celdas(posregx, posregy, maxposregx, maxposregy, incrposregx, incrposregy, 3, 0);

	//Pintar indices region
	Lcd_print_indexes(posregx, posregy, incrposregx, incrposregy, 3, posx + 1, posy + 1);
	//Iteracion en region
	tmpCuadricula = auxCuadricula;	//trabajar con esta
	for(i = 0; i < 3; i++)	//columnas
	{
		for(j = 0; j < 3; j++)	//filas
		{
			//celda leer valor
			valorCelda = (auxCuadricula & 0x000F);
			if(valorCelda > 0)
			{
				//Pintar valor
				Lcd_DspAscII6x8(posregx + (i * incrposregx), posregy + (j * incrposregy), BLACK,
						get_string_from_integer(valorCelda - 1));
			}
			else{
				//Pintar candidatos
				Lcd_print_candidatos(tmpCuadricula, posregx + (i * incrposregx), y + (j * incrposregy), incrposregx, incrposregy, 1);
			}
			auxCuadricula = cuadricula[posx + j][posy];	//posicion region click
		}
		posy++;
	}


}

int comprobar_region_x(int x)
{
	int xreg1, xreg2, xreg3, index;

	xreg1 = 112;
	xreg2 = 214;
	xreg3 = 316;

	index = 0;	//region 0

	if(x > xreg1)
	{
		if(x > xreg2){
			index = 6;	//region 2
		}
		else{
			index = 3;	//region 1
		}
	}
	return index;
}

int comprobar_region_y(int y)
{
	int yreg1, yreg2, yreg3, index;

	yreg1 = 104;
	yreg2 = 170;
	yreg3 = 236;

	index = 0;	//region 0

	if(y > yreg1)
	{
		if(y > yreg2){
			index = 6;	//region 2
		}
		else{
			index = 3;	//region 1
		}
	}
	return index;
}

void Lcd_pantalla_inicial(void) {
	//LCD_XSIZE //320
	//LCD_YSIZE //240
	LCD_display_centrado(0, BLACK, "Instrucciones para jugar:");//Display string centrado
	Lcd_DspAscII6x8(0, 20, BLACK, "Instruccion 1");	//Display string
	Lcd_DspAscII6x8(0, 45, BLACK, "Instruccion 2");	//Display string
	Lcd_DspAscII6x8(0, 70, BLACK, "Instruccion 3");	//Display string
	//Display string en un cuadrado
	Lcd_Draw_Box(10, 190, 310, 225, BLACK);
	LCD_display_centrado(200, BLACK, "Toque la pantalla para jugar");
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
	Lcd_print_indexes(x, y, incrx, incry, 9, 0, 0);

	//Pintar elementos
	Lcd_print_info_celda(cuadricula, x, y, incrx, incry);


	//Mostramos informacion
	// - Introducir fila A para salir
	// - Tiempo de calculo
	// - Tiempo transcurrido
	Lcd_DspAscII6x8(0, 0, BLACK, "T. transcurrido: ");
	Lcd_DspAscII6x8(160, 0, BLACK, "T. calculo: ");
	Lcd_DspAscII6x8(0, 15, BLACK, "Introducir fila A para salir");
}

void Lcd_print_celdas(int x, int y, int xmax, int ymax, int incrx, int incry, int iteraciones, int modificarGrosor) {
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

void Lcd_print_indexes(int x, int y, int incrx, int incry, int iteraciones, int incrindex_x, int incrindex_y) {
	int i, posx, posy, correcty, correctx;
	posx = x + (incrx / 2);
	posy = y + (incry / 2);
	correcty = 5;	//En pixeles
	correctx = 3;	//En pixeles
	for (i = 0; i < iteraciones; i++) {
		Lcd_DspAscII6x8(posx + (i * incrx) - correctx, posy - incry, BLACK,
				get_string_from_integer(i + incrindex_x));	//Eje x
		Lcd_DspAscII6x8(0, (posy + (i * incry)) - correcty, BLACK,
				get_string_from_integer(i + incrindex_y));	//Eje y
	}
}

void Lcd_print_info_celda(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS], int x, int y, int incrx, int incry) {
	int i, j, valorPista, posx, posy, correcty, correctx;
	posx = x + (incrx / 2);
	posy = y + (incry / 2);
	correcty = 5;	//En pixeles
	correctx = 3;	//En pixeles
	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			if (comprobar_celda_pista(cuadricula[i][j]))	//Print pista
			{
				valorPista = celda_leer_valor(cuadricula[i][j]);
				Lcd_Draw_Box(x + (i * incrx) + 3, y + (j * incry) + 3, x + (i + 1) * incrx - 2, y + (j + 1) * incry - 2, BLACK);
				Lcd_DspAscII6x8(posx + (i * incrx) - correctx, posy + (j * incry) - correcty, BLACK,
						get_string_from_integer(valorPista - 1));
			}
			else{	//Print candidatos
				Lcd_print_candidatos(cuadricula[i][j], x + (i * incrx), y + (j * incry), incrx, incry, 0);
			}
		}
	}
}

void Lcd_print_candidatos(CELDA celda, int posx, int posy, int incrx, int incry, int zoom)
{

	int i, value, contx, conty;
	contx = 1;
	conty = 1;
	for(i=0; i<NUM_FILAS; i++){
		value = celda & ~(1 << ((i + 3));
		if(value > 0)
		{
			if(i == 3 || i == 6)	//Reset x al final
			{
				contx = 1;
				conty++;
			}
			if(zoom)	//Hay zoom
			{
				Lcd_DspAscII6x8(posx + (contx * incrx), posy + (conty * incry), BLACK, get_string_from_integer(i));
			}
			else{	//No hay zoom
				Lcd_print_candidato_actual(posx + contx*(incrx / 7), posy + conty*(incry / 7) , (incrx / 7), (incry/7));
			}
			
		}
		contx++;
	}
}

void Lcd_print_candidato_actual(int posx, int posy, int incrx, int incry)
{
	int i;
	for (i = 0; i < incry + 2; i++) {
		Lcd_Draw_HLine(posx, posx + incrx + 4, posy + i, BLACK, 1);
	}
}

char* get_string_from_integer(int num) {
	char* value;
	switch (num) {
	case 0:
		value = "1";
		break;
	case 1:
		value = "2";
		break;
	case 2:
		value = "3";
		break;
	case 3:
		value = "4";
		break;
	case 4:
		value = "5";
		break;
	case 5:
		value = "6";
		break;
	case 6:
		value = "7";
		break;
	case 7:
		value = "8";
		break;
	case 8:
		value = "9";
		break;
	}
	return value;
}

void LCD_display_centrado(int y, int color, char *string) {

	int length = strlen(string);
	Lcd_DspAscII6x8((LCD_XSIZE - (length * 8)) / 2, y, color, string);
}
