#pragma once

#ifndef NUMBER
#define NUMBER double
#define DOUBLE_PRECISION_NUMBER
#endif


//https://en.wikipedia.org/wiki/Van_der_Corput_sequence
NUMBER VanCorputNumber(int base, int n) {
	NUMBER q = 0;
	NUMBER bk = (double)1.0 / base;
	while (n > 0) {
		q += (n % base) * bk;
		n /= base;
		bk /= base;
	}
	return q;
}
int VanCorputSeq(NUMBER* x, int len, int base) {
	x--;
	int n;
#pragma omp parallel for
	for (n = 1; n <= len; n++) {
		x[n] = VanCorputNumber(base, n);
	}
}