#include <stdio.h>
#include <math.h>
#include "utils.h"
#include <omp.h>

void LabelToZero(YDATA* y) {
	for (int i = 0; i < y->n; i++) {
		if (y->data[i] == -1.0) y->data[i] = 0.0;
	}
}
void NormalizeMatrix(XDATA* x) {
	static double mean;
	static double var;

	for (int j = 0; j < x->m; j++) {

		if (TRAINING_SET) {
			
			mean = 0;
			var = 0;
			for (int i = 0; i < x->n; i++) {
				mean += x->data[i * x->m + j];
				var += x->data[i * x->m + j] * x->data[i * x->m + j];
			}


			mean /= x->n;
			var /= x->n;
			var -= mean * mean;
		}

		for (int i = 0; i < x->n; i++) {
			if (var != 0) x->data[i * x->m + j] = (x->data[i * x->m + j] - mean) / sqrt(var);
		}
	}
}


void ReadData (void* x, char* fileName) {
	FILE* file = fopen(fileName, "rb");
	int nAttr;
	int nObs;
	fread(&nObs, 4, 1, file);
	nObs--;
	fread(&nAttr, 4, 1, file);

	if (nAttr) {
		MATRIX* p = (MATRIX*)x;
		p->m = nAttr;
		p->n = nObs;
		p->data = (double*)malloc(sizeof(double) * nAttr * nObs);
		fread(p->data, sizeof(double), nAttr * nObs, file);
	}
	else {
		VECTOR* p = (VECTOR*)x;
		p->n = nObs;
		p->data = (double*)malloc(sizeof(double) * nObs);
		fread(p->data, sizeof(double), nObs, file);
	}

	fclose(file);

}

void HardThreshold(double* w, double thresh,int n){
	int i;
	#pragma omp parallel for
	for (i = 0; i < n; i++) {
		if (fabs(w[i]) < thresh) w[i]=0;
	}
}

int CountZeros(double* w, int n) {
	int temp = 0;
	int i;
	#pragma omp parallel for reduction(+:temp)
	for (i = 0; i < n; i++) {
		if(w[i] == 0) temp++;
	}
	return temp;
}

double LogisticRatioU(double* w, double* x, double y, int col) {
	double sum = *w;
	w++;
	for (int i = 0; i < col; i++) {
		sum += w[i] * x[i];
	}

	return 1.0 / (1.0 + exp(-sum));

}

//Misclassification Error for one set of x and one label y 
int SingleMisclassError(double* w, double* x, double y, int nAttr) {
	double sum = *w;
	w++;
	int i;
	#pragma omp parallel for reduction(+:sum)
	for (i = 0; i < nAttr; i++) {
		sum += w[i] * x[i];
	}
	
	sum =1/(1+exp(-sum));

	if (sum > 0.5) {
		if (y == 0) return 1;
		return 0;
	}
	else {
		if (y == 0) return 0;
		return 1;
	}
}


double MisclassError(VECTOR* w, XDATA* regressors, YDATA* labels){
	double* x = regressors->data;
	double* y = labels->data;
	int m = regressors->m;
	int n = regressors->n;
	int i;
	int temp = 0;

	for (i = 0; i < n; i++) {
		temp += SingleMisclassError(w->data, &x[i * m], y[i], m);
	}

	return (double)temp/ n;

}

double Loss(double* w, double* x, double* y, int n, int m) {
	double loss = 0;
	double temp = 0;
	for (size_t i = 0; i < n; i++)
	{
		double temp = *w;
		w++;
		for (size_t j = 0; j < m; j++)
		{
			temp += w[j] * x[i*m + j];
		}
		w--;
		temp *= -y[i];
		loss += log(1+exp(temp));
	}
	return loss;
}


