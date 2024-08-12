option casemap:none
public Svertka
public SolveModel
; MODEL PARAMS
BufferSizeX equ 512
BufferSizeY equ 512

Svrtk1Step equ 3
Svrtk1SizeX equ (BufferSizeX - 2) / Svrtk1Step
Svrtk1SizeY equ (BufferSizeY - 2) / Svrtk1Step

Svrtk2Step equ 3
Svrtk2SizeX equ (Svrtk1SizeX - 2) / Svrtk2Step
Svrtk2SizeY equ (Svrtk1SizeY - 2) / Svrtk2Step

Svrtk3Step equ 1
Svrtk3SizeX equ (Svrtk2SizeX - 2) / Svrtk3Step
Svrtk3SizeY equ (Svrtk2SizeY - 2) / Svrtk3Step

Layer1NeuroCount equ 20
Layer2NeuroCount equ 10
; DATA
.data
	extern Svrtk1: QWORD
	extern Svrtk1Mat: QWORD

	extern Svrtk2: QWORD
	extern Svrtk2Mat: QWORD

	extern Svrtk3: QWORD
	extern Svrtk3Mat: QWORD

	extern Layer1Neuro: QWORD
	extern Layer1Container: QWORD

	extern Layer2Neuro: QWORD
	extern Layer2Container: QWORD
; CODE
.code
externdef expf:proc

Svertka PROC
; void Svertka(RCX: unsigned int* Data)
; FIRST STEP
	MOVUPS xmm0, XMMWORD PTR [Svrtk1Mat];
	MOVHLPS xmm1, xmm0; 
	MOVUPS xmm2, XMMWORD PTR [Svrtk1Mat + 12];
	MOVHLPS xmm3, xmm2; 
	MOVUPS xmm4, XMMWORD PTR [Svrtk1Mat + 24];
	MOVHLPS xmm5, xmm4; 

	MOV r8, Svrtk1SizeY;

	MOV rdx, OFFSET Svrtk1;

	CYC:
		MOV r9, Svrtk1SizeX;
		C1:
			CVTPI2PS xmm6, QWORD PTR [rcx];
			MULPS xmm6, xmm0;
			HADDPS xmm6, xmm6;

			CVTSI2SS xmm7, DWORD PTR [rcx + 8];
			MULSS xmm7, xmm1;
			ADDSS xmm6, xmm7;

			CVTPI2PS xmm7, QWORD PTR [rcx + BufferSizeX * 4];
			MULPS xmm7, xmm2;
			HADDPS xmm7, xmm7;
			ADDSS xmm6, xmm7;

			CVTSI2SS xmm7, DWORD PTR [rcx + 8 + BufferSizeX * 4];
			MULSS xmm7, xmm3;
			ADDSS xmm6, xmm7;

			CVTPI2PS xmm7, QWORD PTR [rcx + BufferSizeX * 4 + BufferSizeX * 4];
			MULPS xmm7, xmm4;
			HADDPS xmm7, xmm7;
			ADDSS xmm6, xmm7;

			CVTSI2SS xmm7, DWORD PTR [rcx + BufferSizeX * 4 + 8 + BufferSizeX * 4];
			MULSS xmm7, xmm5;
			ADDSS xmm6, xmm7;

			MOVD DWORD PTR [rdx], xmm6;

			ADD rcx, 4 * Svrtk1Step;
			ADD rdx, 4;

			DEC r9
			JNZ C1;

		ADD rcx, 4 * BufferSizeX * (Svrtk1Step - 1) + 8;
		
		DEC r8;
		JNZ CYC;
; SECOND STEP
	MOVUPS xmm0, XMMWORD PTR [Svrtk2Mat];
	MOVHLPS xmm1, xmm0; 
	MOVUPS xmm2, XMMWORD PTR [Svrtk2Mat + 12];
	MOVHLPS xmm3, xmm2; 
	MOVUPS xmm4, XMMWORD PTR [Svrtk2Mat + 24];
	MOVHLPS xmm5, xmm4; 

	MOV r8, Svrtk2SizeY;

	MOV rcx, OFFSET Svrtk2;
	MOV rdx, OFFSET Svrtk1;

	CYC1:
		MOV r9, Svrtk2SizeX;
		C11:
			MOVLPS xmm6, QWORD PTR [rdx];
			MULPS xmm6, xmm0;
			HADDPS xmm6, xmm6;

			MOVSS xmm7, DWORD PTR [rdx + 8];
			MULSS xmm7, xmm1;
			ADDSS xmm6, xmm7;

			MOVLPS xmm7, QWORD PTR [rdx + Svrtk1SizeX * 4];
			MULPS xmm7, xmm2;
			HADDPS xmm7, xmm7;
			ADDSS xmm6, xmm7;

			MOVSS xmm7, DWORD PTR [rdx + 8 + Svrtk1SizeX * 4];
			MULSS xmm7, xmm3;
			ADDSS xmm6, xmm7;

			MOVLPS xmm7, QWORD PTR [rdx + Svrtk1SizeX * 4 + Svrtk1SizeX * 4];
			MULPS xmm7, xmm4;
			HADDPS xmm7, xmm7;
			ADDSS xmm6, xmm7;

			MOVSS xmm7, DWORD PTR [rdx + Svrtk1SizeX * 4 + 8 + Svrtk1SizeX * 4];
			MULSS xmm7, xmm5;
			ADDSS xmm6, xmm7;

			MOVSS DWORD PTR [rcx], xmm6;

			ADD rdx, 4 * Svrtk2Step;
			ADD rcx, 4;

			DEC r9
			JNZ C11;

		ADD rdx, 4 * Svrtk1SizeX * (Svrtk2Step - 1) + 8;
		
		DEC r8;
		JNZ CYC1;
