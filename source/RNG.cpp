/* -------------------------------------------------------------------------------------
 * Copyright 2012 EPFL-I&C-LCA
 *
 * Designed and Developed by: Vincent Bindschaedler and Reza Shokri
 *
 * Authors: Reza Shokri, George Theodorakopoulos, Vincent Bindschaedler, George Danezis, 
 *          Jean-Pierre Hubaux, Jean-Yves Le Boudec
 * 
 * Contact: reza.shokri@epfl.ch
 *
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this 
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, 
 *    this list of conditions and the following disclaimer in the documentation 
 *    and/or other materials provided with the distribution.
 * 3. The name of the authors/developers may not be used to endorse or promote 
 *    products derived from this software without specific prior written permission.
 * 4. All advertising materials and/or any publications mentioning features or use of 
 *    this software must acknowledge it by citing the followings:
 *    [a] Location-Privacy Meter: A Tool to Quantify Location Privacy. 
 *        Available at http://lca.epfl.ch/projects/quantifyingprivacy/
 *    [b] Reza Shokri, George Theodorakopoulos, Jean-Yves Le Boudec, and Jean-Pierre 
 *        Hubaux. Quantifying Location Privacy. In IEEE Symposium on Security and 
 *        Privacy (S&P), Oakland, CA, USA, May 22-25, 2011.
 *    [c] Reza Shokri, George Theodorakopoulos, George Danezis, Jean-Pierre Hubaux, 
 *        and Jean-Yves Le Boudec. Quantifying Location Privacy: The Case of Sporadic
 *        Location Exposure. In The 11th Privacy Enhancing Technologies Symposium 
 *        (PETS), Waterloo, Canada, July 27-29, 2011.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * -------------------------------------------------------------------------------------
 */
//!
//! \file
//! The implementation of some of the methods below is inspired by the GNU Scientific Library (GSL)
//! See: http://www.gnu.org/software/gsl/.
//!
#include "../include/RNG.h"

