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

#include "../include/ContextAnalysisOperations.h"
#include "../include/MetricOperation.h"
#include "../include/Context.h"
#include "../include/File.h"

namespace lpm {

PredictabilityAnalysisOperation::PredictabilityAnalysisOperation(string name, MetricDistance* distance, bool normalize) : ContextAnalysisOperation(name, distance)
{
  // Bouml preserved body begin 000B9B91

	this->normalize = normalize;

  // Bouml preserved body end 000B9B91
}

//! 
//! \brief Execute the operation
//!
//! Pure virtual method which executes the operation. 
//!
//! \tparam[[in] input 	InputType* to the input object of the operation.
//! \tparam[[in,out] output 	OutputType* to the output object of the operation.
//!
//! \return true if the operation is successful, false otherwise
//!
bool PredictabilityAnalysisOperation::Execute(const Context* input, File* output) 
{
  // Bouml preserved body begin 000B9A11

	if(distanceFunction == NULL)
	{
		SET_ERROR_CODE(ERROR_CODE_INVALID_ARGUMENTS);
		return false;
	}

	VERIFY(input != NULL && output != NULL && output->IsGood());

	Parameters* params = Parameters::GetInstance();

	// get location parameters
	ull minLoc = 0; ull maxLoc = 0;
	VERIFY(params->GetLocationstampsRange(&minLoc, &maxLoc) == true);
	ull numLoc = maxLoc - minLoc + 1;

	// get time period parameters
	ull numPeriods = 0; TPInfo tpInfo;
	VERIFY(params->GetTimePeriodInfo(&numPeriods, &tpInfo) == true);
	ull minPeriod = tpInfo.minPeriod;
	ull maxPeriod = tpInfo.maxPeriod;

	ull numStates = numPeriods * numLoc;

	// compute max distance
	double maxDistance = 0.0;
	for(ull loc = minLoc; loc <= maxLoc; loc++)
	{
		for(ull loc2 = minLoc; loc2 <= maxLoc; loc2++)
		{
			double distance = distanceFunction->ComputeDistance(loc, loc2);
			if(distance > maxDistance)
			{
				maxDistance = distance;
			}
		}
	}

	map<ull, UserProfile*> profiles = map<ull, UserProfile*>();
	VERIFY(input->GetProfiles(profiles) == true);

	pair_foreach_const(map<ull, UserProfile*>, profiles, iterProfiles)
	{
		ull user = iterProfiles->first;
		UserProfile* profile = iterProfiles->second;

		double* steadyStateVector = NULL;
		double* transitionMatrix = NULL;

		VERIFY(profile->GetSteadyStateVector(&steadyStateVector) == true);
		VERIFY(profile->GetTransitionMatrix(&transitionMatrix) == true);

		// Compute prediction error for zeroth-order (epred0) and first-order (epred1)
		double epred0 = 0.0; double epred1 = 0.0;

		// compute the adjusted stationary distribution (called \twidle{\pi})
		double* adjustedSteadyStateVector = (double*)Allocate(numPeriods * numLoc * sizeof(double));
		double* tpConditionalSteadyStateVector = (double*)Allocate(numPeriods * numLoc * sizeof(double));
		double sum = 0.0;
		for(ull tp = minPeriod; tp <= maxPeriod; tp++)
		{
			double pitp = 0.0;
			for(ull loc = minLoc; loc <= maxLoc; loc++)
			{
				ull idx = GET_INDEX((tp - minPeriod), (loc - minLoc), numLoc);
				pitp += steadyStateVector[idx];
			}

			for(ull loc = minLoc; loc <= maxLoc; loc++)
			{
				ull idx = GET_INDEX((tp - minPeriod), (loc - minLoc), numLoc);
				double piltp = steadyStateVector[idx];
				tpConditionalSteadyStateVector[idx] = piltp / pitp;
				adjustedSteadyStateVector[idx] = piltp; //(tpInfo.propTPVector[(tp - minPeriod)] / pitp) * piltp;
				sum += adjustedSteadyStateVector[idx];
			}
		}
		VERIFY(abs(sum - 1) < EPSILON);

		// zeroth-order
		for(ull tp = minPeriod; tp <= maxPeriod; tp++)
		{
			double proptp = tpInfo.propTPVector[(tp - minPeriod)];

			for(ull loc = minLoc; loc <= maxLoc; loc++)
			{
				ull idxr = GET_INDEX((tp - minPeriod), (loc - minLoc), numLoc);
				for(ull loc2 = minLoc; loc2 <= maxLoc; loc2++)
				{
					double distance = distanceFunction->ComputeDistance(loc, loc2);

					if(distance > 0.0)
					{
						ull idxp = GET_INDEX((tp - minPeriod), (loc2 - minLoc), numLoc);
						epred0 += proptp * tpConditionalSteadyStateVector[idxr] * tpConditionalSteadyStateVector[idxp] * distance;
					}
				}
			}
		}
		if(normalize == true) { epred0 = epred0 / maxDistance; }

		// first-order
		for(ull tp = minPeriod; tp <= maxPeriod; tp++)
		{
			for(ull loc = minLoc; loc <= maxLoc; loc++)
			{
				ull idx = GET_INDEX((tp - minPeriod), (loc - minLoc), numLoc);
				double stationaryProb = adjustedSteadyStateVector[idx];

				ull state1Idx = GET_INDEX((tp - minPeriod), (loc - minLoc), numLoc);

				for(ull tp2 = minPeriod; tp2 <= maxPeriod; tp2++)
				{
					if(tpInfo.propTransMatrix[GET_INDEX(tp - minPeriod, tp2 - minPeriod, numPeriods)] == 0) { continue; } // if the time period transition is not possible (has prob. 0), skip it.

					double* transitionVector = NULL;
					VERIFY(Algorithms::GetTransitionVectorOfSubChain(transitionMatrix, tp, loc, tp2, &transitionVector, false) == true);

					for(ull loc2 = minLoc; loc2 <= maxLoc; loc2++)
					{
						ull state2Idx = GET_INDEX((tp2 - minPeriod), (loc2 - minLoc), numLoc);
						ull fullChainIdx = GET_INDEX(state1Idx, state2Idx, numStates);

						for(ull loc3 = minLoc; loc3 <= maxLoc; loc3++)
						{
							double distance = distanceFunction->ComputeDistance(loc2, loc3);

							if(distance > 0.0)
							{
								epred1 += stationaryProb * transitionMatrix[fullChainIdx] * transitionVector[(loc3 - minLoc)] * distance;
							}
						}
					}

					Free(transitionVector);
				}
			}
		}

		if(normalize == true) { epred1 = epred1 / maxDistance; }

		Free(tpConditionalSteadyStateVector);
		Free(adjustedSteadyStateVector);

		stringstream ss("");
		ss << user << ": ";
		ss << epred0 << DEFAULT_FIELDS_DELIMITER << " " << epred1;
		output->WriteLine(ss.str());
	}

	return true;

  // Bouml preserved body end 000B9A11
}

RandomnessAnalysisOperation::RandomnessAnalysisOperation(string name, bool normalize) : ContextAnalysisOperation(name)
{
  // Bouml preserved body begin 000B9A91

	this->normalize = normalize;

  // Bouml preserved body end 000B9A91
}

//! 
//! \brief Execute the operation
//!
//! Pure virtual method which executes the operation. 
//!
//! \tparam[[in] input 	InputType* to the input object of the operation.
//! \tparam[[in,out] output 	OutputType* to the output object of the operation.
//!
//! \return true if the operation is successful, false otherwise
//!
bool RandomnessAnalysisOperation::Execute(const Context* input, File* output) 
{
  // Bouml preserved body begin 000B9991

	VERIFY(input != NULL && output != NULL && output->IsGood());

	Parameters* params = Parameters::GetInstance();

	// get location parameters
	ull minLoc = 0; ull maxLoc = 0;
	VERIFY(params->GetLocationstampsRange(&minLoc, &maxLoc) == true);
	ull numLoc = maxLoc - minLoc + 1;

	// get time period parameters
	ull numPeriods = 0; TPInfo tpInfo;
	VERIFY(params->GetTimePeriodInfo(&numPeriods, &tpInfo) == true);
	ull minPeriod = tpInfo.minPeriod;
	ull maxPeriod = tpInfo.maxPeriod;

	ull numStates = numPeriods * numLoc;


	map<ull, UserProfile*> profiles = map<ull, UserProfile*>();
	VERIFY(input->GetProfiles(profiles) == true);

	pair_foreach_const(map<ull, UserProfile*>, profiles, iterProfiles)
	{
		ull user = iterProfiles->first;
		UserProfile* profile = iterProfiles->second;

		double* steadyStateVector = NULL;
		double* transitionMatrix = NULL;

		VERIFY(profile->GetSteadyStateVector(&steadyStateVector) == true);
		VERIFY(profile->GetTransitionMatrix(&transitionMatrix) == true);

		// Compute entropy rate for zeroth-order (er0) and first-order (er1)
		double er0 = 0.0; double er1 = 0.0;

		// compute the adjusted stationary distribution (called \twidle{\pi})
		double* adjustedSteadyStateVector = (double*)Allocate(numPeriods * numLoc * sizeof(double));
		double* tpConditionalSteadyStateVector = (double*)Allocate(numPeriods * numLoc * sizeof(double));
		double sum = 0.0;
		for(ull tp = minPeriod; tp <= maxPeriod; tp++)
		{
			double pitp = 0.0;
			for(ull loc = minLoc; loc <= maxLoc; loc++)
			{
				ull idx = GET_INDEX((tp - minPeriod), (loc - minLoc), numLoc);
				pitp += steadyStateVector[idx];
			}

			for(ull loc = minLoc; loc <= maxLoc; loc++)
			{
				ull idx = GET_INDEX((tp - minPeriod), (loc - minLoc), numLoc);
				double piltp = steadyStateVector[idx];
				tpConditionalSteadyStateVector[idx] = piltp / pitp;
				adjustedSteadyStateVector[idx] = piltp; //(tpInfo.propTPVector[(tp - minPeriod)] / pitp) * piltp;
				sum += adjustedSteadyStateVector[idx];
			}
		}
		VERIFY(abs(sum - 1) < EPSILON);

		// zeroth-order
		for(ull tp = minPeriod; tp <= maxPeriod; tp++)
		{
			for(ull loc = minLoc; loc <= maxLoc; loc++)
			{
				ull idx = GET_INDEX((tp - minPeriod), (loc - minLoc), numLoc);
				er0 -= adjustedSteadyStateVector[idx] * log2(tpConditionalSteadyStateVector[idx]);
			}
		}
		if(normalize == true) { er0 /= log2(numLoc); }// normalize

		// first-order
		for(ull tp = minPeriod; tp <= maxPeriod; tp++)
		{
			for(ull loc = minLoc; loc <= maxLoc; loc++)
			{
				ull idx = GET_INDEX((tp - minPeriod), (loc - minLoc), numLoc);
				double stationaryProb = adjustedSteadyStateVector[idx];

				ull state1Idx = GET_INDEX((tp - minPeriod), (loc - minLoc), numLoc);

				for(ull tp2 = minPeriod; tp2 <= maxPeriod; tp2++)
				{
					if(tpInfo.propTransMatrix[GET_INDEX(tp - minPeriod, tp2 - minPeriod, numPeriods)] == 0) { continue; } // if the time period transition is not possible (has prob. 0), skip it.

					double* transitionVector = NULL;
					VERIFY(Algorithms::GetTransitionVectorOfSubChain(transitionMatrix, tp, loc, tp2, &transitionVector, false) == true);

					for(ull loc2 = minLoc; loc2 <= maxLoc; loc2++)
					{
						ull state2Idx = GET_INDEX((tp2 - minPeriod), (loc2 - minLoc), numLoc);
						ull fullChainIdx = GET_INDEX(state1Idx, state2Idx, numStates);
						er1 -= stationaryProb * transitionMatrix[fullChainIdx] * log2(transitionVector[(loc2 - minLoc)]);
					}

					Free(transitionVector);
				}
			}
		}
		if(normalize == true) { er1 /= log2(numLoc); } // normalize

		Free(tpConditionalSteadyStateVector);
		Free(adjustedSteadyStateVector);

		stringstream ss("");
		ss << user << ": ";
		ss << er0 << DEFAULT_FIELDS_DELIMITER << " " << er1;
		output->WriteLine(ss.str());
	}

	return true;

  // Bouml preserved body end 000B9991
}

AbsoluteSimilarityAnalysisOperation::AbsoluteSimilarityAnalysisOperation(string name, MetricDistance* distance) : ContextAnalysisOperation(name, distance)
{
  // Bouml preserved body begin 000B9C91
  // Bouml preserved body end 000B9C91
}

//! 
//! \brief Execute the operation
//!
//! Pure virtual method which executes the operation. 
//!
//! \tparam[[in] input 	InputType* to the input object of the operation.
//! \tparam[[in,out] output 	OutputType* to the output object of the operation.
//!
//! \return true if the operation is successful, false otherwise
//!
bool AbsoluteSimilarityAnalysisOperation::Execute(const Context* input, File* output) 
{
  // Bouml preserved body begin 000B9C11

	VERIFY(input != NULL && output != NULL && output->IsGood());

	Parameters* params = Parameters::GetInstance();

	// get location parameters
	ull minLoc = 0; ull maxLoc = 0;
	VERIFY(params->GetLocationstampsRange(&minLoc, &maxLoc) == true);
	ull numLoc = maxLoc - minLoc + 1;

	// get time period parameters
	ull numPeriods = 0; TPInfo tpInfo;
	VERIFY(params->GetTimePeriodInfo(&numPeriods, &tpInfo) == true);
	ull minPeriod = tpInfo.minPeriod;
	ull maxPeriod = tpInfo.maxPeriod;

	ull numStates = numPeriods * numLoc;


	map<ull, UserProfile*> profiles = map<ull, UserProfile*>();
	VERIFY(input->GetProfiles(profiles) == true);

	bool isDefaultDistance = (dynamic_cast<DefaultMetricDistance*>(distanceFunction) != NULL);

	pair_foreach_const(map<ull, UserProfile*>, profiles, iterProfiles)
	{
		ull user1 = iterProfiles->first;
		UserProfile* profile1 = iterProfiles->second;

		double* steadyStateVector1 = NULL;
		double* transitionMatrix1 = NULL;

		VERIFY(profile1->GetSteadyStateVector(&steadyStateVector1) == true);
		VERIFY(profile1->GetTransitionMatrix(&transitionMatrix1) == true);

		// compute the adjusted stationary distribution for user1 (called \twidle{\pi})
		double* adjustedSteadyStateVector1 = (double*)Allocate(numPeriods * numLoc * sizeof(double));
		double sum1 = 0.0;
		for(ull tp = minPeriod; tp <= maxPeriod; tp++)
		{
			double pitp = 0.0;
			for(ull loc = minLoc; loc <= maxLoc; loc++)
			{
				ull idx = GET_INDEX((tp - minPeriod), (loc - minLoc), numLoc);
				pitp += steadyStateVector1[idx];
			}

			for(ull loc = minLoc; loc <= maxLoc; loc++)
			{
				ull idx = GET_INDEX((tp - minPeriod), (loc - minLoc), numLoc);
				double piltp = steadyStateVector1[idx];
				adjustedSteadyStateVector1[idx] = piltp; // (tpInfo.propTPVector[(tp - minPeriod)] / pitp) * piltp;
				sum1 += adjustedSteadyStateVector1[idx];
			}
		}
		VERIFY(abs(sum1 - 1) < EPSILON);


		pair_foreach_const(map<ull, UserProfile*>, profiles, iterProfiles2)
		{
			ull user2 = iterProfiles2->first;
			UserProfile* profile2 = iterProfiles2->second;

			double* steadyStateVector2 = NULL;
			double* transitionMatrix2 = NULL;

			VERIFY(profile2->GetSteadyStateVector(&steadyStateVector2) == true);
			VERIFY(profile2->GetTransitionMatrix(&transitionMatrix2) == true);

			double sim0 = 0.0; double sim1 = 0.0;

			// compute the adjusted stationary distribution for user2 (called \twidle{\pi})
			double* adjustedSteadyStateVector2 = (double*)Allocate(numPeriods * numLoc * sizeof(double));
			double sum2 = 0.0;
			for(ull tp = minPeriod; tp <= maxPeriod; tp++)
			{
				double pitp = 0.0;
				for(ull loc = minLoc; loc <= maxLoc; loc++)
				{
					ull idx = GET_INDEX((tp - minPeriod), (loc - minLoc), numLoc);
					pitp += steadyStateVector2[idx];
				}

				for(ull loc = minLoc; loc <= maxLoc; loc++)
				{
					ull idx = GET_INDEX((tp - minPeriod), (loc - minLoc), numLoc);
					double piltp = steadyStateVector2[idx];
					adjustedSteadyStateVector2[idx] = piltp; // (tpInfo.propTPVector[(tp - minPeriod)] / pitp) * piltp;
					sum2 += adjustedSteadyStateVector2[idx];
				}
			}
			VERIFY(abs(sum2 - 1) < EPSILON);


			if(isDefaultDistance == true) // for now, we implement the metric only for the default distance
			{
				// zeroth-order
				for(ull tp = minPeriod; tp <= maxPeriod; tp++)
				{
					for(ull loc = minLoc; loc <= maxLoc; loc++)
					{
						ull idx = GET_INDEX((tp - minPeriod), (loc - minLoc), numLoc);

						sim0 += min(adjustedSteadyStateVector1[idx], adjustedSteadyStateVector2[idx]);
					}
				}


				// first-order
				for(ull tp = minPeriod; tp <= maxPeriod; tp++)
				{
					for(ull loc = minLoc; loc <= maxLoc; loc++)
					{
						ull idx = GET_INDEX((tp - minPeriod), (loc - minLoc), numLoc);
						double stationaryProb = adjustedSteadyStateVector1[idx]; // prob. of user1 (leader) being there

						ull state1Idx = GET_INDEX((tp - minPeriod), (loc - minLoc), numLoc);

						for(ull tp2 = minPeriod; tp2 <= maxPeriod; tp2++)
						{
							if(tpInfo.propTransMatrix[GET_INDEX(tp - minPeriod, tp2 - minPeriod, numPeriods)] == 0) { continue; } // if the time period transition is not possible (has prob. 0), skip it.

							for(ull loc2 = minLoc; loc2 <= maxLoc; loc2++)
							{
								ull state2Idx = GET_INDEX((tp2 - minPeriod), (loc2 - minLoc), numLoc);
								ull idx2 = GET_INDEX(state1Idx, state2Idx, numStates);
								sim1 += stationaryProb * min(transitionMatrix1[idx2], transitionMatrix2[idx2]);
							}
						}
					}
				}
			}
			else
			{
				CODING_ERROR;
			}

			Free(adjustedSteadyStateVector2);

			stringstream ss("");
			ss << user1 << DEFAULT_FIELDS_DELIMITER << " " << user2 << ": "; // leader, follower
			ss << sim0 << DEFAULT_FIELDS_DELIMITER << " " << sim1;
			output->WriteLine(ss.str());
		}

		Free(adjustedSteadyStateVector1);
	}

	return true;

  // Bouml preserved body end 000B9C11
}



} // namespace lpm
