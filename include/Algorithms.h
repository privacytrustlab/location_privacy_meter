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
#ifndef LPM_ALGORITHMS_H
#define LPM_ALGORITHMS_H

//!
//! \file
//!
#include "Defs.h"
#include "Private.h"

namespace lpm {

//!
//! \brief Implements useful algorithms used by the library
//!
//! Static class which provides methods for some common useful algorithms such as MaximumWeightAssignment() and MultiplySquareMatrices().
//!
class Algorithms 
{
  private:
    //[MinimumCostAssignment]: Step 1: Reduce the cost matrix by subtracting the row min from every row, and the column min from every column.
    //This works because the resulting assignment (on the reduce matrix) corresponds to the same assignment on the non-reduced matrix
    static void ReduceCostMatrix(ll* M, ll n);

    //[MinimumCostAssignment]: Step 2: Star some arbitrary zeros (making sure that no two zeros in the same row or column are starred).
    static ll StarArbitraryZeros(ll* M, ll* starred, ll n);

    //[MinimumCostAssignment]: Find any uncovered zeros
    static bool FindAnyUncoveredZero(ll* M, ll n, ll& row, ll& col);

    //[MinimumCostAssignment]: Cover all zeros by changing the adding/removing cover lines.
    static ll CoverAllZeros(ll* M, ll* starred, ll* primed, ll n, ll& covers);


  public:
    //This implementation is heavily inspired from the code of Dariush Lotfi (June 2008).
    //See http://csclab.murraystate.edu/bob.pilgrim/445/munkres.html for the algorithm itself.
    static void MinimumCostAssignment(ll* costMatrix, ll numItems, ll* assignment);

    static void MultiplySquareMatrices(const double* leftMatrix, const double* rightMatrix, ull dimension, double* resultMatrix);

    static bool GetSteadyStateVectorOfSubChain(double* fullChainSS, ull timePeriodId, double** subChainSS, bool inclDummyTPs = false);

    static bool GetTransitionVectorOfSubChain(double* fullChainTransitionMatrix, ull tp1, ull loc1, ull tp2, double** transitionVector, bool inclDummyTPs = false);

};

} // namespace lpm
#endif