;THIRD STEP
	MOVUPS xmm0, XMMWORD PTR [Svrtk3Mat];
	MOVHLPS xmm1, xmm0; 
	MOVUPS xmm2, XMMWORD PTR [Svrtk3Mat + 12];
	MOVHLPS xmm3, xmm2; 
	MOVUPS xmm4, XMMWORD PTR [Svrtk3Mat + 24];
	MOVHLPS xmm5, xmm4; 

	MOV r8, Svrtk3SizeY;

	MOV rcx, OFFSET Svrtk3
	MOV rdx, OFFSET Svrtk2

	CYC2:
		MOV r9, Svrtk3SizeX;
		C12:
			MOVLPS xmm6, QWORD PTR [rdx];
			MULPS xmm6, xmm0;
			HADDPS xmm6, xmm6;

			MOVSS xmm7, DWORD PTR [rdx + 8];
			MULSS xmm7, xmm1;
			ADDSS xmm6, xmm7;

			MOVLPS xmm7, QWORD PTR [rdx + Svrtk2SizeX * 4];
			MULPS xmm7, xmm2;
			HADDPS xmm7, xmm7;
			ADDSS xmm6, xmm7;

			MOVSS xmm7, DWORD PTR [rdx + 8 + Svrtk2SizeX * 4];
			MULSS xmm7, xmm3;
			ADDSS xmm6, xmm7;

			MOVLPS xmm7, QWORD PTR [rdx + Svrtk2SizeX * 4 + Svrtk2SizeX * 4];
			MULPS xmm7, xmm4;
			HADDPS xmm7, xmm7;
			ADDSS xmm6, xmm7;

			MOVSS xmm7, DWORD PTR [rdx + Svrtk2SizeX * 4 + 8 + Svrtk2SizeX * 4];
			MULSS xmm7, xmm5;
			ADDSS xmm6, xmm7;

			MOVSS DWORD PTR [rcx], xmm6;

			ADD rdx, 4 * Svrtk3Step;
			ADD rcx, 4;

			DEC r9
			JNZ C12;

		ADD rdx, 8;
		
		DEC r8;
		JNZ CYC2;

	RET;
Svertka ENDP

SolveModel PROC
; float SolveFirstLayerNeuro(RCX: unsigned int* Data)
	CALL Svertka
; FIRST LAYER
	MOV r8, Layer1NeuroCount;

	MOV rcx, OFFSET Layer1Neuro
	MOV rdx, OFFSET Layer1Container

	CYC:
		XORPS xmm0, xmm0;
		MOV r9, Svrtk3SizeX * Svrtk3SizeY / 4;
		MOV rbx, OFFSET Svrtk3;
		C1:
			MOVUPS xmm1, XMMWORD PTR [rcx];
			MULPS xmm1, XMMWORD PTR [rbx];
			ADDPS xmm0, xmm1;

			ADD rbx, 16;
			ADD rcx, 16;
			DEC r9;
			JNZ C1;
		HADDPS xmm0, xmm0;
		HADDPS xmm0, xmm0;

		PUSH rdx;
		PUSH rcx;
		CALL expf;
		POP rcx;
		POP rdx;

		MOV rax, 1;
		CVTSI2SS xmm1, rax;
		RCPSS xmm0, xmm0;
		ADDSS xmm0, xmm1;
		RCPSS xmm0, xmm0;
		MOVSS DWORD PTR [rdx], xmm0;

		ADD rdx, 4;
		DEC r8;
		JNZ CYC;
; SECOND LAYER
	MOV r8, Layer2NeuroCount;

	MOV rcx, OFFSET Layer2Neuro
	MOV rdx, OFFSET Layer2Container

	CYC1:
		XORPS xmm0, xmm0;
		MOV r9, Layer1NeuroCount / 4;
		MOV rbx, OFFSET Layer1Container;
		C11:
			MOVUPS xmm1, XMMWORD PTR [rcx];
			MULPS xmm1, XMMWORD PTR [rbx];
			ADDPS xmm0, xmm1;

			ADD rbx, 16;
			ADD rcx, 16;
			DEC r9;
			JNZ C11;
		HADDPS xmm0, xmm0;
		HADDPS xmm0, xmm0;

		PUSH rdx;
		PUSH rcx;
		CALL expf;
		POP rcx;
		POP rdx;

		MOV rax, 1;
		CVTSI2SS xmm1, rax;
		RCPSS xmm0, xmm0;
		ADDSS xmm0, xmm1;
		RCPSS xmm0, xmm0;
		MOVSS DWORD PTR [rdx], xmm0;

		ADD rdx, 4;
		DEC r8;
		JNZ CYC1;

	RET;
SolveModel ENDP

end