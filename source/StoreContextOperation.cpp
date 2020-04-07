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
#include "../include/StoreContextOperation.h"

namespace lpm {

StoreContextOperation::StoreContextOperation(string name) : Operation<Context, File>(name)
{
  // Bouml preserved body begin 00066E91
  // Bouml preserved body end 00066E91
}

StoreContextOperation::~StoreContextOperation() 
{
  // Bouml preserved body begin 00066F11
  // Bouml preserved body end 00066F11
}

//! \brief Executes the store context operation
//! 
//! \param[in] input 	Context*, input knowledge object 
//! \param[in,out] output 	File*, output knowledge file
//!
//! \return true or false, depending on whether the call is successful
//!
bool StoreContextOperation::Execute(const Context* input, File* output) 
{
  // Bouml preserved body begin 00066F91

	if(input == NULL || output == NULL || output->IsGood() == false) { return false; }

	const Context* context = input;

	stringstream info("");
	info << "Storing the context to file...";
	Log::GetInstance()->Append(info.str());

	// get location parameters
	ull minLoc = 0; ull maxLoc = 0;
	VERIFY(Parameters::GetInstance()->GetLocationstampsRange(&minLoc, &maxLoc) == true);
	ull numLoc = maxLoc - minLoc + 1;

	// get time period parameters
	ull numPeriods = 0; TPInfo tpInfo;
	VERIFY(Parameters::GetInstance()->GetTimePeriodInfo(&numPeriods, &tpInfo) == true);
	TPNode* partitioning = tpInfo.partitioning;

	ull numStates = numPeriods * numLoc;

	stringstream line(""); // line.precision(8);

	// store the minLoc, maxLoc
	line << minLoc << ", " << maxLoc;
	output->WriteLine(line.str());

	output->WriteLine(""); // leave one line empty

	// now store the time partitioning
	string partitionStr = "";
	VERIFY(partitioning->GetStringRepresentation(partitionStr) == true);
	// Note: technically the partitioning may be encoded using multiples lines but we can still write it to the output like this:
	output->WriteLine(partitionStr);

	output->WriteLine(""); // leave one line empty

	// get user profiles
	map<ull, UserProfile*> profiles = map<ull, UserProfile*>();
	VERIFY(context->GetProfiles(profiles) == true);

	// first, we store the user profiles
	pair_foreach_const(map<ull, UserProfile*>, profiles, usersIter)
	{
		ull user = usersIter->first;
		UserProfile* profile = usersIter->second;

		double* transitionMatrix = NULL;
		VERIFY(profile->GetTransitionMatrix(&transitionMatrix) == true);

		double* steadyStateVector = NULL;
		VERIFY(profile->GetSteadyStateVector(&steadyStateVector) == true);

		VERIFY(transitionMatrix != NULL && steadyStateVector != NULL);

		line.str(""); line << user;
		output->WriteLine(line.str());

		output->WriteLine(""); // leave one line empty

		// store transition matrix (numStates x numStates) for this user
		vector<string> lines = vector<string>();
		VERIFY(LineFormatter<double>::GetInstance()->FormatMatrix(transitionMatrix, numStates, numStates, lines) == true);
		foreach_const(vector<string>, lines, lineIter) { output->WriteLine(*lineIter); }

		output->WriteLine(""); // leave one line empty

		// store steady-state vector (numStates x 1) for this user
		string stringLine = "";
		VERIFY(LineFormatter<double>::GetInstance()->FormatVector(steadyStateVector, numStates, stringLine) == true);

		output->WriteLine(stringLine);

		output->WriteLine("");
		output->WriteLine("");
	}

	return true;

  // Bouml preserved body end 00066F91
}

string StoreContextOperation::GetDetailString() 
{
  // Bouml preserved body begin 00067011

	return "StoreContextOperation: " + operationName + "()";

  // Bouml preserved body end 00067011
}


} // namespace lpm
