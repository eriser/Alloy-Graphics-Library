/*
 * Copyright(C) 2014, Blake C. Lucas, Ph.D. (img.science@gmail.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "AlloyMath.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
using namespace std;
namespace aly {
static double pythag(double a, double b) {
	double at = std::abs(a), bt = std::abs(b), ct, result;
	if (at > bt) {
		ct = bt / at;
		result = at * std::sqrt(1.0 + ct * ct);
	} else if (bt > 0.0) {
		ct = at / bt;
		result = bt * std::sqrt(1.0 + ct * ct);
	} else
		result = 0.0;
	return (result);
}

/*
 * SVD algorithm adapted from Numerical Recipes, originally written by Luke Tierney and David Betz.
 */

template<class T, int m, int n> void svd(const matrix<T, m, n>& M,
		matrix<T, m, m>& U, matrix<T, m, n>& D, matrix<T, n, n>& Vt) {
	double v[n][n];
	double u[m][m];
	double w[n];
	double rv1[n];
	int flag, i, its, j, jj, k, l, nm;
	double c, f, h, s, x, y, z;
	double anorm = 0.0, g = 0.0, scale = 0.0;
	if (m < n) {
		throw std::runtime_error("SVD error, rows must be greater than cols.");
	}
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			u[i][j] = M(i, j);
		}
	}
	for (i = 0; i < n; i++) {
		l = i + 1;
		rv1[i] = scale * g;
		g = s = scale = 0.0;
		if (i < m) {
			for (k = i; k < m; k++)
				scale += std::abs((double) u[k][i]);
			if (scale) {
				for (k = i; k < m; k++) {
					u[k][i] = (T) ((double) u[k][i] / scale);
					s += ((double) u[k][i] * (double) u[k][i]);
				}
				f = (double) u[i][i];
				g = -sign(std::sqrt(s), f);
				h = f * g - s;
				u[i][i] = (T) (f - g);
				if (i != n - 1) {
					for (j = l; j < n; j++) {
						for (s = 0.0, k = i; k < m; k++)
							s += ((double) u[k][i] * (double) u[k][j]);
						f = s / h;
						for (k = i; k < m; k++)
							u[k][j] += (T) (f * (double) u[k][i]);
					}
				}
				for (k = i; k < m; k++)
					u[k][i] = (T) ((double) u[k][i] * scale);
			}
		}
		w[i] = (T) (scale * g);
		g = s = scale = 0.0;
		if (i < m && i != n - 1) {
			for (k = l; k < n; k++)
				scale += std::abs((double) u[i][k]);
			if (scale) {
				for (k = l; k < n; k++) {
					u[i][k] = (T) ((double) u[i][k] / scale);
					s += ((double) u[i][k] * (double) u[i][k]);
				}
				f = (double) u[i][l];
				g = -sign(std::sqrt(s), f);
				h = f * g - s;
				u[i][l] = (T) (f - g);
				for (k = l; k < n; k++)
					rv1[k] = (double) u[i][k] / h;
				if (i != m - 1) {
					for (j = l; j < m; j++) {
						for (s = 0.0, k = l; k < n; k++)
							s += ((double) u[j][k] * (double) u[i][k]);
						for (k = l; k < n; k++)
							u[j][k] += (T) (s * rv1[k]);
					}
				}
				for (k = l; k < n; k++)
					u[i][k] = (T) ((double) u[i][k] * scale);
			}
		}
		anorm = std::max(anorm, (std::abs((double) w[i]) + std::abs(rv1[i])));
	}
	for (i = n - 1; i >= 0; i--) {
		if (i < n - 1) {
			if (g) {
				for (j = l; j < n; j++)
					v[j][i] = (T) (((double) u[i][j] / (double) u[i][l]) / g);
				for (j = l; j < n; j++) {
					for (s = 0.0, k = l; k < n; k++)
						s += ((double) u[i][k] * (double) v[k][j]);
					for (k = l; k < n; k++)
						v[k][j] += (T) (s * (double) v[k][i]);
				}
			}
			for (j = l; j < n; j++)
				v[i][j] = v[j][i] = 0.0;
		}
		v[i][i] = 1.0;
		g = rv1[i];
		l = i;
	}
	for (i = n - 1; i >= 0; i--) {
		l = i + 1;
		g = (double) w[i];
		if (i < n - 1)
			for (j = l; j < n; j++)
				u[i][j] = 0.0;
		if (g) {
			g = 1.0 / g;
			if (i != n - 1) {
				for (j = l; j < n; j++) {
					for (s = 0.0, k = l; k < m; k++)
						s += ((double) u[k][i] * (double) u[k][j]);
					f = (s / (double) u[i][i]) * g;
					for (k = i; k < m; k++)
						u[k][j] += (T) (f * (double) u[k][i]);
				}
			}
			for (j = i; j < m; j++)
				u[j][i] = (T) ((double) u[j][i] * g);
		} else {
			for (j = i; j < m; j++)
				u[j][i] = 0.0;
		}
		++u[i][i];
	}
	for (k = n - 1; k >= 0; k--) {
		for (its = 0; its < 30; its++) {
			flag = 1;
			for (l = k; l >= 0; l--) {
				nm = l - 1;
				if (std::abs(rv1[l]) + anorm == anorm) {
					flag = 0;
					break;
				}
				if (std::abs((double) w[nm]) + anorm == anorm)
					break;
			}
			if (flag) {
				c = 0.0;
				s = 1.0;
				for (i = l; i <= k; i++) {
					f = s * rv1[i];
					if (std::abs(f) + anorm != anorm) {
						g = (double) w[i];
						h = pythag(f, g);
						w[i] = (T) h;
						h = 1.0 / h;
						c = g * h;
						s = (-f * h);
						for (j = 0; j < m; j++) {
							y = (double) u[j][nm];
							z = (double) u[j][i];
							u[j][nm] = (T) (y * c + z * s);
							u[j][i] = (T) (z * c - y * s);
						}
					}
				}
			}
			z = (double) w[k];
			if (l == k) {
				if (z < 0.0) {
					w[k] = (T) (-z);
					for (j = 0; j < n; j++)
						v[j][k] = (-v[j][k]);
				}
				int iii, jjj;
				for (iii = k; (iii < n - 1) && (w[iii] < w[iii + 1]); iii++) {
					std::swap(w[iii], w[iii + 1]);
					for (jjj = 0; jjj < m; jjj++)
						std::swap(u[jjj][iii], u[jjj][iii + 1]);
					for (jjj = 0; jjj < n; jjj++)
						std::swap(v[jjj][iii], v[jjj][iii + 1]);
				}
				break;
			}
			if (its >= 30) {
				throw runtime_error("SVD did not converge.");
			}
			x = (double) w[l];
			nm = k - 1;
			y = (double) w[nm];
			g = rv1[nm];
			h = rv1[k];
			f = ((y - z) * (y + z) + (g - h) * (g + h)) / (2.0 * h * y);
			g = pythag(f, 1.0);
			f = ((x - z) * (x + z) + h * ((y / (f + sign(g, f))) - h)) / x;
			c = s = 1.0;
			for (j = l; j <= nm; j++) {
				i = j + 1;
				g = rv1[i];
				y = (double) w[i];
				h = s * g;
				g = c * g;
				z = pythag(f, h);
				rv1[j] = z;
				c = f / z;
				s = h / z;
				f = x * c + g * s;
				g = g * c - x * s;
				h = y * s;
				y = y * c;
				for (jj = 0; jj < n; jj++) {
					x = (double) v[jj][j];
					z = (double) v[jj][i];
					v[jj][j] = (T) (x * c + z * s);
					v[jj][i] = (T) (z * c - x * s);
				}
				z = pythag(f, h);
				w[j] = (T) z;
				if (z) {
					z = 1.0 / z;
					c = f * z;
					s = h * z;
				}
				f = (c * g) + (s * y);
				x = (c * y) - (s * g);
				for (jj = 0; jj < m; jj++) {
					y = (double) u[jj][j];
					z = (double) u[jj][i];
					u[jj][j] = (T) (y * c + z * s);
					u[jj][i] = (T) (z * c - y * s);
				}
			}
			rv1[l] = 0.0;
			rv1[k] = f;
			w[k] = (T) x;
		}
	}
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			if (i == j) {
				D(i, j) = w[j];
			} else {
				D(i, j) = 0;
			}
		}
	}
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < m; j++) {
			U(i, j) = u[i][j];
		}
	}
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			Vt(i, j) = v[j][i];
		}
	}
}

void SVD(const matrix<float, 3, 3> &A, matrix<float, 3, 3>& U,
		matrix<float, 3, 3>& D, matrix<float, 3, 3>& Vt) {
	svd(A, U, D, Vt);
}
void SVD(const matrix<float, 4, 4> &A, matrix<float, 4, 4>& U,
		matrix<float, 4, 4>& D, matrix<float, 4, 4>& Vt) {
	svd(A, U, D, Vt);
}

}
