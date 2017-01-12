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
#ifndef LPM_RNG_H
#define LPM_RNG_H

//!
//! \file
//! The implementation of some of the methods below is inspired by the GNU Scientific Library (GSL)
//! See: http://www.gnu.org/software/gsl/.
//!
#include "Singleton.h"

#include "Defs.h"
namespace lpm {

//!
//! \brief Implements random number generating facilities
//!
//! Singleton class which provides convenient methods to generate random numbers (integers and real numbers) uniformly or
//! according to a distribution (e..g Normal, Gamma), and, to sample from a distribution (e.g. Dirichlet).
//!
class RNG : public Singleton<RNG> 
{
friend class Singleton<RNG>;
  private:
    RNG();

    inline uint64 RandomUINT64() const;


  public:
    //! 
    //! \brief Returns a uniform random double in ]0; 1[
    //!
    //! \return double, the number generated
    //!
    double GetUniformRandomDouble() const;


  private:
    inline ull GetUniformRandomULL() const;


  public:
    //! 
    //! \brief Returns a uniform random unsigned long long (\a ull) between \a min and \a max (inclusive)
    //!
    //! \param[in] min 	ull, the lower bound.
    //! \param[in] max 	ull, the upper bound.
    //!
    //! \return ull, the number generated
    //!
    ull GetUniformRandomULLBetween(ull min, ull max) const;

    //! 
    //! \brief Returns a random permutation of the given \a input array
    //!
    //! \param[in] input 	ull*, the input array (an array of \a count \a ulls).
    //! \param[in] count 	ull, the number of elements in the \a input array
    //! \param[in,out] output ull*, the output array (an array of \a count \a ull numbers) which will be filled with the output permutation (if the call is successful).
    //!
    //! \return true or false, depending on whether the call is successful
    //!
    bool RandomPermutation(const ull* input, const ull count, ull* output) const;

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
    ull SampleIndexFromVector(double* probVector, ull length) const;


  private:
    inline double GetGaussianRandomDouble(double sigma) const;

    inline double GetGammaRandomDouble(double a, double b) const;

    inline void GetDirichletRandomSampleSmall(const double* alpha, ull K, double* theta) const;


  public:
    void GetDirichletRandomSample(const double* alpha, ull K, double* theta) const;

    void GetDirichletExpectedValue(double* alpha, ull K, double* theta) const;

};

} // namespace lpm
#endif
