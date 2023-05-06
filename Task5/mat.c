#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define SIZE 40

void read_vector(double x[], int n) {
	for (int i = 0; i < n; ++i) {
		scanf("%lf", x++);
	}
}

void print_vector(double x[], int n) {
	for (int i = 0; i < n; ++i) {
		printf("%.4f ", x[i]);
	}
	printf("\n");
}

void read_mat(double A[][SIZE], int m, int n) {
	for (int i = 0; i < m; ++i) {
		for (int j = 0; j < n; ++j) {
			scanf("%lf", &A[i][j]);
		}
	}
}

void print_mat(double A[][SIZE], int m, int n) {
	for (int i = 0; i < m; ++i) {
		for (int j = 0; j < n; ++j) {
			printf("%.4f ", A[i][j]);
		}
		printf("\n");
	}
}

// 1. Calculate matrix product, AB = A X B
// A[m][p], B[p][n], AB[m][n]
void mat_product(double A[][SIZE], double B[][SIZE], double AB[][SIZE], int m, int p, int n) 
{
	for (int r = 0; r < m; r++)
		for (int c = 0; c < n; c++)
			AB[r][c] = 0;

	for (int r = 0; r < m; r++)
		for (int c = 0; c < n; c++)
			for (int rp = 0; rp < p; rp++)
				AB[r][c] += A[r][rp] * B[rp][c];
}


// 2. Matrix triangulation and determinant calculation - simplified version
// (no rows' swaps). If A[i][i] == 0, function returns NAN.
// Function may change A matrix elements.
double gauss_simplified(double A[][SIZE], int n) 
{
	for (int col = 0; col < n - 1; col++)
	{
		if (A[col][col] == 0) return NAN;

		for (int row = col + 1; row < n; row++)
		{
			double k = A[row][col] / A[col][col];
			for (int c = col; c < n; c++)
			{
				A[row][c] -= k * A[col][c];
			}
		}
	}

	double det = 1.0;
	for (int i = 0; i < n; i++)
		det *= A[i][i];

	return det;
}

void backward_substitution_index(double A[][SIZE], const int indices[], double x[], int n) {
}

// 3. Matrix triangulation, determinant calculation, and Ax = b solving - extended version
// (Swap the rows so that the row with the largest, leftmost nonzero entry is on top. While
// swapping the rows use index vector - do not copy entire rows.)
// If max A[i][i] < eps, function returns 0.
// If det != 0 && b != NULL && x != NULL then vector x should contain solution of Ax = b.

double gauss(double A[][SIZE], const double b[], double x[], const int n, const double eps) 
{
	unsigned int* permTab = calloc(n, sizeof(unsigned int));
	double k_ = 1.0;
	for (int i = 0; i < n; i++)
		permTab[i] = i;

	double* b_ = calloc(n, sizeof(double));
	for (int i = 0; i < n; i++)
		b_[i] = b[i];

	for (int col = 0; col < n - 1; col++)
	{
		unsigned int maxInd = col;

		for (int row = col+1; row < n; row++)
			if (fabs(A[permTab[row]][col]) > fabs(A[permTab[maxInd]][col])) maxInd = row;

		unsigned int temp = permTab[maxInd];
		permTab[maxInd] = permTab[col];
		permTab[col] = temp;
		if (maxInd != col) k_ *= -1.0;

		if (fabs(A[permTab[col]][col]) < eps) return 0;

		for (int row = col+1; row < n; row++)
		{
			double k = A[permTab[row]][col] / A[permTab[col]][col];
			for (int c = col; c < n; c++)
			{
				A[permTab[row]][c] -= k * A[permTab[col]][c];
			}
			b_[permTab[row]] -= k * b_[permTab[col]];
		}
	}

	double det = k_;
	for (int i = 0; i < n; i++)
		det *= A[permTab[i]][i];

	if (det != 0.0 && b && x)
	{
		double toSubs = 0.0;
		for (int i = n - 1; i >= 0; i--)
		{
			x[i] = (b_[permTab[i]] - toSubs) / A[permTab[i]][i];
			toSubs = 0.0;
			for (int j = n - 1; j >= i; j--)
				if (i > 0) toSubs += x[j] * A[permTab[i-1]][j];
		}
	}

	free(permTab);
	free(b_);

	return det;
}