VECTOR Tisp(XDATA* regressors, YDATA* labels, double eta, int nFeatures, double thresh) {
	double* x = regressors->data;
	double* y = labels->data;
	VECTOR w;
	int m = regressors->m;
	int n = regressors->n;
	double learningRate = eta / n;
	double value;
	//loop variables
	int i;
	int k;
	int pos;
	//w0 holds the gradient
	w.n = m + 1;
	double* w0;
	if (!(w0 = (double*)malloc((m + 1) * sizeof(double)))) {
	}
	double* w1;
	if (!(w1 = (double*)calloc(m + 1, sizeof(double)))) {

	}
	double* ratios; 
		if (!(ratios = (double*)malloc(n* sizeof(double)))) {
	}

	
	for (size_t iteration = 1; iteration <= 100; iteration++)
	{	
		
		#pragma omp parallel for 
		for (i = 0; i < n; i++) {
			ratios[i] = LogisticRatioU(w1, x + i * m, y[i], m);
		}

		#pragma omp parallel for private(pos, value)
		for (k = 0; k <=m; k++) {
			w0[k] = 0;
			if (k == 0) {
				for (i = 0; i < n; i++) {
					w0[k] += y[i] - ratios[i];
					
				}
			}else{
				for (i = 0; i < n; i++) {
					pos = i * m + (k - 1);
					value =  x[pos];
					w0[k] +=  value * (y[i] -ratios[i]);
				}
			}
			
		}

		
		#pragma omp parallel for 
		for (k = 0; k <= m; k++) {
			w1[k] += learningRate*w0[k];
		}

		

		HardThreshold(w1, thresh, m + 1);

	}

	w.data = w1;
	w.n = m+1;
	free(ratios);
	free(w0);

	return w;

}

VECTOR Tisp300(XDATA* regressors, YDATA* labels, double eta, double thresh, const char* fileName) {
	double* x = regressors->data;
	double* y = labels->data;
	VECTOR w;
	int m = regressors->m;
	int n = regressors->n;
	double learningRate = eta / n;
	int featuresLeft;
	double error = 0;
	FILE* plot = _popen("gnuplot.exe","w");
	fprintf(plot, "set terminal png\n");
	fprintf(plot, "set output '%s'\n", fileName);
	fprintf(plot, "set title 'Misclassification error vs. Iteration for 300 features'\n");
	fprintf(plot, "set xrange [0:100]\n");
	fprintf(plot, "set xtics ('1' 0,'50'50,'100' 100)\n");
	fprintf(plot, "plot '-' with lines\n");
	double value;
	//loop variables
	int i;
	int k;
	int pos;
	//w0 holds the gradient
	double* w0;
	if (!(w0 = (double*)malloc((m + 1) * sizeof(double)))) {
	}
	double* w1;
	if (!(w1 = (double*)calloc(m + 1, sizeof(double)))) {

	}
	double* ratios;
	if (!(ratios = (double*)malloc(n * sizeof(double)))) {
	}

	for (size_t iteration = 1; iteration <= 100; iteration++)
	{

		#pragma omp parallel for 
		for (i = 0; i < n; i++) {
			ratios[i] = LogisticRatioU(w1, x + i * m, y[i], m);
		}

		#pragma omp parallel for private(pos, value)
		for (k = 0; k <= m; k++) {
			w0[k] = 0;
			if (k == 0) {
				for (i = 0; i < n; i++) {
					w0[k] += y[i] - ratios[i];

				}
			}
			else {
				for (i = 0; i < n; i++) {
					pos = i * m + (k - 1);
					value = x[pos];
					w0[k] += value * (y[i] - ratios[i]);
				}
			}

		}

		#pragma omp parallel for 
		for (k = 0; k <= m; k++) {
			w1[k] += learningRate * w0[k];
		}

		HardThreshold(w1, thresh, m + 1);

		#pragma omp parallel for reduction(+:error)
		for (i = 0; i < n; i++) {
			error += SingleMisclassError(w1, &x[i * m], y[i], m);
		}
		error /= n;

		fprintf(plot, "%f\n", error);

		featuresLeft = m - CountZeros(w1, m + 1);

	}

	fprintf(plot, "e\n");
	w.data = w1;
	w.n = m + 1;
	free(ratios);
	free(w0);

	return w;

}

