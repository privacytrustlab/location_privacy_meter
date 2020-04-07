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
#include "../include/WeakAttackOperation.h"
#include "../include/MetricOperation.h"
#include "../include/TraceSet.h"
#include "../include/AttackOutput.h"

namespace lpm {

WeakAttackOperation::WeakAttackOperation() : AttackOperation("WeakAttackOperation")
{
  // Bouml preserved body begin 0004B111
  // Bouml preserved body end 0004B111
}

WeakAttackOperation::~WeakAttackOperation() 
{
  // Bouml preserved body begin 0004B191
  // Bouml preserved body end 0004B191
}

bool WeakAttackOperation::CreateMetric(MetricType type, MetricOperation** metric) const 
{
  // Bouml preserved body begin 0004B311


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

  // Bouml preserved body end 0004B311
}

bool WeakAttackOperation::Execute(const TraceSet* input, AttackOutput* output) 
{
  // Bouml preserved body begin 0004D011

	if(input == NULL || output == NULL || context == NULL)
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

	// get profiles
	map<ull, UserProfile*> profiles = map<ull, UserProfile*>();
	VERIFY(context->GetProfiles(profiles) == true);

	ull Nusers = profiles.size();

	// de-anonymization
	double* likelihoodMatrix = NULL;
	VERIFY(ComputeLikelihood(input, &likelihoodMatrix) == true);
	VERIFY(likelihoodMatrix != NULL);

	// log the likelihood matrix
	Log::GetInstance()->Append("Likelihood Matrix (weak adv):");
	for(ull i = 0; i < Nusers; i++)
	{
		stringstream info(""); info.precision(16);

		for(ull j = 0; j < Nusers; j++)
		{
			info << " " << likelihoodMatrix[GET_INDEX(i, j, Nusers)];
		}

		Log::GetInstance()->Append(info.str());
	}

	VERIFY((ull)((ll)Nusers) == Nusers); // check overflow

	// allocate weight and mapping
	ull byteSizeMatrix = (Nusers * Nusers) * sizeof(ll);
	ull byteSizeVector = Nusers * sizeof(ll);

	ll* weight = (ll*)Allocate(byteSizeMatrix);
	ll* mapping = (ll*)Allocate(byteSizeVector);
	memset(weight, 0, byteSizeMatrix);
	memset(mapping, 0, byteSizeVector);

	VERIFY(weight != NULL && mapping != NULL);

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

	Free(likelihoodMatrix);

	// log the weight matrix
	Log::GetInstance()->Append("Weight Matrix (weak adv):");
	for(ull i = 0; i < Nusers; i++)
	{
		stringstream info(""); info.precision(16);

		for(ull j = 0; j < Nusers; j++)
		{
			info << " " << weight[GET_INDEX(i, j, Nusers)];
		}

		Log::GetInstance()->Append(info.str());
	}

	// prev code: VERIFY(Algorithms::MaximumWeightAssignment(Nusers, weight, mapping) == 0); // this method was called maximum weight assignment but it was computing minimum cost assignement!

	ll* costMatrix = weight;
	Algorithms::MinimumCostAssignment(costMatrix, Nusers, mapping); // Note: the weight matrix is indeed a cost matrix, the mapping which maximizes the likelihood is the minimum cost assignment!

	Free(weight); weight = costMatrix = NULL;

	// get mapping (pseudonym -> observed trace)
	map<ull, Trace*> traceMapping = map<ull, Trace*>();
	input->GetMapping(traceMapping);

	VERIFY(Nusers == traceMapping.size());

	map<ull, ull> userToPseudonymMapping = map<ull, ull>();
	ull* pseudonyms = (ull*)Allocate(byteSizeVector);
	memset(pseudonyms, 0, byteSizeVector);
	ull* users = (ull*)Allocate(byteSizeVector);
	memset(users, 0, byteSizeVector);

	VERIFY(pseudonyms != NULL && users != NULL);

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
	Free(mapping);

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

	// de-obfuscation

	ull outputByteSize = Nusers * numTimes * numLoc * sizeof(double);
	double* locationDistribution = (double*)Allocate(outputByteSize);
	VERIFY(locationDistribution != NULL);
	memset(locationDistribution, 0, outputByteSize);

	VERIFY(ComputeLocationDistribution(input, &userToPseudonymMapping, locationDistribution) == true);

	userToPseudonymMapping.clear();

	output->SetProbabilityDistribution(locationDistribution);

	return true;

  // Bouml preserved body end 0004D011
}

bool WeakAttackOperation::ComputeLikelihood(const TraceSet* trace, double** matrix) const 
{
  // Bouml preserved body begin 00052111

	const double bigNumber = 1e20;
	const double bigNumberInverse = 1.0 / bigNumber;

	// get user profiles
	map<ull, UserProfile*> profiles = map<ull, UserProfile*>();
	VERIFY(context->GetProfiles(profiles) == true);

	ull Nusers = profiles.size();

	// get mapping (pseudonym -> observed trace)
	map<ull, Trace*> mapping = map<ull, Trace*>();
	trace->GetMapping(mapping);

	VERIFY(Nusers == mapping.size());

	ull minTime = 0; ull maxTime = 0;
	VERIFY(Parameters::GetInstance()->GetTimestampsRange(&minTime, &maxTime) == true);
	ull numTimes = maxTime - minTime + 1;

	ull minLoc = 0; ull maxLoc = 0;
	VERIFY(Parameters::GetInstance()->GetLocationstampsRange(&minLoc, &maxLoc) == true);

	ull byteSize = (Nusers * Nusers) * sizeof(double);
	double* likelihood = *matrix = (double*)Allocate(byteSize);
	VERIFY(likelihood != NULL);
	memset(likelihood, 0, byteSize);

	ull userIndex = 0;
	pair_foreach_const(map<ull, UserProfile*>, profiles, usersIter)
	{
		ull user = usersIter->first;
		UserProfile* profile = usersIter->second;

		double* steadyStateVector = NULL;
		VERIFY(profile->GetSteadyStateVector(&steadyStateVector) == true);
		VERIFY(steadyStateVector != NULL);

		ull pseudonymIndex = 0;
		pair_foreach_const(map<ull, Trace*>, mapping, pseudonymsIter)
		{
			// ull pseudonym = pseudonymsIter->first;
			Trace* observedTrace = pseudonymsIter->second;

			vector<Event*> events = vector<Event*>();
			observedTrace->GetEvents(events);

			VERIFY(numTimes == events.size());

			double logsum = 0.0;

			ull tm = minTime;
			foreach_const(vector<Event*>, events, eventsIter)
			{
				ObservedEvent* observedEvent = dynamic_cast<ObservedEvent*>(*eventsIter);
				set<ull> timestamps = set<ull>();
				observedEvent->GetTimestamps(timestamps);

				VERIFY(timestamps.size() == 1);

				ull timestamp = *(timestamps.begin());

				VERIFY(tm == timestamp && (timestamp >= minTime && timestamp <= maxTime));

				ull tp = Parameters::GetInstance()->LookupTimePeriod(timestamp);
				if(tp == INVALID_TIME_PERIOD)
				{
					SET_ERROR_CODE(ERROR_CODE_INCONSISTENT_TIME_PARTITIONING_USAGE);
					return false;
				}

				// get the proper sub-chain steady-state vector according to the time period of the event
				double* subChainSteadyStateVector = NULL;
				VERIFY(Algorithms::GetSteadyStateVectorOfSubChain(steadyStateVector, tp, &subChainSteadyStateVector) == true);


				double sum = 0.0;
				for(ull loc = minLoc; loc <= maxLoc; loc++)
				{
					ActualEvent* actualEvent = new ActualEvent(user, timestamp, loc);
					ExposedEvent* exposedEvent = new ExposedEvent(*actualEvent);

					VERIFY(actualEvent != NULL && exposedEvent != NULL);

					double lppmProb0 = lppmPDF->PDF(context, actualEvent, observedEvent);
					double applicationProb0 = applicationPDF->PDF(context, actualEvent, actualEvent);

					double lppmProb1 = lppmPDF->PDF(context, exposedEvent, observedEvent);
					double applicationProb1 = applicationPDF->PDF(context, actualEvent, exposedEvent);

					double presenceProb = subChainSteadyStateVector[loc - minLoc];

					sum += ((lppmProb0 * applicationProb0) + (lppmProb1 * applicationProb1)) * presenceProb;

					/*
					stringstream info("");
					info << "prob: " << user << ", " << pseudonym << ", "<< timestamp << ", " << loc << ", ";
					info << lppmProb0 << ", " << applicationProb0 << ", " << lppmProb1 << ", " << applicationProb1 << ", ";
					info << presenceProb << " = " << sum;
					Log::GetInstance()->Append(info.str());
					*/

					actualEvent->Release();
					exposedEvent->Release();
				}

				Free(subChainSteadyStateVector); // free the sub-chain steady-state vector

				// take care of small sum
				VERIFY(sum == 0.0 || sum > bigNumberInverse);

				if(sum == 0.0) { sum = DBL_MIN; }

				logsum += log(sum);

				/*
				stringstream info(""); info.precision(16);
				info << "logsum: " << userIndex << ", " << pseudonymIndex << " = "<< sum << ", " << tmp << ", " << logsum;
				Log::GetInstance()->Append(info.str());
				*/

				tm++;
			}

			likelihood[GET_INDEX(userIndex, pseudonymIndex, Nusers)] = logsum;

			pseudonymIndex++;
		}
		userIndex++;
	}

	return true;

  // Bouml preserved body end 00052111
}

bool WeakAttackOperation::ComputeLocationDistribution(const TraceSet* trace, const map<ull, ull>* mapping, double* locationDistribution) const 
{
  // Bouml preserved body begin 00053B11

	const double bigNumber = 1e20;
	const double bigNumberInverse = 1.0 / bigNumber;

	VERIFY(trace != NULL && mapping != NULL && locationDistribution != NULL);

	ull minTime = 0; ull maxTime = 0;
	VERIFY(Parameters::GetInstance()->GetTimestampsRange(&minTime, &maxTime) == true);
	ull numTimes = maxTime - minTime + 1;

	ull minLoc = 0; ull maxLoc = 0;
	VERIFY(Parameters::GetInstance()->GetLocationstampsRange(&minLoc, &maxLoc) == true);
	ull numLoc = maxLoc - minLoc + 1;

	stringstream info2("");
	info2 << "minTime: " << minTime << " maxTime: " << maxTime << " minLoc: " << minLoc << " maxLoc: " << maxLoc;

	Log::GetInstance()->Append(info2.str());

	// get profiles
	map<ull, UserProfile*> profiles = map<ull, UserProfile*>();
	VERIFY(context->GetProfiles(profiles) == true);

	// get mapping (pseudonym -> observed trace)
	map<ull, Trace*> observedTraces = map<ull, Trace*>();
	trace->GetMapping(observedTraces);

	// for each user
	ull userIndex = 0;
	pair_foreach_const(map<ull, UserProfile*>, profiles, userIter)
	{
		ull user = userIter->first;
		UserProfile* profile = userIter->second;

		double* steadyStateVector = NULL;
		profile->GetSteadyStateVector(&steadyStateVector);
		VERIFY(steadyStateVector != NULL);

		map<ull, ull>::const_iterator iter = mapping->find(user);
		VERIFY(iter != mapping->end());

		ull pseudonym = iter->second;

		map<ull, Trace*>::const_iterator traceIter = observedTraces.find(pseudonym);
		VERIFY(traceIter != observedTraces.end());

		Trace* observedTrace = traceIter->second;
		vector<Event*> events = vector<Event*>();
		observedTrace->GetEvents(events);

		ull tm = minTime;
		foreach_const(vector<Event*>, events, eventsIter)
		{
			ObservedEvent* observedEvent = dynamic_cast<ObservedEvent*>(*eventsIter);
			set<ull> timestamps = set<ull>();
			observedEvent->GetTimestamps(timestamps);

			VERIFY(timestamps.size() == 1);

			ull timestamp = *(timestamps.begin());

			VERIFY(tm == timestamp && (timestamp >= minTime && timestamp <= maxTime));


			ull tp = Parameters::GetInstance()->LookupTimePeriod(timestamp);
			if(tp == INVALID_TIME_PERIOD)
			{
				SET_ERROR_CODE(ERROR_CODE_INCONSISTENT_TIME_PARTITIONING_USAGE);
				return false;
			}

			// get the proper sub-chain steady-state vector according to the time period of the event
			double* subChainSteadyStateVector = NULL;
			VERIFY(Algorithms::GetSteadyStateVectorOfSubChain(steadyStateVector, tp, &subChainSteadyStateVector) == true);


			double sum = 0.0;
			for(ull loc = minLoc; loc <= maxLoc; loc++)
			{
				double prob = 0.0;

				ActualEvent* actualEvent = new ActualEvent(user, timestamp, loc);
				ExposedEvent* exposedEvent = new ExposedEvent(*actualEvent);

				VERIFY(actualEvent != NULL && exposedEvent != NULL);

				double lppmProb0 = lppmPDF->PDF(context, actualEvent, observedEvent);
				double applicationProb0 = applicationPDF->PDF(context, actualEvent, actualEvent);

				double lppmProb1 = lppmPDF->PDF(context, exposedEvent, observedEvent);
				double applicationProb1 = applicationPDF->PDF(context, actualEvent, exposedEvent);

				double presenceProb = subChainSteadyStateVector[loc - minLoc];

				prob = ((lppmProb0 * applicationProb0) + (lppmProb1 * applicationProb1)) * presenceProb;

				/*
				stringstream info("");
				info << "prob: " << user << ", " << timestamp << ", " << loc << ", ";
				info << lppmProb0 << ", " << applicationProb0 << ", " << lppmProb1 << ", " << applicationProb1 << ", ";
				info << presenceProb << " = " << prob;
				Log::GetInstance()->Append(info.str());
				*/

				actualEvent->Release();
				exposedEvent->Release();

				// take care of small prob
				VERIFY(prob == 0.0 || prob > bigNumberInverse);

				ull index = GET_INDEX_3D(userIndex, (timestamp - minTime), (loc - minLoc), numTimes, numLoc);
				locationDistribution[index] = (double)prob;

				/*
				stringstream info("");
				info << "Calculated the location distribution at index " << index << " user: " << user;
				info << " userIndex: " << userIndex << " time: " << timestamp;
				info << " loc: " << loc << " memory addr: " << hex << (ull)&locationDistribution[index];

				Log::GetInstance()->Append(info.str());
				*/

				sum += prob;
			}

			Free(subChainSteadyStateVector); // free the sub-chain steady-state vector

			// normalization
			for(ull loc = minLoc; loc <= maxLoc; loc++)
			{
				ull index = GET_INDEX_3D(userIndex, (timestamp - minTime), (loc - minLoc), numTimes, numLoc);
				locationDistribution[index] /= (double)sum;
			}

			tm++;
		}
		userIndex++;
	}

	return true;

  // Bouml preserved body end 00053B11
}


} // namespace lpm
