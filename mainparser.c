#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t CountSep(char* buffer, char sep) {
	int i = 0;
	int count =0;
	int len = strlen(buffer);
	size_t eol;
	while (buffer[i] != '\n') i++;
	eol = i;
	i = 0;
	#pragma omp parallel for reduction(+:count)
	for(i=0;i<eol;i++) {
		if (buffer[i] == sep) count++;
	}
	return count;
}

size_t CountLines(char* buffer) {
	size_t i = 0;
	size_t count = 1;
	while (buffer[i]) {
		if (buffer[i] == '\n') count++;
		i++;
	}
	return count;
}

double ConvertToDouble(char** data, char sep) {
	int pos = 0;
	while (**data != 0 && **data != sep && **data != '\n') {
		*data = *data + 1;
		pos++;
	}

	char* buffer = (char*)malloc(pos);
	memmove(buffer, *data-pos, pos);
	*data= *data +1;
	return atof(buffer);
}

int main(int argc, char* argv[]) {

	printf("Input File: %s\n", argv[1]);
	printf("Output FIle: %s\n", argv[2]);

	FILE* file;
	FILE* output;
	int status = fopen_s(&file, argv[1], "rb");
	fopen_s(&output, argv[2], "wb");
	//int status = fopen_s(&file, "gisette_train.data", "rb");
	//fopen_s(&output, "test.txt", "wb");
	int pos;
	size_t nObs;
	size_t nAttr;
	char* buffer;
	char* temp;
	double* X;

	if (!status) {
		nObs = ftell(file);
		fseek(file, 0, SEEK_END);
		nObs = ftell(file) - nObs;
		if (!(buffer = (char*)malloc(nObs + 1))) goto leave;
		fseek(file,0, SEEK_SET);
		fread(buffer, 1, nObs, file);
		fclose(file);
		buffer[nObs] = 0;
		nObs = CountLines(buffer);
		nAttr = CountSep(buffer,' ');

		printf("Number of Obs: %d \nNumber of Attributes: %d\n", nObs, nAttr);

		if (!(X = (double*)malloc(sizeof(double)*max(nAttr,1)))) {
			printf("Failled to allocated data block\n");
			goto leave;
		}

		fwrite(&nObs, sizeof(int), 1, output);
		fwrite(&nAttr, sizeof(int), 1, output);
		pos = 0;
		temp = buffer;
		for (int k = 0; k < nObs; k++) {
			for (int i = 0; i < max(nAttr,1); i++) {
				X[i] = ConvertToDouble(&temp, ' ');
			}
			fwrite(X, sizeof(double), max(nAttr,1), output);

		}

		fclose(output);
	
		free(X);
		free(buffer);
		return 0;
	}
	else {
		printf("File read unsuccessful\n");
		goto leave;
	}
	 


leave:
	return 0;
}