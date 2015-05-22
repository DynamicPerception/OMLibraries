// 
// 
// 

#include "hermite_spline.h"

/*


Motion Engine

See dynamicperception.com for more information


(c) 2008-2012 C.A. Church / Dynamic Perception LLC

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.


*/

/***********************************************

Hermite Cubic Spline Evaluation Functions

************************************************/

/******************************************************************************/

void HermiteSpline::r8vec_bracket3(int n, float t[], float tval, int *left)

/******************************************************************************/
/*
Purpose:

R8VEC_BRACKET3 finds the interval containing or nearest a given value.

Discussion:

An R8VEC is a vector of R8's.

The routine always returns the index LEFT of the sorted array
T with the property that either
*  T is contained in the interval [ T[LEFT], T[LEFT+1] ], or
*  T < T[LEFT] = T[0], or
*  T > T[LEFT+1] = T[N-1].

The routine is useful for interpolation problems, where
the abscissa must be located within an interval of data
abscissas for interpolation, or the "nearest" interval
to the (extreme) abscissa must be found so that extrapolation
can be carried out.

This version of the function has been revised so that the value of
LEFT that is returned uses the 0-based indexing natural to C++.

Licensing:

This code is distributed under the GNU LGPL license.

Modified:

31 May 2009

Author:

John Burkardt

Parameters:

Input, int N, length of the input array.

Input, float T[N], an array that has been sorted into ascending order.

Input, float TVAL, a value to be bracketed by entries of T.

Input/output, int *LEFT.
On input, if 0 <= LEFT <= N-2, LEFT is taken as a suggestion for the
interval [ T[LEFT-1] T[LEFT] ] in which TVAL lies.  This interval
is searched first, followed by the appropriate interval to the left
or right.  After that, a binary search is used.
On output, LEFT is set so that the interval [ T[LEFT], T[LEFT+1] ]
is the closest to TVAL; it either contains TVAL, or else TVAL
lies outside the interval [ T[0], T[N-1] ].
*/
{
	int high;
	int low;
	int mid;
	/*
	Check the input data.
	*/
	if (n < 2)
	{
		fprintf(stderr, "\n");
		fprintf(stderr, "R8VEC_BRACKET3 - Fatal error\n");
		fprintf(stderr, "  N must be at least 2.\n");
		exit(1);
	}
	/*
	If *LEFT is not between 0 and N-2, set it to the middle value.
	*/
	if (*left < 0 || n - 2 < *left)
	{
		*left = (n - 1) / 2;
	}
	/*
	CASE 1: TVAL < T[*LEFT]:
	Search for TVAL in (T[I],T[I+1]), for I = 0 to *LEFT-1.
	*/
	if (tval < t[*left])
	{
		if (*left == 0)
		{
			return;
		}
		else if (*left == 1)
		{
			*left = 0;
			return;
		}
		else if (t[*left - 1] <= tval)
		{
			*left = *left - 1;
			return;
		}
		else if (tval <= t[1])
		{
			*left = 0;
			return;
		}
		/*
		...Binary search for TVAL in (T[I],T[I+1]), for I = 1 to *LEFT-2.
		*/
		low = 1;
		high = *left - 2;

		for (;;)
		{
			if (low == high)
			{
				*left = low;
				return;
			}

			mid = (low + high + 1) / 2;

			if (t[mid] <= tval)
			{
				low = mid;
			}
			else
			{
				high = mid - 1;
			}
		}
	}
	/*
	CASE 2: T[*LEFT+1] < TVAL:
	Search for TVAL in (T[I],T[I+1]) for intervals I = *LEFT+1 to N-2.
	*/
	else if (t[*left + 1] < tval)
	{
		if (*left == n - 2)
		{
			return;
		}
		else if (*left == n - 3)
		{
			*left = *left + 1;
			return;
		}
		else if (tval <= t[*left + 2])
		{
			*left = *left + 1;
			return;
		}
		else if (t[n - 2] <= tval)
		{
			*left = n - 2;
			return;
		}
		/*
		...Binary search for TVAL in (T[I],T[I+1]) for intervals I = *LEFT+2 to N-3.
		*/
		low = *left + 2;
		high = n - 3;

		for (;;)
		{

			if (low == high)
			{
				*left = low;
				return;
			}

			mid = (low + high + 1) / 2;

			if (t[mid] <= tval)
			{
				low = mid;
			}
			else
			{
				high = mid - 1;
			}
		}
	}
	/*
	CASE 3: T[*LEFT] <= TVAL <= T[*LEFT+1]:
	T is just where the user said it might be.
	*/
	else
	{
	}

	return;
}

