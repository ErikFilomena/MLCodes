#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <intrin.h>

#define NUMBER double 

#include "LinearAlgebra.h"
#include "VanCorput.h"
#include "Faure.h"
#include "RandomGenerator.h"	
#include "MonteCarlo.h"
#include "MarsenneGenerator.h"
#include "Permuter.h"
#include "HaltonGenerator.h"
#include "Utils.h"
#include "Vector.h"

#include "cblas.h"



double f(NUMBER* x) {
	double result = 1;
	for (int i = 0; i < 20; i++) {
		result*=fabs(4 * *x - 2);
		x++;
	}

	return result;
}

double f2(NUMBER* x) {
	
	if (x[0] > x[1])return 0.0;
	return 1.0;
}


//Input and output are always row major, while thetas are always column major
int main() {

	FaureGenerator<double> gen(2,20);
	MonteCarlo<double> monty(2, f2, &gen);
	
	monty.Simulate(10000);

	printf("%f\n", monty.mean);
	

}

