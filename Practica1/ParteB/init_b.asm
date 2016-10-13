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
        LDR     r0, =cuadricula  /*  puntero a la @ inicial de la cuadricula */
		B		ARM_sudoku_2016
.extern     sudoku9x9
        ldr         r5, =sudoku9x9
        mov         lr, pc
        bx          r5

stop:
        B       stop        /*  end of program */

#	USING ARM CODE
#
# Comprobar estado del sudoku tras llamada a funcion C
# En caso de estar establecido todo deberiamos limpiar
# los candidatos.

		# Preparar bloque de activacion

		BL      ARM_sudoku_2016     /* FUNCTION CALL */


#	USING THUMB CODE

# Funcion ARM

ARM_sudoku_2016:

		#  saves the working registers
        # Recordad que puede modificar r0, r1, r2 y r3 sin guardarlos previamente
		STMFD   sp!, {r4-r11}

		#Inicializamos sudoku
		BL sudoku_candidatos_init_arm

end:	B		end


sudoku_candidatos_init_arm:

		STMFD   sp!, {r0-r6, LR}

		# @cuadricula en r0
		MOV		r1, #0			//IterFila
		MOV		r2, #0			//IterColumna
		#Cosicas para hacer mov con mas de 1 byte
		MOV		r3, #0x1f
		MOV		r3, r3, LSL #8
		ADD		r3, r3,#0xf0

		MOV		r4, r0			//Direccion elemento a modificar
		#Inicio bucle
		columnloop:
		ADD		r2, r2, #1

		rowloop:

		ADD		r1, r1, #1

		#Comprobamos bit 15 del elemento para saber si es Pista
		LDRH	r5, [r4]		//Celda actual
		MOV		r6,r5,LSR #15
		CMP		r6, #1

		#Celda es pista
		ADDEQ	r4, r4, #2		//Actualizamos direccion de memoria al siguiente elemento

		#Elemento vacio
		STRNEH	r3, [r4], #2

		#Comprobamos si es ultimo elemento (fin bucle fila)
		CMP		r1,#9
		BNE		rowloop
		MOV		r1, #0
		ADD		r4, r4, #14	//Actualizamos direccion elemento teniendo en cuenta el padding
		CMP		r2,#9		//Comparamos que hemos recorrido todo el sudoku
		BLO		columnloop	//Fin de fila

		BL 		sudoku_candidatos_propagar_todos_arm

		LDMFD SP!, { r0-r6, PC }

sudoku_candidatos_propagar_todos_arm:

		STMFD   sp!, {r0-r7, LR}

		# @cuadricula en r0
		MOV		r1, #0			//IterFila
		MOV		r2, #0			//IterColumna

		rows:

		LDRH	r4, [r0]		//Celda
		AND		r3, r4, #0xf	//AND para quedarnos con el valor de la celda

		CMP		r3, #0			//Comprobamos que es una pista
		BEQ		next

		#Creamos mascara para modificar bit
		MOV		r6, #1
		MOV		r5, #0				//Empleamos el 0 para el LSL
		ADD		r3, r3, #3			//Sumamos (4-1) para evitar los 4 bits de valor
		ADD		r6, r5, r6, LSL r3	//Colocamos bit a modificar en ((valor - 1) + 4)

		#Preparamos registro para modificar candidatos
		MOV		r7, #0xff
		ADD		r7, r5, r7, LSL #8
		ADD		r7 , r7, #0xff		//r7 = 0xffff <- aprovechamos para AND posterior
		SUB		r4, r7, r6

		#comprobar fila
		BL	checkfila
		#comprobar columna
		BL	checkcolum
		#comprobar region
		BL	checkreg

		#Check iteracion fila
next:	ADD		r0, r0, #2
		ADD		r1, r1, #1
		CMP		r1, #9
		BNE		rows			//Iteracion siguiente celda en fila
		#Check iteracion columna
		ADD		r2, r2, #1
		CMP		r2, #9
		ADDLO	r0, r0, #14		//Modificamos direccion inicio nueva fila
		MOVLO	r1, #0
		BLO		rows			//Iteracion nueva fila

		LDMFD SP!, { r0-r7, PC }


################################################################################
# BLOQUES DE ACTIVACION PARA OPTIMIZAR CODIGO ARM Y THUMB (LIMITACION REGISTROS)
checkfila:
		#Obtener registro que hemos apilado
		#r0 -> @celda
		#r1 -> posicion fila
		#r2 -> posicion columna
		#r3 -> valor
		#r4 -> mascara para AND

		STMFD   sp!, {r0-r6, LR}

		MOV		r5, #0
		#Colocamos registro al principio de la fila
		SUB		r0, r0, r1, LSL #1

		loopfila:
		LDRH	r6, [r0]

		AND		r6, r6, r4			//Guardamos la modificacion de la celda
		STRH	r6, [r0], #2
		ADD		r5, r5, #1
		//Comprobamos iteracion
		CMP		r5, #9
		BLT		loopfila

		LDMFD SP!, { r0-r6, PC }

checkcolum:
		#Obtener registro que hemos apilado
		#r0 -> @celda
		#r1 -> posicion fila
		#r2 -> posicion columna
		#r3 -> valor
		#r4 -> mascara para AND

		STMFD   sp!, {r0-r6, LR}

		MOV		r5, #0
		#Colocamos registro al principio de la columna
		SUB		r0, r0, r2, LSL #5

		loopcol:
		LDRH	r6, [r0]
		AND		r6, r6, r4
		STRH	r6, [r0], #32
		ADD		r5, r5, #1
		//Comprobamos iteracion
		CMP		r5, #9
		BLT		loopcol

		LDMFD SP!, { r0-r6, PC }

checkreg:
		#Obtener registro que hemos apilado
		#r0 -> @celda
		#r1 -> posicion fila
		#r2 -> posicion columna
		#r3 -> valor
		#r4 -> mascara para AND
		STMFD   sp!, {r0-r6, LR}

		#Evitamos convertir entero en long y recuperarlo mas tarde
		#como se hace en C
		#Obtenemos posicion inicio region fila
		CMP 	r1, #0
		MOVHS 	r5, #0
		CMP 	r1, #3
		MOVHS 	r5, #3
		CMP 	r1, #6
		MOVHS 	r5, #6
		# Movemos posicion inicio region fila para ahorrar registro
		SUB 	r1, r1, r5
		# Obtenemos posicion inicio region columna
		CMP 	r2, #0
		MOVHS 	r6, #0
		CMP 	r2, #3
		MOVHS 	r6, #3
		CMP 	r2, #6
		MOVHS 	r6, #6
		# Movemos posicion inicio region columna para ahorrar registro
		SUB 	r2, r2, r6

		#r1 -> posicion inicio region fila
		#r2 -> posicion inicio region columna

		SUB		r0, r0, r1, LSL #1		//Movemos posicion inicio fila region
		SUB		r0, r0, r2, LSL #5		//Movemos posicion inicio columna region

		MOV		r2, #0		//Contador hasta tres por fila
		MOV		r3, #0
		loopreg:
		MOV		r1, r0		//Copiamos direccion inicial fila

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
		ADDLT	r0, r0, #32
		BLT		loopreg

		LDMFD SP!, { r0-r6, PC }
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
