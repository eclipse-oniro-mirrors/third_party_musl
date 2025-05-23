.syntax unified
.global __cp_begin
.hidden __cp_begin
.global __cp_end
.hidden __cp_end
.global __cp_cancel
.hidden __cp_cancel
.hidden __cancel
.global __syscall_cp_asm
.ifdef __LITEOS__
.hidden __syscall_cp_asm
.endif
.type __syscall_cp_asm,%function
.ifndef __LITEOS__
.cfi_startproc
.endif
__syscall_cp_asm:
	mov ip,sp
	stmfd sp!,{r4,r5,r6,r7}
	.ifndef __LITEOS__
	.cfi_def_cfa_offset 16
	.cfi_rel_offset r4, 0
	.cfi_rel_offset r5, 4
	.cfi_rel_offset r6, 8
	.cfi_rel_offset r7, 12
	.endif
__cp_begin:
	ldr r0,[r0]
	cmp r0,#0
	bne __cp_cancel
	mov r7,r1
	mov r0,r2
	mov r1,r3
	ldmfd ip,{r2,r3,r4,r5,r6}
	svc 0
__cp_end:
	ldmfd sp!,{r4,r5,r6,r7}
	.ifndef __LITEOS__
	.cfi_def_cfa_offset 0
	.endif
	bx lr
__cp_cancel:
	ldmfd sp!,{r4,r5,r6,r7}
	.ifndef __LITEOS__
	.cfi_def_cfa_offset 0
	.endif
	b __cancel
.ifndef __LITEOS__
.cfi_endproc
.endif
