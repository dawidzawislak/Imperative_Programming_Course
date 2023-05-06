#include <stdio.h>
#include <math.h>

#define RECURS_LEVEL_MAX  10
#define N_MAX             10

// pointer to function of one variable
typedef double (*Func1vFp)(double);


// example functions of one variable
double f_poly(double x) 
{
	return 2 * pow(x, 5.0) - 4 * pow(x, 4.0) + 3.5 * pow(x, 2.0) + 1.35 * x - 6.25;
}

double f_rat(double x) 
{
	return 1.0 / (pow(x - 0.5, 2.0) + 0.01);
}

double f_exp(double x) 
{
	return 2.0 * x * exp(-1.5 * x) - 1.0;
}

double f_trig(double x) 
{
	return x * tan(x);
}

// Quadratures

// rectangle rule, leftpoint
double quad_rect_left(Func1vFp f1, double a, double b, int n) 
{
	double h = (b - a) / n;
	double res = 0.0;
	double x = a;

	for (int i = 0; i < n; i++) {
		res += h * f1(x);
		x += h;
	}

	return res;
}

// rectangle rule, rightpoint
double quad_rect_right(Func1vFp f1, double a, double b, int n) 
{
	double h = (b - a) / n;
	double res = 0.0;
	double x = b;

	for (int i = 0; i < n; i++) {
		res += h * f1(x);
		x -= h;
	}

	return res;
}

// rectangle rule, midpoint
double quad_rect_mid(Func1vFp f1, double a, double b, int n) 
{
	double h = (b - a) / n;
	double res = 0.0;
	double x = a + (h / 2.0);

	for (int i = 0; i < n; i++) {
		res += h * f1(x);
		x += h;
	}

	return res;
}

// trapezoidal rule
double quad_trap(Func1vFp func, double a, double b, int n) 
{
	double h = (b - a) / n;
	double res = 0.0;

	double f_prev = func(a);

	for (int i = 1; i <= n; i++) {
		double f_val = func(a + i * h);
		res += h / 2.0 * (f_prev + f_val);
		f_prev = f_val;
	}

	return res;
}

// Simpson's rule
double quad_simpson(Func1vFp f, double a, double b, int n) 
{
	double h = (b - a) / (2 * n);
	double res = 0.0;

	double f_prev = f(a);
	
	for (int i = 2; i < 2*n+1; i+=2) {
		double f_val = f(a + i * h);
		double c = (a + ((i - 2) * h) + a + (i * h)) / 2.0;
		res += h / 3.0 * (f_prev + 4*f(c) + f_val);
		f_prev = f_val;
	}

	return res;

}

// pointer to quadrature function
typedef double (*QuadratureFp)(Func1vFp, double, double, int);

// array of pointers to integrand functions
Func1vFp func_tab[] = { f_poly, f_rat, f_trig, f_exp };

// array of pointers to quadrature functions
QuadratureFp quad_tab[] = {
	quad_rect_left, quad_rect_right, quad_rect_mid, quad_trap, quad_simpson };

// calls 'quad_no' quadrature function for 'fun_no' integrand function
// on interval [a, b] and n subintervals
double quad_select(int fun_no, int quad_no, double a, double b, int n) 
{
	return quad_tab[quad_no](func_tab[fun_no], a, b, n);
}

// adaptive algorithm
double recurs(Func1vFp f, double a, double b, double S, double delta, QuadratureFp quad, int level) 
{
	if (level > RECURS_LEVEL_MAX) return NAN;
	double c = (b + a) / 2.0;
	double S1 = quad(f, a, c, 1);
	double S2 = quad(f, c, b, 1);
	double newS = S1 + S2;
	if (fabs(S - newS) <= delta) return newS;
	return recurs(f, a, c, S1, delta/2.0, quad, level + 1) + recurs(f, c, b, S2, delta / 2.0, quad, level + 1);
}

// initialization for adaptive algorithm
double init_recurs(Func1vFp f, double a, double b, double delta, QuadratureFp quad) 
{
	double S = quad(f, a, b, 1);
	return recurs(f, a, b, S, delta, quad, 1);
}

// double integrals

// pointer to function of two variables
typedef double(*Func2vFp)(double, double);

double func2v_2(double x, double y) {
	return 2 - x * x - y * y * y;
}

// sample functions to define the normal domain

