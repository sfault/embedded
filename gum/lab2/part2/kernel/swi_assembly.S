	.file "swi_assembly.S"
	.extern C_SWI_handler
	.text
    .globl swi_assembly
    .globl user_switch
    .globl exit_handler
    @.type swi_assembly, @function

swi_assembly:				@ Reentrant SWI handler from lecture
        SUB sp, sp, #4			@ leave room on stack for SPSR
	STMFD sp!, {r0-r12, lr}		@ store user's gp registers
        MRS r2, spsr			@ get SPSR into gp registers
        STR r2, [sp, #14*4]		@ store SPSR above gp registers
	MOV r1, sp			
	LDR r0, [lr, #-4]
	BIC r0, r0, #0xff000000
	BL C_SWI_handler
        LDR r2, [sp, #14*4]
        MSR spsr, r2
	ADD sp, sp, #4
	LDMFD sp!, {r1-r12, lr}
        ADD sp, sp, #4
	MOVS pc,lr

user_switch:
    MRS r2, cpsr
    MVN r3, #0
    MOV r3, r3, LSL #8
    AND r2, r2, #0x10
    ORR r2, r2, #0xC0
    MSR cpsr, r2
    MOV lr, r4
    LDR sp, =0xa3000000
    STMFD sp!, {r0, r1}
    LDR pc, =0xa2000000

exit_handler:
    LDR r1, =stack_p
    LDR sp, [r1]
    LDR r1, =link_r
    LDR pc, [r1]
    
