global buter

section		.text

buter:

;(double** matrix, double** matrix_f,double* a, double* b, int coef, int tamFila, int offsetBines, int shots)
;rdi:m
;rdi+8:mf
;rdi+16:ad  coeficiente matrix filtrada
;rdi+24:bd. coeficiente matrix original
;rdi+32:coef
;rdi+36:tamFila. cuenta iteraciones para recorrer toda una fila con ymm
;rdi+40:offsetBines distancia en bytes entre 2 bines de shots consecutivos
;rdi+44:shots

	 	push rbp
	 	mov rbp, rsp
	 	push rbx
	 	push r12
	 	push r13
	 	push r14
	 	push r15

;hay que conservarlos
	
		mov rax,[rdi] ;m
		mov rbx,[rdi+8]	;mf
		
		xor r11,r11;contador incremental
		xor r13,r13;contador incremental

		xor rsi,rsi
		 mov esi,[rdi+40];offsetbines
		 imul esi,2; para volver para atras tanto como haga falta
		xor rcx,rcx
		mov ecx,[rdi+40];offsetbines
;para inicializar las primeras coef filas tengo que recorrer desde la fila que estoy inicializando hacia atras tantas como filas haya disponibles. una vez que inicialice coef filas, paso al loop general donde recorre para cada fila las coef-1 filas anteriores en el orden en el que estan en memoria
	.setup1st:	
		add r11d,1
		cmp r11d, [rdi+32];comparo para ver si ya incialice todo. Cuando r11 es mas grande que los coef es que ya incialice coef filas
		jg .finInicializacion
		
		mov r13d,r11d;copio para saber cuantas filas tengo que visitar

		mov r9,[rdi+16];*coef a
		mov r10,[rdi+24];*coef b
		vbroadcastsd ymm1,[r10];primera fila solo necesita el coef b
		
		xor r14,r14
		mov r14d,[rdi+36];r14=TamFila
		
		;mov rax,r15;ubico rax

		
	;	mov r14,1
		

	.loop:
		vmulpd ymm0,ymm1,[rax]
		VMOVAPD [rbx],ymm0
		add rax, 32
		add rbx, 32;muevo punteros size(ymm) en bytes
		
		sub r14d,1 ;contador tamFila
	jnz .loop

	mov r8, rbx; para estar como rax asi les puedo restar lo mismo a r8 y rax
	mov r15,rax;para la proxima iteracion

	.setup2aFin:
		sub r13d,1
		jz .setup1st;chequeo si faltan filas en este paso

		mov r14d,[rdi+36];rescato TamFila

		add r9,8
		add r10,8;los coef son double, muevo punteros
		vbroadcastsd ymm1,[r10];ymm1 coeficientes de m
		vbroadcastsd ymm2,[r9];ymm2 coeficientes de mf


		sub r15, rsi; vuelvo 2 cantidad de bines para atras en m 
		sub r8,rsi; vuelvo 2 cantidad de bines para atras en mf
		sub rbx,rcx; vuelvo al principio de la fila que quiero actualizar
		
	.2aFin:
		vmulpd ymm0, ymm2,[r8]; 
		VFMSUB231PD ymm0, ymm1,[r15];
		vaddpd ymm0,ymm0,[rbx]
		vmovapd [rbx],ymm0 ;muevo resultado
		add r15,32
		add r8,32
		add rbx,32;muevo punteros
		sub r14d,1
		jnz .2aFin
	
	jmp .setup2aFin;cuando termino el loop vuelvo a setup2aFin para ver si faltan filas
	
	;.finFila:
	;	sub r12,1 ;me fijo si sigo teniendo que actualizar filas
	;	jnz .setup2aFin

	
	.finInicializacion:
		;para fila desde m en adelante, primero inicializo multiplicando por B y luego recorro las coef-1 anteriores multitiplancando y sumando por los coeficientes correspondientes.
		;mf está posicionada.m no
				mov r9,[rdi+16]
				mov r10,[rdi+24];setedos en el primer coef

				xor rsi,rsi 
				mov esi,[rdi+44];muevo shots
				sub esi,[rdi+32];resto coef 
				;esi=shots-coef

				xor r11,r11
				xor r12,r12
				xor rcx,rcx
				;necesito registro con offset_bines, tamFila y coeficientes: r11,r12,rcx
				mov r11d,[rdi+36]; r11d= tamFila
				mov r12d, [rdi+40];r12d= offsetbines
				mov ecx, [rdi+32];ecx=coef
				

				mov rax,[rdi]

				mov rdx,r12
				dec rcx;=coef-1
				imul rdx,rcx;rdx=offsetbines*(coef-1)

				add rax,rdx;(coef-1)*offssetbines;;acomodo rax para empezar post inciializacion


				mov r13,rcx;r13 tiene coef-1 para contar cuantas filas tengo que loopear por cada fila nueva a calcular 
				;inc rcx
	.setup1era:	
		vbroadcastsd ymm1,[r10]
		mov r14,r11;reset tamFila
		mov r8,rbx
		add rax, r12;posiciono rax para coincidir con la fila de rbx (mf) en m
	.1era:
		vmulpd ymm0,ymm1,[rax]
		vmovapd [rbx],ymm0
		add rax,32
		add rbx,32
		sub r14,1 
	jnz .1era
	;jmp .exit
		sub r8,rdx;offset_bines*(coef-1);coef-1 porque no avanzo como rax vuelvo m y mf para atras tantas filas como coeficientes
		
		sub rax,rdx
		sub rax,r12 ;sub rax,offset_bines*coef

		push rcx
		imul rcx,8
		add r10,rcx;8*(coef-1)
		add r9,rcx;8*(coef-1);set coef_a y b en el ultimo coef
		pop rcx

		;sub r13,1;tengo que hacer el loop de abajo coef-1 veces (el coef 0 es el de la fila actual)
	.loopSetup:
		;mf y m estan en la siguiente a la que actualizo. Tengo que restarle coef*offset_bines

		mov r14,r11;reset tamFila

		sub rbx,r12;set mf_actual, mf_actual-offset_bines;mf_actual apunta a la fila que inicializo 1era

		vbroadcastsd ymm1,[r10]
		vbroadcastsd ymm2,[r9]
	.loopFila:
		vmulpd ymm0, ymm2,[r8]; 
		VFMSUB231PD ymm0, ymm1,[rax];
		vaddpd ymm0,ymm0,[rbx]
		vmovapd [rbx],ymm0 ;muevo resultado
		add r8,32
		add rax,32
		add rbx,32
		sub r14,1
	jnz .loopFila

		sub r10,8
		sub r9,8;bajo indice para broadcast

		sub r13,1;este cuenta la cantidad de filas totales necesarias para calcular una fila
	jnz .loopSetup
	.cambioFila: ;mf_actual esta donde tiene que estar. a m hay que sumarle offssetbines
		mov r13,rcx;reset coeficientes-1

		mov r9,[rdi+16]
		mov r10,[rdi+24];tal vez no hace falta porque el add ya los fue poniendo
		;retraer punteros m,mf a su nueva posicion (menos (coef-1)*offssetbines);deberia salir con subs
		;add rax, r12;para que el add de setup la deje bien 
		;rbx setea r8 en setup. rbx esta bien posicionado
		sub esi,1;mientras haya filas sin calcular sigo
	jnz .setup1era


	.exit:

		xor rax,rax;rax=0, todo ok
	;	mov eax,r11d;
;mov rax, [rdi+36]

		pop r15
		pop r14
		pop r13
		pop r12
		pop rbx
		pop rbp
		ret
