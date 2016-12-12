#------------------------------------------------------------------------------
#- entry function:	void	user_irq(void)
#- exit function:	void	user_handle(void)
#------------------------------------------------------------------------------
#		.INCLUDE	"include.x"

          .macro IRQHandle user_handle:
            stmdb	sp!, {r0-r11, ip, lr}	/* save r0-r11, ip, lr */
            ldr		r0, =\user_handle
            mov		lr, pc
            bx		r0						/* invoke user interrupt handler */
            ldmia	sp!, {r0-r11, ip, lr}	/* resume r0, ip, lr */
			subs	pc, r14, #4				/* return from interruption */
          .endm
          
       .extern TSInt
       .global user_irq1
user_irq1: IRQHandle TSInt

