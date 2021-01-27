#pragma once

#include "Generator.h"
#include "PrimeList.h"
#include <vector>
#include "Permuter.h"
#include "Utils.h"
#define SHUFLIST std::vector< DigitPermuter<size_t> >

template <class NBR>
struct HaltonGenerator :Generator<NBR> {

	size_t dim;
	bool shuffle;
	SHUFLIST shufflers;

	HaltonGenerator(size_t dim, bool shuffle):dim(dim), shuffle(shuffle){
		if (shuffle) {
			shufflers = SHUFLIST(dim);
			int i;
#pragma omp parallel for
			for (i = 0; i < dim; i++) {
				shufflers[i] = DigitPermuter<size_t>(BLPN[i]);
				shufflers[i].Shuffle();
			}
		}
	}
	
	void Dim(size_t dim) {
		this->dim = dim;
		if (shuffle) {
			shufflers = SHUFLIST(dim);
			int i;
#pragma omp parallel for
			for (i = 0; i < dim; i++) {
				shufflers[i] = DigitPermuter<size_t>(BLPN[i]);
				shufflers[i].Shuffle();
			}
		}
	}

	virtual void GenerateSequence(NBR* dst, size_t len) {
		
#pragma omp parallel
		{
			size_t** digits = new size_t * [dim];
			for (size_t i = 0; i < dim; i++) {
				size_t nDigits = FastLog(len, BLPN[i]);
				digits[i] = new size_t[nDigits + 1];
				*digits[i] = nDigits;
			}

			int i;
#pragma omp for
			for (i = 0; i < len; i++) {
				for (int j = 0; j < dim; j++) {
					BaseExpansion(digits[j] + 1, i + 1, BLPN[j], *digits[j]);
					if (shuffle)shufflers[j].Permute(digits[j] + 1, *digits[j]);
					dst[i * dim + j] = NumberFromDigits<NBR>(digits[j] + 1, BLPN[j], *digits[j]);
				}
			}
			for (size_t i = 0; i < dim; i++) {
				delete[] digits[i];
			}
			delete[] digits;
		}

		
		
	}
	void GenerateRange(NBR* dst, size_t len, NBR a, NBR b) { }

	virtual NBR GenRnd() { return 0; }
	virtual NBR GenRange(NBR a, NBR b) { return 0; }
};