#include "sudoku_2016.h"

/* *****************************************************************************
 * Funciones privadas (static)
 * (no pueden ser invocadas desde otro fichero) */
 
/* *****************************************************************************
 * modifica el valor almacenado en la celda indicada */
static inline void
celda_poner_valor(CELDA *celdaptr, uint8_t val)
{
    *celdaptr = (*celdaptr & 0xFFF0) | (val & 0x000F);
}

/* *****************************************************************************
 * extrae el valor almacenado en los 16 bits de una celda */
static inline uint8_t
celda_leer_valor(CELDA celda)
{
    return (celda & 0x000F);
}
/* *****************************************************************************
 * recorrer fila descartando valor de listas candidatos */
static inline void descartar_candidatos_fila(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS],
                             uint8_t fila, uint8_t valor)
{
	int i;	
	//Iteramos sobre NUM_FILAS porque vale 9 pero iteramos sobre las columnas.
	for(i=0; i<NUM_FILAS; i++){
		cuadricula[fila][i] &= 0 << ((valor-1) + 4);
	}
}

/* *****************************************************************************
 * recorrer columna descartando valor de listas candidatos */
static inline void descartar_candidatos_columna(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS],
                             uint8_t columna, uint8_t valor)
{
	int i;	

	for(i=0; i<NUM_FILAS; i++){
		cuadricula[i][columna] &= 0 << ((valor-1) + 4);
	}
}

/* *****************************************************************************
 * recorrer region descartando valor de listas candidatos */
static inline void descartar_candidatos_region(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS],
                             uint8_t fila, uint8_t columna, uint8_t valor)
{
	int i, j, filai, colj;
	filai=3*(fila/3);	//Fila de inicio de la region
	colj=3*(columna/3);	//Columna de inicio de la region
		
	for(i=0; i<3; i++){
		for(j=0; j<3; j++){
			cuadricula[filai+i][colj+j] &= 0 << ((valor-1) + 4);
		}
	}
}

/* *****************************************************************************
 * propaga el valor de una determinada celda
 * para actualizar las listas de candidatos
 * de las celdas en su su fila, columna y región */
static void
sudoku_candidatos_propagar_c(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS],
                             uint8_t fila, uint8_t columna)
{
    /* valor que se propaga */
    uint8_t valor = celda_leer_valor(cuadricula[fila][columna]);

    /* recorrer fila descartando valor de listas candidatos */
	descartar_candidatos_fila(cuadricula, fila, valor);
    /* recorrer columna descartando valor de listas candidatos */
	descartar_candidatos_columna(cuadricula, columna, valor);
    /* recorrer region descartando valor de listas candidatos */
	descartar_candidatos_region(cuadricula, fila, columna, valor);
}

/* *****************************************************************************
 * calcula todas las listas de candidatos (9x9)
 * necesario tras borrar o cambiar un valor (listas corrompidas)
 * retorna el numero de celdas vacias */
static int
sudoku_candidatos_init_c(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS])
{
    int celdas_vacias = 0;

    /* recorrer cuadricula celda a celda */
	//Creacion de variables	
	int i, j;
	for(i=0; i<NUM_FILAS; i++){
		for(j=0; j<NUM_COLUMNAS; j++){
			//Si la celda no contiene un valor, ponemos todos los candidatos a 1
			//para mas tarde eliminar los que ya no lo sean
			if(cuadricula[i][j] & (0 << 15)){
				/* inicializa lista de candidatos */
				cuadricula[i][j]=cuadricula[i][j] bitor 0x1FF0;
			}
		}
	}
        
	/* recorrer cuadricula celda a celda */
	for(i=0; i<NUM_FILAS; i++){
		for(j=0; j<NUM_COLUMNAS; j++){
        	/* si celda tiene valor */
			if(cuadricula[i][j] & (1 << 15)){
        		/*    sudoku_candidatos_propagar_c(...); */
				sudoku_candidatos_propagar_c(cuadricula, i, j);
        	/* else actualizar contador de celdas vacias */
			}else{
				celdas_vacias++;
			}
		}
	}

    /* retorna el numero de celdas vacias */
    return (celdas_vacias);
}

/* *****************************************************************************
 * Funciones públicas
 * (pueden ser invocadas desde otro fichero) */

/* *****************************************************************************
 * programa principal del juego que recibe el tablero,
 * y la señal de ready que indica que se han actualizado fila y columna */
void
sudoku9x9(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS], char *ready)
{
    int celdas_vacias;     //numero de celdas aun vacias

    /* calcula lista de candidatos, versión C */
    celdas_vacias = sudoku_candidatos_init_c(cuadricula);

    /* verificar que la lista de candidatos calculada es correcta */
    /* cuadricula_candidatos_verificar(...) */

    /* repetir para otras versiones (C optimizado, ARM, THUMB) */
}

