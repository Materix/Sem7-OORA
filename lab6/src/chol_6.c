#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include "papiLib.c"

/* for Fortran - j*n + i */
//#define IDX(i, j, n)	(((i) * (n)) + (j))
#define IDX(i, j, n) (((j)+ (i)*(n)))

static double gtod_ref_time_sec = 0.0;

/* Adapted from the bl2_clock() routine in the BLIS library */

double dclock()
{
	double         the_time, norm_sec;
	struct timeval tv;
	gettimeofday( &tv, NULL );
	if ( gtod_ref_time_sec == 0.0 )
		gtod_ref_time_sec = ( double ) tv.tv_sec;
	norm_sec = ( double ) tv.tv_sec - gtod_ref_time_sec;
	the_time = norm_sec + tv.tv_usec * 1.0e-6;
	return the_time;
}

int
chol(double *A, unsigned int n)
{
	register unsigned int i;
	register unsigned int j;
	register unsigned int k;
	register unsigned int local_size = n;
	register double Aij;

	for (j = 0; j < local_size; j++) {
		for (i = j; i < local_size; i++) {
			Aij = A[IDX(i, j, local_size)];
			if(j>8){
				for (k = 0; k < j;) {
					if(k < j-8){
						Aij -= A[IDX(i, k, local_size)] * A[IDX(j, k, local_size)];
						Aij -= A[IDX(i, k+1, local_size)] * A[IDX(j, k+1, local_size)];
						Aij -= A[IDX(i, k+2, local_size)] * A[IDX(j, k+2, local_size)];
						Aij -= A[IDX(i, k+3, local_size)] * A[IDX(j, k+3, local_size)];
						Aij -= A[IDX(i, k+4, local_size)] * A[IDX(j, k+4, local_size)];
						Aij -= A[IDX(i, k+5, local_size)] * A[IDX(j, k+5, local_size)];
						Aij -= A[IDX(i, k+6, local_size)] * A[IDX(j, k+6, local_size)];
						Aij -= A[IDX(i, k+7, local_size)] * A[IDX(j, k+7, local_size)];
						k+=8;
					} else{
						Aij -= A[IDX(i, k, local_size)] * A[IDX(j, k, local_size)];
						k++;
					}
				}
			} else{
				for(k=0; k<j; k++){
					Aij -= A[IDX(i, k, local_size)] * A[IDX(j, k, local_size)];
				}
			}
			 A[IDX(i, j, local_size)] = Aij;
		}

		if (A[IDX(j, j, local_size)] < 0.0) {
			return (1);
		}

		A[IDX(j, j, local_size)] = sqrt(A[IDX(j, j, local_size)]);
		for (i = j + 1; i < local_size; i++)
			A[IDX(i, j, local_size)] /= A[IDX(j, j, n)];
	}

	return (0);
}


double* load_matrix(char* filename, int n){

	char *line = NULL;
	char *token = NULL;
	size_t len = 0;
	ssize_t read;
	FILE *fp;
	double *A;
	char newline[2] = "\n";
	int i = 0;
	int j = 0;


	fp = fopen(filename, "r");
	if (fp == NULL) {
		printf("Cannot open input file\n");
		exit(1);
	}


	A = calloc(n*n, sizeof(double));
	assert(A != NULL);

	while ((read = getline(&line, &len, fp)) != -1){
		while ((token = (char*)strsep(&line, ",")) != NULL){
			if (strcmp(token, newline) != 0){
				A[IDX(i, j, n)] = atof(token);
				j++;
				if (j == n){
					j = 0;
					i++;
				}
			}
		}
	}

	fclose(fp);
	if (line) {
		free(line);
	}
	return A;
}

int main(int argc, char** argv) {
	double *A;
	int n, ret, event;
	double startTime;
	double endTime;
	long long value;

	n = atoi(argv[2]);
	A = load_matrix(argv[1], n);
	event = atoi(argv[3]);
	init();
	startCount(event);
	startTime = dclock();
	ret = chol(A, n);
	stopAndPrint();
	endTime = dclock();
//	printf("%lf\n", endTime - startTime);
	free(A);
	return 0;
}
