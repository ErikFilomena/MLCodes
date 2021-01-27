#include "VectorOps.h"

double DoubleVectorDot(double* v1, double* v2, int len, int offset)
{	
	double temp = 0;
	int i;
	#pragma omp parallel for reduction(+:temp)
	for (i = offset; i < len; i++)
	{
		temp += v1[i] * v2[i];
	}
	return temp;
}

int DoubleVectorMax(double* src, int len, int offset) {
	int i;
	double globalMax = *src;
	double localMax = *src;
	int localPos = 0;
	int globalPos = 0;
#pragma omp parallel for private(localMax) private(localPos)
	for (i = offset; i < len; i++) {
		if (src[i] > localMax) {
			localMax = src[i];
			localPos = i;
		}
	}

#pragma omp critical
	{
		if (localMax > globalMax) {
			globalPos = localPos;
		}
	}

	return globalPos;
}

void DoubleVectorElementProd(double* dst, double* v1, double* v2, int len, int offset) {
	int i;
	if (offset) {
#pragma omp parallel for 
		for (i = offset; i < len; i++)
		{
			dst[i - offset] = v1[i] * v2[i];
		}
	}
	else {
#pragma omp parallel for 
		for (i = offset; i < len; i++)
		{
			dst[i] = v1[i] * v2[i];
		}
	}

}