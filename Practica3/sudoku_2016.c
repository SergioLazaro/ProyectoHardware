/*
 * Autores:
 * Oscar Leon Barbed Perez 666137
 * Sergio Lazaro Magdalena 556030
 *
 * */

#include "sudoku_2016.h"

//http://stackoverflow.com/questions/14526153/multiple-definition-of-a-global-variable

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
inline uint8_t
celda_leer_valor(CELDA celda)
{
    return (celda & 0x000F);
}

/* *****************************************************************************
 * Devuelve 1 si y solo si la celda es una pista */

int comprobar_celda_pista(CELDA celda)
{
	return (celda & 0x8000);
}

/* *****************************************************************************
 * Devuelve 1 si y solo si la celda es un error */
int comprobar_celda_error(CELDA celda)
{
	return (celda & 0x4000);
}
void celda_poner_error(CELDA *celdaptr){
	*celdaptr |= 0x4000;
}

void celda_quitar_error(CELDA *celdaptr){
	*celdaptr &= 0xbfff;
}

/* *****************************************************************************
 * recorrer fila descartando valor de listas candidatos */
inline void descartar_candidatos_fila(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS],
                             uint8_t fila, uint8_t columna, uint8_t valor)
{
	int i;
	//Iteramos sobre NUM_FILAS porque vale 9 pero iteramos sobre las columnas.
	for(i=0; i<NUM_FILAS; i++){
		if(celda_leer_valor(cuadricula[fila][i]) == valor){	//Celda repetida
			celda_poner_error(&cuadricula[fila][i]);	//Poner error celda secundaria
			celda_poner_error(&cuadricula[fila][coumna]);	//Poner error celda elegida
		}
		else{
			celda_quitar_error(&cuadricula[fila][i]);	//Quitar error si lo hay
		}
		cuadricula[fila][i] &= ~(1 << ((valor-1) + 4));
	}
}

/* *****************************************************************************
 * recorrer columna descartando valor de listas candidatos */
inline void descartar_candidatos_columna(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS],
		uint8_t fila, uint8_t columna, uint8_t valor)
{
	int i;

	for(i=0; i<NUM_FILAS; i++){
		if(celda_leer_valor(cuadricula[i][columna]) == valor){	//Celda repetida
			celda_poner_error(&cuadricula[i][columna]);	//Poner error celda secundaria
			celda_poner_error(&cuadricula[fila][coumna]);	//Poner error celda elegida
		}
		else{
			celda_quitar_error(&cuadricula[i][columna]);
		}
		cuadricula[i][columna] &= ~(1 << ((valor-1) + 4));
	}
}

/* *****************************************************************************
 * recorrer region descartando valor de listas candidatos */
inline void descartar_candidatos_region(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS],
                             uint8_t fila, uint8_t columna, uint8_t valor)
{
	int i, j, filai, colj;
	if(fila<3){
		filai=0;//Fila de inicio de la region
	}
	else if(fila<6){
		filai=3;
	}
	else{
		filai=6;
	}
	if(columna<3){
		colj=0;//Columna de inicio de la region
	}
	else if(fila<6){
		colj=3;
	}
	else{
		colj=6;
	}

	for(i=0; i<3; i++){
		for(j=0; j<3; j++){
			if(celda_leer_valor(cuadricula[filai + i][colj + j]) == valor){	//Celda repetida
				celda_poner_error(&cuadricula[filai + i][colj + j]);	//Poner error celda secundaria
				celda_poner_error(&cuadricula[fila][coumna]);	//Poner error celda elegida
			}
			else{
				celda_quitar_error(cuadricula[filai+i][colj+j]);
			}
			cuadricula[filai+i][colj+j] &= ~(1 << ((valor-1) + 4));
		}
	}
}

int sudoku_candidatos_modificar(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS], uint8_t fila, 
	uint8_t columna, uint8_t valor){
	//Declaracion variables
	CELDA celda; 
	int celdas_vacias;

	celda = cuadricula[fila][columna];
	celdas_vacias = 0;
	if(!comprobar_celda_pista(celda)){	//No es pista
		celda_poner_valor(&celda[i][j], valor);
		if(valor >= 0){	//Borrar celda o modificar
			//cuadricula[i][j] = celda & 0xbff0;
			//Borramos celda -> aumenta num. celdas vacias
			if(valor == 0) celdas_vacias = 1;	
			sudoku_candidatos_init_c(cuadricula);
		}
		else{	//Celda vacia -> metemos nuevo valor
			sudoku_candidatos_propagar_c(cuadricula, fila, columna);
			celdas_vacias = -1;
		}
	}
	return celdas_vacias;
}


/* *****************************************************************************
 * propaga el valor de una determinada celda
 * para actualizar las listas de candidatos
 * de las celdas en su su fila, columna y región
 * */

int sudoku_candidatos_propagar_c(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS],
                             uint8_t fila, uint8_t columna)
{
	uint8_t valor;
	valor = celda_leer_valor(cuadricula[fila][columna]) 
	if(valor > 0){
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

	for(i=0; i<NUM_FILAS; i++){
		for(j=0; j<NUM_FILAS; j++){
			celdas_vacias += sudoku_candidatos_propagar_c(cuadricula, i, j); //THUMB
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

    celdas_vacias = sudoku_candidatos_init_arm(cuadricula);
}

