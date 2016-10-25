/*
 * Autores:
 * Oscar Leon Barbed Perez 666137
 * Sergio Lazaro Magdalena 556030
 *
 * */

#include "sudoku_2016.h"


/* *****************************************************************************
 * Funciones privadas (static)
 * (no pueden ser invocadas desde otro fichero) */
 

static inline int OPCION_EJECUCION = 0;

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
                             uint8_t fila, uint8_t columna, uint8_t valor)
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
		uint8_t fila, uint8_t columna, uint8_t valor)
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
 * de las celdas en su su fila, columna y región
 * */

int sudoku_candidatos_propagar_c(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS],
                             uint8_t fila, uint8_t columna)
{

	if(cuadricula[fila][columna] & (1 << 15)){
		/* valor que se propaga */
		uint8_t valor = celda_leer_valor(cuadricula[fila][columna]);

		/* recorrer fila descartando valor de listas candidatos */
		descartar_candidatos_fila(cuadricula, fila, columna, valor);
		/* recorrer columna descartando valor de listas candidatos */
		descartar_candidatos_columna(cuadricula, fila, columna, valor);
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
 * retorna el numero de celdas vacias
 * */

int sudoku_candidatos_init_c(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS])
{

    /* recorrer cuadricula celda a celda */
	//Creacion de variables
	int i, j, celdas_vacias;

	celdas_vacias = 0;
	/* inicializa lista de candidatos */
	for(i=0; i<NUM_FILAS; i++){
		for(j=0; j<NUM_FILAS; j++){
			cuadricula[i][j] = cuadricula[i][j] | 0x1FF0;		//Todos los candidatos a 1
		}
	}

	/* Ejecucion candidatos propagar. Depende de OPCION_EJECUCION. */
	if(OPCION_EJECUCION == 0){
		for(i=0; i<NUM_FILAS; i++){
			for(j=0; j<NUM_FILAS; j++){
				celdas_vacias += sudoku_candidatos_propagar_c(cuadricula, i, j);	//C
			}
		}
	}else if(OPCION_EJECUCION == 1){
		for(i=0; i<NUM_FILAS; i++){
			for(j=0; j<NUM_FILAS; j++){
				celdas_vacias += sudoku_candidatos_propagar_arm(cuadricula, i, j); //ARM
			}
		}
	}else{
		for(i=0; i<NUM_FILAS; i++){
			for(j=0; j<NUM_FILAS; j++){
				celdas_vacias += puente_arm_thumb(cuadricula, i, j); //THUMB
			}
		}
	}

    /* retorna el numero de celdas vacias */
    return (celdas_vacias);
}

int cuadricula_candidatos_verificar(CELDA cuadricula_verificar[NUM_FILAS][NUM_COLUMNAS],
		CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS]){
	int i, j, check;
	check = 0;
	while(i < NUM_FILAS && check){	//En caso de fallo, evitamos seguir iterando
		while(j < NUM_FILAS && check){	//En caso de fallo, evitamos seguir iterando
			if(cuadricula_verificar[i][j] != cuadricula[i][j]){
				check = 1;
			}
			j++;
		}
		i++;
		j = 0;
	}

	return check;
}

/* Declaracion funciones externas a C */

extern int sudoku_candidatos_init_arm(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS]);
extern int sudoku_candidatos_propagar_arm(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS],uint8_t fila, uint8_t columna);
extern int puente_arm_thumb(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS],uint8_t fila, uint8_t columna);

/* *****************************************************************************
 * Funciones públicas
 * (pueden ser invocadas desde otro fichero) */

/* *****************************************************************************
 * programa principal del juego que recibe el tablero,
 * y la señal de ready que indica que se han actualizado fila y columna */
void
sudoku9x9(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS], char *ready)
{
    int celdas_vacias, i, j, k, num_incorrectas;     //numero de celdas aun vacias
    CELDA cuadricula_verificar[NUM_FILAS][NUM_COLUMNAS];

    //Iteramos 3 veces sudoku_candidatos_init_c y sudoku_candidatos_init_arm

    for(i = 0; i < 3; i++)
    {
    	celdas_vacias = sudoku_candidatos_init_c(cuadricula);

    	//Codigo para obtener cuadricula resuelta en primera llamada
    	if(OPCION_EJECUCION == 0){	//Breakpoint aqui
    		for(j = 0; j < NUM_FILAS; j++){
				for(k = 0; k < NUM_COLUMNAS; k++){
					cuadricula_verificar[j][k] = cuadricula[j][k];
				}
			}
    	}

    	//Verificamos si la cuadricula obtenida con C es correcta
    	num_incorrectas += cuadricula_candidatos_verificar(cuadricula_verificar,cuadricula);

    	//Ejecucion codigo ARM. Se gestiona dentro las llamadas a las distintas posibilidades
    	//con una variable global similar a la empleada en C.
    	celdas_vacias = sudoku_candidatos_init_arm(cuadricula);

    	//Verificamos si la cuadricula obtenida con ARM es correcta
    	//Breakpoint aqui
    	num_incorrectas += cuadricula_candidatos_verificar(cuadricula_verificar,cuadricula);

    	OPCION_EJECUCION++;	//Modificamos variable para proxima combinacion
    }

}

