.text

#        ENTRY                  /*  mark the first instruction to call */
.global start

start:
.arm    /* indicates that we are using the ARM instruction set */

#------standard initial code
# --- Setup interrupt / exception vectors
      B       Reset_Handler
/* In this version we do not use the following handlers */
################################################################################
#-----------Undefined_Handler:
#      B       Undefined_Handler
#----------SWI_Handler:
#      B       SWI_Handler
#----------Prefetch_Handler:
#      B       Prefetch_Handler
#----------Abort_Handler:
#      B       Abort_Handler
#         NOP      /* Reserved vector */
#----------IRQ_Handler:
#      B       IRQ_Handler
#----------FIQ_Handler:
#      B       FIQ_Handler

################################################################################
# Reset Handler:
# the processor starts executing this code after system reset
################################################################################
Reset_Handler:
#
        MOV     sp, #0x4000      /*  set up stack pointer (r13) */
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

.global sudoku_candidatos_init_arm
.global sudoku_candidatos_propagar_arm
.global sudoku_candidatos_propagar_thumb
.global puente_arm_thumb


##############################################################################
		#B		arm			//Prueba ARM
		B		c			//Prueba C
c:
        LDR     r0, =cuadricula  /*  puntero a la @ inicial de la cuadricula */

.extern     sudoku9x9
        ldr         r5, =sudoku9x9
        mov         lr, pc
        bx          r5
        B			c
arm:
        MOV			r1, #250
        ADD			r1, r1, #250
        ADD			r1, r1, #250
        ADD			r1, r1, #250

arm_1:

        LDR		r0, =cuadricula	/* Volvemos a obtener direccion cuadricula */
		BL		sudoku_candidatos_init_arm

		SUB 		r1, r1 ,#1
		CMP			r1, #0
		BNE			arm_1
		B			arm
stop:
        B       stop        /*  end of program */


#	USING THUMB CODE
################################################################################
.arm		//funcion puente

puente_arm_thumb:
		#Obtener registro que hemos apilado
		#r0 -> @celda_inicial
		#r1 -> numero fila
		#r2 -> numero columna
		PUSH	{r6, LR}

		ADR		r6, sudoku_candidatos_propagar_thumb
		ADD		r6, r6, #1
		ADR		r14, return_puente
		BX		r6

return_puente:
		MOV		r0, r0
		POP 	{r6, PC}


.thumb
sudoku_candidatos_propagar_thumb:
		#Obtener registro que hemos apilado
		#r0 -> @celda_inicial
		#r1 -> numero fila
		#r2 -> numero columna

		PUSH	{r0-r6}
		MOV		r7, r14			//Movemos LR a r7

		MOV		r4, r0			//r4 -> posicion celda inicial

		#Obtenemos celda de interes
		LSL		r3, r1, #5		//Posicionamos eje Y
		ADD		r4, r4, r3
		LSL		r3, r2, #1		//Posicionamos eje X
		ADD		r4,r4, r3		//r4 -> contiene la posicion de interes

		LDRH	r3, [r4]		//r3 -> elemento

		# Comprobamos si pista
		LSR		r6, r3, #15		//r4 -> 1 (pista) o 0 (vacio)

 		CMP		r6, #0
		BEQ		fin_thumb

		BL		checkfila_thumb
		BL		checkcol_thumb
		BL		checkreg_thumb

		MOV		r0, #1
		POP		{r0-r6}
		BX		r7

fin_thumb:
		MOV		r0, #0
		POP		{r0-r6}
		BX		r7

