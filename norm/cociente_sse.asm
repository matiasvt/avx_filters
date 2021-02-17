global cocientes_sse

section		.text

cocientes_sse:
;asume entrada short, no se pasan de int en shotNumber sumas y mucho mas

;parametros
;rdi=&memoria-> rdi=short*,rdi+8=int*, rdi+16=long*, rdi+24=double*
;shotNumber =rsi
;binesProcesados =rdx
;offset_bines( en bytes)=rcx
;contadorLoopBines =r8

	 	push rbp
	 	mov rbp, rsp
	 	push rbx
	 	push r12
	 	push r13
	 	push r14
	 	push r15

;hay que conservarlos
		VZEROUPPER
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
		mov r15,[rdi+24];double*
		mov r13,[rdi+32];double*  ;todos inicializados en 0 salvo datos

		mov r14,rdx;era rcx
		imul r14,2;binesProcesados*sizeof(short) para usar en bytes y mover en datos

		imul rdx,4;binesProcesados*sizeof(int) para usar en bytes y mover en resultados; era rcx

	.setup:
		;hay que buscar los bines correspondientes para acumular. Para eso hay que sumar el offset_bines a rdi tantas veces como shotNumber
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

		Vmovdqa xmm12,xmm2

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


			;cociente
			VPSHUFD xmm12,xmm12, 11011000b
			VCVTDQ2PD xmm13,xmm12
			VPSHUFD xmm12,xmm12, 11101110b
			VCVTDQ2PD xmm8,xmm12
			Vpxor xmm10,xmm10
			VCMPPD xmm10,xmm8,100b
			VDIVPD xmm13,xmm8
			VANDPD xmm13,xmm10
			vmovapd xmm7,[r15]
			VADDPD xmm7,xmm13
			vmovapd [r15],xmm7
			add r15,rdx
			Vmulpd xmm13,xmm13;cociente^2
			vmovapd xmm11,[r13]
			VADDPD xmm11,xmm13
	        vmovapd [r13],xmm11
			add r13,rdx
;
        sub r9,1
	jnz .loop
        mov r9,r8;reinicio contadores y punteros
		mov rbx,[rdi+8];int*
		mov r10,[rdi+16];long*
			mov r15,[rdi+24];double*
			mov r13,[rdi+32];double*

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
