#pragma once

//Code stolen from https://en.wikipedia.org/wiki/Mersenne_Twister

#include "Generator.h"

#define mar_upperbits(x) (x & (0x80 << 24))
#define mar_lowerbits(x) ((x & 0x00000000FFFFFFFF) >>1)
#define mar_a 0x99083B0DF
#define mar_w 32
#define mar_n 624
#define mar_r 31
#define mar_m 397
#define mar_u 11
#define mar_s 7
#define mar_t 15
#define mar_l 18
#define mar_b 0x9D2C5680
#define mar_c 0xEFC60000

#define mar_f 1812433253

unsigned int mar_x[mar_n];

void InitArray(unsigned int *x,size_t seed) {
	x[0] = seed;
	for (int i = 1; i < mar_n; i++) {
		x[i] = mar_f * (x[i - 1] ^ (x[i - 1] >> (mar_w - 2))) + i;
	}
}

unsigned int NextMarsenne(unsigned int* x,size_t k) {
	unsigned int temp;
	if (x[k % mar_n] & 1) {
		temp = (0b1000000000000000000000000000000) | (x[(k + 1) % mar_n]& 0xEFFFFFFF);
		temp = temp >> 1;
	}
	else {
		temp = x[(k + 1) % mar_n] & 0xEFFFFFFF;
		temp = (temp >> 1) & mar_a;
	}
	unsigned int next = x[(k + mar_m) % mar_n] ^ temp;
	x[k % mar_n] = next;
	next = next ^ (next >> mar_u);
	next = next ^ ((next << mar_s) & mar_b);
	next = next ^ ((next << mar_t) & mar_c);
	next = next ^ (next >> mar_l);

	return next;
}

template <class NBR>
struct MarsenneTwisterGenerator :Generator<NBR> {

	int dim;
	size_t k = 0;
	unsigned int x[mar_n];
	size_t _seed;

	MarsenneTwisterGenerator(int dim,size_t seed) :dim(dim), _seed(seed) {
		InitArray(x, seed);
	}
	virtual ~MarsenneTwisterGenerator() {}

	NBR GenRnd() {
		NBR temp = (NBR)NextMarsenne(x, k) / 0xFFFFFFFF;
		k++;
		return temp;
	}
	NBR GenRange(NBR a, NBR b) {
		NBR temp = (NBR)NextMarsenne(x, k)/0xFFFFFFFF;
		k++;
		return (b-a)*temp;
	}

	void seed(unsigned long long s) {
		_seed = s;
		InitArray(x, s);
	}

	void GenerateSequence(NBR* dst, size_t len) {
		for (int i = 0; i < len; i++) {
			for (int j = 0; j < dim; j++) {
				dst[i * dim + j] = GenRnd();
			}
		}
	}
	void GenerateRange(NBR* dst, size_t len, NBR a, NBR b) {
		for (int i = 0; i < len; i++) {
			for (int j = 0; j < dim; j++) {
				dst[i * dim + j] = GenRange(a,b);
			}
		}
	}
};