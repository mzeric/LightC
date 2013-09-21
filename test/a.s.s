	.section	__TEXT,__text,regular,pure_instructions
	.globl	_main
	.align	4, 0x90
_main:                                  ## @main
	.cfi_startproc
## BB#0:
	movl	$0, -4(%rsp)
	movl	$3, -12(%rsp)
	movl	-12(%rsp), %eax
	incl	%eax
	ret
	.cfi_endproc


.subsections_via_symbols