double lower_bound2(double x) {
	return 0.7 * exp(-2 * x * x);
}
double upper_bound2(double x) {
	return sin(10 * x);
}

// rectangle rule (leftpoint) - double integral over rectangular domain
double dbl_integr(Func2vFp f, double x1, double x2, int nx, double y1, double y2, int ny) 
{
	double hx = (x2 - x1) / nx;
	double hy = (y2 - y1) / ny;
	double res = 0.0;

	double x = x1;

	for (int i = 0; i < nx; i++) {
		double y = y1;
		for (int j = 0; j < ny; j++) {
			res += f(x, y);
			y += hy;
		}
		x += hx;
	}

	return res * hx * hy;
}

// rectangle rule (midpoint) - double integral over normal domain with respect to the x-axis
double dbl_integr_normal_1(Func2vFp f, double x1, double x2, int nx, double hy,
	Func1vFp fg, Func1vFp fh) 
{
	double hx = (x2 - x1) / nx;
	double res = 0.0;
	double x = x1 + (hx / 2.0);

	for (int i = 0; i < nx; i++) {
		double y1 = fg(x);
		double y2 = fh(x);
		int ny = round((y2 - y1) / hy);
		double y = y1 + (hy / 2.0);
		for (int j = 0; j < ny; j++) {
			res += f(x, y);
			y += hy;
		}
		x += hx;
	}

	return res * hx * hy;
}

double min(double x, double y) {
	return (x <= y) ? x : y;
}
double max(double x, double y) {
	return (x >= y) ? x : y;
}

// rectangle rule (leftpoint) - double integral over multiple normal
// domains with respect to the x-axis
double dbl_integr_normal_n(Func2vFp f, double x1, double x2, int nx, double y1, double y2,
	int ny, Func1vFp fg, Func1vFp fh) 
{
	double hx = (x2 - x1) / nx;
	double hy = (y2 - y1) / ny;
	double res = 0.0;

	double x = x1;

	for (int i = 0; i < nx; i++) {
		double y1_ = max(y1, fg(x));
		double y2_ = min(y2, fh(x));
		if (y1_ <= y2_) {
			int ny = ceil((y2_ - y1_) / hy);
			double hy_ = (y2_ - y1_) / ny;
			double y = y1_;
			for (int j = 0; j < ny; j++) {
				res += f(x, y) * hy_;
				y += hy_;
			}
		}
		x += hx;
	}

	return res * hx;
}

// multiple quadratures

typedef double (*FuncNvFp)(const double*, int);
typedef int (*BoundNvFp)(const double*, int);

// sample function of three variables
double func3v(const double v[], int n) 
{
	return v[0] - v[1] + 2 * v[2];
}

// sample predicate in 3D
int bound3v(const double v[], int n) { // a cylinder
	return v[0] > 0 && v[0] < 0.5 && v[1] * v[1] + (v[2] - 1) * (v[2] - 1) < 1;
}

// sample function of n variables
double funcNv(const double v[], int n) 
{
	double fv = 1.;
	for (int i = 1; i < n; ++i) {
		fv += sin(i * v[i]);
	}
	return fv;
}
// sample n-dimensional predicate (n-dim hypersphere)
int boundNv(const double v[], int n) 
{
	double r = 0.0;
	for (int i = 0; i < n; ++i) r += (v[i] - 1) * (v[i] - 1);
	return r <= 1.;
}

// multiple integrals over a cuboid with predicate (if boundary != NULL)
// rectangular rule (rightpoint)
double trpl_quad_rect(FuncNvFp f, double variable_lim[][2], const int tn[], BoundNvFp boundary) 
{
	double x1 = variable_lim[0][0], x2 = variable_lim[0][1];
	double y1 = variable_lim[1][0], y2 = variable_lim[1][1];
	double z1 = variable_lim[2][0], z2 = variable_lim[2][1];
	int nx = tn[0], ny = tn[1], nz = tn[2];

	double hx = (x2 - x1) / nx;
	double hy = (y2 - y1) / ny;
	double hz = (z2 - z1) / nz;

	double res = 0.0;

	double x = x1+hx;

	for (int i = 0; i < nx; i++) {
		double y = y1+hy;
		for (int j = 0; j < ny; j++) {
			double z = z1+hz;
			for (int k = 0; k < nz; k++) {
				double v_tab[] = { x, y, z };
				
				if(boundary == NULL || boundary(v_tab, 3))
					res += f(v_tab, 3) * hz * hy * hx;

				z += hz;
			}
			y += hy;
		}
		x += hx;
	}
	return res;
}

