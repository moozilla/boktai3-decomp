@ Top-level ROM image.
@ gen/code_sym.s and gen/data.s are generated from the base ROM
@ (tools/disasm.py, tools/symbolize.py): every ROM pointer is a symbol, so code
@ and data can move.  Set SHIFT to insert padding between code and data as a
@ shiftability test.
	.include "asm/macros.inc"
	.syntax unified
	.text
	.include "gen/code_sym.s"
	.ifdef SHIFT
	.space SHIFT
	.endif
	.include "gen/data.s"
