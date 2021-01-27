#pragma once

#include <stdlib.h>

#define per_rnd(b) rand()%b

template <class INT>
struct DigitPermuter {

	INT base;
	INT* permuted = nullptr;
	DigitPermuter(INT base) :base(base) {
		permuted = new size_t[base];
		for (size_t i = 0; i < base; i++) {
			permuted[i] = i;
		}
	}

	DigitPermuter(){
	}
	~DigitPermuter() {
		if(permuted)delete[] permuted;
	}

	DigitPermuter& operator=(const DigitPermuter<INT>& src) {
		if (this == &src) return *this;
		if (permuted) delete[] permuted;
		base = src.base;
		permuted = new INT[base];
		for (size_t i = 0; i < base; i++) {
			permuted[i] = src.permuted[i];
		}
		return *this;
	}

	DigitPermuter(const DigitPermuter<INT>& src) {
		base = src.base;
		permuted = new size_t[base];
		for (size_t i = 0; i < base; i++) {
			permuted[i] = i;
		}
	}


	inline void Shuffle() {
		for (INT i = 0; i < 4 * base; i++) {
			INT i1 = per_rnd(base);
			INT i2 = per_rnd(base);
			INT temp = permuted[i1];
			permuted[i1] = permuted[i2];
			permuted[i2] = temp;
		}
	}

	inline void Permute(INT* list, size_t len) {
		for (size_t i = 0; i < len; i++) {
			list[i] = permuted[list[i]];
		}
	}
};