
    .extern main
    .equ STACK, 0x0c7ff000
.text
.equ    num, 10  /* this is used to indicate the number of words to copy */
#        ENTRY                    /* mark the first instruction to call */
.arm /* indicates that we are using the ARM instruction set */
#------standard initial code
# --- Setup interrupt / exception vectors
.global    start
start:
       B     Reset_Handler
/* In this version we do not use the following handlers */
################################################################################
#-----------Undefined_Handler:
#      B        Undefined_Handler
#----------SWI_Handler:
#      B        SWI_Handler
#----------Prefetch_Handler:
#      B        Prefetch_Handler
#----------Abort_Handler:
#      B        Abort_Handler
#      NOP      /* Reserved vector */
#----------IRQ_Handler:
#      B        IRQ_Handler
#----------FIQ_Handler:
#      B        FIQ_Handler

################################################################################
# Reset Handler:
# the processor starts executing this code after system reset
################################################################################
Reset_Handler:
#
        LDR     sp,=STACK       /*  set up stack pointer (r13) */
# colocamos los parámetros para la llamada a la primera función
        LDR     r0, =src        /*  r0 = pointer to source block */
# guardamos r0 en r4 porque al llamar a las funciones podemos perder su valor
# así lo podremos restaurar sin acceder a memoria
        MOV     r4, r0
        LDR     r1, =dst1       /*  r1 = pointer to destination block 1 */

#
# PART 1: USING ARM CODE
#
        BL      ARM_copy_10     /* FUNCTION CALL */
#
# PART 2: USING THUMB CODE
#
# colocamos los parámetros para la llamada a la segunda función
# porque ARM_copy_10 los ha podido eliminar
# IMPORTANTE: si queremos tener un código modular,
# debemos volver a apilar los parámetros a pesar de que en teoría
# alguno pueda estar ya en la pila
        MOV     r0, r4           /* r0 = pointer to source block */
        LDR     r1, =dst2        /* r1 = pointer to destination block 1 */
        ADR     r3, th_copy_10+1 /* the last address bit is not really used to specify the address but to select between ARM and Thumb code */
        ADR     r14,return       /* we store the return address in r14 */
        BX      r3               /* FUNCTION CALL, we jump to th_mul. +1 indicates that we want to switch to thumb */
return:

#
# PART 3: USING A .C FUNCTION
#
# ponemos los parámetros de nuevo
        MOV     r0, r4           /* r0 = pointer to source block */
        LDR     r1, =dst3        /* r1 = pointer to destination block 1 */
# function __c_copy is in copy.c
.extern     __c_copy_10
        LDR     r3, = __c_copy_10
        MOV     lr, pc

# FUNCTION CALL the parameters are stored in r0 and r1
# If there are 4 or less parameters when calling a C function
# the compiler assumes that they have been stored in r0-r3.
# If there are more parameters you have to store them
# in the data stack using the stack pointer
        BX      r3
stop:
        B       stop        /*  end of program */

################################################################################
# Función ARM:
# copia 10 palabras de la dirección indicada en r0 a la indicada por r1
# debería crear un marco de pila con la misma estructura que en el resto de llamadas a funciones
# pero como es un nodo hoja (no llama a ninguna función) vamos a hacer un marco simplificado:
# sólo guardamos los registros que utiliza y que no tiene permiso para alterar
################################################################################
ARM_copy_10:
        #  saves the working registers
        # Recordad que puede modificar r0, r1, r2 y r3 sin guardarlos previamente
        STMFD   sp!, {r4-r11}

        # Poned el código aquí: sólo hacen falta dos instrucciones
		LDM		r0, {r2-r11}	/*Load todos los numeros de src en adelante.*/
								/*Se guardan de r2-r11*/
		STM		r1, {r2-r11}	/*Store de los registro r2-r11 a partir de*/
								/*la direccion dst1.*/

		# A diferencia del codigo THUMB, no necesitamos {!} ya que el load multiple es completo de todos los elementos.
		# No hace falta hacer otro load de r0.

        # restore the original registers
        LDMFD   sp!, {r4-r11}
        # return to the instruccion that called the rutine and to arm mode
        BX      r14

################################################################################
# Función thumb:
# copia 10 palabras de la dirección indicada en r0 a la indicada por r1
# De nuevo, al ser un nodo hoja hacemos un marco simplificado:
# sólo los registros que toca y debe restaurar
################################################################################
/* indicates that we are using the thumb instruction set */
.thumb
th_copy_10:
        PUSH    {r4-r7}

        # poned aquí el código,
        # como no podemos leer y escribir 10 palabras de golpe lo haremos en dos veces

		# Metemos {!} para actualizar r0 en el load de cada elemento contenido a partir de src.
		MOV		r2, r0		/*Instruccion de control para comprobar la actualizacion de src (r0)*/
		# Load de los 5 primeros numeros
		LDM		r0!, {r3-r7}
		# Store de los 5 primeros numeros
		STM		r1!, {r3-r7}
		# Load de los ultimos 5 numeros
		LDM		r0!, {r3-r7}
		# Store de los ultimos 5 numeros
		STM		r1!, {r3-r7}

        POP     {r4-r7} /* restores the registers */
        BX      r14     /* this is the return instrucction */

################################################################################
#        AREA BlockData, DATA, READWRITE
#    src:  source block (10 words)
#    dst1: destination block 1 (10 words)
#    dst2: destination block 2 (10 words)
#    dst3: destination block 3 (10 words)
################################################################################
.data
.ltorg /* guarantees the alignment */
src:
     .long     1,2,3,4,5,6,7,8,9,10
dst1:
     .long     0,0,0,0,0,0,0,0,0,0
dst2:
     .long     0,0,0,0,0,0,0,0,0,0
dst3:
     .long     0,0,0,0,0,0,0,0,0,0
.end
#        END


