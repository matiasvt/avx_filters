global suma2_sse

section		.text

suma2_sse:
;asume entrada short, no se pasan de int en shotNumber sumas y mucho mas

;parametros
;rdi=&memoria-> rdi=short*,rdi+8=int*, rdi+16=long*, rdi+24=double*, rdi+32=double*
;shotNumber =rsi
;binesProcesados =rdx
;offset_bines( en bytes)=rcx PARAMETRO SIN USO
;contadorLoopBines =r8

	 	push rbp
	 	mov rbp, rsp
	 	push rbx
	 	push r12
	 	push r13
	 	push r14
	 	push r15

		VZEROUPPER ;para evitar penalidades de avx
		vpxor xmm0,xmm0
		vpxor xmm1,xmm1
		vpxor xmm2,xmm2
		vpxor xmm3,xmm3
		vpxor xmm4,xmm4
		vpxor xmm5,xmm5
		vpxor xmm6,xmm6
		vpxor xmm7,xmm7
		vpxor xmm8,xmm8
		vpxor xmm9,xmm9
		vpxor xmm10,xmm10;maybe useless

		mov rax,[rdi];short*
		mov rbx,[rdi+8];int*
		mov r10,[rdi+16];long*

		mov r14,rdx;era rcx
		imul r14,2;binesProcesados*sizeof(short) para usar en bytes y mover en datos

		imul rdx,4;binesProcesados*sizeof(int) para usar en bytes y mover en resultados; era rcx

	.setup:

        mov r9,r8
		mov r12,rsi ;r12=shots ;era rdx


	.loop:

		VPMOVSXWD xmm2,[rax]
		add rax,r14;muevo bin siguiente
		VPSRAD xmm2,2;shift 2 right por primeros 2 bits inutiles

		;suma
		vmovdqa xmm0,[rbx]
		VPADDD xmm0,xmm2 ;xmm0+=xmm2
        vmovdqa [rbx],xmm0
        add rbx,rdx
        
		;suma^2
		VPMULLD xmm2,xmm2
		Vmovq xmm4,xmm2;high de xmm4 es todo 0
		VPUNPCKLDQ xmm4,xmm3;
        vmovdqa xmm6,[r10]
		VPADDQ xmm6,xmm4; xxm6+=low
        vmovdqa [r10],xmm6
		add r10,rdx

		VPUNPCKHDQ xmm2,xmm3;
		vmovdqa xmm1,[r10]
		VPADDQ xmm1,xmm2; xxm1+=high
		vmovdqa [r10],xmm1
		add r10,rdx



		
        sub r9,1
	jnz .loop
        mov r9,r8;reinicio contadores y punteros
		mov rbx,[rdi+8];int*
		mov r10,[rdi+16];long*

        sub r12,1
    jnz .loop;salto si quedan shots
 
	.exit:

		xor rax,rax;rax=0, todo ok


		pop r15
		pop r14
		pop r13
		pop r12
		pop rbx
		pop rbp
		ret
