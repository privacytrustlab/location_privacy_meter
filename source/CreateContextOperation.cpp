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
#include "../include/CreateContextOperation.h"
#include "../include/File.h"
#include "../include/Trace.h"
#include "../include/UserProfile.h"

namespace lpm {

CreateContextOperation::CreateContextOperation(string name) : Operation<KnowledgeInput, Context>(name)
{
  // Bouml preserved body begin 00045E11

	SetLimits(1024, 1); // by default spend at most 1024 iterations per user or 1 sec per user

  // Bouml preserved body end 00045E11
}

CreateContextOperation::~CreateContextOperation() 
{
  // Bouml preserved body begin 00066D11
  // Bouml preserved body end 00066D11
}

//! \brief Executes the knowledge construction
//! 
//! \param[in] input 	KnowledgeInput*, input object 
//! \param[in,out] output 	Context*, output knowledge object. 
//!
//! \note The procedure used to construct the knowledge is Gibbs sampling. 
//! The number of iterations can be specified by calling \a SetLimits() before this.
//!
//! \return true or false, depending on whether the call is successful
bool CreateContextOperation::Execute(const KnowledgeInput* input, Context* output) 
{
  // Bouml preserved body begin 00045911

	if(input == NULL || output == NULL) { return false; }

	Context* context = output;

	set<ull> unknownUsers = set<ull>(); // sets of users for which we have no mobility info
	VERIFY(Parameters::GetInstance()->GetUsersSet(unknownUsers) == true && unknownUsers.size() > 0);


	map<ull, vector<TraceVector> > learningTraces = map<ull, vector<TraceVector> >();
	map<ull, double*> priorTransitionsCount = map<ull, double*>();

	bool* transFeasibilityMatrix = NULL;
	if(ReadKnowledgeFiles(input, learningTraces, priorTransitionsCount, &transFeasibilityMatrix) == false){ return false; }

	// ull Nusers = learningTraces.size();
	// VERIFY(Nusers != 0 && learningTraces.size() == priorTransitionsCount.size());
	VERIFY(learningTraces.size() == priorTransitionsCount.size());

	// get location parameters
	ull minLoc = 0; ull maxLoc = 0;
	VERIFY(Parameters::GetInstance()->GetLocationstampsRange(&minLoc, &maxLoc) == true);
	ull numLoc = maxLoc - minLoc + 1;

	// get time period parameters
	ull numPeriods = 0; /* ull minPeriod = 1; */ TPInfo tpInfo;
	VERIFY(Parameters::GetInstance()->GetTimePeriodInfo(&numPeriods, &tpInfo) == true);
	// ull maxPeriod = minPeriod + numPeriods - 1;

	ull numStates = numPeriods * numLoc;

	pair_foreach_const(map<ull, vector<TraceVector> >, learningTraces, iter)
	{
		ull user = iter->first;
		vector<TraceVector> traces = iter->second;

		if(Parameters::GetInstance()->UserExists(user) == true) // only process this user if he exists
		{
			unknownUsers.erase(user); // we have info for this user, remove it from the set

			VERIFY(priorTransitionsCount.find(user) != priorTransitionsCount.end());
			double* aprioriTransitionsCount = (priorTransitionsCount.find(user))->second;

			VERIFY(aprioriTransitionsCount != NULL);

			UserProfile* profile = new UserProfile(user);
			VERIFY(profile != NULL);

			// allocate learning traces for that user for Gibbs sampling
			ull numTraces = traces.size();
			VERIFY(numTraces != 0);

			if(DoGibbsSampling(traces, aprioriTransitionsCount, profile) == false)
			{
				profile->Release();
				Free(aprioriTransitionsCount);
				pair_foreach_const(map<ull, vector<TraceVector> >, learningTraces, iter2)
				{ foreach_const(vector<TraceVector>, iter2->second, iterV) { Free((*iterV).trace); } }

				priorTransitionsCount.clear();
				learningTraces.clear();
				output->ClearProfiles();

				Free(transFeasibilityMatrix);

				return false;
			}

			Free(aprioriTransitionsCount);


			context->AddProfile(profile);
			profile->Release();
		}

		traces.clear();
	}

	pair_foreach_const(map<ull, vector<TraceVector> >, learningTraces, iter2)
	{ foreach_const(vector<TraceVector>, iter2->second, iterV) { Free((*iterV).trace); } }

	priorTransitionsCount.clear();
	learningTraces.clear();

	if(unknownUsers.size() > 0)
	{
		ull numPeriodsInclDummies = tpInfo.numPeriodsInclDummies;
		ull numStatesInclDummies = numPeriodsInclDummies * numLoc;

		// do Gibbs sampling once, and then assign the result to each unknown user

		// generate transitions count
		ull aprioriTransitionsCountByteSize = numStatesInclDummies * numStatesInclDummies * sizeof(double);
		double* aprioriTransitionsCount = (double*)Allocate(aprioriTransitionsCountByteSize);
		VERIFY(aprioriTransitionsCount != NULL);
		memset(aprioriTransitionsCount, 0, aprioriTransitionsCountByteSize);


		// use the transitions feasibility matrix to fill in aprioriTransitionsCount
		// add epsilon according to the transitions feasibility matrix
		VERIFY(transFeasibilityMatrix != NULL);
		const double epsilon = 0.01/numStatesInclDummies; //numPeriodsInclDummies; // 1.0/200; // 1.0 / (numLoc * numLoc); // EPSILON
		for(ull loc = minLoc; loc <= maxLoc; loc++)
		{
			ull numDestLoc = 0; // number of possible destinations regions from loc.
			for(ull loc2 = minLoc; loc2 <= maxLoc; loc2++)
			{
				ull index = GET_INDEX((loc - minLoc), (loc2 - minLoc), numLoc);
				if(transFeasibilityMatrix[index] == true) { numDestLoc++; }
			}

			if(numDestLoc == 0) { continue; }

			for(ull loc2 = minLoc; loc2 <= maxLoc; loc2++)
			{
				ull index = GET_INDEX((loc - minLoc), (loc2 - minLoc), numLoc);

				if(transFeasibilityMatrix[index] == true)
				{
					for(ull tp1Idx = 0; tp1Idx < numPeriodsInclDummies; tp1Idx++)
					{
						for(ull tp2Idx = 0; tp2Idx < numPeriodsInclDummies; tp2Idx++)
						{
							ull tpMatrixIdx = GET_INDEX(tp1Idx, tp2Idx, numPeriodsInclDummies);
							double effectiveEpsilon = epsilon;

							// if you can't (time-semantically) go from tp1 to tp2, we should not add epsilon
							if(tpInfo.propTransMatrix[tpMatrixIdx] == 0) { effectiveEpsilon = 0; }

							ull countIdx = GET_INDEX(tp1Idx * numLoc + (loc - minLoc), tp2Idx * numLoc + (loc2 - minLoc), numStatesInclDummies);
							aprioriTransitionsCount[countIdx] += effectiveEpsilon; // (effectiveEpsilon / numDestLoc); // not yet
						}
					}
				}
			}
		}

		UserProfile* unknownProfile = new UserProfile(0);
		VERIFY(unknownProfile != NULL);

		// allocate learning traces for that user for Gibbs sampling (create empty trace)
		TraceVector vec;
		vec.offset = tpInfo.canonicalPartitionParentNode->GetOffset(true);
		vec.length = tpInfo.canonicalPartitionParentNode->GetLength();

		ull traceByteSize = vec.length * sizeof(ull);
		ull* trace = vec.trace = (ull*)Allocate(traceByteSize);
		VERIFY(trace != NULL);
		memset(trace, 0, traceByteSize);

		vector<TraceVector> tvecs = vector<TraceVector>();
		tvecs.push_back(vec);

		if(DoGibbsSampling(tvecs, aprioriTransitionsCount, unknownProfile) == false)
		{
			unknownProfile->Release();
			Free(aprioriTransitionsCount);
			Free(vec.trace);

			output->ClearProfiles();

			Free(transFeasibilityMatrix);

			return false;
		}

		Free(transFeasibilityMatrix); transFeasibilityMatrix = NULL;
		Free(aprioriTransitionsCount);

		// get the generate profile info
		double* matrix = NULL;
		VERIFY(unknownProfile->GetTransitionMatrix(&matrix) == true);
		double* vector = NULL;
		VERIFY(unknownProfile->GetSteadyStateVector(&vector) == true);

		foreach_const(set<ull>, unknownUsers, iter)
		{
			// copy profile content
			// steady-state vector
			ull steadyStateVectorByteSize = numStates * sizeof(double);
			double* steadyStateVector = (double*)Allocate(steadyStateVectorByteSize);
			VERIFY(steadyStateVector != NULL);
			memcpy(steadyStateVector, vector, steadyStateVectorByteSize);


			// transition matrix
			ull transitionMatrixByteSize = numStates * numStates * sizeof(double);
			double* transitionMatrix = (double*)Allocate(transitionMatrixByteSize);
			VERIFY(transitionMatrix != NULL);
			memcpy(transitionMatrix, matrix, transitionMatrixByteSize);

			ull user = *iter;
			UserProfile* profile = new UserProfile(user);
			VERIFY(profile != NULL);

			profile->SetSteadyStateVector(steadyStateVector);
			profile->SetTransitionMatrix(transitionMatrix);

			context->AddProfile(profile);
			profile->Release();
		}

		unknownProfile->Release();
	}

	if(transFeasibilityMatrix != NULL) { Free(transFeasibilityMatrix); }

	return true;

  // Bouml preserved body end 00045911
}

string CreateContextOperation::GetDetailString() 
{
  // Bouml preserved body begin 00045991

	return "CreateContextOperation: " + operationName + "()";

  // Bouml preserved body end 00045991
}

//! 
//! \brief Sets the limits of the Gibbs sampling procedure.
//!
//! \param[in] maxSamples 	ull, the maximum number of samples (i.e. iterations) for each user.
//! \param[in] maxSeconds 	ull, the maximum time (in number of seconds) to spend for each user.
//!
//! \note The constant KC_NO_LIMITS can be used as a unlimited value for \a maxSamples or \a maxSeconds.
//! If both arguments are set to a value different from KC_NO_LIMITS, the iteration procedure stops when either limit is reached.
//!
//! \return true or false, depending on whether the call is successful
//!
bool CreateContextOperation::SetLimits(ull maxSamples, ull maxSeconds) 
{
  // Bouml preserved body begin 0007E411

	if(maxSamples != KC_NO_LIMITS && maxSamples < 2) { return false; }
	if(maxSamples == KC_NO_LIMITS && maxSeconds == KC_NO_LIMITS) { return false; }

	maxGSPerUser = maxSamples;
	maxSecondsPerUser = maxSeconds;

	return true;

  // Bouml preserved body end 0007E411
}

bool CreateContextOperation::TransitionMatrixFromCountMatrix(const double* count, double* alpha, double* theta, double* transitionMatrix, bool sample) const 
{
  // Bouml preserved body begin 000BCF91

	VERIFY(count != NULL && alpha != NULL && theta != NULL && transitionMatrix != NULL);

	Parameters* params = Parameters::GetInstance();
	RNG* rng = RNG::GetInstance();

	// get location parameters
	ull minLoc = 0; ull maxLoc = 0;
	VERIFY(params->GetLocationstampsRange(&minLoc, &maxLoc) == true);
	ull numLoc = maxLoc - minLoc + 1;

	// get time period parameters
	ull numPeriods = 0;  TPInfo tpInfo;
	VERIFY(params->GetTimePeriodInfo(&numPeriods, &tpInfo) == true);
	// ull minPeriod = tpInfo.minPeriod;

	ull numPeriodsInclDummies = tpInfo.numPeriodsInclDummies;
	ull numStatesInclDummies = numPeriodsInclDummies * numLoc;

	ull thetaByteSize = numLoc * numLoc * sizeof(double);
	ull alphaByteSize = thetaByteSize;

	// we sample on Pr(r1, r2 | p1, p2) and then use this to compute Pr(r2, p2 | r1, p1)
	for(ull tp1Idx = 0; tp1Idx < numPeriodsInclDummies; tp1Idx++)
	{
		for(ull tp2Idx = 0; tp2Idx < numPeriodsInclDummies; tp2Idx++)
		{
			ull tpTransIdx = GET_INDEX(tp1Idx, tp2Idx, numPeriodsInclDummies);
			double tpTransProb = tpInfo.propTransMatrix[tpTransIdx];

			ull rowByteSize = alphaByteSize / numLoc; // alpha has numLoc rows
			// fill up alpha with counts: C(r1, r2, p1, p2)
			for(ull loc1Idx = 0; loc1Idx < numLoc; loc1Idx++)
			{
				ull rowStartIdx = tp1Idx * numLoc + loc1Idx;
				ull colStartIdx = tp2Idx * numLoc;
				ull countMatrixIdx = GET_INDEX(rowStartIdx, colStartIdx, numStatesInclDummies);

				ull rowIdx = loc1Idx * numLoc;
				memcpy(&alpha[rowIdx], &count[countMatrixIdx], rowByteSize);
			}

			memset(theta, 0, thetaByteSize);

			if(tpTransProb == 0)
			{
				// check if the count matrix is consistent
				for(ull loc1Idx = 0; loc1Idx < numLoc; loc1Idx++)
				{
					for(ull loc2Idx = 0; loc2Idx < numLoc; loc2Idx++)
					{
						ull idx = GET_INDEX(loc1Idx, loc2Idx, numLoc);
						if(alpha[idx] != 0) // count matrix is inconsistent
						{
							SET_ERROR_CODE_DETAILS(ERROR_CODE_IMPOSSIBLE_TRACE, "the provided count matrix is inconsistent with the time partitioning");

							return false;
						}
					}
				}
			}
			else // safe to sample
			{
				ull vectorSize = numLoc * numLoc;
				if(sample == true)
				{
					rng->GetDirichletRandomSample(alpha, vectorSize, theta); // do the actual sampling

					for(ull i=0; i < vectorSize; i++)
					{
						VERIFY((alpha[i] > 0 && theta[i] == 0) == false); // make sure that if alpha[i] > 0, so is theta[i]
					}
				}
				else
				{
					CODING_ERROR; // shouldn't be able to get here
					// rng->GetDirichletExpectedValue(alpha, vectorSize, theta);
				}
			}

			// multiply each sample by Pr(p2 | p1) and put the elements in the matrix
			for(ull loc1Idx = 0; loc1Idx < numLoc; loc1Idx++)
			{
				for(ull loc2Idx = 0; loc2Idx < numLoc; loc2Idx++)
				{
					ull idx = GET_INDEX(loc1Idx, loc2Idx, numLoc);
					double prob = theta[idx] * tpTransProb;

					ull transMatrixIdx = GET_INDEX((tp1Idx * numLoc + loc1Idx), (tp2Idx * numLoc + loc2Idx), numStatesInclDummies);
					transitionMatrix[transMatrixIdx] = prob;
				}
			}
		}

		// last step: normalize (divide by the sum over loc2Idx, tp2Idx)
		for(ull loc1Idx = 0; loc1Idx < numLoc; loc1Idx++)
		{
			double sum = 0.0;
			for(ull tp2Idx = 0; tp2Idx < numPeriodsInclDummies; tp2Idx++)
			{
				for(ull loc2Idx = 0; loc2Idx < numLoc; loc2Idx++)
				{
					ull transMatrixIdx = GET_INDEX((tp1Idx * numLoc + loc1Idx), (tp2Idx * numLoc + loc2Idx), numStatesInclDummies);
					sum = sum + transitionMatrix[transMatrixIdx];
				}
			}

			for(ull tp2Idx = 0; tp2Idx < numPeriodsInclDummies; tp2Idx++)
			{
				for(ull loc2Idx = 0; loc2Idx < numLoc; loc2Idx++)
				{
					ull transMatrixIdx = GET_INDEX((tp1Idx * numLoc + loc1Idx), (tp2Idx * numLoc + loc2Idx), numStatesInclDummies);
					transitionMatrix[transMatrixIdx] /= sum;
				}
			}
		}
	}

	return true;

  // Bouml preserved body end 000BCF91
}

void CreateContextOperation::GetIntermediaryTransitionVector(map<ull, double*>& cache, const double* transitionMatrix, ull loc1, ull loc3, ull tp1, ull tp2, ull tp3, double** vector) const 
{
  // Bouml preserved body begin 000BD011
  // Bouml preserved body end 000BD011
}

void CreateContextOperation::ComputeSteadyStateVector(const double* transitionMatrix, double* steadyStateVector) const 
{
  // Bouml preserved body begin 0007E511

	VERIFY(transitionMatrix != NULL && steadyStateVector != NULL);

	// get location parameters
	ull minLoc = 0; ull maxLoc = 0;
	VERIFY(Parameters::GetInstance()->GetLocationstampsRange(&minLoc, &maxLoc) == true);
	ull numLoc = maxLoc - minLoc + 1;

	// get time period parameters
	ull numPeriods = 0; TPInfo tpInfo; // ull minPeriod = 1;
	VERIFY(Parameters::GetInstance()->GetTimePeriodInfo(&numPeriods, &tpInfo) == true);
	// ull maxPeriod = minPeriod + numPeriods - 1;

	ull numPeriodsInclDummies = tpInfo.numPeriodsInclDummies;
	ull numStatesInclDummies = numPeriodsInclDummies * numLoc;

	const double epsilon = EPSILON;
	const ull maxIterations = 30;
	const ull minIterations = 6;

	ull transitionMatrixByteSize = numStatesInclDummies * numStatesInclDummies * sizeof(double);
	double* res = (double*)Allocate(transitionMatrixByteSize);
	VERIFY(res != NULL);
	memcpy(res, transitionMatrix, transitionMatrixByteSize);

	double* temp = (double*)Allocate(transitionMatrixByteSize);
	VERIFY(temp != NULL);
	memset(temp, 0, transitionMatrixByteSize);

	const double normalizationFactor = numStatesInclDummies * numStatesInclDummies;
	ull iter = 0;
	for(iter = 1; iter < maxIterations; iter++)
	{
		double delta = 0.0;

		Algorithms::MultiplySquareMatrices(res, res, numStatesInclDummies, temp);

		for(ull stateIdx1 = 0; stateIdx1 < numStatesInclDummies; stateIdx1++)
		{
			for(ull stateIdx2 = 0; stateIdx2 < numStatesInclDummies; stateIdx2++)
			{
				ull index = GET_INDEX(stateIdx1, stateIdx2, numStatesInclDummies);
				delta += ABS(temp[index] - res[index]);
			}
		}

		delta /= normalizationFactor;

		if(iter >= minIterations && delta < epsilon){ break; }

		memcpy(res, temp, transitionMatrixByteSize);
	}

	ull firstRowIndex = GET_INDEX(0, 0, numStatesInclDummies);

	ull steadyStateVectorByteSize = numStatesInclDummies * sizeof(double);
	memcpy(steadyStateVector, &res[firstRowIndex], steadyStateVectorByteSize);

	Free(temp);
	Free(res);

	// check
	double sum = 0.0;
	for(ull i = 0; i < numStatesInclDummies; i++) { sum += steadyStateVector[i]; }
	VERIFY(ABS(sum - 1.0) <= epsilon);

  // Bouml preserved body end 0007E511
}

bool CreateContextOperation::DoGibbsSampling(vector<TraceVector>& learningTraces, double* priorTransitionsCount, UserProfile* profile) const 
{
  // Bouml preserved body begin 0007E491

	if(learningTraces.empty() == true || priorTransitionsCount == NULL || profile == NULL) { return false; }

	ull numTraces = learningTraces.size();

	ull startTime = (ull)time(NULL);

	ull user = profile->GetUser();

	stringstream info("");
	info << "Starting Gibbs Sampling for user " << user << "!";
	Log::GetInstance()->Append(info.str());

	Parameters* params = Parameters::GetInstance();
	RNG* rng = RNG::GetInstance();

	// get time parameters
	//	ull minTime = 0; ull maxTime = 0;
	//	VERIFY(params->GetTimestampsRange(&minTime, &maxTime) == true);
	//	ull numTimes = maxTime - minTime + 1;

	// get location parameters
	ull minLoc = 0; ull maxLoc = 0;
	VERIFY(params->GetLocationstampsRange(&minLoc, &maxLoc) == true);
	ull numLoc = maxLoc - minLoc + 1;

	// get time period parameters
	ull numPeriods = 0;  TPInfo tpInfo;
	VERIFY(params->GetTimePeriodInfo(&numPeriods, &tpInfo) == true);
	ull minPeriod = tpInfo.minPeriod;
	// ull maxPeriod = tpInfo.maxPeriod;

	ull numStates = numPeriods * numLoc;

	ull numPeriodsInclDummies = tpInfo.numPeriodsInclDummies;
	ull numStatesInclDummies = numPeriodsInclDummies * numLoc;

	VERIFY(numStatesInclDummies*numStatesInclDummies <= ((ull)((ll)-1))); // make sure we the problem size can be handled

	// transition matrix
	ull transitionMatrixByteSize = numStatesInclDummies * numStatesInclDummies * sizeof(double);
	double* transitionMatrix = (double*)Allocate(transitionMatrixByteSize);
	VERIFY(transitionMatrix != NULL);
	memset(transitionMatrix, 0, transitionMatrixByteSize);

	// transition matrix sum
	double* transitionMatrixSum = (double*)Allocate(transitionMatrixByteSize);
	VERIFY(transitionMatrixSum != NULL);
	memset(transitionMatrixSum, 0, transitionMatrixByteSize);

	// steady-state vector
	ull steadyStateVectorByteSize = numStatesInclDummies * sizeof(double);
	double* steadyStateVector = (double*)Allocate(steadyStateVectorByteSize);
	VERIFY(steadyStateVector != NULL);
	memset(steadyStateVector, 0, steadyStateVectorByteSize);

	// variance matrix
	ull varianceMatrixByteSize = numStates * numStates * sizeof(double);
	double* varianceMatrix = (double*)Allocate(varianceMatrixByteSize);
	VERIFY(varianceMatrix != NULL);
	memset(varianceMatrix, 0, varianceMatrixByteSize);


	// alpha (used for dirichlet sampling)
	ull alphaByteSize = numLoc * numLoc * sizeof(double);
	double* alpha = (double*)Allocate(alphaByteSize);
	VERIFY(alpha != NULL);
	memset(alpha, 0, alphaByteSize);

	// theta (used for dirichlet sampling)
	ull thetaByteSize = numLoc * numLoc * sizeof(double);
	double* theta = (double*)Allocate(thetaByteSize);
	VERIFY(theta != NULL);
	memset(theta, 0, thetaByteSize);

	// estimated traces (constructed to "mirror" learningTraces)
	vector<TraceVector> estimatedTraces = vector<TraceVector>();
	foreach_const(vector<TraceVector>, learningTraces, iterTV)
	{
		TraceVector tvec = *iterTV;

		TraceVector tvec2;
		tvec2.offset = tvec.offset;
		tvec2.length = tvec.length;

		ull estimatedTraceByteSize = tvec2.length * sizeof(ull);
		tvec2.trace = (ull*)Allocate(estimatedTraceByteSize);
		VERIFY(tvec2.trace != NULL);
		memcpy(tvec2.trace, tvec.trace, estimatedTraceByteSize);

		estimatedTraces.push_back(tvec2);
	}
	VERIFY(estimatedTraces.size() == numTraces);

	// count
	ull countByteSize = numStatesInclDummies * numStatesInclDummies * sizeof(ull);
	double* count = (double*)Allocate(countByteSize);
	VERIFY(count != NULL);
	memset(count, 0, countByteSize);

	// Step 0

	/**** -- (a) P given LT, CM -- ****/
	memcpy(count, priorTransitionsCount, countByteSize); // put prior transitions count in count

	// add transitions in the learning traces to count
	bool allFull = true; bool computationNeedsSteadyState = false;
	foreach_const(vector<TraceVector>, learningTraces, iterTV)
	{
		TraceVector tvec = *iterTV;
		ull* trace = tvec.trace;
		ull minTime = tvec.offset;
		ull numTimes = tvec.length;
		ull maxTime = minTime + numTimes - 1;

		bool full = true;
		for(ull tm = minTime; tm <= maxTime; tm++) // go to maxTime, but with wrap-around
		{
			ull nexttm = tm == maxTime ? minTime : (tm + 1); // next timestamp (with wrap-around)

			ull startLoc = trace[(tm - minTime)];
			ull endLoc = trace[(nexttm - minTime)];

			if(tm == minTime && startLoc == 0) { computationNeedsSteadyState = true; }

			if(startLoc == 0 || endLoc == 0) { full = false; continue; }


			ull startTP = params->LookupTimePeriod(tm, true);
			ull endTP = params->LookupTimePeriod(nexttm, true);
			if(startTP == INVALID_TIME_PERIOD || endTP == INVALID_TIME_PERIOD)
			{
				SET_ERROR_CODE(ERROR_CODE_INCONSISTENT_TIME_PARTITIONING_USAGE);
				return false;
			}

			ull startTPIdx = (startTP - minPeriod); ull endTPIdx = (endTP - minPeriod);
			ull startLocIdx = (startLoc - minLoc); ull endLocIdx = (endLoc - minLoc);

			ull startStateIdx = startTPIdx * numLoc + startLocIdx;
			ull endStateIdx = endTPIdx * numLoc + endLocIdx;
			ull countIdx = GET_INDEX(startStateIdx, endStateIdx, numStatesInclDummies);
			count[countIdx]++;
		}

		if(full == false) { allFull = false; }
	}

	// Generate P^{0}
	if(TransitionMatrixFromCountMatrix(count, alpha, theta, transitionMatrix, true) == false)
	{
		Free(count); Free(theta); Free(alpha);
		Free(transitionMatrixSum);
		Free(transitionMatrix);
		Free(steadyStateVector);
		foreach_const(vector<TraceVector>, estimatedTraces, iterTV)	{ TraceVector tvec = *iterTV; Free(tvec.trace); }

		return false; // error code is set inside the function
	}

	// Set P^{0} to PSUM
	memcpy(transitionMatrixSum, transitionMatrix, transitionMatrixByteSize);

	if(computationNeedsSteadyState == true)	{ ComputeSteadyStateVector(transitionMatrix, steadyStateVector); }

	// update variance matrix
	for(ull stateIdx1 = 0; stateIdx1 < numStates; stateIdx1++)
	{
		for(ull stateIdx2 = 0; stateIdx2 < numStates; stateIdx2++)
		{
			ull transMatrixIdx = GET_INDEX(stateIdx1, stateIdx2, numStatesInclDummies);
			ull idx = GET_INDEX(stateIdx1, stateIdx2, numStates);

			double p = transitionMatrix[transMatrixIdx];
			varianceMatrix[idx] += p*p;
		}
	}

	ull step = 1;

	double* samplingProbVector = NULL;
	if(allFull == false)
	{
		// vector from which we sample
		ull samplingProbVectorByteSize = numLoc * sizeof(double); // this vector has numLoc entries (and not numStates) this is NOT a mistake!
		samplingProbVector = (double*)Allocate(samplingProbVectorByteSize);
		VERIFY(samplingProbVector != NULL);
		memset(samplingProbVector, 0, samplingProbVectorByteSize);

		/**** -- (b) ET given P -- ****/
		// Generate ET^{0}
		for(ull trace = 0; trace < numTraces; trace++)
		{
			TraceVector tvec = learningTraces[trace];
			ull* learningTrace = tvec.trace; VERIFY(learningTrace != NULL);
			ull minTime = tvec.offset;
			ull numTimes = tvec.length;
			ull maxTime = minTime + numTimes - 1;

			TraceVector tvec2 = estimatedTraces[trace];
			ull* estimatedTrace = tvec2.trace; VERIFY(estimatedTrace != NULL);
			VERIFY(minTime == tvec2.offset && numTimes == tvec2.length);

			double** gapsMatrixArray = NULL;

			// here we want to generate a feasible initial sample (i.e. a trace of probability > 0)
			// first we look over the trace and collect information about the gaps (i.e. the tps that they are composed of)
			map<ull, vector<ull> > gapsTPsVectorMap = map<ull, vector<ull> >();
			for(ull tm = minTime; tm <= maxTime; tm++)
			{
				ull loc = learningTrace[(tm - minTime)];

				if(loc == 0) // no loc available (loc == 0)
				{
					ull key = tm;
					vector<ull> tpVector = vector<ull>();

					while(loc == 0)
					{
						ull tp = params->LookupTimePeriod(tm, true);
						if(tp == INVALID_TIME_PERIOD)
						{
							SET_ERROR_CODE(ERROR_CODE_INCONSISTENT_TIME_PARTITIONING_USAGE);
							return false;
						}

						tpVector.push_back(tp);

						if(tm == maxTime) { break; }

						tm++;
						loc = learningTrace[(tm - minTime)];
					}

					pair<ull, vector<ull> > timeTPVectorPair = pair<ull, vector<ull> >(key, tpVector);
					gapsTPsVectorMap.insert(timeTPVectorPair);
				}
			}

			// In a second step, we fill missing events of each gap using a strategy which depends on the nature of the gap
			pair_foreach_const(map<ull, vector<ull> >, gapsTPsVectorMap, iterGaps)
			{
				ull firstTime = iterGaps->first;
				vector<ull> tpVector = iterGaps->second;

				ull lastTime = firstTime + tpVector.size() - 1;
				ull prevTime = firstTime - 1;
				ull nextTime = lastTime + 1;

				ull prevLoc = 0; ull prevTp = 0;
				ull nextLoc = 0; ull nextTp = 0;

				if(prevTime >= minTime) { prevLoc = learningTrace[(prevTime - minTime)]; prevTp = params->LookupTimePeriod(prevTime, true); }
				if(nextTime <= maxTime) { nextLoc = learningTrace[(nextTime - minTime)]; nextTp = params->LookupTimePeriod(nextTime, true); }

				// switch over the different cases
				if(nextLoc == 0) // fill forward
				{
					if(prevLoc == 0) // both are zero -> trace is empty -> fill forward without constraints
					{
						ull prevFilledLoc = 0;
						ull prevFilledTp = 0;
						for(ull tm = firstTime; tm <= lastTime; tm++)
						{
							double* tmpProbVector = NULL;
							ull loc = 0;

							ull gapElementIdx = tm - firstTime;
							ull tp = tpVector[gapElementIdx];

							if(gapElementIdx == 0) // use steady state
							{
								VERIFY(Algorithms::GetSteadyStateVectorOfSubChain(steadyStateVector, tp, &tmpProbVector, true) == true);
							}
							else // use the transition vector (transition probability conditional on the current tp, the next tp, and the current loc)
							{
								VERIFY(Algorithms::GetTransitionVectorOfSubChain(transitionMatrix, prevFilledTp, prevFilledLoc, tp, &tmpProbVector, true) == true);
							}

							loc = rng->SampleIndexFromVector(tmpProbVector, numLoc) + minLoc; // get sample

							estimatedTrace[(tm - minTime)] = loc; // fill in the sampled location

							prevFilledLoc = loc;
							prevFilledTp = tp;

							Free(tmpProbVector);
						}
					}
					else  // we have a previous loc, but no next loc -> fill forward with prev loc constraint
					{
						ull prevFilledLoc = prevLoc;
						ull prevFilledTp = prevTp;
						for(ull tm = firstTime; tm <= lastTime; tm++)
						{
							double* tmpProbVector = NULL;
							ull loc = 0;

							ull gapElementIdx = tm - firstTime;
							ull tp = tpVector[gapElementIdx];

							 // use the transition vector (transition probability conditional on the current tp, the next tp, and the current loc)
							VERIFY(Algorithms::GetTransitionVectorOfSubChain(transitionMatrix, prevFilledTp, prevFilledLoc, tp, &tmpProbVector, true) == true);

							loc = rng->SampleIndexFromVector(tmpProbVector, numLoc) + minLoc; // get sample

							estimatedTrace[(tm - minTime)] = loc; // fill in the sampled location

							prevFilledLoc = loc;
							prevFilledTp = tp;

							Free(tmpProbVector);
						}
					}
				}
				else // we have a next loc -> fill forward but we need to compute n steps transitions matrices
				{
					ull gapSize = lastTime - firstTime + 1;

					// here we have to compute the i steps time period dependent transition matrix, for i=1, 2, ..., gapSize.
					ull gapsMatrixByteSize = numLoc * numLoc * sizeof(double);
					ull gapsMatrixArrayByteSize = gapSize * sizeof(double*);

					gapsMatrixArray = (double**)Allocate(gapsMatrixArrayByteSize);
					VERIFY(gapsMatrixArray != NULL);
					memset(gapsMatrixArray, 0, gapsMatrixArrayByteSize);

					ull lastGapElementIdx = gapSize - 1;
					for(ull tm = lastTime; tm >= firstTime; tm--)
					{
						ull gapElementIdx = tm - firstTime;
						ull tp = tpVector[gapElementIdx];
						ull tp2 = (tm+1 <= lastTime) ? tpVector[gapElementIdx+1] : nextTp;

						 double* subMatrix = (double*)Allocate(gapsMatrixByteSize);
						 VERIFY(subMatrix != NULL);
						 memset(subMatrix, 0, gapsMatrixByteSize);

						 for(ull loc1Idx = 0; loc1Idx < numLoc; loc1Idx++)
						 {
							 ull transMatrixRowIdx = GET_INDEX((tp - minPeriod) * numLoc + loc1Idx, (tp2 - minPeriod) * numLoc, numStatesInclDummies);
							 memcpy(&subMatrix[loc1Idx * numLoc], &transitionMatrix[transMatrixRowIdx], numLoc * sizeof(double));

							 NORMALIZE_VECTOR(&subMatrix[loc1Idx * numLoc], numLoc); // renormalize
						 }

						 if(gapElementIdx == lastGapElementIdx)
						 {
							 gapsMatrixArray[gapElementIdx] = subMatrix;
						 }
						 else
						 {
							 double* nextMatrix = gapsMatrixArray[gapElementIdx+1];
							 VERIFY(nextMatrix != NULL);

							 double* prevMatrix = (double*)Allocate(gapsMatrixByteSize);
							 VERIFY(prevMatrix != NULL);
							 memset(prevMatrix, 0, gapsMatrixByteSize);

							 Algorithms::MultiplySquareMatrices(subMatrix, nextMatrix, numLoc, prevMatrix);

							 gapsMatrixArray[gapElementIdx] = prevMatrix;

							 Free(subMatrix);
						 }
					}

					// we take care of both prevLoc == 0 and prevLoc != 0 with the code below:
					ull prevFilledLoc = prevLoc;
					ull prevFilledTp = prevTp;
					for(ull tm = firstTime; tm <= lastTime; tm++)
					{
						ull gapElementIdx = tm - firstTime;
						ull tp = tpVector[gapElementIdx];

						double* outgoingMatrix = gapsMatrixArray[gapElementIdx];
						VERIFY(outgoingMatrix != NULL);

						double* incomingVector = NULL;
						if(tm == firstTime && prevLoc == 0) // we have no previous loc, so the incoming vector is the steady-state vector
						{
							VERIFY(Algorithms::GetSteadyStateVectorOfSubChain(steadyStateVector, tp, &incomingVector, true) == true);
						}
						else
						{
							VERIFY(Algorithms::GetTransitionVectorOfSubChain(transitionMatrix, prevFilledTp, prevFilledLoc, tp, &incomingVector, true) == true);
						}

						ull loc = 0;
						ull nextLocIdx = (nextLoc - minLoc);
						double sum = 0.0;
						for(ull loc2 = minLoc; loc2 <= maxLoc; loc2++)
						{
							ull loc2Idx = (loc2 - minLoc);

							// compute the probability of going to loc2 at time tp (from prevFilledLoc, at time prevFilledTp) and then from loc2
							// going to nextLoc at time nextTp (going over lastTime - tm + 1 steps at times tpVector[gapElementIdx], tpVector[gapElementIdx+1], ...)
							double incomingOutgoingProb = incomingVector[loc2Idx] * outgoingMatrix[GET_INDEX(loc2Idx, nextLocIdx, numLoc)];
							samplingProbVector[loc2Idx] = incomingOutgoingProb;

							sum += incomingOutgoingProb;
						}

						if(sum == 0.0) // impossible trace
						{
							SET_ERROR_CODE(ERROR_CODE_IMPOSSIBLE_TRACE);

							Free(transitionMatrix);
							Free(transitionMatrixSum);
							Free(steadyStateVector);

							Free(count);
							foreach_const(vector<TraceVector>, estimatedTraces, iterTV)	{ TraceVector tvec = *iterTV; Free(tvec.trace); }
							Free(samplingProbVector);
							Free(incomingVector);

							for(ull gapElementIdx = 0; gapElementIdx < gapSize; gapElementIdx++) { Free(gapsMatrixArray[gapElementIdx]); }
							Free(gapsMatrixArray);

							return false;
						}

						NORMALIZE_VECTOR(samplingProbVector, numLoc);

						loc = rng->SampleIndexFromVector(samplingProbVector, numLoc) + minLoc; // get sample

						estimatedTrace[(tm - minTime)] = loc; // fill in the sampled location

						prevFilledLoc = loc;
						prevFilledTp = tp;

						Free(incomingVector);
					}

					// cleanup
					for(ull gapElementIdx = 0; gapElementIdx < gapSize; gapElementIdx++) { Free(gapsMatrixArray[gapElementIdx]); }
					Free(gapsMatrixArray);
				}
			}
		}
	}

	// Steps 1, 2, ...
	while(true)
	{
#define GS_MINIMUM_STEPS 2
		// break if either the max number of GS iteration have been reached or the time limit has been exceeded
		if(step == maxGSPerUser && maxGSPerUser != KC_NO_LIMITS) { break; }
		else if(maxSecondsPerUser != KC_NO_LIMITS && step >= GS_MINIMUM_STEPS && (time(NULL) - startTime) > maxSecondsPerUser) { break; }
#undef GS_MINIMUM_STEPS

		if(allFull == false) // if traces are all full, no need to recount
		{
			/**** -- (a) P given ET, CM -- ****/
			memcpy(count, priorTransitionsCount, countByteSize);  // put prior transitions count in count

			// count the number of transitions in ET^{step - 1}
			foreach_const(vector<TraceVector>, estimatedTraces, iterTV)
			{
				TraceVector tvec = *iterTV;
				ull* estimatedTrace = tvec.trace; VERIFY(estimatedTrace != NULL);
				ull minTime = tvec.offset;
				ull numTimes = tvec.length;
				ull maxTime = minTime + numTimes - 1;

				for(ull tm = minTime; tm <= maxTime; tm++) // go to maxTime, but with wrap-around
				{
					ull nexttm = tm == maxTime ? minTime : (tm + 1); // next timestamp (with wrap-around)

					ull startLoc = estimatedTrace[(tm - minTime)];
					ull endLoc = estimatedTrace[(nexttm - minTime)];

					VERIFY(startLoc != 0 && endLoc != 0);

					ull startTP = params->LookupTimePeriod(tm, true);
					ull endTP = params->LookupTimePeriod(nexttm, true);
					if(startTP == INVALID_TIME_PERIOD || endTP == INVALID_TIME_PERIOD)
					{
						SET_ERROR_CODE(ERROR_CODE_INCONSISTENT_TIME_PARTITIONING_USAGE);
						return false;
					}

					ull startTPIdx = (startTP - minPeriod); ull endTPIdx = (endTP - minPeriod);
					ull startLocIdx = (startLoc - minLoc); ull endLocIdx = (endLoc - minLoc);

					ull startStateIdx = startTPIdx * numLoc + startLocIdx;
					ull endStateIdx = endTPIdx * numLoc + endLocIdx;
					ull countIdx = GET_INDEX(startStateIdx, endStateIdx, numStatesInclDummies);
					count[countIdx]++;
				}
			}
		}

		// Generate P^{step}
		VERIFY(TransitionMatrixFromCountMatrix(count, alpha, theta, transitionMatrix, true) == true);

		// Add P^{step} to PSUM
		for(ull stateIdx1 = 0; stateIdx1 < numStatesInclDummies; stateIdx1++)
		{
			for(ull stateIdx2 = 0; stateIdx2 < numStatesInclDummies; stateIdx2++)
			{
				ull index = GET_INDEX(stateIdx1, stateIdx2, numStatesInclDummies);
				transitionMatrixSum[index] += transitionMatrix[index];
			}
		}

		if(computationNeedsSteadyState == true) { ComputeSteadyStateVector(transitionMatrix, steadyStateVector); }

		// update variance matrix
		for(ull stateIdx1 = 0; stateIdx1 < numStates; stateIdx1++)
		{
			for(ull stateIdx2 = 0; stateIdx2 < numStates; stateIdx2++)
			{
				ull transMatrixIdx = GET_INDEX(stateIdx1, stateIdx2, numStatesInclDummies);
				ull idx = GET_INDEX(stateIdx1, stateIdx2, numStates);

				double p = transitionMatrix[transMatrixIdx];
				varianceMatrix[idx] += p*p;
			}
		}

		if(allFull == false)
		{
			/**** -- (b) ET given P -- ****/
			// Generate ET^{step}
			for(ull trace = 0; trace < numTraces; trace++)
			{
				TraceVector tvec = learningTraces[trace];
				ull* learningTrace = tvec.trace; VERIFY(learningTrace != NULL);
				ull minTime = tvec.offset;
				ull numTimes = tvec.length;
				ull maxTime = minTime + numTimes - 1;

				TraceVector tvec2 = estimatedTraces[trace];
				ull* estimatedTrace = tvec2.trace; VERIFY(estimatedTrace != NULL);
				VERIFY(minTime == tvec2.offset && numTimes == tvec2.length);

				for(ull tm = minTime; tm <= maxTime; tm++)
				{
					ull loc = learningTrace[(tm - minTime)];

					// Lookup the proper time periods
					ull tp = params->LookupTimePeriod(tm, true);
					ull prevtp = tp;
					if(tm > minTime) { prevtp = params->LookupTimePeriod(tm-1, true); }
					ull nexttp = tp;
					if(tm < maxTime) { nexttp = params->LookupTimePeriod(tm+1, true); }
					if(prevtp == INVALID_TIME_PERIOD || tp == INVALID_TIME_PERIOD || nexttp == INVALID_TIME_PERIOD)
					{
						SET_ERROR_CODE(ERROR_CODE_INCONSISTENT_TIME_PARTITIONING_USAGE);
						return false;
					}

					if(loc == 0) // no loc available (loc == 0)
					{
						double* probVector = samplingProbVector;

						double sum = 0.0;
						if(tm == minTime) // first event
						{
							double* tmpSSV = NULL; // note: in this case, since tm == minTime, we have prevtp == tp
							VERIFY(Algorithms::GetSteadyStateVectorOfSubChain(steadyStateVector, tp, &tmpSSV, true) == true);

							for(ull loc2 = minLoc; loc2 <= numLoc; loc2++)
							{
								ull locIndex = loc2 - minLoc;
								// probability of being at loc2 (at time tm, i.e. in time period tp == prevtp)
								double presenceProb = tmpSSV[locIndex];

								ull nextLocIndex = ((tm + 1) - minTime);
								ull nextLoc = estimatedTrace[nextLocIndex];

								// probability of going from loc2 (at time tm, i.e. in time period tp == prevtp) to nextLoc (at time tm+1, i.e. in time period nexttp)
								double* transSubVector = NULL;
								VERIFY(Algorithms::GetTransitionVectorOfSubChain(transitionMatrix, tp, loc2, nexttp, &transSubVector, true) == true);
								double transProb = transSubVector[(nextLoc - minLoc)];
								Free(transSubVector); // free the sub-vector

								double product = presenceProb * transProb;

								probVector[locIndex] = product;
								sum += product;
							}

							Free(tmpSSV); // free the sub-chain steady-state vector
						}
						else if(tm == maxTime) // last event
						{
							ull prevLocTmIndex = ((tm - 1) - minTime);
							ull prevLoc = estimatedTrace[prevLocTmIndex];

							double* transSubVector = NULL;
							VERIFY(Algorithms::GetTransitionVectorOfSubChain(transitionMatrix, prevtp, prevLoc, tp, &transSubVector, true) == true);

							for(ull loc2 = minLoc; loc2 <= numLoc; loc2++)
							{
								ull locIndex = loc2 - minLoc;

								// probability of going from prevLoc (at time tm-1, i.e. in time period prevtp) to loc2 (at time tm, i.e. in time period tp)
								double transProb = transSubVector[locIndex];

								probVector[locIndex] = transProb;
								sum += transProb;
							}
							Free(transSubVector); // free the sub-vector
						}
						else // middle
						{

							ull prevLocTmIndex = ((tm - 1) - minTime);
							ull prevLoc = estimatedTrace[prevLocTmIndex];

							ull nextLocTmIndex = ((tm + 1) - minTime);
							ull nextLoc = estimatedTrace[nextLocTmIndex];

							double* transSubVector = NULL;
							VERIFY(Algorithms::GetTransitionVectorOfSubChain(transitionMatrix, prevtp, prevLoc, tp, &transSubVector, true) == true);

							// next step
							// GetIntermediaryTransitionVector(intermProbVectorCache, transitionMatrix, prevLoc, nextLoc, prevTp, tp, nextTp, &transSubVector);

							for(ull loc2 = minLoc; loc2 <= numLoc; loc2++)
							{
								ull locIndex = loc2 - minLoc;

								// probability of going from prevLoc (at time tm-1, i.e. in time period prevtp) to loc2 (at time tm, i.e. in time period tp)
								double prevTransProb = transSubVector[locIndex];

								// probability of going from loc2 (at time tm, i.e. in time period tp) to nextLoc (at time tm+1, i.e. in time period nexttp)
								double* nextTransSubVector = NULL;
								VERIFY(Algorithms::GetTransitionVectorOfSubChain(transitionMatrix, tp, loc2, nexttp, &nextTransSubVector, true) == true);
								double nextTransProb = nextTransSubVector[(nextLoc - minLoc)];
								Free(nextTransSubVector); // free the sub-vector

								double product = prevTransProb * nextTransProb;

								probVector[locIndex] = product;
								sum += product;
							}
							Free(transSubVector); // free the sub-vector
						}


						NORMALIZE_VECTOR(probVector, numLoc);  // normalization
						// for(ull loc2 = minLoc; loc2 <= numLoc; loc2++){ probVector[loc2 - minLoc] /= sum; }

						// get the loc by sampling from probVector
						loc = minLoc + RNG::GetInstance()->SampleIndexFromVector(probVector, numLoc);
						estimatedTrace[(tm - minTime)] = loc;
					}
				}
			}
		}

		step++;
	}
	if(samplingProbVector != NULL) { Free(samplingProbVector); }
	foreach_const(vector<TraceVector>, estimatedTraces, iterTV)	{ TraceVector tvec = *iterTV; Free(tvec.trace); }

	info.str("");
	info << "Finished Gibbs Sampling for user " << user << " after " << step << " iterations (" << (time(NULL) - startTime) << " seconds)!";
	Log::GetInstance()->Append(info.str());

	Free(transitionMatrix);
	Free(count);


	if(step > 1)
	{
		// average the transition matrix
		for(ull stateIdx1 = 0; stateIdx1 < numStatesInclDummies; stateIdx1++)
		{
			for(ull stateIdx2 = 0; stateIdx2 < numStatesInclDummies; stateIdx2++)
			{
				ull index = GET_INDEX(stateIdx1, stateIdx2, numStatesInclDummies);
				transitionMatrixSum[index] /= step;
			}
		}

		// adjust variance matrix
		for(ull stateIdx1 = 0; stateIdx1 < numStates; stateIdx1++)
		{
			for(ull stateIdx2 = 0; stateIdx2 < numStates; stateIdx2++)
			{
				ull index = GET_INDEX(stateIdx1, stateIdx2, numStates);
				varianceMatrix[index] /= step;
			}
		}
	}

	ComputeSteadyStateVector(transitionMatrixSum, steadyStateVector);

	ull maxPeriod = tpInfo.maxPeriod;

	// if we have dummy tps then we need to extract the transition matrix and steady state vector over non-dummy tps
	if(numStates != numStatesInclDummies)
	{
		// extract the transition matrix for non-dummy tps (exclude dummy tps)
		ull transitionMatrixByteSize = numStates * numStates * sizeof(double);
		transitionMatrix = (double*)Allocate(transitionMatrixByteSize);
		VERIFY(transitionMatrix != NULL);
		memset(transitionMatrix, 0, transitionMatrixByteSize);

		for(ull tp1 = minPeriod; tp1 <= maxPeriod; tp1++)
		{
			for(ull loc1 = minLoc; loc1 <= maxLoc; loc1++)
			{
				ull state1Idx = (tp1 - minPeriod) * numLoc + (loc1 - minLoc);

				double sum = 0.0;
				for(ull tp2 = minPeriod; tp2 <= maxPeriod; tp2++)
				{
					for(ull loc2 = minLoc; loc2 <= maxLoc; loc2++)
					{
						ull state2Idx = (tp2 - minPeriod) * numLoc + (loc2 - minLoc);

						ull subMatrixIdx = GET_INDEX(state1Idx, state2Idx, numStates);
						ull fullMatrixIdx = GET_INDEX(state1Idx, state2Idx, numStatesInclDummies);

						transitionMatrix[subMatrixIdx] = transitionMatrixSum[fullMatrixIdx];
						sum += transitionMatrix[subMatrixIdx];
					}
				}

				// normalize
				double check = 0.0;
				for(ull tp2 = minPeriod; tp2 <= maxPeriod; tp2++)
				{
					for(ull loc2 = minLoc; loc2 <= maxLoc; loc2++)
					{
						ull state2Idx = (tp2 - minPeriod) * numLoc + (loc2 - minLoc);

						ull subMatrixIdx = GET_INDEX(state1Idx, state2Idx, numStates);
						transitionMatrix[subMatrixIdx] /= sum;
						check += transitionMatrix[subMatrixIdx];
					}
				}
				VERIFY(abs(check - 1) < EPSILON);


				// adjust variance
				for(ull tp2 = minPeriod; tp2 <= maxPeriod; tp2++)
				{
					for(ull loc2 = minLoc; loc2 <= maxLoc; loc2++)
					{
						ull state2Idx = (tp2 - minPeriod) * numLoc + (loc2 - minLoc);

						ull subMatrixIdx = GET_INDEX(state1Idx, state2Idx, numStates);

						varianceMatrix[subMatrixIdx] /= sum*sum;
					}
				}

			}
		}
		Free(transitionMatrixSum);


		// extract the steady state vector for non-dummy tps (exclude dummy tps)
		ull steadyStateVectorByteSize = numStates * sizeof(double);
		double* subSteadyStateVector = (double*)Allocate(steadyStateVectorByteSize);
		VERIFY(subSteadyStateVector != NULL);
		memset(subSteadyStateVector, 0, steadyStateVectorByteSize);

		double sum = 0.0;
		for(ull tp = minPeriod; tp <= maxPeriod; tp++)
		{
			for(ull loc = minLoc; loc <= maxLoc; loc++)
			{
				ull vectorIdx = GET_INDEX(tp - minPeriod, loc - minLoc, numLoc);
				subSteadyStateVector[vectorIdx] = steadyStateVector[vectorIdx]; // note: no difference in index, this is normal
				sum += subSteadyStateVector[vectorIdx];
			}
		}

		// normalize
		for(ull tp = minPeriod; tp <= maxPeriod; tp++)
		{
			for(ull loc = minLoc; loc <= maxLoc; loc++)
			{
				ull vectorIdx = GET_INDEX(tp - minPeriod, loc - minLoc, numLoc);
				subSteadyStateVector[vectorIdx] /= sum;
			}
		}
		Free(steadyStateVector);

		// set output
		profile->SetTransitionMatrix(transitionMatrix);
		profile->SetSteadyStateVector(subSteadyStateVector);
		profile->SetAccuracyInfo(step, varianceMatrix);
	}
	else
	{
		// set output
		profile->SetTransitionMatrix(transitionMatrixSum);
		profile->SetSteadyStateVector(steadyStateVector);
		profile->SetAccuracyInfo(step, varianceMatrix);
	}

	return true;

  // Bouml preserved body end 0007E491
}

bool CreateContextOperation::ReadKnowledgeFiles(const KnowledgeInput* input, map<ull, vector<TraceVector> >& learningTraces, map<ull, double*>& priorTransitionsCount, bool** transitionsFeasibilityMatrix) 
{
  // Bouml preserved body begin 00081B11

	VERIFY(input != NULL);

	learningTraces.clear();
	priorTransitionsCount.clear();

	File* transFeasibilityFile = input->transitionsFeasibilityFile;
	File* transitionsCountFile = input->transitionsCountFile;
	vector<File*> learningTracesFileVector = input->learningTraceFilesVector;

	vector<ull> values = vector<ull>();

	Parameters* params = Parameters::GetInstance();

	// the location and time partitioning are given by the (human) user.
	// However, the users IDs need to be computed from the learning traces
	ull minLoc = 0; ull maxLoc = 0;
	VERIFY(params->GetLocationstampsRange(&minLoc, &maxLoc) == true);
	ull numLoc = maxLoc - minLoc + 1;

	// get time period parameters
	ull numPeriods = 0; TPInfo tpInfo;
	VERIFY(params->GetTimePeriodInfo(&numPeriods, &tpInfo) == true);

	ull numStates = numPeriods * numLoc;

	const double epsilon = 0.01/(numLoc * tpInfo.numPeriodsInclDummies);

	// learningTraces
	if(ReadLearningTraces(learningTracesFileVector, learningTraces) == false)
	{
		return false; // nothing allocated yet at this point
	}

	set<ull> users = set<ull>();

	// add users found in learning traces files to the set of users
	pair_foreach_const(map<ull, vector<TraceVector> >, learningTraces, iter) { users.insert(iter->first); }

	if(ReadTransitionsCount(transitionsCountFile, priorTransitionsCount) == false)
	{
		// Free learningsTraces
		pair_foreach_const(map<ull, vector<TraceVector> >, learningTraces, iter)
		{
			vector<TraceVector> traces = iter->second;
			foreach_const(vector<TraceVector>, traces, iterVector) { Free((*iterVector).trace); }
		}
		learningTraces.clear();

		return false;
	}

	// add users found in transitions count file to the set of users
	pair_foreach_const(map<ull, double*>, priorTransitionsCount, iter) { users.insert(iter->first); }

	// iterate over all users and complete learningsTraces and priorTransitionsCount
	foreach_const(set<ull>, users, usersIter)
	{
		ull user = *usersIter;

		if(learningTraces.find(user) == learningTraces.end())
		{
			// create empty trace
			TraceVector vec;
			vec.offset = tpInfo.canonicalPartitionParentNode->GetOffset(true);
			vec.length = tpInfo.canonicalPartitionParentNode->GetLength();

			ull traceByteSize = vec.length * sizeof(ull);
			ull* trace = vec.trace = (ull*)Allocate(traceByteSize);
			VERIFY(trace != NULL);
			memset(trace, 0, traceByteSize);

			vector<TraceVector> tvecs = vector<TraceVector>();
			tvecs.push_back(vec);

			learningTraces.insert(pair<ull, vector<TraceVector> >(user, tvecs));

			VERIFY(priorTransitionsCount.find(user) != priorTransitionsCount.end());
		}

		if(priorTransitionsCount.find(user) == priorTransitionsCount.end())
		{
			// create empty transitionsCount matrix (numStates x numStates), i.e. it includes time periods but only non-dummy ones
			ull transitionsCountByteSize = numStates * numStates * sizeof(double);
			double* transitionsCount = (double*)Allocate(transitionsCountByteSize);
			VERIFY(transitionsCount != NULL);
			memset(transitionsCount, 0, transitionsCountByteSize);

			priorTransitionsCount.insert(pair<ull, double*>(user, transitionsCount));

			VERIFY(learningTraces.find(user) != learningTraces.end());
		}
	}

	// the transitions feasibility matrix is a numLoc x numLoc (not numStates x numStates), i.e. it applies to all time periods!
	ull transFeasibilityByteSize = numLoc * numLoc * sizeof(bool);
	bool* transFeasibility = (bool*)Allocate(transFeasibilityByteSize);
	VERIFY(transFeasibility != NULL);
	memset(transFeasibility, 0, transFeasibilityByteSize);

	stringstream info("");
	info << "transFeasibility = " <<  hex << transFeasibility << " | transFeasibilityByteSize = " << dec << transFeasibilityByteSize;
	info << " | sizeof(size_t) = " << sizeof(size_t) << " | sizeof(ull) = " << sizeof(ull);
	Log::GetInstance()->Append(info.str());

	if(ReadTransitionsFeasibility(transFeasibilityFile, transFeasibility) == false)
	{
		Free(transFeasibility);

		// Free learningsTraces
		pair_foreach_const(map<ull, vector<TraceVector> >, learningTraces, iter)
		{
			vector<TraceVector> traces = iter->second;
			foreach_const(vector<TraceVector>, traces, iterVector) { Free((*iterVector).trace); }
		}
		learningTraces.clear();

		// Free priorTransitionsCount
		pair_foreach_const(map<ull, double*>, priorTransitionsCount, iter){ Free(iter->second); }
		priorTransitionsCount.clear();

		return false;
	}

	ull numPeriodsInclDummies = tpInfo.numPeriodsInclDummies;
	ull numStatesInclDummies = numPeriodsInclDummies * numLoc;

	// complete the priorTransitionCount using the transitions feasibility matrix and extend it to include dummy tps
	pair_foreach(map<ull, double*>, priorTransitionsCount, iter)
	{
		double* transitionsCount = iter->second;

		ull extTransCountByteSize = numStatesInclDummies * numStatesInclDummies * sizeof(double);
		double* extTransCount = (double*)Allocate(extTransCountByteSize);
		VERIFY(extTransCount != NULL);
		memset(extTransCount, 0, extTransCountByteSize);

		iter->second = extTransCount;

		// add epsilon according to the transitions feasibility matrix
		for(ull loc = minLoc; loc <= maxLoc; loc++)
		{
			ull numDestLoc = 0; // number of possible destinations regions from loc.
			for(ull loc2 = minLoc; loc2 <= maxLoc; loc2++)
			{
				ull index = GET_INDEX((loc - minLoc), (loc2 - minLoc), numLoc);
				if(transFeasibility[index] == true) { numDestLoc++; }
			}

			if(numDestLoc == 0) { continue; }

			for(ull loc2 = minLoc; loc2 <= maxLoc; loc2++)
			{
				ull index = GET_INDEX((loc - minLoc), (loc2 - minLoc), numLoc);

				if(transFeasibility[index] == true)
				{
					for(ull tp1Idx = 0; tp1Idx < numPeriodsInclDummies; tp1Idx++)
					{
						for(ull tp2Idx = 0; tp2Idx < numPeriodsInclDummies; tp2Idx++)
						{
							ull tpMatrixIdx = GET_INDEX(tp1Idx, tp2Idx, numPeriodsInclDummies);
							double effectiveEpsilon = epsilon;

							// if you can't (time-semantically) go from tp1 to tp2, we should not add epsilon
							if(tpInfo.propTransMatrix[tpMatrixIdx] == 0) { effectiveEpsilon = 0; }

							ull countIdx = GET_INDEX(tp1Idx * numLoc + (loc - minLoc), tp2Idx * numLoc + (loc2 - minLoc), numStatesInclDummies);
							if(tp1Idx < numPeriods && tp2Idx < numPeriods) // for non-dummy tps, copy the info, for dummy ones, it is initialized with 0
							{
								extTransCount[countIdx] = transitionsCount[GET_INDEX(tp1Idx * numLoc + (loc - minLoc), tp2Idx * numLoc + (loc2 - minLoc), numStates)];
							}

							extTransCount[countIdx] += effectiveEpsilon; // (effectiveEpsilon / numDestLoc); // not yet
						}
					}
				}
			}
		}

		Free(transitionsCount);
	}

	if(transitionsFeasibilityMatrix != NULL) { *transitionsFeasibilityMatrix = transFeasibility; } // retrieve the transitions feasibility matrix
	else { Free(transFeasibility); }

	return true;

  // Bouml preserved body end 00081B11
}

bool CreateContextOperation::ReadTransitionsFeasibility(const File* transFeasibilityFile, bool* transFeasibilityMatrix) 
{
  // Bouml preserved body begin 00081E91

	VERIFY(transFeasibilityMatrix != NULL);

	ull minLoc = 0; ull maxLoc = 0;
	VERIFY(Parameters::GetInstance()->GetLocationstampsRange(&minLoc, &maxLoc) == true);
	ull numLoc = maxLoc - minLoc + 1;

	stringstream info("");
	info << "transFeasibilityFile == " << hex << transFeasibilityFile;
	Log::GetInstance()->Append(info.str());

	info.str("");
	info << "transFeasibilityMatrix == " << hex << transFeasibilityMatrix;
	Log::GetInstance()->Append(info.str());

	if(transFeasibilityFile == NULL)
	{
		 // no mobility file provided, assume that every transition from loc to loc2 for all loc, loc2 is possible
		for(ull loc = minLoc; loc <= maxLoc; loc++)
		{
			for(ull loc2 = minLoc; loc2 <= maxLoc; loc2++)
			{
				ull index = GET_INDEX((loc - minLoc), (loc2 - minLoc), numLoc);
				transFeasibilityMatrix[index] = true;
			}
		}

		return true;
	}

	VERIFY(transFeasibilityFile->IsGood() == true);

	for(ull loc = minLoc; loc <= maxLoc; loc++)
	{
		string line = "";
		if(transFeasibilityFile->ReadNextLine(line) == false)
		{
			SET_ERROR_CODE(ERROR_CODE_INCOMPLETE_INPUT_FILE);
			return false;
		}

		vector<ull> values = vector<ull>();
		size_t pos = 0;
		if(LineParser<ull>::GetInstance()->ParseFields(line, values, numLoc, &pos) == false || pos != string::npos)
		{
			SET_ERROR_CODE(ERROR_CODE_INVALID_FORMAT);
			return false;
		}

		for(ull loc2 = minLoc; loc2 <= maxLoc; loc2++)
		{
			ull feasibilityValue = values[loc2 - minLoc];

			if(feasibilityValue != 0 && feasibilityValue != 1)
			{
				SET_ERROR_CODE(ERROR_CODE_INVALID_FORMAT);
				return false;
			}

			ull index = GET_INDEX((loc - minLoc), (loc2 - minLoc), numLoc);
			transFeasibilityMatrix[index] = (feasibilityValue == 0) ? false : true;
		}
	}

	return true;

  // Bouml preserved body end 00081E91
}

bool CreateContextOperation::ReadTransitionsCount(const File* transitionsCountFile, map<ull, double*>& priorTransitionsCount) 
{
  // Bouml preserved body begin 00081F11

	priorTransitionsCount.clear();

	Parameters* params = Parameters::GetInstance();

	// get location parameters
	ull minLoc = 0; ull maxLoc = 0;
	VERIFY(params->GetLocationstampsRange(&minLoc, &maxLoc) == true);
	ull numLoc = maxLoc - minLoc + 1;

	// get time period parameters
	ull numPeriods = 0; // ull minPeriod = 1;
	VERIFY(params->GetTimePeriodInfo(&numPeriods, NULL) == true);
	// ull maxPeriod = minPeriod + numPeriods - 1;

	ull numStates = numPeriods * numLoc;


	// if there is no transitions count file, we do nothing
	if(transitionsCountFile != NULL)
	{
		VERIFY(transitionsCountFile->IsGood() == true);

		while(transitionsCountFile->IsGood())
		{
			string line = "";

			bool readOk = transitionsCountFile->ReadNextLine(line);

			if(priorTransitionsCount.size() > 0 && transitionsCountFile->IsEOF() == true) { break; } // EOF
			else if(readOk == false)
			{
				SET_ERROR_CODE(ERROR_CODE_INCOMPLETE_INPUT_FILE);
				goto CCOP_RTC_CLEANUP;
				return false;
			}

			ull user = 0;

			size_t pos = 0;
			if(LineParser<ull>::GetInstance()->ParseValue(line, &user, &pos) == false || pos != string::npos || user == 0)
			{
				SET_ERROR_CODE(ERROR_CODE_INVALID_FORMAT);
				goto CCOP_RTC_CLEANUP;
				return false;
			}


			if(priorTransitionsCount.find(user) != priorTransitionsCount.end()) // duplicate user
			{
				SET_ERROR_CODE(ERROR_CODE_INVALID_FORMAT);
				goto CCOP_RTC_CLEANUP;
				return false;
			}

			 // read one empty line
			if(transitionsCountFile->ReadNextLine(line) == false || line.empty() == false)
			{
				SET_ERROR_CODE(ERROR_CODE_INVALID_FORMAT);
				goto CCOP_RTC_CLEANUP;
				return false;
			}

			// transitionsCount
			ull transitionsCountByteSize = numStates * numStates * sizeof(double);
			double* transitionsCount = (double*)Allocate(transitionsCountByteSize);
			VERIFY(transitionsCount != NULL);
			memset(transitionsCount, 0, transitionsCountByteSize);

			priorTransitionsCount.insert(pair<ull, double*>(user, transitionsCount));

			// Recall that the matrix includes only non-dummy time periods
			for(ull stateIdx = 0; stateIdx < numStates; stateIdx++)
			{
				line = "";
				if(transitionsCountFile->ReadNextLine(line) == false)
				{
					SET_ERROR_CODE(ERROR_CODE_INCOMPLETE_INPUT_FILE);
					goto CCOP_RTC_CLEANUP;
					return false;
				}

				vector<ull> values = vector<ull>();
				pos = 0;
				if(LineParser<ull>::GetInstance()->ParseFields(line, values, numStates, &pos) == false || pos != string::npos)
				{
					SET_ERROR_CODE(ERROR_CODE_INCOMPLETE_INPUT_FILE);
					goto CCOP_RTC_CLEANUP;
					return false;
				}

				for(ull state2Idx = 0; state2Idx < numStates; state2Idx++)
				{
					ull index = GET_INDEX(stateIdx, state2Idx, numStates);

					double val =  (double)values[state2Idx];
					transitionsCount[index] = val;
				}
			}

			// read two empty line (break from while if EOF)
			readOk = transitionsCountFile->ReadNextLine(line);

			if(transitionsCountFile->IsEOF() == true) { break; } // EOF
			else if(readOk == false || line.empty() == false)
			{
				SET_ERROR_CODE(ERROR_CODE_INCOMPLETE_INPUT_FILE);
				goto CCOP_RTC_CLEANUP;
				return false;
			}

			readOk = transitionsCountFile->ReadNextLine(line);

			if(transitionsCountFile->IsEOF() == true) { break; } // EOF
			else if(readOk == false || line.empty() == false)
			{
				SET_ERROR_CODE(ERROR_CODE_INCOMPLETE_INPUT_FILE);
				goto CCOP_RTC_CLEANUP;
				return false;
			}
		}
	}

	return true;

CCOP_RTC_CLEANUP:

	pair_foreach_const(map<ull, double*>, priorTransitionsCount, iter){ Free(iter->second); }
	priorTransitionsCount.clear();

	return false;

  // Bouml preserved body end 00081F11
}

bool CreateContextOperation::ReadLearningTraces(const vector<File*>& learningTracesFileVector, map<ull, vector<TraceVector> >& learningTraces) 
{
  // Bouml preserved body begin 00081F91

	learningTraces.clear();

	Parameters* params = Parameters::GetInstance();

	ull minLoc = 0; ull maxLoc = 0;
	VERIFY(params->GetLocationstampsRange(&minLoc, &maxLoc) == true);
	// ull numLoc = maxLoc - minLoc + 1;

	ull minTime = 1; // minimum valid timestamp

	// get time period parameters
	ull numPeriods = 0; TPInfo tpInfo;
	VERIFY(params->GetTimePeriodInfo(&numPeriods, &tpInfo) == true);

	// collect each event from the learning trace files and assemble them into (possible partial) learning traces vectors
	vector<File*> files = learningTracesFileVector;
	foreach_const(vector<File*>, files, iter)
	{
		File* file = *iter;

		VERIFY(file != NULL && file->IsGood() == true);

		 // Use a TraceSet to temporarily store the events read from file, before we create the TraceVector
		TraceSet* traceSet = new TraceSet(ActualTrace);

		map<ull, vector<TraceVector> > traces = map<ull, vector<TraceVector> >();
		while(file->IsGood()) // read all events from file and store them in 'traceSet'
		{
			string line = "";
			if(file->ReadNextLine(line) == false)
			{
				SET_ERROR_CODE(ERROR_CODE_INCOMPLETE_INPUT_FILE);
				traceSet->Release();
				return false;
			}

			if(line.empty() == true)
			{
				if(file->IsEOF() == true) { continue; } // EOF

				file->ReadNextLine(line);

				if(file->IsEOF() == true) { continue; } // EOF
				else
				{
					SET_ERROR_CODE(ERROR_CODE_INVALID_FORMAT);
					traceSet->Release();
					return false;
				}
			}

			ActualEvent* actualEvent = NULL;
			if(EventParser::GetInstance()->ParseActualEvent(line, &actualEvent) == false)
			{
				SET_ERROR_CODE(ERROR_CODE_INVALID_FORMAT);
				traceSet->Release();
				return false;
			}

			ull user = actualEvent->GetUser();
			ull tm = actualEvent->GetTimestamp();
			ull loc = actualEvent->GetLocationstamp();

			ull tp = params->LookupTimePeriod(tm, true); // FIXFIX P.3

			if(user == 0 || tm < minTime || tp == INVALID_TIME_PERIOD || loc < minLoc || loc > maxLoc)
			{
				stringstream ss("");

				if(user == 0)
				{ ss << "found event with user id 0, 0 is not a valid user id"; }
				else if(tm < minTime) { ss << "found an event of user " << user << " with timestamp " << tm << " (out of range)";}
				else if(loc < minLoc || loc > maxLoc)
				{ ss << "the location associated with the event of user " << user << " at time " << tm << " is out of range"; }

				SET_ERROR_CODE_DETAILS(ERROR_CODE_INVALID_FORMAT, ss.str());
				traceSet->Release();
				return false;
			}

			traceSet->AddEvent(actualEvent);
			actualEvent->Release();
		}

		map<ull, Trace*> mapping = map<ull, Trace*>();
		traceSet->GetMapping(mapping);

		// In order to construct a partial trace vector we need to keep track of the TPNode (called partition parent node) to which each event belong
		pair_foreach_const(map<ull, Trace*>, mapping, iterMapping)
		{
			ull user = iterMapping->first;
			Trace* trace = iterMapping->second;

			vector<Event*> events = vector<Event*>();
			trace->GetEvents(events);

			map<TPNode*, TraceVector> partNodeTraceVectorMap = map<TPNode*, TraceVector>();

			foreach_const(vector<Event*>, events, iter) // fill in the proper trace vectors
			{
				ActualEvent* event = dynamic_cast<ActualEvent*>(*iter);
				VERIFY(event != NULL); VERIFY(user == event->GetUser());

				ull tm = event->GetTimestamp();
				ull loc = event->GetLocationstamp();

				TimePeriod absTP; TPNode* partitionParentNode = NULL;
				ull tp = params->LookupTimePeriod(tm, true, &absTP, &partitionParentNode);
				VERIFY(tp != INVALID_TIME_PERIOD);

				ull offset = partitionParentNode->GetOffset(true); // get the absolute offset
				ull len = partitionParentNode->GetLength();

				map<TPNode*, TraceVector>::iterator iter = partNodeTraceVectorMap.find(partitionParentNode);
				if(iter == partNodeTraceVectorMap.end()) // not found -> create a new trace vector
				{
					TraceVector vec;
					vec.offset = offset;
					vec.length = len;
					ull traceByteSize = len * sizeof(ull);
					ull* trace = vec.trace = (ull*)Allocate(traceByteSize);
					VERIFY(trace != NULL);
					memset(trace, 0, traceByteSize); // location being equal 0 means event is not available

					partNodeTraceVectorMap.insert(partNodeTraceVectorMap.end(), pair<TPNode*, TraceVector>(partitionParentNode, vec));
					iter = partNodeTraceVectorMap.find(partitionParentNode);
				}

				TraceVector* vec = &(iter->second);
				ull* trace = vec->trace;
				VERIFY(trace != NULL && vec->offset == offset && vec->length == len);

				// fill in this event in the trace vector
				ull traceIndex = tm - offset;
				if(trace[traceIndex] != 0) // check for duplicates
				{
					SET_ERROR_CODE(ERROR_CODE_DUPLICATE_ENTRIES);
					traceSet->Release();
					pair_foreach_const(map<ull, vector<TraceVector> >, traces, iterTraces)
					{ foreach_const(vector<TraceVector>, iterTraces->second, iterVector) { Free((*iterVector).trace); } }
					learningTraces.clear();
					return false;
				}

				trace[traceIndex] = loc;
			}

			// find / create the vector for each user
			if(learningTraces.find(user) == learningTraces.end())
			{
				learningTraces.insert(pair<ull, vector<TraceVector> >(user, vector<TraceVector>()));
			}
			vector<TraceVector>* tracesVector = &(learningTraces.find(user)->second);

			// add trace vectors to the vector of this user
			pair_foreach_const(map<TPNode*, TraceVector>, partNodeTraceVectorMap, iterMap)
			{
				tracesVector->push_back(iterMap->second);
			}
		}

		traceSet->Release(); // free the traceset
	}

	return true;

  // Bouml preserved body end 00081F91
}


} // namespace lpm
