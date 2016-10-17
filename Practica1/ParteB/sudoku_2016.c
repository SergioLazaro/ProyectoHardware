#include "sudoku_2016.h"


/* *****************************************************************************
 * Funciones privadas (static)
 * (no pueden ser invocadas desde otro fichero) */
 

static inline int OPCION_EJECUCION;

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
		cuadricula[fila][i] &= ~(1 << ((valor-1) + 4));
	}
}

/* *****************************************************************************
 * recorrer columna descartando valor de listas candidatos */
static inline void descartar_candidatos_columna(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS],
                             uint8_t columna, uint8_t valor)
{
	int i;

	for(i=0; i<NUM_FILAS; i++){
		cuadricula[i][columna] &= ~(1 << ((valor-1) + 4));
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
			cuadricula[filai+i][colj+j] &= ~(1 << ((valor-1) + 4));
		}
	}
}

/* *****************************************************************************
 * propaga el valor de una determinada celda
 * para actualizar las listas de candidatos
 * de las celdas en su su fila, columna y región */
int sudoku_candidatos_propagar_c(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS],
                             uint8_t fila, uint8_t columna)
{

	if(cuadricula[fila][columna] & (1 << 15)){
		/* valor que se propaga */
		uint8_t valor = celda_leer_valor(cuadricula[fila][columna]);

		/* recorrer fila descartando valor de listas candidatos */
		descartar_candidatos_fila(cuadricula, fila, valor);
		/* recorrer columna descartando valor de listas candidatos */
		descartar_candidatos_columna(cuadricula, columna, valor);
		/* recorrer region descartando valor de listas candidatos */
		descartar_candidatos_region(cuadricula, fila, columna, valor);

		return 0;

	}
	else{
		/* else actualizar contador de celdas vacias */
		return 1;
	}
}

/* *****************************************************************************
 * calcula todas las listas de candidatos (9x9)
 * necesario tras borrar o cambiar un valor (listas corrompidas)
 * retorna el numero de celdas vacias */
int sudoku_candidatos_init_c(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS])
{
    int celdas_vacias = 0;

    /* recorrer cuadricula celda a celda */
	//Creacion de variables
	int i, j, check;
	//loop
	for(i=0; i<NUM_FILAS; i++){
		for(j=0; j<NUM_FILAS; j++){
			/* inicializa lista de candidatos */
			cuadricula[i][j] = cuadricula[i][j] | 0x1FF0;
		}
	}
	if(OPCION_EJECUCION == 0){

	}
	/* recorrer cuadricula celda a celda */
	for(i=0; i<NUM_FILAS; i++){
		for(j=0; j<NUM_FILAS; j++){
			celdas_vacias += sudoku_candidatos_propagar_c(cuadricula, i, j);
		}
	}

    /* retorna el numero de celdas vacias */
    return (celdas_vacias);
}

extern int sudoku_candidatos_init_arm(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS]);
extern int sudoku_candidatos_propagar_arm(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS],uint8_t fila, uint8_t columna);

/**void call_sudoku_arm_arm(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS])
{
	struct timeval  tv1, tv2;

	int celdas_vacias;
	gettimeofday(&tv1, 0);
	celdas_vacias = sudoku_candidatos_init_arm(cuadricula);
	gettimeofday(&tv2, 0);
	double tiempo = (tv2.tv_usec - tv1.tv_usec)/1000;
	printf("Tiempo ejecucion ARM-ARM: %f",tiempo);
}

void call_sudoku_c_c(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS])
{
		struct timeval  tv1, tv2;

		int celdas_vacias;
		gettimeofday(&tv1, 0);
		celdas_vacias = sudoku_candidatos_init_c(cuadricula);
		gettimeofday(&tv2, 0);
		double tiempo = (tv2.tv_usec - tv1.tv_usec)/1000;
		printf("Tiempo ejecucion C-C: %f",tiempo);
}

/*void call_sudoku_arm_arm(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS])
{
	int celdas_vacias;
	clock_t inicio = clock();
	celdas_vacias = sudoku_candidatos_init_arm(cuadricula);
	clock_t fin = clock();
	double tiempo = (fin - inicio)/CLOCKS_PER_SEC;
	printf("Tiempo ejecucion ARM-ARM: %f",tiempo);
}
*/



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

	//celdas_vacias = sudoku_candidatos_init_c(cuadricula);

    //call_sudoku_arm_arm(cuadricula);
    //call_sudoku_c_c(cuadricula);

    /* verificar que la lista de candidatos calculada es correcta */
    /* cuadricula_candidatos_verificar(...) */

    /* repetir para otras versiones (C optimizado, ARM, THUMB) */
}