/******************************************************************************/

void HermiteSpline::cubic_value(float x1, float f1, float d1, float x2,
	float f2, float d2, int n, float x[], float f[], float d[],
	float s[])

	/******************************************************************************/
	/*
	Purpose:

	HERMITE_CUBIC_VALUE evaluates a Hermite cubic polynomial.

	Discussion:

	The input arguments can be vectors.

	Licensing:

	This code is distributed under the GNU LGPL license.

	Modified:

	12 February 2012

	Author:

	John Burkardt

	Reference:

	Fred Fritsch, Ralph Carlson,
	Monotone Piecewise Cubic Interpolation,
	SIAM Journal on Numerical Analysis,
	Volume 17, Number 2, April 1980, pages 238-246.

	Parameters:

	Input, float X1, F1, D1, the left endpoint, function value
	and derivative.

	Input, float X2, F2, D2, the right endpoint, function value
	and derivative.

	Input, int N, the number of evaluation points.

	Input, float X[N], the points at which the Hermite cubic
	is to be evaluated.

	Output, float F[N], D[N], S[N], T[N], the value and first
	three derivatives of the Hermite cubic at X.
	*/
{
	float c2;
	float c3;
	float df;
	float h;
	int i;

	h = x2 - x1;
	df = (f2 - f1) / h;

	c2 = -(2.0 * d1 - 3.0 * df + d2) / h;
	c3 = (d1 - 2.0 * df + d2) / h / h;

	for (i = 0; i < n; i++)
	{
		f[i] = f1 + (x[i] - x1) * (d1
			+ (x[i] - x1) * (c2
			+ (x[i] - x1) *   c3));
		d[i] = d1 + (x[i] - x1) * (2.0 * c2
			+ (x[i] - x1) * 3.0 * c3);
		s[i] = 2.0 * c2 + (x[i] - x1) * 6.0 * c3;
	}
	return;
}



/******************************************************************************/

void HermiteSpline::cubic_spline_value(int nn, float xn[], float fn[],
	float dn[], int n, float x[], float f[], float d[], float s[])

	/******************************************************************************/
	/*
	Purpose:

	HERMITE_CUBIC_SPLINE_VALUE evaluates a Hermite cubic spline.

	Licensing:

	This code is distributed under the GNU LGPL license.

	Modified:

	12 February 2012

	Author:

	John Burkardt

	Reference:

	Fred Fritsch, Ralph Carlson,
	Monotone Piecewise Cubic Interpolation,
	SIAM Journal on Numerical Analysis,
	Volume 17, Number 2, April 1980, pages 238-246.

	Parameters:

	Input, int NN, the number of data points.

	Input, float XN[NN], the coordinates of the data points.
	The entries in XN must be in strictly ascending order.

	Input, float FN[NN], the function values.

	Input, float DN[NN], the derivative values.

	Input, int N, the number of sample points.

	Input, float X[N], the coordinates of the sample points.

	Output, float F[N], the function value at the sample points.

	Output, float D[N], the derivative value at the sample points.

	Output, float S[N], the second derivative value at the
	sample points.

	Output, float T[N], the third derivative value at the
	sample points.
	*/
{
	int i;
	int left;

	left = n / 2;

	for (i = 0; i < n; i++)
	{
		r8vec_bracket3(nn, xn, x[i], &left);

		cubic_value(xn[left], fn[left], dn[left], xn[left + 1],
			fn[left + 1], dn[left + 1], 1, x + i, f + i, d + i, s + i);
	}
	return;
}
