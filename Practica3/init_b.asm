.text
/*
 * Autores:
 * Oscar Leon Barbed Perez 666137
 * Sergio Lazaro Magdalena 556030
 *
 */
#
#  USING A .C FUNCTION
#
# FUNCTION CALL the parameters are stored in r0 and r1
# If there are 4 or less parameters when calling a C function the compiler
# assumes that they have been stored in r0-r3.
# If there are more parameters you have to store them in the data stack
# using the stack pointer
# function __c_copy is in copy.c

# Declaramos subrutinas globales para poder llamar desde C
.arm
.global sudoku_candidatos_init_arm
.global sudoku_candidatos_propagar_arm
.global sudoku_candidatos_modificar
.global init

#	USING THUMB CODE
################################################################################

#Llamada a nuevo codigo C

init:
		.extern Main
		LDR		r4, =Main
		MOV     lr, pc
        BX      r4

#	USING ARM CODE
#
# Comprobar estado del sudoku tras llamada a funcion C
# En caso de estar establecido todo deberiamos limpiar
# los candidatos.

sudoku_candidatos_init_arm:

		MOV IP, SP
		STMDB SP!, {r4-r9,FP,IP,LR,PC}
		SUB FP, IP, #4

		MOV		r4, r0
		# @cuadricula en r0
		MOV		r1, #0			//IterFila
		MOV		r2, #0			//IterColumna
		MOV		r9, #0			//Celdas_vacias
		#Cosicas para hacer mov con mas de 1 byte
		MOV		r3, #0x1f
		MOV		r3, r3, LSL #8
		ADD		r3, r3,#0xf0

		#Inicio bucle
		columnloop:
		ADD		r1, r1, #1

		rowloop:

		ADD		r2, r2, #1		//Actualizamos iterFile

		LDRH	r5, [r4]
		ORR		r5, r5, r3
		STRH	r5, [r4], #2

		#Comprobamos si es ultimo elemento (fin bucle fila)
		CMP		r2,#9
		BNE		rowloop
		MOV		r2, #0
		ADD		r4, r4, #14	//Actualizamos direccion elemento teniendo en cuenta el padding
		CMP		r1,#9		//Comparamos que hemos recorrido todo el sudoku
		BLO		columnloop	//Fin de fila
		MOV		r1, #0

arm_call:
		//LLAMADA ARM
		MOV		r4, r0
		MOV		r5, r1
		MOV		r6, r2
		MOV		r7, r3
		BL		sudoku_candidatos_propagar_arm
		MOV		r0, r4
		MOV		r1, r5
		MOV		r2, r6
		MOV		r3, r7
		B 		return

return:
		ADD		r9, r9, r0		//Sumamos valor de celdas_vacias
		.extern cuadricula
		LDR		r0, =cuadricula //Necesario ya que r0 contiene el return del metodo propagar (celdas_vacias)
		ADD		r2, r2, #1
		CMP		r2, #9
		BNE		arm_call
		#Check iteracion columna
		ADD		r1, r1, #1
		CMP		r1, #9
		MOVLO	r2, #0
		BLO		arm_call

		MOV		r0, r9			//Return celdas_vacias
		LDMDB 	FP, { r4-r9,FP,SP,PC }

sudoku_candidatos_modificar:
		#Obtener registro que hemos apilado
		#r0 -> @celda
		#r1 -> posicion fila
		#r2 -> posicion columna
		#r3 -> valor

		MOV IP, SP
		STMDB SP!, {r4-r8,FP,IP,LR,PC}
		SUB FP, IP, #4

		ADD		r4, r0, r1, LSL #5		//Nos colocamos en la j
		ADD		r4, r4, r2, LSL #1		//Nos colocamos en la i

		# r4 contiene direccion celda de interes
		LDRH	r5, [r4]		//r5 -> valor celda

		#Comprobar si es pista
		MOV		r6, #0
		ADD		r7, r6, r5, LSR #15	//r7 contiene bit pista
		CMP		r7, #1
		MOVEQ	r0, #0				//celdas_vacias += 0
		BEQ		fin_comprobar		//Celda con pista -> fin

		#Se comprueba si se quiere borrar la celda
		CMP		r3, #0
		BEQ		borrar_valor_celda

		#Se comprueba si se modifica una celda con valor previo
		AND		r6, r5, #0xf	//AND para quedarnos con el valor de la celda
		CMP		r6, #0
		BGT		modificar_valor_celda

propagar:
		ORR		r5, r5, r3		//Meter valor del usuario
		STRH	r5, [r4]
		BL		sudoku_candidatos_propagar_arm
		B		fin_comprobar
		#MOV		r0, #1		//Return -> celdas vacias += 1

modificar_valor_celda:
		AND		r5, r5, #0		//Limpiar valor
		ORR		r5, r5, r3		//Añadir nuevo valor
		STRH	r5, [r4]		//Modificamos valor con el nuevo
		BL		sudoku_candidatos_init_arm
		MOV		r0, #2
		B		fin_comprobar

