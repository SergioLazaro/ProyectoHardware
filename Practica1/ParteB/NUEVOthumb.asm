.text
/*
 * Autores:
 * Oscar Leon Barbed Perez 666137
 * Sergio Lazaro Magdalena 556030
 *
 */


.global sudoku_candidatos_propagar_thumb


.thumb
sudoku_candidatos_propagar_thumb:
		#Obtener registro que hemos apilado
		#r0 -> @celda_inicial
		#r1 -> numero fila
		#r2 -> numero columna

		MOV 	IP, SP
		PUSH 	{r4-r7,FP,IP,LR,PC}
		SUB 	FP, IP, #4

		MOV		r7, r14			//Movemos LR a r7

		MOV		r4, r0			//r4 -> posicion celda inicial

		#Obtenemos celda de interes
		LSL		r3, r1, #5		//Posicionamos eje Y
		ADD		r4, r4, r3
		LSL		r3, r2, #1		//Posicionamos eje X
		ADD		r4,	r4, r3		//r4 -> contiene la posicion de interes

		LDRH	r3, [r4]		//r3 -> elemento

		# Comprobamos si pista
		LSR		r6, r3, #15		//r4 -> 1 (pista) o 0 (vacio)

 		CMP		r6, #0
		BEQ		fin_thumb

		MOV		r4, r0
		MOV		r5, r1
		MOV		r6, r2
		MOV		r7, r3
		BL		checkfila_thumb

		MOV		r0, r4
		MOV		r1, r5
		MOV		r2, r6
		MOV		r3, r7
		BL		checkcol_thumb

		MOV		r0, r4
		MOV		r1, r5
		MOV		r2, r6
		MOV		r3, r7
		BL		checkreg_thumb

		MOV		r0, #0
		POP		{r4-r7,FP,SP,PC}
		BX		r7

fin_thumb:
		MOV		r0, #1
		POP		{r4-r7,FP,SP,PC}
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
