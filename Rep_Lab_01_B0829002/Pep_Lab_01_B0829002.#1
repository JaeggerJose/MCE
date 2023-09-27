ORG 0x0000 
MOV R0, #20h ;
MOV R1, #28h ;
MOV R4, #8 ;
MOV R5, #0 ; summary

Matrix_LOOP:
	MOV A, @R0;
	MOV B, @R1;
	MUL AB;
	ADD A, R5;
	MOV R5, A;
	INC R0;
	INC R1;
	DJNZ R4, Matrix_LOOP;

HERE:SJMP HERE
END