checkfila_thumb:
		#Obtener registro que hemos apilado
		#r0 -> @celda_inicia
		#r1 -> numero fila
		#r2 -> numero columna
		#r3 -> valor

		PUSH	{r0-r6, LR}

		#Obtenemos mascara para bajar bit
		ADD		r5, r3, #3			//Sumamos (4-1) para evitar los 4 bits de valor
		MOV		r6, #0xf
		AND		r5, r6				//r5 contiene el bit a modificar
		MOV		r6, #1
		LSL		r6, r5			//Obtenemos mascara metiendo ceros a la derecha N veces segun valor
		MVN		r4, r6			//r4 -> mascara

		LSL		r2, r1, #5
		ADD		r2, r0, r2			//Obtenemos posicion inicial de la fila
		MOV		r3, #0				//Contador iteraciones

		loopfila_thumb:
		LDRH	r5, [r2]			//r4 contiene valor de celda
		AND		r5, r4
		STRH	r5, [r2]
		ADD		r2, r2, #2			//Post indexado
		ADD		r3, r3, #1

		//Comprobamos iteracion
		CMP		r3, #9
		BLT		loopfila_thumb

		POP		{r0-r6, PC}


checkcol_thumb:
		#Obtener registro que hemos apilado
		#r0 -> @celda
		#r1 -> numero fila
		#r2 -> numero columna
		#r3 -> valor

		PUSH	{r0-r6, LR}

		#Obtenemos mascara para bajar bit
		ADD		r5, r3, #3			//Sumamos (4-1) para evitar los 4 bits de valor
		MOV		r6, #0xf
		AND		r5, r6				//r5 contiene el bit a modificar
		MOV		r6, #1
		LSL		r6, r5			//Obtenemos mascara metiendo ceros a la derecha N veces segun valor
		MVN		r4, r6			//r4 -> mascara

		LSL		r1, r2, #1
		ADD		r2, r0, r1			//Obtenemos posicion inicial de la fila
		MOV		r3, #0				//Contador iteraciones

		LDRH	r5, [r2]			//r4 contiene valor de celda

		loopcol_thumb:

		LDRH	r5, [r2]
		AND		r5, r4
		STRH	r5, [r2]
		ADD		r2, #32
		ADD		r3, r3, #1

		//Comprobamos iteracion
		CMP		r3, #9
		BLT		loopcol_thumb


		POP		{r0-r6, PC}

checkreg_thumb:
		#Obtener registro que hemos apilado
		#r0 -> @celda_inicial
		#r1 -> posicion fila
		#r2 -> posicion columna
		#r3 -> valor

		PUSH	{r0-r6, LR}

		#Obtenemos mascara para bajar bit
		ADD		r5, r3, #3			//Sumamos (4-1) para evitar los 4 bits de valor
		MOV		r6, #0xf
		AND		r5, r6				//r5 contiene el bit a modificar
		MOV		r6, #1
		LSL		r6, r5			//Obtenemos mascara metiendo ceros a la derecha N veces segun valor
		MVN		r4, r6			//r4 -> mascara

		#Evitamos convertir entero en long y recuperarlo mas tarde
		#como se hace en C
		#Obtenemos posicion inicio region fila
		MOV 	r5, #0
		CMP 	r1, #3
		BLT		eje2
		MOV		r5, #3
		CMP 	r1, #6
		BLT		eje2
		MOV 	r5, #6
		# Obtenemos posicion inicio region columna
eje2:
		MOV 	r6, #0
		CMP 	r2, #3
		BLT		contreg
		MOV		r6, #3
		CMP 	r2, #6
		BLT		contreg
		MOV 	r6, #6

contreg:

		#Posicionamos en eje Y
		LSL		r2, r5, #5
		ADD		r2, r0, r2			//Obtenemos posicion inicial de la fila

		#Posicionamos en eje X
		LSL		r1, r6, #1
		ADD		r2, r2, r1			//Obtenemos posicion inicial de la fila
		MOV		r0, #0				//contador hasta 3 en horizontal
		MOV		r1, #0				//contador hasta 3 en vertical
		MOV		r5, r2				//Posicion auxiliar para modificar, mantenemos r2
loop_reg_thumb:

		LDRH	r6, [r5]
		AND		r6, r4			//Modificamos con la mascara
		STRH	r6, [r5]
		ADD		r5, r5, #2

		ADD		r0, r0, #1			//Continuamos en fila
		CMP		r0, #3
		BLT		loop_reg_thumb

		ADD		r1, r1, #1
		CMP		r1, #3				//comprobamos si hemos acabado la region
		BHS		end_reg
		MOV		r0, #0				//Avanzamos de fia
		ADD		r2, #32
		MOV		r5, r2				//actualizamos registro r2 persistente para apuntar siguiente columna
		B		loop_reg_thumb
