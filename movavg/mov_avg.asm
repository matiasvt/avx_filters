global mov_avg_avx

section		.text

mov_avg_avx:

;rdi:short*
;rsi:double*
;rdx:filasRestantes*bin/binesProcesados
;rcx:bines
;r8: double w

		push rbp
	 	mov rbp, rsp
	 	push rbx
	 	push r12
	 	push r13
	 	push r14
	 	push r15

mov r9,r8

movsd xmm7,[r9]
CVTTSD2SI r9d,xmm7
imul rcx,2;bines*sizeof(short)
imul r9,rcx
mov rax,rdi
add rax,r9

mov rbx,rsi
imul rcx,4;bins*sizeof(double)
sub rbx,rcx

;Están los 4 punteros, matrix inicio,matriz actual, vector actual,vector inicio. Todos avanzan hasta el final-

movsd xmm7,[r8]
vbroadcastsd ymm15,xmm7;ymm15 tiene lo que va a dividir

.loop:

	vpmovsxwd  ymm0,[rdi];ymm0=old
	vpmovsxwd  ymm1,[rax];ymm1=new
	add rax,16
	add rdi,16

	vpsrad ymm0,2
	vpsrad ymm1,2

	VPSUBD  ymm0,ymm1,ymm0;ymm0=ymm1-ymm0;Hay 8 ints para convertir a dbl

	vcvtdq2pd ymm3,xmm0;convertido baja
	vextracti128 xmm1,ymm0,1b
	vcvtdq2pd ymm1,xmm1;convertido alta
	vdivpd ymm3,ymm3,ymm15
	vdivpd ymm1,ymm1,ymm15,;ymm1/ymm15
	vaddpd ymm3,ymm3,[rbx]
	add rbx,32	
	vaddpd ymm1,ymm1,[rbx]
	add rbx,32

	vmovapd [rsi],ymm3
	add rsi,32
	vmovapd [rsi],ymm1
	add rsi,32

	sub rdx,1
	jnz .loop
.exit:
	xor rax,rax;

		pop r15
		pop r14
		pop r13
		pop r12
		pop rbx
		pop rbp
		ret