// 4. Returns the determinant; B contains the inverse of A (if det(A) != 0)
// If max A[i][i] < eps, function returns 0.
double matrix_inv(double A[][SIZE], double B[][SIZE], int n, double eps) 
{
	unsigned int* permTab = calloc(n, sizeof(unsigned int));
	double k_ = 1.0;
	for (int i = 0; i < n; i++)
		permTab[i] = i;

	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
		{
			B[i][j] = -0.0;
			if (i == j) B[i][j] = 1.0;
		}



	for (int col = 0; col < n - 1; col++)
	{
		unsigned int maxInd = col;

		for (int row = col + 1; row < n; row++)
			if (fabs(A[permTab[row]][col]) > fabs(A[permTab[maxInd]][col])) maxInd = row;

		unsigned int temp = permTab[maxInd];
		permTab[maxInd] = permTab[col];
		permTab[col] = temp;
		if (maxInd != col) k_ *= -1.0;

		if (fabs(A[permTab[col]][col]) < eps) return 0;

		for (int row = col + 1; row < n; row++)
		{
			double k = A[permTab[row]][col] / A[permTab[col]][col];
			for (int c = col; c < n; c++)
			{
				A[permTab[row]][c] -= k * A[permTab[col]][c];
			}
			for (int c = 0; c < n; c++)
			{
				B[permTab[row]][c] -= k * B[permTab[col]][c];
			}
		}
	}

	double det = k_;
	for (int i = 0; i < n; i++)
		det *= A[permTab[i]][i];

	for (int col = n-1; col > 0; col--)
	{
		for (int row = col - 1; row >= 0; row--)
		{
			double k = A[permTab[row]][col] / A[permTab[col]][col];
			for (int c = col; c < n; c++)
			{
				A[permTab[row]][c] -= k * A[permTab[col]][c];
			}
			for (int c = 0; c < n; c++)
			{
				B[permTab[row]][c] -= k * B[permTab[col]][c];
			}
		}
	}

	for (int c = 0; c < n; c++)
	{
		double k = A[permTab[c]][c];
		for (int cx = 0; cx < n; cx++)
		{
			A[permTab[c]][cx] /= k;
			B[permTab[c]][cx] /= k;
		}
	}

	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			A[i][j] = B[i][j];
	
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			B[i][j] = A[permTab[i]][j];

	return det;
}

int main(void) {

	double A[SIZE][SIZE], B[SIZE][SIZE], C[SIZE][SIZE];
	double b[SIZE], x[SIZE], det, eps = 1.e-13;

	int to_do;
	int m, n, p;

	scanf("%d", &to_do);

	switch (to_do) {
	case 1:
		scanf("%d %d %d", &m, &p, &n);
		read_mat(A, m, p);
		read_mat(B, p, n);
		mat_product(A, B, C, m, p, n);
		print_mat(C, m, n);
		break;
	case 2:
		scanf("%d", &n);
		read_mat(A, n, n);
		printf("%.4f\n", gauss_simplified(A, n));
		break;
	case 3:
		scanf("%d", &n);
		read_mat(A, n, n);
		read_vector(b, n);
		det = gauss(A, b, x, n, eps);
		printf("%.4f\n", det);
		if (det) print_vector(x, n);
		break;
	case 4:
		scanf("%d", &n);
		read_mat(A, n, n);
		det = matrix_inv(A, B, n, eps);
		printf("%.4f\n", det);
		if (det) print_mat(B, n, n);
		break;
	default:
		printf("NOTHING TO DO FOR %d\n", to_do);
		break;
	}
	return 0;
}
