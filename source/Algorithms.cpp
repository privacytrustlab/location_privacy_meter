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
//!
#include "../include/Algorithms.h"

namespace lpm {

//[MinimumCostAssignment]: Step 1: Reduce the cost matrix by subtracting the row min from every row, and the column min from every column.
//This works because the resulting assignment (on the reduce matrix) corresponds to the same assignment on the non-reduced matrix
void Algorithms::ReduceCostMatrix(ll* M, ll n)
{
  // Bouml preserved body begin 000C5091

	for(ll i=0;i<n;i++)
	{
		ll min = LONG_LONG_MAX;
		for(ll j=0;j<n;j++) {  if (M[GET_INDEX(i, j, n+1)] < min) { min = M[GET_INDEX(i, j, n+1)]; } }
		for(ll j=0;j<n;j++) {  M[GET_INDEX(i, j, n+1)] -= min; }
	}
	for(ll j=0;j<n;j++)
	{
		ll min = LONG_LONG_MAX;
		for(ll i=0;i<n;i++) {  if (M[GET_INDEX(i, j, n+1)] < min) { min = M[GET_INDEX(i, j, n+1)]; } }
		for(ll i=0;i<n;i++) {  M[GET_INDEX(i, j, n+1)] -=  min; }
	}

  // Bouml preserved body end 000C5091
}

//[MinimumCostAssignment]: Step 2: Star some arbitrary zeros (making sure that no two zeros in the same row or column are starred).
ll Algorithms::StarArbitraryZeros(ll* M, ll* starred, ll n)
{
  // Bouml preserved body begin 000C5111

	ll starredCount = 0;

	for(ll i=0;i<n;i++)
	{
		for(ll j=0;j<n;j++)
		{
			if(starred[i] == -1 && M[GET_INDEX(n, j, n+1)] == 0 && M[GET_INDEX(i, j, n+1)] == 0)
			{
				M[GET_INDEX(n, j, n+1)] = 1;
				starred[i] = j;
				starredCount++;
			}
		}
	}

	return starredCount;

  // Bouml preserved body end 000C5111
}

//[MinimumCostAssignment]: Find any uncovered zeros
bool Algorithms::FindAnyUncoveredZero(ll* M, ll n, ll& row, ll& col)
{
  // Bouml preserved body begin 000C5191

	for(ll i=0;i<n;i++)
	{
		for(ll j=0;j<n;j++)
		{
			if(M[GET_INDEX(i, n, n+1)] == 0 && M[GET_INDEX(n, j, n+1)] == 0 && M[GET_INDEX(i, j, n+1)] == 0)
			{
				row = i;
				col = j;
				return true; // found
			}
		}
	}

	return false;

  // Bouml preserved body end 000C5191
}

//[MinimumCostAssignment]: Cover all zeros by changing the adding/removing cover lines.
ll Algorithms::CoverAllZeros(ll* M, ll* starred, ll* primed, ll n, ll& covers)
{
  // Bouml preserved body begin 000C5211

	ll row = 0;
	ll col = 0;

	//Repeat until there are no uncovered zeros left
	while(FindAnyUncoveredZero(M, n, row, col) == true)
	{
		if(starred[row] != -1) // If there is a starred zero in the same row of uncovered zero
		{
			primed[row] = col; // Priming the uncovered zero
			M[GET_INDEX(row, n, n+1)] = 1; // Covering the row of uncovered zero
			M[GET_INDEX(n, starred[row], n+1)] = 0; // Uncovering the column containing the starred zero
		}
		else // If there is no starred zero in the same row of uncovered zero
		{
			// According to the algorithm uncovered zero should be primed first, but
			// because it will be starred later, I don't prime it.

			ll i = 0;
			// Looking for a starred zero in the same column of uncovered (primed) zero
			for(i=0;i<n;i++) { if(starred[i] == col) { break; } }

			// Repeat until there is no starred zero in the same column as primed zero
			while(i != n)
			{
				starred[row] = col; // Starring the primed zero
				starred[i] = -1; // Un-starring the starred zero in the same column of primed zero

				//Finding the primed zero in the same row of that starred zero
				//(there will always be one)
				col = primed[i];
				row = i;

				//Looking for a starred zero in the same column of the found primed zero
				for(i=0;i<n;i++) { if(starred[i] == col) { break; } }
			}

			//Starring the last primed zero that has no starred zero in its column
			starred[row] = col;

			//Erasing all primes and uncover every rows in the matrix while covering
			//all columns containing a starred zero
			for(ll j=0;j<n;j++)
			{
				primed[j] = -1;
				M[GET_INDEX(j, n, n+1)] = 0;
				if(starred[j] != -1) { M[GET_INDEX(n, starred[j], n+1)] = 1; }
			}
			covers++;
			if(covers == n) { return n; }
		}
	}

	return covers;

  // Bouml preserved body end 000C5211
}

//This implementation is heavily inspired from the code of Dariush Lotfi (June 2008).
//See http://csclab.murraystate.edu/bob.pilgrim/445/munkres.html for the algorithm itself.
void Algorithms::MinimumCostAssignment(ll* costMatrix, ll numItems, ll* assignment)
{
  // Bouml preserved body begin 000C5291

	ll n = numItems;

	// Starred zeros indicates assignment pairs when the algorithm
	// is finished; so there is no need to define a new vector.
	ll* starred = assignment;

	// Allocate memory for primed
	ull primedByteSize = n * sizeof(ll);
	ll* primed = (ll*)Allocate(primedByteSize);
	VERIFY(primed != NULL); memset(primed, 0, primedByteSize);

	// Allocate M, same as weight matrix but with an extra line and column
	ull matrixByteSize = (n+1) * (n+1) * sizeof(ll);
	ll* M = (ll*)Allocate(matrixByteSize);
	VERIFY(M != NULL); memset(M, 0, matrixByteSize);

	// Initialize M
	for(ll i = 0; i < n; i++)
	{
		for(ll j = 0; j < n; j++)
		{
			M[GET_INDEX(i, j, n+1)] = costMatrix[GET_INDEX(i, j, n)]; // notice: n+1 vs. n
		}
	}

	// Note: covered-flag row & column of M (i.e. row & column having index n) indicate which rows or columns are covered.
	for(ll i=0;i<n;i++) { M[GET_INDEX(i, n, n+1)] = 0; }
	for(ll j=0;j<n;j++) { M[GET_INDEX(n, j, n+1)] = 0; }

	// Initialize starred and primed arrays.
	// If M[GET_INDEX(i, j, n+1)] is a starred zero, starred[i] has the value of j and if
	// there is no starred zero at the row i, starred[i] has the value of -1.
	// The same is true for primed zeros and primed vector values.
	for(ll i=0;i<n;i++)
	{
		starred[i] = -1;
		primed[i] = -1;
	}

	//Step 0: Making reduced cost matrix
	ReduceCostMatrix(M, n);

	//Step 1: Marking some zeros with star for beginning
	ll covers = StarArbitraryZeros(M, starred, n);

	while(covers != n)
	{
		//Steps 2 and 3: Changing or adding the cover lines until all zeros are covered
		covers = CoverAllZeros(M, starred, primed, n, covers);
		if(covers == n) { break; }

		//Step 4: Finding smallest uncovered value and add it to every element of each covered row,
		// subtract it from every element of each uncovered column)
		ll min = LONG_LONG_MAX;
		for(ll i=0;i<n;i++) { for(ll j=0;j<n;j++) { if(M[GET_INDEX(i, n, n+1)] != 1 && M[GET_INDEX(n, j, n+1)] != 1 && M[GET_INDEX(i, j, n+1)] < min) { min = M[GET_INDEX(i, j, n+1)]; } } }

		for(ll i=0;i<n;i++)
		{
			if(M[GET_INDEX(i, n, n+1)] == 1)
			{
				for(ll j=0;j<n;j++) { M[GET_INDEX(i, j, n+1)] += min; }
			}
		}

		for(ll j=0;j<n;j++)
		{
			if(M[GET_INDEX(n, j, n+1)] == 0)
			{
				for(ll i=0;i<n;i++) { M[GET_INDEX(i, j, n+1)] -= min; }
			}
		}
	}

	// cleanup
	Free(primed);
	Free(M);

  // Bouml preserved body end 000C5291
}


void Algorithms::MultiplySquareMatrices(const double* leftMatrix, const double* rightMatrix, ull dimension, double* resultMatrix)

{
  // Bouml preserved body begin 00081A91

	VERIFY(leftMatrix != NULL && rightMatrix != NULL && resultMatrix != NULL && dimension != 0);

	for(ull i = 0; i < dimension; i++)
	{
		for(ull j = 0; j < dimension; j++)
		{
			double sum = 0.0;
			for(ull k = 0; k < dimension; k++)
			{
				ull leftIndex = GET_INDEX(i, k, dimension);
				ull rightIndex = GET_INDEX(k, j, dimension);
				sum += leftMatrix[leftIndex] * rightMatrix[rightIndex];
			}

			ull resIndex = GET_INDEX(i, j, dimension);
			resultMatrix[resIndex] = sum;
		}
	}

  // Bouml preserved body end 00081A91
}

bool Algorithms::GetSteadyStateVectorOfSubChain(double* fullChainSS, ull timePeriodId, double** subChainSS, bool inclDummyTPs)
{
  // Bouml preserved body begin 000ADF91

	if(fullChainSS == NULL || subChainSS == NULL) { return false; }

	// get time period parameters
	ull numPeriods = 0; TPInfo tpInfo;
	VERIFY(Parameters::GetInstance()->GetTimePeriodInfo(&numPeriods, &tpInfo) == true);
	ull minPeriod = tpInfo.minPeriod;
	if(inclDummyTPs == true) { numPeriods = tpInfo.numPeriodsInclDummies; }
	ull maxPeriod = minPeriod + numPeriods - 1;

	if(timePeriodId < minPeriod || timePeriodId > maxPeriod) { return false; }

	// get location parameters
	ull minLoc = 0; ull maxLoc = 0;
	VERIFY(Parameters::GetInstance()->GetLocationstampsRange(&minLoc, &maxLoc) == true);
	ull numLoc = maxLoc - minLoc + 1;

	// allocated here, but freed by the caller
	ull resVectorByteSize = numLoc * sizeof(double);
	double* resVector = (double*)Allocate(resVectorByteSize);
	memset(resVector, 0, resVectorByteSize);

	// extract the subvector
	ull startState = (timePeriodId - minPeriod)*numLoc; double sum = 0.0;
	for(ull loc = minLoc; loc <= maxLoc; loc++)
	{
		ull locIndex = (loc - minLoc);
		resVector[locIndex] = fullChainSS[startState + locIndex];
		sum += resVector[locIndex];
	}
	VERIFY(sum != 0.0);

	// renormalize
	double check = 0.0;
	for(ull loc = minLoc; loc <= maxLoc; loc++)
	{
		ull locIndex = (loc - minLoc);
		resVector[locIndex] /= sum;
		check += resVector[locIndex];
	}
	VERIFY(abs(check-1) < EPSILON); // sanity check


	*subChainSS = resVector;


	return true;

  // Bouml preserved body end 000ADF91
}

bool Algorithms::GetTransitionVectorOfSubChain(double* fullChainTransitionMatrix, ull tp1, ull loc1, ull tp2, double** transitionVector, bool inclDummyTPs)

{
  // Bouml preserved body begin 000AF911

	if(fullChainTransitionMatrix == NULL || transitionVector == NULL) { return false; }

	// get time period parameters
	ull numPeriods = 0; TPInfo tpInfo;
	VERIFY(Parameters::GetInstance()->GetTimePeriodInfo(&numPeriods, &tpInfo) == true);
	ull minPeriod = tpInfo.minPeriod;
	if(inclDummyTPs == true) { numPeriods = tpInfo.numPeriodsInclDummies; }
	ull maxPeriod = minPeriod + numPeriods - 1;

	if(tp1 < minPeriod || tp1 > maxPeriod || tp2 < minPeriod || tp2 > maxPeriod) { return false; }

	// get location parameters
	ull minLoc = 0; ull maxLoc = 0;
	VERIFY(Parameters::GetInstance()->GetLocationstampsRange(&minLoc, &maxLoc) == true);
	ull numLoc = maxLoc - minLoc + 1;

	VERIFY(loc1 >= minLoc && loc1 <= maxLoc);

	ull numStates = numPeriods * numLoc;

	// allocated here, but freed by the caller
	ull resVectorByteSize = numLoc * sizeof(double);
	double* resVector = (double*)Allocate(resVectorByteSize);
	memset(resVector, 0, resVectorByteSize);

	// extract the subvector
	ull currentState = (tp1 - minPeriod)*numLoc + (loc1 - minLoc); double sum = 0.0;
	for(ull loc2 = minLoc; loc2 <= maxLoc; loc2++)
	{
		ull locIdx = (loc2 - minLoc);
		ull tpIdx = (tp2 - minPeriod);
		ull nextState = tpIdx * numLoc + locIdx;
		resVector[locIdx] = fullChainTransitionMatrix[GET_INDEX(currentState, nextState, numStates)];
		sum += resVector[locIdx];
	}
	VERIFY(sum != 0.0);

	// renormalize
	double check = 0.0;
	for(ull loc = minLoc; loc <= maxLoc; loc++)
	{
		ull locIdx = (loc - minLoc);
		resVector[locIdx] /= sum;
		check += resVector[locIdx];
	}
	VERIFY(abs(check-1) < EPSILON); // sanity check


	*transitionVector = resVector;

	return true;

  // Bouml preserved body end 000AF911
}


} // namespace lpm