double h_g[20];
// multiple integrals over a n-dim hypercuboid with predicate (if boundary != NULL)
// rectangular rule (midpoint)
void recur_quad_rect_mid(double* p_sum, FuncNvFp f, int variable_no, double t_variable[],
	double variable_lim[][2], const int tn[], int level, BoundNvFp boundary) 
{
	double h = (variable_lim[level][1] - variable_lim[level][0]) / tn[level];
	t_variable[level] = variable_lim[level][0] + (h/2.0);
	
	h_g[level] = h;

	for (int i = 0; i < tn[level]; i++)
	{
		if (level < variable_no-1) recur_quad_rect_mid(p_sum, f, variable_no, t_variable, variable_lim, tn,level+1, boundary);
		
		else if (level == variable_no - 1 && (boundary == NULL || boundary(t_variable, variable_no))) {
			double to_add = f(t_variable, variable_no);
			for (int j = 0; j <= level; j++)
				to_add *= h_g[j];
			(*p_sum) += to_add;
		}

		t_variable[level] += h;
	}
}

int main(void) {
	int to_do, n, nx, ny, integrand_fun_no, method_no, flag;
	int no_funcs = sizeof(func_tab) / sizeof(Func1vFp);
	int no_quads = sizeof(quad_tab) / sizeof(QuadratureFp);
	double a, b, x1, x2, y1, y2, hy, sum, delta;
	double t_variable[N_MAX], variable_lim[N_MAX][2];
	int tn[N_MAX];

	scanf("%d", &to_do);
	switch (to_do) {
	case 1: // loop over quadratures and integrands
		scanf("%lf %lf %d", &a, &b, &n);
		for (int q = 0; q < no_quads; ++q) {
			for (int f = 0; f < no_funcs; ++f) {
				printf("%.5f ", quad_select(f, q, a, b, n));
			}
			printf("\n");
		}
		break;
	case 2: // adaptive algorithm
		scanf("%d %d", &integrand_fun_no, &method_no);
		scanf("%lf %lf %lf", &a, &b, &delta);
		printf("%.5f\n", init_recurs(func_tab[integrand_fun_no], a, b, delta, quad_tab[method_no]));
		break;
	case 3: // double integral over a rectangle
		scanf("%lf %lf %d", &x1, &x2, &nx);
		scanf("%lf %lf %d", &y1, &y2, &ny);
		printf("%.5f\n", dbl_integr(func2v_2, x1, x2, nx, y1, y2, ny));
		break;
	case 4: // double integral over normal domain
		scanf("%lf %lf %d", &x1, &x2, &nx);
		scanf("%lf", &hy);
		printf("%.5f\n", dbl_integr_normal_1(func2v_2, x1, x2, nx, hy, lower_bound2, upper_bound2));
		break;
	case 5: // double integral over multiple normal domains
		scanf("%lf %lf %d", &x1, &x2, &nx);
		scanf("%lf %lf %d", &y1, &y2, &ny);
		printf("%.5f\n", dbl_integr_normal_n(func2v_2, x1, x2, nx, y1, y2, ny, lower_bound2, upper_bound2));
		break;
	case 6: // triple integral over a cuboid
		scanf("%lf %lf %d", &variable_lim[0][0], &variable_lim[0][1], tn);
		scanf("%lf %lf %d", &variable_lim[1][0], &variable_lim[1][1], tn + 1);
		scanf("%lf %lf %d", &variable_lim[2][0], &variable_lim[2][1], tn + 2);
		scanf("%d", &flag);
		printf("%.5f\n", trpl_quad_rect(func3v, variable_lim, tn, flag ? bound3v : NULL));

		break;
	case 7: // multiple integral over hyper-cuboid
		scanf("%d", &n);
		if (n > N_MAX) break;
		for (int i = 0; i < n; ++i) {
			scanf("%lf %lf %d", &variable_lim[i][0], &variable_lim[i][1], tn + i);
		}
		scanf("%d", &flag);
		sum = 0.;
		recur_quad_rect_mid(&sum, funcNv, n, t_variable, variable_lim, tn, 0, flag ? boundNv : NULL);
		printf("%.5f\n", sum);
		break;

	default:
		printf("Nothing to do for %d\n", to_do);
		break;
	}
	return 0;
}