namespace lpm {

RNG::RNG() 
{
  // Bouml preserved body begin 00039691

	srand(time(NULL));

  // Bouml preserved body end 00039691
}

uint64 RNG::RandomUINT64() const 
{
  // Bouml preserved body begin 00039791

	return (uint64)(((uint64)-1) * GetUniformRandomDouble());

  // Bouml preserved body end 00039791
}

//! 
//! \brief Returns a uniform random double in ]0; 1[
//!
//! \return double, the number generated
//!
double RNG::GetUniformRandomDouble() const 
{
  // Bouml preserved body begin 00039611

	double r = 0.0;

	do
	{
		r = (double)rand() * (double)(1.0 / (double)(RAND_MAX));
	}
	while(r <= 0.0 || r >= 1.0); // rejection sampling to make the bounds exclusive: i.e. r \in ]0, 1[

	return r;

  // Bouml preserved body end 00039611
}

ull RNG::GetUniformRandomULL() const 
{
  // Bouml preserved body begin 0003D091

	return (ull)RandomUINT64();

  // Bouml preserved body end 0003D091
}

//! 
//! \brief Returns a uniform random unsigned long long (\a ull) between \a min and \a max (inclusive)
//!
//! \param[in] min 	ull, the lower bound.
//! \param[in] max 	ull, the upper bound.
//!
//! \return ull, the number generated
//!
ull RNG::GetUniformRandomULLBetween(ull min, ull max) const 
{
  // Bouml preserved body begin 0007B011

	return (ull)(min + floor((double)((max - min + 1) * GetUniformRandomDouble())));

  // Bouml preserved body end 0007B011
}

//! 
//! \brief Returns a random permutation of the given \a input array
//!
//! \param[in] input 	ull*, the input array (an array of \a count \a ulls).
//! \param[in] count 	ull, the number of elements in the \a input array
//! \param[in,out] output ull*, the output array (an array of \a count \a ull numbers) which will be filled with the output permutation (if the call is successful).
//!
//! \return true or false, depending on whether the call is successful
//!
bool RNG::RandomPermutation(const ull* input, const ull count, ull* output) const 
{
  // Bouml preserved body begin 00072891

	if(input == NULL || count == 0 || output == NULL) { return false; }

	memcpy(output, input, count * sizeof(ull));

	if(count == 1) { return true; }

	for(ull i = 0; i < count - 1; i++)
	{
		size_t idx = GetUniformRandomULLBetween(i, count - 1);

		// VERIFY(idx >= i && idx <= (count - 1));

		// swap
		ull tmp = output[idx];
		output[idx] = output[i];
		output[i] = tmp;
	}

	return true;

  // Bouml preserved body end 00072891
}

//! 
//! \brief Samples (returns an index) from the given probability vector
//!
//! Samples an index in the range \[\a 0; \a length - 1\] according to the probabilities given by \a probVector.
//!
//! \note Naturally, this only make sense if the elements of \a probVector sum up to 1.
//!
//! \param[in] probVector 	ull*, the probability vector (an array of \a length \a doubles).
//! \param[in] length 	ull, the number of elements in the probability vector
//!
//! \return ull, the sampled index
//!
ull RNG::SampleIndexFromVector(double* probVector, ull length) const 
{
  // Bouml preserved body begin 0007FE91

	DEBUG_VERIFY(probVector != NULL && length != 0);

	double r = GetUniformRandomDouble();
	double sum = 0.0;

	for(ull i = 0; i < length; i++)
	{
		double v = probVector[i];
		sum += v;

		if(r <= sum) { return i; }
	}

	CODING_ERROR;

	return -1;

  // Bouml preserved body end 0007FE91
}

double RNG::GetGaussianRandomDouble(double sigma) const 
{
  // Bouml preserved body begin 0007FF91

	double x = 0.0;
	double y = 0.0;

	double norm = 0.0;

	do
	{
		/* choose x,y in uniform square (-1,-1) to (+1,+1) */
		x = -1.0 + 2.0 * GetUniformRandomDouble();
		y = -1.0 + 2.0 * GetUniformRandomDouble();

		/* see if it is in the unit circle */
		norm = x * x + y * y;
	}
	while(norm > 1.0 || norm <= 0);

	/* Box-Muller transform */
	return sigma * y * sqrt(-2.0 * log(norm) / norm);

  // Bouml preserved body end 0007FF91
}

double RNG::GetGammaRandomDouble(double a, double b) const 
{
  // Bouml preserved body begin 00080011

	//VERIFY(a > 0);
	if(a == 0.0) { return 0.0; }

	double u = 0.0;

	if (a < 1.0)
	{
		u = GetUniformRandomDouble();
		double e = 1.0 / a;
		double p = pow(u, e);
		if(p < SQRT_DBL_MIN) { p = SQRT_DBL_MIN; }

		double rec = GetGammaRandomDouble(1.0 + a, b);
		return (double)(rec * p);
	}

	double x = 0.0;
	double v = 0.0;
	double d = a - 1.0 / 3.0;
	double c = (1.0 / 3.0) / sqrt(d);

	while(true)
	{
		do
		{
			x = GetGaussianRandomDouble(1.0);
			v = 1.0 + c * x;
		}
		while (v <= 0.0);

		v = v * v * v;
		u = GetUniformRandomDouble();

		if(u < (1.0 - 0.0331 * x * x * x * x)) { break; }

		if(log(u) < (0.5 * x * x + d * (1.0 - v + log(v)))) { break; }
	}

	return b * d * v;

  // Bouml preserved body end 00080011
}

void RNG::GetDirichletRandomSampleSmall(const double* alpha, ull K, double* theta) const 
{
  // Bouml preserved body begin 00080091

	/* When the values of alpha[] are small, scale the variates to avoid
		underflow so that the result is not 0/0.  Note that the Dirichlet
		distribution is defined by a ratio of gamma functions so we can
		take out an arbitrary factor to keep the values in the range of
		double precision. */

	DEBUG_VERIFY(alpha != NULL && K != 0 && theta != NULL);

	size_t i = 0;
	double norm = 0.0;
	double umax = 0;

	for (i = 0; i < K; i++)
	{
		double u = log(GetUniformRandomDouble()) / alpha[i];

		theta[i] = u;

		if (u > umax || i == 0) { umax = u; }
	}

	for (i = 0; i < K; i++)
	{
		double e = theta[i] - umax;
		double r = exp(e);
		bool err = (errno == ERANGE) || (errno == -ERANGE);
		if(err == true) { r = SQRT_DBL_MIN; }

		theta[i] = (double)r;

	}

	for (i = 0; i < K; i++)
	{
		// FIX 02.04.2012: if alpha[i] is 0.0, then theta[i] should be 0.0
		if(alpha[i] == 0.0) { theta[i] = 0.0; }
		else { theta[i] = theta[i] * GetGammaRandomDouble(alpha[i] + 1.0, 1.0); }
	}

	for (i = 0; i < K; i++)
	{
		norm += theta[i];
	}

	for (i = 0; i < K; i++)
	{
		theta[i] /= norm;
	}

  // Bouml preserved body end 00080091
}

void RNG::GetDirichletRandomSample(const double* alpha, ull K, double* theta) const 
{
  // Bouml preserved body begin 00080111

	// TODO: check all of this -- the theory also
	DEBUG_VERIFY(alpha != NULL && K != 0 && theta != NULL);

	memset(theta, 0, K * sizeof(double));

	ull i = 0;
	double norm = 0.0;

	bool underflow = false;

	for (i = 0; i < K; i++)
	{
		double g = GetGammaRandomDouble(alpha[i], 1.0);
		if((g == 0) && (alpha[i] > 0)) { underflow = true; } // underflow

		theta[i] = g;
	}

	for (i = 0; i < K; i++)
	{
		norm += theta[i];
	}

	if(norm < SQRT_DBL_MIN || underflow == true)  /* Handle underflow */
	{
		GetDirichletRandomSampleSmall(alpha, K, theta);

		return;
	}

	bool renormalize = false;
	for (i = 0; i < K; i++)
	{
		double t = theta[i];
		theta[i] /= norm;
		if(t > 0 && theta[i] == 0) { theta[i] = SQRT_DBL_MIN; renormalize = true;}  // prevent underflow
	}

	if(renormalize == true)
	{
		norm = 0.0;
		for (i = 0; i < K; i++)
		{
			norm += theta[i];
		}

		for (i = 0; i < K; i++)
		{
			theta[i] /= norm;
		}
	}

  // Bouml preserved body end 00080111
}

void RNG::GetDirichletExpectedValue(double* alpha, ull K, double* theta) const 
{
  // Bouml preserved body begin 000BE991

	DEBUG_VERIFY(alpha != NULL && K != 0 && theta != NULL);

	memset(theta, 0, K * sizeof(double));

	double sum = 0.0;

	for(ull i=0; i < K; i++) { sum += alpha[i]; }

	for(ull i=0; i < K; i++) { theta[i] = alpha[i] / sum; }

  // Bouml preserved body end 000BE991
}


} // namespace lpm
