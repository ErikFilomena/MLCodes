public VanDerCorputAsm

.data 
	zero REAL8 0.0
	one  REAL8 1.0
.code

;rcx - base rdx - n
VanDerCorputAsm PROC
	ret
VanDerCorputAsm ENDP
END

push rcx
	push rdx
	mov rax, rdx
	movsd xmm0, QWORD ptr [zero]
	movsd xmm1, QWORD ptr [one]; 1/base
	movsd xmm2, QWORD ptr [rsp]
	divsd xmm1, xmm2
Loop1:
	div rcx
	push rdx
	cvtsi2sd xmm2, QWORD ptr [rsp]
	
	pop
	cmp rax,0
	jl Loop1
	pop rdx
	pop rcx
	ret
NUMBER VanCorputNumber(int base, int n) {
	NUMBER q = 0;
	NUMBER bk = (double) 1.0 / base;
	while (n > 0) {
		q += (n % base) * bk;
		n /= base;
		bk /= base;
	}
	return q;
}