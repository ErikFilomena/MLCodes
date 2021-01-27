#pragma once

#include "Generator.h"
#include "RowDenseMatrix.h"

template <class NBR>
struct MonteCarlo {


	size_t dim;
	NBR (*f) (NBR* x);
	Generator<NBR> *gen;
	NBR* _samples = nullptr;
	NBR* _simValues=nullptr;
	NBR mean;
	size_t cSampleSize=0;
	RowDenseMatrixInterface<NBR> samples;
	RowDenseMatrixInterface<NBR> simValues;

	MonteCarlo(size_t dim, NBR (*f) (NBR* x),Generator<NBR>* gen):dim(dim), f(f),gen(gen) {
		samples.n = &cSampleSize;
		samples.m = &(*this).dim;
		samples.data = &_samples;

		simValues.n = &cSampleSize;
		simValues.m = new size_t;
		*simValues.m = 1;
		simValues.data = &_simValues;
	}

	virtual ~MonteCarlo() {
		if (_samples) delete[] _samples;
		if (_simValues) delete[] _simValues;
		delete simValues.m;
	}

	void Simulate(int n) {
		if (_samples) delete[] _samples;
		if (_simValues) delete[] _simValues;
		cSampleSize = n;
		_simValues = new NBR[n];
		_samples = new NBR[n * dim];
		gen->GenerateSequence(_samples, n);
		int i;
		NBR localMean = 0;
#pragma omp parallel for reduction(+:localMean)
		for (i = 0; i < n; i++) {
			_simValues[i] = f(_samples + i * dim);
			localMean += _simValues[i];
		}

		mean = localMean/cSampleSize;
	}





};