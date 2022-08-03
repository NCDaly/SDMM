#include <stdio.h>
#include <cblas.h>

int main(void)
{
	const int lda=3, ldb=3, ldc=3;
	int m, n, k;
	float alpha, beta;

	float a[] = {1,2,3,4,5,6,7,8,9};

	float b[] = {1,2,3,4,5,6,7,8,9};

	float c[] = {0,0,0,0,0,0,0,0,0};

	m = 3; n = 3; k = 3;

	alpha = 1.0; beta = 0.0;

	cblas_sgemm (CblasColMajor, CblasNoTrans, CblasNoTrans,
		m, n, k, alpha, a, lda, b, ldb, beta, c, ldc);

	printf ("[ %g, %g, %g\n", c[0], c[1], c[2]);
	printf ("  %g, %g, %g\n", c[3], c[4], c[5]);
	printf ("  %g, %g, %g ]\n", c[6], c[7], c[8]);

	return 0;
}