end_reg:
		POP		{r0-r6, PC}


################################################################################
.arm

#	USING ARM CODE
#
# Comprobar estado del sudoku tras llamada a funcion C
# En caso de estar establecido todo deberiamos limpiar
# los candidatos.

sudoku_candidatos_init_arm:

		STMFD   sp!, {r1-r7, LR}

		MOV		r4, r0
		# @cuadricula en r0
		MOV		r1, #0			//IterFila
		MOV		r2, #0			//IterColumna
		MOV		r6, #0			//Celdas_vacias
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

		MOV		r1, #0			//IterHorizontal = 0
		MOV		r2, #0			//IterVertical = 0

rows:
		LDR		r0, =cuadricula //Necesario ya que r0 contiene el return del metodo propagar
		/*//LLAMADA C
		MOV		r4, r1
		MOV		r5, r2
.extern     sudoku_candidatos_propagar_c
        ldr         r6, =sudoku_candidatos_propagar_c
        mov         lr, pc
        bx          r6
        MOV		r1, r4
        MOV		r2, r5
		*/
			//LLAMADA ARM
		BL		sudoku_candidatos_propagar_arm


		/*//llamada thumb
		ADR		r6, sudoku_candidatos_propagar_thumb
		ADD		r6, r6, #1
		ADR		r14, return_thumb
		BX		r6
		*/

return_thumb:
		ADD		r6, r6, r0		//Sumamos valor de celdas_vacias
		ADD		r2, r2, #1
		CMP		r2, #9
		BNE		rows			//Iteracion siguiente celda en fila
		#Check iteracion columna
		ADD		r1, r1, #1
		CMP		r1, #9
		MOVLO	r2, #0
		BLO		rows			//Iteracion nueva fila

		MOV		r0, r6
		LDMFD SP!, { r1-r7, PC }

sudoku_candidatos_propagar_arm:
		#Obtener registro que hemos apilado
		#r0 -> @celda
		#r1 -> posicion fila
		#r2 -> posicion columna

		STMFD   sp!, {r4-r7, LR}

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
		MOV		r5, #0
		ADD		r4, r5, r4, LSR #15	//r4 contiene el bit pista

		CMP		r4, #0			//Comprobamos que es una pista
		BEQ		fin_propagar_1

		#comprobar fila
		BL	checkfila
		#comprobar columna
		BL	checkcolum
		#comprobar region
		BL	checkreg

		# Return 0 celdas_vacias
		MOV		r0, #0
		LDMFD 	SP!, { r4-r7, PC }

		# Return 1 celdas_vacias
fin_propagar_1:
		MOV		r0, #1
		LDMFD 	SP!, { r4-r7, PC }


################################################################################
# BLOQUES DE ACTIVACION PARA OPTIMIZAR CODIGO ARM Y THUMB (LIMITACION REGISTROS)
checkfila:
		#Obtener registro que hemos apilado
		#r0 -> @celda
		#r1 -> numero fila
		#r2 -> numero columna
		#r3 -> valor
		#r4 -> mascara para AND

		STMFD   sp!, {r0-r7, LR}

		#Creamos mascara para modificar bit
		MOV		r6, #1
		MOV		r5, #0				//Empleamos el 0 para el LSL
		ADD		r3, r3, #3			//Sumamos (4-1) para evitar los 4 bits de valor
		ADD		r6, r5, r6, LSL r3	//Colocamos bit a modificar en ((valor - 1) + 4)
		MVN		r4, r6

		MOV		r5, #0
		#Colocamos registro al principio de la fila
		MOV		r7, r0
		ADD		r7, r7, r1, LSL #5

		loopfila:
		LDRH	r6, [r7]

		AND		r6, r6, r4			//Guardamos la modificacion de la celda
		STRH	r6, [r7], #2
		ADD		r5, r5, #1
		//Comprobamos iteracion
		CMP		r5, #9
		BLT		loopfila

		LDMFD SP!, { r0-r7, PC }