borrar_valor_celda:
		//Cosicas para poder hacer AND
		MOV 	r6, #0xbf
		MOV		r6, r6, LSL #8
		ADD		r6, r6, #0xf0
		AND		r5, r5, r6			//Metemos 0 en bits de valor
		STRH	r5, [r4]
		BL		sudoku_candidatos_init_arm
		MOV		r0, #3
		B 		fin_comprobar


fin_comprobar:
		# r0 = -1 -> insertada celda erronea
		# r0 = 0 -> insertada celda pista
		# r0 = 1 -> insertada celda correcta
		# r0 = 2 -> insertada celda y modificada
		# r0 = 3 -> insertada celda para borrar
		LDMDB 	FP, {r4-r8,FP,SP,PC}



sudoku_candidatos_propagar_arm:
		#Obtener registro que hemos apilado
		#r0 -> @celda
		#r1 -> posicion fila
		#r2 -> posicion columna

		MOV IP, SP
		STMDB SP!, {r4-r7,FP,IP,LR,PC}
		SUB FP, IP, #4

		#Obtenemos direccion elemento a modificar

		ADD		r4, r0, r1, LSL #5		//Nos colocamos en la j
		ADD		r4, r4, r2, LSL #1		//Nos colocamos en la i

		# Obtenemos el primer bit que nos indica si es pista o no
		/*
		Dos opciones:

		1) Hacer comprobacion del bit 15 y si es pista se obtiene el valor, evitando asi ejecutar
			instrucciones no necesarias. Se necesitaria una instruccion mas para copiar el contenido
			de la celda.

		2) Realizarlo como esta actualmente.

		*/

		LDRH	r4, [r4]		//Celda
		AND		r3, r4, #0xf	//AND para quedarnos con el valor de la celda

		CMP		r3, #0
		BEQ		fin_propagar_1

		MOV		r4, r0
		MOV		r5, r1
		MOV		r6, r2
		MOV		r7, r3
		#comprobar fila
		BL	checkfila

		MOV		r0, r4
		MOV		r1, r5
		MOV		r2, r6
		MOV		r3, r7
		#comprobar columna
		BL	checkcolum

		MOV		r0, r4
		MOV		r1, r5
		MOV		r2, r6
		MOV		r3, r7
		#comprobar region
		BL	checkreg

		# Return 0 celdas_vacias
		MOV		r0, #0
		LDMDB	FP, {r4-r7,FP,SP,PC}

		# Return 1 celdas_vacias
fin_propagar_1:
		MOV		r0, #1
		LDMDB 	FP, {r4-r7,FP,SP,PC}


################################################################################
# BLOQUES DE ACTIVACION PARA OPTIMIZAR CODIGO ARM Y THUMB (LIMITACION REGISTROS)
checkfila:
		#Obtener registro que hemos apilado
		#r0 -> @celda
		#r1 -> numero fila
		#r2 -> numero columna
		#r3 -> valor

		STMFD   sp!, {r4-r9, LR}

		#Creamos mascara para modificar bit
		MOV		r6, #1
		MOV		r5, #0				//Empleamos el 0 para el LSL
		ADD		r3, r3, #3			//Sumamos (4-1) para evitar los 4 bits de valor
		ADD		r6, r5, r6, LSL r3	//Colocamos bit a modificar en ((valor - 1) + 4)
		MVN		r4, r6				//Mascara
		SUB		r3, r3, #3			//Recuperamos valor real

		MOV		r5, #0
		#Colocamos registro al principio de la fila
		MOV		r7, r0
		ADD		r7, r7, r1, LSL #5
		#Calcular direccion elemento propagado
		ADD		r0, r0, r1, LSL #5		//Nos colocamos en la j
		ADD		r0, r0, r2, LSL #1		//Nos colocamos en la i

		loopfila:
		LDRH	r6, [r7]
		# r6 -> celda actual
		# r3 -> valor insertado
		CMP		r0, r7
		BEQ		continuar1
		# Comprobar si hay coincidencia
		AND		r9, r6, #0xf		//r9 contiene valor celda actual
		CMP		r9, r3
		BNE		noerror1
		#pone error en celda origen
		LDRH	r9, [r0]
		ORR		r9, r9, #0x4000			//Si coincide -> metemos error siempre
		STRH	r9, [r0]
		ORR		r6, r6, #0x4000		//pone error en celda actual
		B 		continuar1

noerror1:
		LDRH	r9, [r0]
		#AND		r9, r9, #0xBFFF			//Si coincide -> metemos error siempre
		STRH	r9, [r0]
		#AND		r6, r6, #0xBFFF		//pone error en celda actual

continuar1:
		AND		r6, r6, r4			//Guardamos la modificacion de la celda
		STRH	r6, [r7], #2
		ADD		r5, r5, #1
		//Comprobamos iteracion
		CMP		r5, #9
		BLT		loopfila

		LDMFD SP!, { r4-r9, PC }

