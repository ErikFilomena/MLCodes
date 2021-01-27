#pragma once

#include "PrimeList.h"
#include "LinearAlgebra.h"
#include "Generator.h"
#include <vector>
#include <math.h>
#include <iostream>
#include "Utils.h"
#include "PrimeList.h"
typedef std::vector<INTUTMATRIX> GENLIST;


inline INTUTMATRIX Pk(const INTUTMATRIX& src, size_t k, size_t p) {
	INTUTMATRIX pk = src;
	for (int j = 1; j < pk.n; j++) {
		for (int i = 0; i <j; i++) {
			int val = PowMod(k, j - i, p);
			pk.set(i, j, (val * pk(i, j))%p);
		}
	}
	return pk;
}

template <class NBR>
inline NBR FaureNumber(size_t* digits, size_t base, size_t len) {
	NBR faureNumber = 0;
	NBR baseInv = 1.0 / (double)base;
	for (size_t i = 0; i < len; i++) {
		faureNumber += baseInv * (double)(digits[i]);
		baseInv /= base;
	}
	return faureNumber;
}

/**
	* Faure Generator class
**/
template <class NBR>
struct FaureGenerator:Generator<NBR> {

	size_t dim;
	size_t maxNDigits;
	size_t base;
	size_t* digits;
	size_t* digitsDimCoord;
	GENLIST generators;
	int skip;

	/**
	* Construtor for FaureGernerator 
	* @param base the base for the number expansion
	* @param dim dimension of the tuples the generator outputs
	* @param maxNDigits maximum number of digits to be used in the expansion
	**/
	FaureGenerator(size_t dim, size_t maxNDigits):dim(dim), maxNDigits(maxNDigits)  {
		int* prime = BLPN;
		while (*prime < dim) prime++;
		base = *prime;
		skip = Pow(base, 4) - 1;
		digits = new size_t[maxNDigits];
		digitsDimCoord = new size_t[maxNDigits];
		generators = GENLIST(dim);
		generators[0] = UpperTriangularEye<int>(maxNDigits);
		generators[1] = UpperPascalMatrixModP<int>(maxNDigits, base);
		int k;
#pragma omp parallel for
		for (k = 2; k <dim; k++) {
			generators[k] = Pk(generators[1], k, base);
		}
	}

	virtual ~FaureGenerator() {
		delete[] digits;
		delete[] digitsDimCoord;
	}

	//Assume that dst is a row-major matrix with dimentions dim x len
	inline void GenerateSequence(NBR* dst, size_t len) {
		
		for (int i = 0; i < len; i++) {
			BaseExpansion(digits, i + skip, base, maxNDigits);
			for (int j = 0; j < dim; j++) {
				if (j) {
					generators[j].LeftMatrixVecMulMod(digitsDimCoord, digits, maxNDigits, base);
				}
				else {
					for (int k = 0; k < maxNDigits; k++)digitsDimCoord[k] = digits[k];
				}
				*dst = FaureNumber<NBR>(digitsDimCoord, base, maxNDigits);
				dst++;
			}
		}
	}

	NBR GenRnd() { return 0; };
	NBR GenRange(NBR a, NBR b) { return 0; };
	void GenerateRange(NBR* dst, size_t len, NBR a, NBR b) {};
};