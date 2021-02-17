global suma_avx

section		.text

suma_avx:

;parametros
;rdi=&memoria-> rdi=short*,rdi+8=int*, rdi+16=long*, rdi+24=double*,rdi+32=double*
;shotnumber =rsi
;binesprocesados =rdx
;offset_bines( en bytes)=rcx PARAMETRO SIN USO 
;contadorLoop =r8
;ASUME MEMORIA INICIALIZADA EN 0 y alineada a 32

	 	push rbp
	 	mov rbp, rsp
	 	push rbx
	 	push r12
	 	push r13
	 	push r14
	 	push r15

		vpxor ymm3,ymm3

		mov rax,[rdi];short*
		mov rbx,[rdi+8];int*

		mov r14,rdx;bines Por iteraciones
		imul r14,2;binesprocesados*sizeof(short) para moverse en la matriz
		imul rdx,4;para moverse en los resultados. Equivale a una movida a memoria de un xmm/ymm segun corresponda (4 u 8)

		mov r9,r8
		

	.loop:

		vpmovsxwd ymm2,[rax]; Extiendo signo de los datos en ymm2
		add rax,r14;muevo bin siguiente
		vpsrad ymm2,2;shift 2 right por primeros 2 bits inutile

		;suma
		;vmovdqa ymm0,
		vpaddd ymm0,ymm2,[rbx] ;xmm0+=xmm2
        vmovdqa [rbx],ymm0
        add rbx,rdx

		sub r9,1
        
		jnz .loop

        mov r9,r8;reinicio contadores y punteros
		mov rbx,[rdi+8];int*
        sub rsi,1 ;resto un shot
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

section		.data

permutacion:
			dd 0d
			dd 2d
			dd 4d
			dd 6d
			dd 1d
			dd 3d
			dd 5d
			dd 7d

permutacion_long:
			dd 0d
			dd 1d
			dd 4d
			dd 5d
			dd 2d
			dd 3d
			dd 6d
			dd 7d