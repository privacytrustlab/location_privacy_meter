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
#include "../include/LoadContextOperation.h"

namespace lpm {

LoadContextOperation::LoadContextOperation(string name) : Operation<File, Context>(name)
{
  // Bouml preserved body begin 00066C11
  // Bouml preserved body end 00066C11
}

LoadContextOperation::~LoadContextOperation() 
{
  // Bouml preserved body begin 00066C91
  // Bouml preserved body end 00066C91
}

//! \brief Executes the load context operation
//! 
//! \param[in] input 	File*, input knowledge file 
//! \param[in,out] output 	Context*, output knowledge object
//!
//! \return true or false, depending on whether the call is successful
//!
bool LoadContextOperation::Execute(const File* input, Context* output) 
{
  // Bouml preserved body begin 00066D91

	if(input == NULL || input->IsGood() == false || output == NULL)
	{
		SET_ERROR_CODE(ERROR_CODE_INVALID_ARGUMENTS);
		return false;
	}

	stringstream info("");
	info << "Loading the context from file...";
	Log::GetInstance()->Append(info.str());

	// GUR: ### get users parameters
	// GUR: ### ull minUser = 0; ull maxUser = 0;
	// GUR: ### VERIFY(Parameters::GetInstance()->GetUsersRange(&minUser, &maxUser) == true);
	Parameters* params = Parameters::GetInstance();

	// get location parameters
	ull minLoc = 0; ull maxLoc = 0;
	VERIFY(params->GetLocationstampsRange(&minLoc, &maxLoc) == true);

	vector<ull> values = vector<ull>();

	string line = "";
	if(input->ReadNextLine(line) == false)
	{
		SET_ERROR_CODE(ERROR_CODE_INCOMPLETE_INPUT_FILE);
		return false;
	}

	// retrieve the minLoc, maxLoc
	size_t pos = 0;
	ull storedMinLoc = 0; ull storedMaxLoc = 0;
	if(LineParser<ull>::GetInstance()->ParseFields(line, values, 2, &pos) == false || pos != string::npos)
	{
		SET_ERROR_CODE_DETAILS(ERROR_CODE_INVALID_FORMAT, "the provided knowledge file has an invalid format with respect to the minimum and maximum loc parameters");
		return false;
	}

	storedMinLoc = values[0];
	storedMaxLoc = values[1];

	if(storedMaxLoc <= storedMinLoc)
	{
		SET_ERROR_CODE_DETAILS(ERROR_CODE_INVALID_FORMAT, "the provided knowledge file has an invalid format with respect to the minimum and maximum loc parameters");
		return false;
	}

	// if the loc params differ, issue a warning stating that the ones coming from Parameters are ignored.
	if(storedMinLoc != minLoc || storedMaxLoc != maxLoc)
	{
		stringstream ss("");
		ss << "LoadContextOperation: (minLoc, maxLoc) = (" << minLoc << ", " << maxLoc << ")" << " - (storedMinLoc, storedMaxLoc) = (" << storedMinLoc << ", " << storedMaxLoc << "). ";
		Log::GetInstance()->Append(ss.str(), Log::infoLevel);
		ss.str("");
		ss << "Location parameters and location from knowledge differ: ignoring (minLoc, maxLoc)...";
		Log::GetInstance()->Append(ss.str(), Log::warningLevel);
	}


	// update the locs.
	minLoc = storedMinLoc;
	maxLoc = storedMaxLoc;
	ull numLoc = maxLoc - minLoc + 1;

	// Set the parameters
	VERIFY(Parameters::GetInstance()->SetLocationstampsRange(minLoc, maxLoc) == true);

	// read one empty line
	if(input->ReadNextLine(line) == false || line.empty() == false)
	{
		SET_ERROR_CODE(ERROR_CODE_INCOMPLETE_INPUT_FILE);
		return false;
	}

	// read the time partitioning
	TPNode* partitioning = TPNode::FromFile(const_cast<File*>(input));
	if(partitioning == NULL || Parameters::GetInstance()->SetTimePartitioning(partitioning) == false)
	{
		SET_ERROR_CODE(ERROR_CODE_INVALID_TIME_PARTITIONING);
		return false;
	}

	// get time period parameters
	ull numPeriods = 0; TPInfo tpInfo; // ull minPeriod = 1;
	VERIFY(Parameters::GetInstance()->GetTimePeriodInfo(&numPeriods, &tpInfo) == true);
	// ull maxPeriod = minPeriod + numPeriods - 1;

	ull numStates = numPeriods * numLoc;

	// read one empty line (TOCHECK: !! MAY NOT BE NEEDED !!)
	if(input->ReadNextLine(line) == false || line.empty() == false)
	{
		SET_ERROR_CODE(ERROR_CODE_INCOMPLETE_INPUT_FILE);
		return false;
	}

	ull users = 0;
	vector<double> dvalues = vector<double>();
	vector<ull> skippedUsers = vector<ull>();
	while(true)
	{
		// read user ID
		bool readOk = input->ReadNextLine(line);

		if(users > 0 && input->IsEOF() == true) { break; } // EOF

		if(readOk == false) // read failed
		{
			SET_ERROR_CODE(ERROR_CODE_INCOMPLETE_INPUT_FILE);
			output->ClearProfiles();
			return false;
		}

		ull user = 0; pos = 0;
		if(LineParser<ull>::GetInstance()->ParseValue(line, &user, &pos) == false || pos != string::npos || user == 0)
		{
			SET_ERROR_CODE(ERROR_CODE_INVALID_FORMAT);
			output->ClearProfiles();
			return false;
		}

		// skip that user if not in range
		// GUR: ### if(user > maxUser || user < minUser)
		if(params->UserExists(user) == false)
		{ skippedUsers.push_back(user); }

		UserProfile* profile = NULL;

		// check for duplicate users
		if(output->GetUserProfile(user, &profile) != false)
		{
			SET_ERROR_CODE(ERROR_CODE_DUPLICATE_ENTRIES);
			output->ClearProfiles();
			return false;
		}

		profile = new UserProfile(user);
		VERIFY(output->AddProfile(profile) == true);

		// release ownership
		profile->Release();

		// read one empty line
		if(input->ReadNextLine(line) == false || line.empty() == false)
		{
			SET_ERROR_CODE(ERROR_CODE_INCOMPLETE_INPUT_FILE);
			return false;
		}

		// read the transition matrix
		ull transitionMatrixByteSize = numStates * numStates * sizeof(double);
		double* transitionMatrix = (double*)Allocate(transitionMatrixByteSize);
		VERIFY(transitionMatrix != NULL);
		memset(transitionMatrix, 0, transitionMatrixByteSize);

		for(ull state = 0; state < numStates; state++)
		{
			line = "";
			if(input->ReadNextLine(line) == false)
			{
				SET_ERROR_CODE(ERROR_CODE_INCOMPLETE_INPUT_FILE);
				output->ClearProfiles();
				return false;
			}

			dvalues.clear();
			if(LineParser<double>::GetInstance()->ParseFields(line, dvalues, numStates, &pos) == false || pos != string::npos)
			{
				SET_ERROR_CODE(ERROR_CODE_INCOMPLETE_INPUT_FILE);
				output->ClearProfiles();
				return false;
			}

			double sum = 0.0;
			for(ull state2 = 0; state2 < numStates; state2++)
			{
				double val = dvalues[state2];
				transitionMatrix[GET_INDEX(state, state2, numStates)] = val;

				sum += val;
			}

			// we'll re-normalize (to improve precision), but each row of the matrix from the knowledge file must in any case ROUGHLY sum up to 1.
			if(ABS(sum - 1) > EPSILON)
			{
				SET_ERROR_CODE_DETAILS(ERROR_CODE_INVALID_FORMAT, "the transition matrix is not normalized, if the knowledge was not tampered with this can only be due to rounding off errors");
				return false;
			}

			// re-normalize
			for(ull state2 = 0; state2 < numStates; state2++)
			{
				transitionMatrix[GET_INDEX(state, state2, numStates)] /= sum;
			}
		}

		VERIFY(profile->SetTransitionMatrix(transitionMatrix) == true);

		// read one empty line
		if(input->ReadNextLine(line) == false || line.empty() == false)
		{
			SET_ERROR_CODE(ERROR_CODE_INCOMPLETE_INPUT_FILE);
			return false;
		}

		// read steady-state vector
		ull steadyStateVectorByteSize = numStates * sizeof(double);
		double* steadyStateVector = (double*)Allocate(steadyStateVectorByteSize);
		VERIFY(steadyStateVector != NULL);
		memset(steadyStateVector, 0, steadyStateVectorByteSize);

		line = "";
		if(input->ReadNextLine(line) == false)
		{
			SET_ERROR_CODE(ERROR_CODE_INCOMPLETE_INPUT_FILE);
			output->ClearProfiles();
			return false;
		}

		dvalues.clear();
		if(LineParser<double>::GetInstance()->ParseFields(line, dvalues, numStates, &pos) == false || pos != string::npos)
		{
			SET_ERROR_CODE(ERROR_CODE_INCOMPLETE_INPUT_FILE);
			output->ClearProfiles();
			return false;
		}

		double sum = 0.0;
		for(ull state = 0; state < numStates; state++)
		{
			double val = dvalues[state];
			steadyStateVector[state] = val;

			sum += val;
		}

		// we'll re-normalize (to improve precision), but the vector from the knowledge file must in any case ROUGHLY sum up to 1.
		if(ABS(sum - 1) > EPSILON)
		{
			SET_ERROR_CODE_DETAILS(ERROR_CODE_INVALID_FORMAT, "the steady-state vector is not normalized, if the knowledge was not tampered with this can only be due to rounding off errors");
			return false;
		}

		// normalize
		for(ull state = 0; state < numStates; state++)	{ steadyStateVector[state] /= sum; }

		VERIFY(profile->SetSteadyStateVector(steadyStateVector) == true);

		users++;

		// read two empty line (break from while if EOF)
		readOk = input->ReadNextLine(line);

		if(input->IsEOF() == true) { break; } // EOF
		else if(readOk == false || line.empty() == false)
		{
			SET_ERROR_CODE(ERROR_CODE_INCOMPLETE_INPUT_FILE);
			return false;
		}

		readOk = input->ReadNextLine(line);

		if(input->IsEOF() == true) { break; } // EOF
		else if(readOk == false || line.empty() == false)
		{
			SET_ERROR_CODE(ERROR_CODE_INCOMPLETE_INPUT_FILE);
			return false;
		}
	}

	foreach_const(vector<ull>, skippedUsers, iter) // remove the profiles of skipped users
	{
		ull user = *iter;
		VERIFY(output->RemoveUserProfile(user) == true);
	}

	map<ull, UserProfile*> profiles = map<ull, UserProfile*>();
	VERIFY(output->GetProfiles(profiles) == true);

	if(profiles.size() == 0) // issue warning if we have no extracted anything
	{
		Log::GetInstance()->Append("LoadContextOperation: No user profiles were extracted during the operation (this is due to an improper users' range parameter setting).", Log::warningLevel);
	}

	return true;

  // Bouml preserved body end 00066D91
}

string LoadContextOperation::GetDetailString() 
{
  // Bouml preserved body begin 00066E11

	return "LoadContextOperation: " + operationName + "()";

  // Bouml preserved body end 00066E11
}


} // namespace lpm