checkcolum:
		#Obtener registro que hemos apilado
		#r0 -> @celda
		#r1 -> numero fila
		#r2 -> numero columna
		#r3 -> valor
		#r4 -> mascara para AND

		STMFD   sp!, {r0-r7, LR}

		#Creamos mascara para modificar bit
		MOV		r6, #1
		MOV		r5, #0				//Empleamos el 0 para el LSL
		ADD		r3, r3, #3			//Sumamos (4-1) para evitar los 4 bits de valor
		ADD		r6, r5, r6, LSL r3	//Colocamos bit a modificar en ((valor - 1) + 4)
		MVN		r4, r6

		MOV		r5, #0
		#Colocamos registro al principio de la columna
		MOV		r7, r0
		ADD		r7, r7, r2, LSL #1

		loopcol:
		LDRH	r6, [r7]
		AND		r6, r6, r4
		STRH	r6, [r7], #32
		ADD		r5, r5, #1
		//Comprobamos iteracion
		CMP		r5, #9
		BLT		loopcol

		LDMFD SP!, { r0-r7, PC }

checkreg:
		#Obtener registro que hemos apilado
		#r0 -> @celda
		#r1 -> numero fila
		#r2 -> numero columna
		#r3 -> valor
		#r4 -> mascara para AND
		STMFD   sp!, {r0-r7, LR}

		#Creamos mascara para modificar bit
		MOV		r6, #1
		MOV		r5, #0				//Empleamos el 0 para el LSL
		ADD		r3, r3, #3			//Sumamos (4-1) para evitar los 4 bits de valor
		ADD		r6, r5, r6, LSL r3	//Colocamos bit a modificar en ((valor - 1) + 4)
		MVN		r4, r6

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

		MOV		r2, #0		//Contador hasta tres por fila
		MOV		r3, #0
		loopreg:
		MOV		r1, r7		//Copiamos direccion inicial fila

		iterfilareg:
		LDRH	r5, [r1]
		AND		r5, r5, r4
		STRH	r5, [r1], #2
		ADD		r2, r2, #1
		CMP		r2, #3
		BLT		iterfilareg
		ADD		r3, r3, #1
		CMP		r3, #3
		MOVLT	r2, #0		//Reiniciamos contador
		ADDLT	r7, r7, #32
		BLT		loopreg

		LDMFD SP!, { r0-r7, PC }
################################################################################
.data
.ltorg     
.align 5    /* guarantees 32-byte alignment (2^5) */

# huecos para cuadrar
cuadricula:
     /* 9 filas de 16 entradas para facilitar la visualizacion, 16 bits por celda */
    .hword   0x8005,0x0000,0x0000,0x8003,0x0000,0x0000,0x0000,0x0000,0x0000,0,0,0,0,0,0,0
    .hword   0x0000,0x0000,0x0000,0x0000,0x8009,0x0000,0x0000,0x0000,0x8005,0,0,0,0,0,0,0
    .hword   0x0000,0x8009,0x8006,0x8007,0x0000,0x8005,0x0000,0x8003,0x0000,0,0,0,0,0,0,0
    .hword   0x0000,0x8008,0x0000,0x8009,0x0000,0x0000,0x8006,0x0000,0x0000,0,0,0,0,0,0,0
    .hword   0x0000,0x0000,0x8005,0x8008,0x8006,0x8001,0x8004,0x0000,0x0000,0,0,0,0,0,0,0
    .hword   0x0000,0x0000,0x8004,0x8002,0x0000,0x8003,0x0000,0x8007,0x0000,0,0,0,0,0,0,0
    .hword   0x0000,0x8007,0x0000,0x8005,0x0000,0x8009,0x8002,0x8006,0x0000,0,0,0,0,0,0,0
    .hword   0x8006,0x0000,0x0000,0x0000,0x8008,0x0000,0x0000,0x0000,0x0000,0,0,0,0,0,0,0
    .hword   0x0000,0x0000,0x0000,0x0000,0x0000,0x8002,0x0000,0x0000,0x8001,0,0,0,0,0,0,0

.end
#        END
