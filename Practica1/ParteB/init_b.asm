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

		# @cuadricula en r0
		MOV		r1, #0			//Fila
		MOV		r2, #0			//Columna
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
		#Load celda actual
		LDRH	r5, [r4]		//Celda actual
		#Comprobamos bit 15 del elemento para saber si es Pista
		MOV		r6,r5,LSR #15
		CMP		r6, #1
		ADDEQ	r4, r4, #2
		BEQ		rowloop			//Elemento es pista

		#Elemento vacio
		STRH	r3, [r4], #2
		CMP		r1,#9
		BNE		rowloop
		MOV		r1, #0
		ADD		r4, r4, #14	//Actualizamos direccion elemento teniendo en cuenta el padding
		CMP		r2,#9		//Comparamos que hemos recorrido todo el sudoku
		BEQ		end
		B		columnloop	//Fin de fila


end:	B		end


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