checkcolum:
		#Obtener registro que hemos apilado
		#r0 -> @celda
		#r1 -> numero fila
		#r2 -> numero columna
		#r3 -> valor

		STMFD   sp!, {r4-r9, LR}

		#Creamos mascara para modificar bit
		MOV		r6, #1
		MOV		r5, #0				//Empleamos el 0 para el LSL
		ADD		r3, r3, #3			//Sumamos (4-1) para evitar los 4 bits de valor
		ADD		r6, r5, r6, LSL r3	//Colocamos bit a modificar en ((valor - 1) + 4)
		MVN		r4, r6				//Mascara
		SUB		r3, r3, #3			//Recuperamos valor real

		MOV		r5, #0
		#Colocamos registro al principio de la columna
		MOV		r7, r0
		ADD		r7, r7, r2, LSL #1		//Direccion actual iteracion

		#Calcular direccion elemento propagado
		ADD		r0, r0, r1, LSL #5		//Nos colocamos en la j
		ADD		r0, r0, r2, LSL #1		//Nos colocamos en la i

		loopcol:
		LDRH	r6, [r7]
		CMP		r0, r7
		BEQ		continuar2
				# Comprobar si hay coincidencia
		AND		r9, r6, #0xf		//r9 contiene valor celda actual
		CMP		r9, r3
		BNE		noerror2
		#pone error en celda origen
		LDRH	r9, [r0]
		ORR		r9, r9, #0x4000			//Si coincide -> metemos error siempre
		STRH	r9, [r0]
		ORR		r6, r6, #0x4000		//pone error en celda actual
		B		continuar2

noerror2:
		LDRH	r9, [r0]
		#AND		r9, r9, #0xBFFF			//Si coincide -> metemos error siempre
		STRH	r9, [r0]
		#AND		r6, r6, #0xBFFF		//pone error en celda actual

continuar2:
		AND		r6, r6, r4
		STRH	r6, [r7], #32
		ADD		r5, r5, #1
		//Comprobamos iteracion
		CMP		r5, #9
		BLT		loopcol

		LDMFD SP!, { r4-r9, PC }

checkreg:
		#Obtener registro que hemos apilado
		#r0 -> @celda
		#r1 -> numero fila
		#r2 -> numero columna
		#r3 -> valor
		STMFD   sp!, {r4-r11, LR}

		#Creamos mascara para modificar bit
		MOV		r6, #1
		MOV		r5, #0				//Empleamos el 0 para el LSL
		ADD		r3, r3, #3			//Sumamos (4-1) para evitar los 4 bits de valor
		ADD		r6, r5, r6, LSL r3	//Colocamos bit a modificar en ((valor - 1) + 4)
		MVN		r4, r6				//Mascara
		SUB		r3, r3, #3			//Recuperamos valor real

		#Evitamos convertir entero en long y recuperarlo mas tarde
		#como se hace en C
		#Obtenemos posicion inicio region fila
		CMP 	r1, #0
		MOVHS 	r5, #0
		CMP 	r1, #3
		MOVHS 	r5, #3
		CMP 	r1, #6
		MOVHS 	r5, #6
		# Obtenemos posicion inicio region columna
		CMP 	r2, #0
		MOVHS 	r6, #0
		CMP 	r2, #3
		MOVHS 	r6, #3
		CMP 	r2, #6
		MOVHS 	r6, #6

		#r1 -> posicion inicio region fila
		#r2 -> posicion inicio region columna
		MOV		r7, r0
		ADD		r7, r7, r6, LSL #1		//Movemos posicion inicio fila region
		ADD		r7, r7, r5, LSL #5		//Movemos posicion inicio columna region

		MOV		r10, #0		//Contador hasta tres por fila
		MOV		r11, #0
		loopreg:
		MOV		r6, r7		//Copiamos direccion inicial fila

		iterfilareg:
		LDRH	r5, [r6]

		#Si no son iguales las direcciones
		ADD		r8, r0, r1, LSL #5		//Nos colocamos en la j
		ADD		r8, r8, r2, LSL #1		//Nos colocamos en la i
		CMP		r6, r8
		BEQ		continuar3

		# Comprobar si hay coincidencia
		AND		r9, r5, #0xf		//r9 contiene valor celda actual
		CMP		r9, r3
		BNE		noerror3
		#pone error en celda origen
		LDRH	r9, [r8]
		ORR		r9, r9, #0x4000			//Si coincide -> metemos error siempre
		STRH	r9, [r8]
		ORR		r5, r5, #0x4000		//pone error en celda actual
		B		continuar3

noerror3:
		LDRH	r9, [r8]
		#AND		r9, r9, #0xBFFF			//Si coincide -> metemos error siempre
		STRH	r9, [r8]
		#AND		r5, r5, #0xBFFF		//pone error en celda actual

continuar3:
		AND		r5, r5, r4
		STRH	r5, [r6], #2
		ADD		r10, r10, #1
		CMP		r10, #3
		BLT		iterfilareg
		ADD		r11, r11, #1
		CMP		r11, #3
		MOVLT	r10, #0		//Reiniciamos contador
		ADDLT	r7, r7, #32
		BLT		loopreg

		LDMFD SP!, { r4-r11, PC }

#        END
