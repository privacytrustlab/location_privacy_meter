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
#include "../include/StrongAttackOperation.h"
#include "../include/MetricOperation.h"
#include "../include/TraceSet.h"
#include "../include/AttackOutput.h"

namespace lpm {

StrongAttackOperation::StrongAttackOperation() : AttackOperation("StrongAttackOperation")
{
  // Bouml preserved body begin 0004CD91
  // Bouml preserved body end 0004CD91
}

StrongAttackOperation::~StrongAttackOperation() 
{
  // Bouml preserved body begin 0004CE11
  // Bouml preserved body end 0004CE11
}

bool StrongAttackOperation::CreateMetric(MetricType type, MetricOperation** metric) const 
{
  // Bouml preserved body begin 0004CE91


	switch(type)
	{
		case Distortion:
			*metric = new DistortionMetricOperation();
			return true;
		case MostLikelyLocationDistortion:
			*metric = new MostLikelyLocationDistortionMetricOperation();
			return true;
		case MostLikelyTraceDistortion:
			*metric = new MostLikelyTraceDistortionMetricOperation();
			return true;
		case Entropy:
			*metric = new EntropyMetricOperation();
			return true;
		case Density:
			*metric = new DensityMetricOperation();
			return true;
		case MeetingDisclosure:
			*metric = new MeetingDisclosureMetricOperation();
			return true;
		case Anonymity:
			*metric = new AnonymityMetricOperation();
			return true;
		default:
			CODING_ERROR;
			break;
	}

	return false;

  // Bouml preserved body end 0004CE91
}

bool StrongAttackOperation::Execute(const TraceSet* input, AttackOutput* output) 
{
  // Bouml preserved body begin 0004CF91

/**/
	const double bigNumber = 1e20;
	const double bigNumberInverse = 1.0 / bigNumber;
/**/

	if(input == NULL || output == NULL || context == NULL)
	{
		SET_ERROR_CODE(ERROR_CODE_INVALID_ARGUMENTS);
		return false;
	}

	stringstream info("");
	info << "Starting the strong attack!";
	Log::GetInstance()->Append(info.str());

	// get time parameters
	ull minTime = 0; ull maxTime = 0;
	VERIFY(Parameters::GetInstance()->GetTimestampsRange(&minTime, &maxTime) == true);
	ull numTimes = maxTime - minTime + 1;

	// get location parameters
	ull minLoc = 0; ull maxLoc = 0;
	VERIFY(Parameters::GetInstance()->GetLocationstampsRange(&minLoc, &maxLoc) == true);
	ull numLoc = maxLoc - minLoc + 1;

	// get user profiles
	map<ull, UserProfile*> profiles = map<ull, UserProfile*>();
	VERIFY(context->GetProfiles(profiles) == true);

	ull Nusers = profiles.size();

	// normalization variable
	double* lrnrm = NULL;

	double* alpha = NULL;
	double* beta = NULL;

	info.str("");
	info << "Computing alpha and beta!";
	Log::GetInstance()->Append(info.str());

	//compute alpha and beta matrices for all users, pseudonyms, times, and locations
	VERIFY(ComputeAlphaBeta(input, &alpha, &beta, &lrnrm) == true);
	VERIFY(alpha != NULL && beta != NULL && lrnrm != NULL);

	// de-anonymization
	ull byteSize = (Nusers * Nusers) * sizeof(double);
	double* likelihoodMatrix = (double*)Allocate(byteSize);
	VERIFY(likelihoodMatrix != NULL);

	for (ull userIndex = 0; userIndex < Nusers; userIndex++)
	{
		for (ull nymIndex = 0; nymIndex < Nusers; nymIndex++)
		{
			double sum = 0.0;
			for (ull loc = minLoc; loc <= maxLoc; loc++)
			{
				ull index = GET_INDEX_4D(userIndex, nymIndex, (maxTime - minTime), (loc - minLoc), Nusers, numTimes, numLoc);
				sum += alpha[index];
			}

			/*
			stringstream info("");
			info << "likelihood: " << (double)sum << "   " << DBL_MIN;
			Log::GetInstance()->Append(info.str());
			*/

			if(sum <= 0.0)
			{
				sum = DBL_MIN;

/**/
				lrnrm[GET_INDEX(userIndex, nymIndex, Nusers)] = 0;
/**/
			}
			else
			{
				// re-normalization
/**/
				while (sum < bigNumberInverse)
				{
					sum *= bigNumber;
					lrnrm[GET_INDEX(userIndex, nymIndex, Nusers)]++;
				}
/**/
			}

/**/
			likelihoodMatrix[GET_INDEX(userIndex, nymIndex, Nusers)] = log(sum) + lrnrm[GET_INDEX(userIndex, nymIndex, Nusers)] * log(bigNumberInverse);
/**/
//			likelihoodMatrix[GET_INDEX(userIndex, nymIndex, Nusers)] = log(sum);
		}
	}

/**/
	Free(lrnrm);
/**/

	// log the likelihood matrix
	Log::GetInstance()->Append("Likelihood Matrix (strong adv):");
	for(ull i = 0; i < Nusers; i++)
	{
		stringstream info("");

		for(ull j = 0; j < Nusers; j++)
		{
			info << " " << likelihoodMatrix[GET_INDEX(i, j, Nusers)];
		}

		Log::GetInstance()->Append(info.str());
	}

	// allocate weight and mapping
	ull byteSizeMatrix = (Nusers * Nusers) * sizeof(ll);
	ull byteSizeVector = Nusers * sizeof(ll);

	ll* weight = (ll*)Allocate(byteSizeMatrix);
	ll* mapping = (ll*)Allocate(byteSizeVector);
	VERIFY(weight != NULL && mapping != NULL);
	memset(weight, 0, byteSizeMatrix);
	memset(mapping, 0, byteSizeVector);

	// convert likelihood matrix to weight matrix
	double maxnll = 0.0;
	for(ull i = 0; i < Nusers; i++)
	{
		for(ull j = 0; j < Nusers; j++)
		{
			ull index = GET_INDEX(i, j, Nusers);

			double nll = -likelihoodMatrix[index];
			if(nll > maxnll) { maxnll = nll; }
		}
	}

	ll llhoodBigNumber = (maxnll == 0.0) ? -1 : (ll)(-(double)LONG_LONG_MAX/maxnll + 10000);
	for(ull i = 0; i < Nusers; i++)
	{
		for(ull j = 0; j < Nusers; j++)
		{
			ull index = GET_INDEX(i, j, Nusers);
			double llhood = likelihoodMatrix[index];
			if(llhood > 0) { weight[index] = 0; }
			else { weight[index] = (ll)(llhoodBigNumber * llhood); }
		}
	}

	Free(likelihoodMatrix); likelihoodMatrix = NULL;

	// log the weight matrix
	Log::GetInstance()->Append("Weight Matrix (strong adv):");
	for(ull i = 0; i < Nusers; i++)
	{
		stringstream info(""); info.precision(16);

		for(ull j = 0; j < Nusers; j++)
		{
			info << " " << weight[GET_INDEX(i, j, Nusers)];
		}

		Log::GetInstance()->Append(info.str());
	}

	//de-anonymization
	// prev code: VERIFY(Algorithms::MaximumWeightAssignment(Nusers, weight, mapping) == 0); // this method was called maximum weight assignment but it was computing minimum cost assignement!

	ll* costMatrix = weight;
	Algorithms::MinimumCostAssignment(costMatrix, Nusers, mapping); // Note: the weight matrix is indeed a cost matrix, the mapping which maximizes the likelihood is the minimum cost assignment!

	Free(weight); weight = costMatrix = NULL;


	map<ull, ull> userToPseudonymMapping = map<ull, ull>();
	// get mapping (pseudonym -> observed trace): just for logging
	{
		map<ull, Trace*> traceMapping = map<ull, Trace*>();
		input->GetMapping(traceMapping);

		VERIFY(Nusers == traceMapping.size());

		ull* pseudonyms = (ull*)Allocate(byteSizeVector);
		VERIFY(pseudonyms != NULL);
		memset(pseudonyms, 0, byteSizeVector);

		ull* users = (ull*)Allocate(byteSizeVector);
		VERIFY(users != NULL);
		memset(users, 0, byteSizeVector);

		ull i = 0;
		pair_foreach_const(map<ull, UserProfile*>, profiles, userIter)
		{
			users[i++] = userIter->first;
		}

		ull j = 0;
		pair_foreach_const(map<ull, Trace*>, traceMapping, pseudonymIter)
		{
			pseudonyms[j++] = pseudonymIter->first;
		}

		for(ull k = 0; k < Nusers; k++)
		{
			ull pseudonym = pseudonyms[mapping[k]];
			ull user = users[k];
			userToPseudonymMapping.insert(pair<ull, ull>(user, pseudonym));
		}

		VERIFY(userToPseudonymMapping.size() == Nusers);

		pair_foreach_const(map<ull, ull>, userToPseudonymMapping, iter)
		{
			stringstream info("");
			info << "Assignment: user -> pseudonym " << iter->first << " " << iter->second;
			Log::GetInstance()->Append(info.str());
		}

		output->SetAnonymizationMap(userToPseudonymMapping); // set the mapping

		Free(pseudonyms);
		Free(users);
	}

	ull mostLikelyTraceByteSize = Nusers * numTimes * sizeof(ull);
	ull* mostLikelyTrace = (ull*)Allocate(mostLikelyTraceByteSize);
	VERIFY(mostLikelyTrace != NULL);
	memset(mostLikelyTrace, 0, mostLikelyTraceByteSize);

	// tracking
	VERIFY(ComputeMostLikelyTrace(input, userToPseudonymMapping, mostLikelyTrace) == true);
	userToPseudonymMapping.clear();

	output->SetMostLikelyTrace(mostLikelyTrace);

	// de-obfuscation

	ull outputByteSize = Nusers * numTimes * numLoc * sizeof(double);
	double* locationDistribution = (double*)Allocate(outputByteSize);
	VERIFY(locationDistribution != NULL);
	memset(locationDistribution, 0, outputByteSize);


	// for each user
	for(ull userIndex = 0; userIndex < Nusers; userIndex++)
	{
		for (ull tm = minTime; tm <= maxTime; tm++)
		{
			double sum = 0.0;

			for (ull loc = minLoc; loc <= maxLoc; loc++)
			{
				ull index = GET_INDEX_4D(userIndex, mapping[userIndex], (tm - minTime), (loc - minLoc), Nusers, numTimes, numLoc);

				double product = 0.0;
				product = alpha[index] * beta[index];


				stringstream info("");
				info << "alpha beta " << userIndex << " " << tm << " " << loc << " | " << alpha[index] << " * " << beta[index] << " = " << product;
				Log::GetInstance()->Append(info.str());


				ull index2 = GET_INDEX_3D(userIndex, (tm - minTime), (loc - minLoc), numTimes, numLoc);

				locationDistribution[index2] = product;

				sum += product;

				stringstream info2("");
				info2 << "       sum " << userIndex << " " << tm << " " << loc << " | " << sum;
				Log::GetInstance()->Append(info2.str());

			}

			for (ull loc = minLoc; loc <= maxLoc; loc++)
			{
				ull index2 = GET_INDEX_3D(userIndex, (tm - minTime), (loc - minLoc), numTimes, numLoc);

				double tmp = locationDistribution[index2];
				VERIFY(sum != 0.0 && (tmp/sum) != nan("n-char-sequence"));
				locationDistribution[index2] /= (double)sum;
			}
		}
	}

	Free(mapping);
	Free(alpha);
	Free(beta);

	output->SetProbabilityDistribution(locationDistribution);

	return true;

  // Bouml preserved body end 0004CF91
}

bool StrongAttackOperation::ComputeAlphaBeta(const TraceSet* traces, double** alpha, double** beta, double** lrnrm) const 
{
  // Bouml preserved body begin 0001F582

/**/
	const double bigNumber = 1e20;
	const double bigNumberInverse = 1.0 / bigNumber;
/**/

	if(traces == NULL || alpha == NULL || beta == NULL)
	{
		SET_ERROR_CODE(ERROR_CODE_INVALID_ARGUMENTS);
		return false;
	}

	ull minTime = 0; ull maxTime = 0;
	VERIFY(Parameters::GetInstance()->GetTimestampsRange(&minTime, &maxTime) == true);
	ull numTimes = maxTime - minTime + 1;

	ull minLoc = 0; ull maxLoc = 0;
	VERIFY(Parameters::GetInstance()->GetLocationstampsRange(&minLoc, &maxLoc) == true);
	ull numLoc = maxLoc - minLoc + 1;

	// get the user profiles
	map<ull, UserProfile*> profiles = map<ull, UserProfile*>();
	VERIFY(context->GetProfiles(profiles) == true);

	ull Nusers = profiles.size();

	//allocate memory for Alpha and Beta
	ull byteSizeAB = (Nusers * Nusers * numTimes * numLoc) * sizeof(double);

	double* myalpha = *alpha = (double*)Allocate(byteSizeAB);
	VERIFY(myalpha != NULL);
	memset(myalpha, 0, byteSizeAB);

	double* mybeta = *beta = (double*)Allocate(byteSizeAB);
	VERIFY(mybeta != NULL);
	memset(mybeta, 0, byteSizeAB);

	// normalization variables
/**/
	ull byteSizeNorm = Nusers * Nusers * numTimes * sizeof(double);

	double* arnrm = (double*)Allocate(byteSizeNorm);
	VERIFY(arnrm != NULL);
	memset(arnrm, 0, byteSizeNorm);

	double* mylrnrm = *lrnrm = (double*)Allocate(Nusers * Nusers * sizeof(double));
	VERIFY(mylrnrm != NULL);
	memset(mylrnrm, 0, Nusers * Nusers * sizeof(double));
/**/

	// get the mapping (pseudonym -> observed trace)
	map<ull, Trace*> mappingNymObserved = map<ull, Trace*>();
	traces->GetMapping(mappingNymObserved);

	VERIFY(Nusers == mappingNymObserved.size());

	// for all users
	ull userIndex = 0;
	pair_foreach_const(map<ull, UserProfile*>, profiles, usersIter)
	{
		ull user = usersIter->first;
		UserProfile* profile = usersIter->second;

		double* transitionMatrix = NULL;
		profile->GetTransitionMatrix(&transitionMatrix);

		double* steadyStateVector = NULL;
		profile->GetSteadyStateVector(&steadyStateVector);

		VERIFY(transitionMatrix != NULL && steadyStateVector != NULL);

		// for all observed traces
		ull pseudonymIndex = 0;
		pair_foreach_const(map<ull, Trace*>, mappingNymObserved, pseudonymsIter)
		{
			//ull pseudonym = pseudonymsIter->first;
			Trace* observedTrace = pseudonymsIter->second;

			vector<Event*> events = vector<Event*>();
			observedTrace->GetEvents(events);

			VERIFY(numTimes == events.size());

			// compute alpha
/*
*/
			// for all time instants
			ull tm = minTime;
			foreach_const(vector<Event*>, events, eventsIter)
			{
				double asum = 0.0;

				ObservedEvent* observedEvent = dynamic_cast<ObservedEvent*>(*eventsIter);
				set<ull> timestamps = set<ull>();
				observedEvent->GetTimestamps(timestamps);

				VERIFY(timestamps.size() == 1);

				ull timestamp = *(timestamps.begin());

				VERIFY(tm == timestamp && (timestamp >= minTime && timestamp <= maxTime));

				ull tp = Parameters::GetInstance()->LookupTimePeriod(timestamp);
				ull prevtp = tp; // ensure prevtp is always consistent with its usage
				if(timestamp > minTime) { prevtp = Parameters::GetInstance()->LookupTimePeriod(timestamp - 1); }
				if(prevtp == INVALID_TIME_PERIOD || tp == INVALID_TIME_PERIOD)
				{
					SET_ERROR_CODE(ERROR_CODE_INCONSISTENT_TIME_PARTITIONING_USAGE);
					return false;
				}

				// get the proper sub-chain steady-state vector according to the time period of the event
				double* subChainSteadyStateVector = NULL;
				VERIFY(Algorithms::GetSteadyStateVectorOfSubChain(steadyStateVector, tp, &subChainSteadyStateVector) == true);


				for(ull loc = minLoc; loc <= maxLoc; loc++)
				{
					ActualEvent* actualEvent = new ActualEvent(user, timestamp, loc);
					ExposedEvent* exposedEvent = new ExposedEvent(*actualEvent);

					VERIFY(actualEvent != NULL && exposedEvent != NULL);

					double lppmProb0 = lppmPDF->PDF(context, actualEvent, observedEvent);
					double applicationProb0 = applicationPDF->PDF(context, actualEvent, actualEvent);

					double lppmProb1 = lppmPDF->PDF(context, exposedEvent, observedEvent);
					double applicationProb1 = applicationPDF->PDF(context, actualEvent, exposedEvent);

					actualEvent->Release();
					exposedEvent->Release();

					double presenceProb = subChainSteadyStateVector[loc - minLoc];

					// compute alpha_1
					if (timestamp == minTime)
					{
						double prob = 0.0;
						prob = (double)presenceProb * ((lppmProb0 * applicationProb0 ) + (lppmProb1 * applicationProb1));

						ull index = GET_INDEX_4D(userIndex, pseudonymIndex, (timestamp - minTime), (loc - minLoc), Nusers, numTimes, numLoc);
						myalpha[index] = prob;
					}
					else // compute alpha_t
					{
						double sum = 0.0;
						for(ull prevloc = minLoc; prevloc <= maxLoc; prevloc++)
						{
							ull index = GET_INDEX_4D(userIndex, pseudonymIndex, (timestamp - minTime - 1), (prevloc - minLoc), Nusers, numTimes, numLoc);
							double previousAlpha = myalpha[index];

							// get the proper sub-chain transition vector to the time period of the previous event (we're computing alpha, remember?)
							double* subChainTransitionVector = NULL;
							VERIFY(Algorithms::GetTransitionVectorOfSubChain(transitionMatrix, prevtp, prevloc, tp, &subChainTransitionVector) == true);

							ull index2 = (loc - minLoc);
							double transitionProb = subChainTransitionVector[index2];

							Free(subChainTransitionVector);  // free the sub-chain transition vector

							sum += previousAlpha * transitionProb;
						}

						double prob = 0.0;
						prob = (double)sum * ((lppmProb0 * applicationProb0 ) + (lppmProb1 * applicationProb1));

						ull index = GET_INDEX_4D(userIndex, pseudonymIndex, (timestamp - minTime), (loc - minLoc), Nusers, numTimes, numLoc);
						myalpha[index] = prob;

						asum += prob;

						// take care of small alpha
						//VERIFY(prob == 0.0 || prob > bigNumberInverse);
					}
				}

				Free(subChainSteadyStateVector); // free the sub-chain steady-state vector

				// Re-normalize the alpha's s necessary to avoid underflow, keeping track of how many re-normalizations for each alpha
/**/
				if (timestamp == minTime)
				{
					arnrm[GET_INDEX_3D(userIndex, pseudonymIndex, 0, Nusers, numTimes)] = 0;
				}
				else
				{
					arnrm[GET_INDEX_3D(userIndex, pseudonymIndex, (timestamp - minTime), Nusers, numTimes)] = arnrm[GET_INDEX_3D(userIndex, pseudonymIndex, (timestamp - minTime - 1), Nusers, numTimes)];
				}

				if (asum < bigNumberInverse)
				{
					++arnrm[GET_INDEX_3D(userIndex, pseudonymIndex, timestamp - minTime, Nusers, numTimes)];

					for (ull loc = minLoc; loc <= maxLoc; loc++)
					{
						ull index = GET_INDEX_4D(userIndex, pseudonymIndex, (timestamp - minTime), (loc - minLoc), Nusers, numTimes, numLoc);
						myalpha[index] *= bigNumber;
					}
				}
/**/
				tm++;
				// Done with the re-normalization of alpha
			}

			//keeping track of how many re-normalizations for alpha of user u and pseudonym u'
/**/
			mylrnrm[GET_INDEX(userIndex, pseudonymIndex, Nusers)] = arnrm[GET_INDEX_3D(userIndex, pseudonymIndex, maxTime - minTime, Nusers, numTimes)];
/**/


			ObservedEvent* prevObservedEvent = NULL;


			// compute beta

			// for all time instants
			tm = maxTime;
			foreach_const_reverse(vector<Event*>, events, eventsIter)
			{
				double bsum = 0.0;

				ObservedEvent* observedEvent = dynamic_cast<ObservedEvent*>(*eventsIter);
				set<ull> timestamps = set<ull>();
				observedEvent->GetTimestamps(timestamps);

				VERIFY(timestamps.empty() == false);

				ull timestamp = *(timestamps.begin());

				VERIFY(tm == timestamp && (timestamp >= minTime && timestamp <= maxTime));

				ull tp = Parameters::GetInstance()->LookupTimePeriod(timestamp);
				ull nexttp = tp; // ensure nexttp is always consistent with its usage
				if(timestamp < maxTime) { nexttp = Parameters::GetInstance()->LookupTimePeriod(timestamp + 1); }
				if(nexttp == INVALID_TIME_PERIOD || tp == INVALID_TIME_PERIOD)
				{
					SET_ERROR_CODE(ERROR_CODE_INCONSISTENT_TIME_PARTITIONING_USAGE);
					return false;
				}

				for(ull loc = minLoc; loc <= maxLoc; loc++)
				{
					// compute beta_T
					if (timestamp == maxTime)
					{
						ull index = GET_INDEX_4D(userIndex, pseudonymIndex, (timestamp - minTime), (loc - minLoc), Nusers, numTimes, numLoc);
						mybeta[index] = 1.0;
					}
					else // compute beta_t
					{
						double sum = 0.0;

						for(ull nextloc = minLoc; nextloc <= maxLoc; nextloc++)
						{
							ActualEvent* actualEvent = new ActualEvent(user, timestamp + 1, nextloc);
							ExposedEvent* exposedEvent = new ExposedEvent(*actualEvent);

							VERIFY(actualEvent != NULL && exposedEvent != NULL);

							double lppmProb0 = lppmPDF->PDF(context, actualEvent, prevObservedEvent);
							double applicationProb0 = applicationPDF->PDF(context, actualEvent, actualEvent);

							double lppmProb1 = lppmPDF->PDF(context, exposedEvent, prevObservedEvent);
							double applicationProb1 = applicationPDF->PDF(context, actualEvent, exposedEvent);

							actualEvent->Release();
							exposedEvent->Release();

							ull index2 = GET_INDEX_4D(userIndex, pseudonymIndex, (timestamp - minTime + 1), (nextloc - minLoc), Nusers, numTimes, numLoc);
							double nextBeta = mybeta[index2];

							// get the proper sub-chain transition vector to the time period of the next event (we're computing beta, remember?)
							double* subChainTransitionVector = NULL;
							VERIFY(Algorithms::GetTransitionVectorOfSubChain(transitionMatrix, tp, loc, nexttp, &subChainTransitionVector) == true);

							ull index3 = (nextloc - minLoc);
							double transitionProb = subChainTransitionVector[index3];

							Free(subChainTransitionVector);  // free the sub-chain transition vector

							sum += (double)nextBeta * transitionProb * ((lppmProb0 * applicationProb0 ) + (lppmProb1 * applicationProb1));
						}

						ull index = GET_INDEX_4D(userIndex, pseudonymIndex, (timestamp - minTime), (loc - minLoc), Nusers, numTimes, numLoc);
						mybeta[index] = sum;

						bsum += sum;

						// take care of small beta
						//VERIFY(sum == 0.0 || sum > bigNumberInverse);
					}
				}

				prevObservedEvent = observedEvent;

				// Re-normalize the alpha's s necessary to avoid underflow
/**/
				if (bsum < bigNumberInverse)
				{
					for (ull loc = minLoc; loc <= maxLoc; loc++)
					{
						ull index = GET_INDEX_4D(userIndex, pseudonymIndex, (timestamp - minTime), (loc - minLoc), Nusers, numTimes, numLoc);
						mybeta[index] *= bigNumber;
					}
				}
/**/
				// Done with the re-normalization of beta
				tm--;
			}

			stringstream info("");
			info << "Alpha-Beta: user : pseudonym " << userIndex << " " << pseudonymIndex;
			Log::GetInstance()->Append(info.str());

			pseudonymIndex++;
		}

		userIndex++;
	}

/**/
	Free(arnrm);
/**/

	return true;

  // Bouml preserved body end 0001F582
}

bool StrongAttackOperation::ComputeMostLikelyTrace(const TraceSet* traces, const map<ull, ull>& userToPseudonymMap, ull* mostLikelyTrace) 
{
  // Bouml preserved body begin 0007C991

	if(traces == NULL || userToPseudonymMap.empty() == true || mostLikelyTrace == NULL)
	{
		SET_ERROR_CODE(ERROR_CODE_INVALID_ARGUMENTS);
		return false;
	}

	ull minTime = 0; ull maxTime = 0;
	VERIFY(Parameters::GetInstance()->GetTimestampsRange(&minTime, &maxTime) == true);
	ull numTimes = maxTime - minTime + 1;

	ull minLoc = 0; ull maxLoc = 0;
	VERIFY(Parameters::GetInstance()->GetLocationstampsRange(&minLoc, &maxLoc) == true);
	ull numLoc = maxLoc - minLoc + 1;

	// get the user profiles
	map<ull, UserProfile*> profiles = map<ull, UserProfile*>();
	VERIFY(context->GetProfiles(profiles) == true);

	ull Nusers = profiles.size();

	ull deltaByteSize = Nusers * numTimes * numLoc * sizeof(double);
	double* delta = (double*)Allocate(deltaByteSize);

	VERIFY(delta != NULL);
	memset(delta, 0, deltaByteSize);


	ull predecessorByteSize = Nusers * numTimes * numLoc * sizeof(ull);
	ull* predecessor = (ull*)Allocate(predecessorByteSize);

	VERIFY(predecessor != NULL);
	memset(predecessor, 0, predecessorByteSize);


	// get the mapping (pseudonym -> observed trace)
	map<ull, Trace*> mappingNymObserved = map<ull, Trace*>();
	traces->GetMapping(mappingNymObserved);

	VERIFY(Nusers == mappingNymObserved.size());

	// for all users
	ull userIndex = 0;
	pair_foreach_const(map<ull, UserProfile*>, profiles, usersIter)
	{
		ull user = usersIter->first;
		UserProfile* profile = usersIter->second;

		double* transitionMatrix = NULL;
		profile->GetTransitionMatrix(&transitionMatrix);

		double* steadyStateVector = NULL;
		profile->GetSteadyStateVector(&steadyStateVector);

		VERIFY(transitionMatrix != NULL && steadyStateVector != NULL);

		map<ull, ull>::const_iterator iter = userToPseudonymMap.find(user);
		VERIFY(iter != userToPseudonymMap.end());

		ull pseudonym = iter->second;

		map<ull, Trace*>::const_iterator mappingIter = mappingNymObserved.find(pseudonym);
		VERIFY(mappingIter != mappingNymObserved.end());

		Trace* observedTrace = mappingIter->second;

		vector<Event*> events = vector<Event*>();
		observedTrace->GetEvents(events);

		VERIFY(numTimes == events.size());

		ull mostLikelyLastLoc = minLoc;
		double mostLikelyLastLocValue = log(SQRT_DBL_MIN); // initially a very large (negative) value

		// for all time instants
		ull tm = minTime;
		foreach_const(vector<Event*>, events, eventsIter)
		{
			ObservedEvent* observedEvent = dynamic_cast<ObservedEvent*>(*eventsIter);
			set<ull> timestamps = set<ull>();
			observedEvent->GetTimestamps(timestamps);

			VERIFY(timestamps.size() == 1);
			ull timestamp = *(timestamps.begin());

			VERIFY(timestamp == tm && (timestamp >= minTime && timestamp <= maxTime));

			ull tp = Parameters::GetInstance()->LookupTimePeriod(timestamp);
			ull prevtp = tp; // ensure prevtp is always consistent with its usage
			if(timestamp > minTime) { prevtp = Parameters::GetInstance()->LookupTimePeriod(timestamp - 1); }
			if(prevtp == INVALID_TIME_PERIOD || tp == INVALID_TIME_PERIOD)
			{
				SET_ERROR_CODE(ERROR_CODE_INCONSISTENT_TIME_PARTITIONING_USAGE);
				return false;
			}

			// get the proper sub-chain steady-state vector according to the time period of the event
			double* subChainSteadyStateVector = NULL;
			if(timestamp == minTime) // only needed for timestamp == minTime
			{ VERIFY(Algorithms::GetSteadyStateVectorOfSubChain(steadyStateVector, tp, &subChainSteadyStateVector) == true); }

			for(ull loc = minLoc; loc <= maxLoc; loc++)
			{
				ull deltaIndex = GET_INDEX_3D(userIndex, (timestamp - minTime), (loc - minLoc), numTimes, numLoc);

				ActualEvent* actualEvent = new ActualEvent(user, timestamp, loc);
				ExposedEvent* exposedEvent = new ExposedEvent(*actualEvent);

				VERIFY(actualEvent != NULL && exposedEvent != NULL);

				double lppmProb0 = lppmPDF->PDF(context, actualEvent, observedEvent);
				double applicationProb0 = applicationPDF->PDF(context, actualEvent, actualEvent);

				double lppmProb1 = lppmPDF->PDF(context, exposedEvent, observedEvent);
				double applicationProb1 = applicationPDF->PDF(context, actualEvent, exposedEvent);

				actualEvent->Release();
				exposedEvent->Release();

				double f = ((lppmProb0 * applicationProb0) + (lppmProb1 * applicationProb1));
				double logf = log(f);

				if(f <= 0.0 || logf == nan("n-char-sequence"))
				{
					logf = log(SQRT_DBL_MIN); // avoid log overflow/underflow/nan
				}

				if(timestamp == minTime) // initialization
				{
					double presenceProb = subChainSteadyStateVector[loc - minLoc];
					double logpp = log(presenceProb);

					if(presenceProb <= 0.0 || logpp == nan("n-char-sequence"))
					{
						logpp = log(SQRT_DBL_MIN); // avoid log overflow/underflow/nan
					}

					// delta[deltaIndex] = f * presenceProb;
					delta[deltaIndex] = logf + log(presenceProb); // use logarithms to avoid underflow
				}
				else
				{
					ull maximizingLoc = minLoc;
					double maximizingValue = log(SQRT_DBL_MIN); // initially a very large (negative) value
					for(ull loc2 = minLoc; loc2 <= maxLoc; loc2++)
					{
						ull prevDeltaIndex = GET_INDEX_3D(userIndex, ((timestamp - 1) - minTime), (loc2 - minLoc), numTimes, numLoc);

						double prevDelta = delta[prevDeltaIndex];

						// get the proper sub-chain transition vector to the time period of the previous event
						double* subChainTransitionVector = NULL;
						VERIFY(Algorithms::GetTransitionVectorOfSubChain(transitionMatrix, prevtp, loc2, tp, &subChainTransitionVector) == true);

						ull currLocIdx = (loc - minLoc);
						double transProb = subChainTransitionVector[currLocIdx];

						Free(subChainTransitionVector);  // free the sub-chain transition vector

						// double m = (prevDelta * transProb);
						double m = prevDelta + log(transProb);  // use logarithms to avoid underflow

						if(maximizingValue < m)
						{
							maximizingLoc = loc2;
							maximizingValue = m;
						}

						/*// debug
						{
							stringstream info("");
							info << "User: " << user << " - possible loc at time " <<  timestamp << " for loc: " << loc;
							info << " is " << loc2 << " | m: " << m << " | prevDelta: " << prevDelta << " | transProb: " << transProb;
							Log::GetInstance()->Append(info.str());
						}*/
					}

					// delta[deltaIndex] = f * maximizingValue;
					delta[deltaIndex] = maximizingValue + logf; // use logarithms to avoid underflow

					// store predecessor
					ull predecessorIndex = GET_INDEX_3D(userIndex, (timestamp - minTime), (loc - minLoc), numTimes, numLoc);
					predecessor[predecessorIndex] = maximizingLoc;

					/* // debug
					{
						stringstream info("");
						info << "User: " << user << " - maximizing predecessor at time " <<  timestamp << " for loc: " << loc;
						info << " is " << maximizingLoc << " | delta: " << delta[deltaIndex] << " | f: " << f << " | maximizingValue: " << maximizingValue;
						Log::GetInstance()->Append(info.str());
					}*/
				}

				if(timestamp == maxTime) // find the max
				{
					if(mostLikelyLastLocValue < delta[deltaIndex])
					{
						mostLikelyLastLocValue = delta[deltaIndex];
						mostLikelyLastLoc = loc;
					}
				}
			}
			if(timestamp == minTime) // only needed for tm == minTime
			{ Free(subChainSteadyStateVector); /* free the sub-chain steady-state vector */ }

			tm++;
		}

		// reconstruct most likely trace for this user
		VERIFY(minTime > 0);
		ull predecessorLoc = mostLikelyLastLoc;

		ull index = GET_INDEX(userIndex, (maxTime - minTime), numTimes);
		mostLikelyTrace[index] = mostLikelyLastLoc;

		VERIFY(mostLikelyLastLoc >= minLoc && mostLikelyLastLoc <= maxLoc);

		for(ull tm = maxTime - 1; tm >= minTime; tm--)
		{
			ull predecessorIndex = GET_INDEX_3D(userIndex, ((tm + 1) - minTime), (predecessorLoc - minLoc), numTimes, numLoc);
			predecessorLoc = predecessor[predecessorIndex];

			VERIFY(predecessorLoc >= minLoc && predecessorLoc <= maxLoc);

			index = GET_INDEX(userIndex, (tm - minTime), numTimes);
			mostLikelyTrace[index] = predecessorLoc;
		}

		userIndex++;
	}

	Free(delta);
	Free(predecessor);

	return true;

  // Bouml preserved body end 0007C991
}


} // namespace lpm
