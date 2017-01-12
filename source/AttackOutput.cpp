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
#include "../include/AttackOutput.h"

namespace lpm {

AttackOutput::AttackOutput() 
{
  // Bouml preserved body begin 00094411

	probabilityDistribution = NULL;
	anonymizationMap = map<ull, ull>();
	mostLikelyTrace = NULL;

  // Bouml preserved body end 00094411
}

AttackOutput::~AttackOutput() 
{
  // Bouml preserved body begin 00094491

	if(probabilityDistribution != NULL) { Free(probabilityDistribution); }
	anonymizationMap.clear();
	if(mostLikelyTrace != NULL) { Free(mostLikelyTrace); }

  // Bouml preserved body end 00094491
}

bool AttackOutput::GetProbabilityDistribution(double** output) const 
{
  // Bouml preserved body begin 00055811

	if(output == NULL) { return false; }

	*output = probabilityDistribution;

	return true;

  // Bouml preserved body end 00055811
}

void AttackOutput::SetProbabilityDistribution(double* prob) 
{
  // Bouml preserved body begin 00055891

	DEBUG_VERIFY(prob != NULL);
	probabilityDistribution = prob;

  // Bouml preserved body end 00055891
}

void AttackOutput::GetAnonymizationMap(map<ull, ull>& map) const 
{
  // Bouml preserved body begin 00076291

	map = anonymizationMap;

  // Bouml preserved body end 00076291
}

void AttackOutput::SetAnonymizationMap(const map<ull, ull>& map) 
{
  // Bouml preserved body begin 00076311

	anonymizationMap = map;

  // Bouml preserved body end 00076311
}

bool AttackOutput::GetMostLikelyTrace(ull** output) const 
{
  // Bouml preserved body begin 0007CA11

	if(output == NULL) { return false; }

	*output = mostLikelyTrace;

	return true;

  // Bouml preserved body end 0007CA11
}

void AttackOutput::SetMostLikelyTrace(const ull* trace) 
{
  // Bouml preserved body begin 0007CA91

	DEBUG_VERIFY(trace != NULL);
	mostLikelyTrace = const_cast<ull*>(trace);

  // Bouml preserved body end 0007CA91
}


} // namespace lpm
