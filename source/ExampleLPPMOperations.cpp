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
//! \brief Provides some implementation of examples LPPM operations by implementing the LPPMOperation class
//!
#include "../include/ExampleLPPMOperations.h"
#include "../include/Context.h"
#include "../include/ActualEvent.h"
#include "../include/ObservedEvent.h"
#include "../include/ExposedEvent.h"
#include "../include/Event.h"

namespace lpm {

DefaultLPPMOperation::DefaultLPPMOperation(ushort obfuscationLevel, double fakeInjectionProbability, const FakeInjectionAlgorithm fakeInjectionAlgorithm, double hidingProbability) : LPPMOperation("Default LPPMOperation")
{
  // Bouml preserved body begin 0003EF11

	this->obfuscationLevel = obfuscationLevel;
	this->fakeInjectionProbability = fakeInjectionProbability;
	this->fakeInjectionAlgorithm = fakeInjectionAlgorithm;
	this->hidingProbability = hidingProbability;

  // Bouml preserved body end 0003EF11
}

DefaultLPPMOperation::~DefaultLPPMOperation() 
{
  // Bouml preserved body begin 0003EF91
  // Bouml preserved body end 0003EF91
}

void DefaultLPPMOperation::ObfuscateLocation(ull location, set<ull>& obfucatedSet) const 
{
  // Bouml preserved body begin 00050711

	ull minLoc = 0; ull maxLoc = 0;
	VERIFY(Parameters::GetInstance()->GetLocationstampsRange(&minLoc, &maxLoc) == true);

	obfucatedSet.clear();
	ull numObf = (ull)pow(2, obfuscationLevel);

	ull firstObfLoc = location - ((location - minLoc) % numObf);
	for(ull i = 0; i < numObf; i++)
	{
		ull obf = firstObfLoc + i;

		if(obf >= minLoc && obf <= maxLoc)
		{
			obfucatedSet.insert(obf);
		}
	}

	VERIFY(obfuscationLevel != 0 || (obfuscationLevel == 0 && obfucatedSet.size() == 1 && firstObfLoc == location));

  // Bouml preserved body end 00050711
}

void DefaultLPPMOperation::ComputeGeneralStatistics(ull tp, double** avg) const 
{
  // Bouml preserved body begin 00050791

	VERIFY(avg != NULL);

	ull minLoc = 0; ull maxLoc = 0;
	VERIFY(Parameters::GetInstance()->GetLocationstampsRange(&minLoc, &maxLoc) == true);
	ull numLoc = (maxLoc - minLoc + 1);

	map<ull, UserProfile*> profiles = map<ull, UserProfile*>();
	VERIFY(context->GetProfiles(profiles) == true);

	ull sizeInBytes = numLoc * sizeof(double);
	double* tmp = *avg = (double*)Allocate(sizeInBytes);
	memset(tmp, 0, sizeInBytes);

	// for each user
	pair_foreach_const(map<ull, UserProfile*>, profiles, iter)
	{
		UserProfile* profile = iter->second;
		VERIFY(profile != NULL);

		double* steadyStateVector = NULL;
		VERIFY(profile->GetSteadyStateVector(&steadyStateVector) == true);

		// get the proper sub-chain steady-state vector according to the time period of the event
		double* subChainSteadyStateVector = NULL;
		VERIFY(Algorithms::GetSteadyStateVectorOfSubChain(steadyStateVector, tp, &subChainSteadyStateVector) == true);

		for(ull loc = minLoc; loc <= maxLoc; loc++)
		{
			ull locIndex = loc - minLoc;
			tmp[locIndex] += subChainSteadyStateVector[locIndex];
		}

		Free(subChainSteadyStateVector); // free the sub-chain steady-state vector
	}

	// normalization
	ull profileCount = profiles.size();
	for(ull loc = minLoc; loc <= maxLoc; loc++)
	{
		tmp[loc - minLoc] /= (double)profileCount;
	}

  // Bouml preserved body end 00050791
}

bool DefaultLPPMOperation::Filter(const Context* context, const ActualEvent* inEvent, ObservedEvent** outEvent) 
{
  // Bouml preserved body begin 0003ED11

	if(context == NULL || inEvent == NULL || outEvent == NULL)
	{
		SET_ERROR_CODE(ERROR_CODE_INVALID_ARGUMENTS);
		return false;
	}

	VERIFY(inEvent->GetType() == Actual || inEvent->GetType() == Exposed);

	ull pseudonym = GetPseudonym(inEvent->GetUser());

	ObservedEvent* event = *outEvent = new ObservedEvent(pseudonym);
	ull timestamp = inEvent->GetTimestamp();
	event->AddTimestamp(timestamp);

	ull minLoc = 0; ull maxLoc = 0;
	VERIFY(Parameters::GetInstance()->GetLocationstampsRange(&minLoc, &maxLoc) == true);
	ull numLoc = (maxLoc - minLoc + 1);

	ull location = inEvent->GetLocationstamp();

	if(inEvent->GetType() == Actual)
	{
		// no fake injection
		if(fakeInjectionProbability == 0.0 || RNG::GetInstance()->GetUniformRandomDouble() <= (1.0 - fakeInjectionProbability))
		{
			//stringstream info("");
			//info << "Obs: " << nym << ", " << timestamp << ", {}, 0";
			//Log::GetInstance()->Append(info.str());

			VERIFY(ABS(PDF(context, inEvent, *outEvent) - (1.0 - fakeInjectionProbability)) <= EPSILON);

			return true;
		}

		ull fakeLocation = 0;

		switch(fakeInjectionAlgorithm)
		{
			case UniformSelection:
				fakeLocation = RNG::GetInstance()->GetUniformRandomULLBetween(minLoc, maxLoc);
				break;
			case GeneralStatisticsSelection:
				{
					double* avg = NULL;
					ull tp = Parameters::GetInstance()->LookupTimePeriod(timestamp);
					if(tp == INVALID_TIME_PERIOD)
					{
						SET_ERROR_CODE(ERROR_CODE_INCONSISTENT_TIME_PARTITIONING_USAGE);
						return false;
					}

					ComputeGeneralStatistics(tp, &avg);

					fakeLocation = minLoc + RNG::GetInstance()->SampleIndexFromVector(avg, numLoc);

					Free(avg); // Free

					VERIFY(fakeLocation >= minLoc && fakeLocation <= maxLoc);
				}
				break;
			default:
				CODING_ERROR; break;
		}

		location = fakeLocation;
	}
	else if(hidingProbability != 0.0 && RNG::GetInstance()->GetUniformRandomDouble() <= hidingProbability) // event is exposed and we should hide it
	{
		VERIFY(inEvent->GetType() == Exposed && ABS(PDF(context, inEvent, *outEvent) - hidingProbability) < EPSILON);

		return true; // outEvent has no location set
	}

	// location obfuscation
	set<ull> obfucatedLocations = set<ull>();
	ObfuscateLocation(location, obfucatedLocations);

	//stringstream info("");
	//info << "Obs: " << nym << ", " << timestamp << ", {";

	foreach_const(set<ull>, obfucatedLocations, iter)
	{
		event->AddLocationstamp(*iter);
		//info << *iter << ", ";
	}
	obfucatedLocations.clear();

	//info << "}, " << ((inEvent->GetType() == Actual) ? 0 : 1);
	//Log::GetInstance()->Append(info.str());

	VERIFY(PDF(context, inEvent, *outEvent) != 0.0);

	return true;

  // Bouml preserved body end 0003ED11
}

double DefaultLPPMOperation::PDF(const Context* context, const ActualEvent* inEvent, const ObservedEvent* outEvent) const 
{
  // Bouml preserved body begin 00042591

	VERIFY(context != NULL && inEvent != NULL && outEvent != NULL);

	ull minLoc = 0; ull maxLoc = 0;
	VERIFY(Parameters::GetInstance()->GetLocationstampsRange(&minLoc, &maxLoc) == true);
	ull numLoc = maxLoc - minLoc + 1;

	ull trueTimestamp = inEvent->GetTimestamp();

	set<ull> timestamps = set<ull>();
	outEvent->GetTimestamps(timestamps);

	if(timestamps.size() != 1 || timestamps.find(trueTimestamp) == timestamps.end()) { return 0.0; }

	set<ull> locs = set<ull>();
	outEvent->GetLocationstamps(locs);

	if(locs.empty() == true)
	{
		if(inEvent->GetType() == Actual) { return 1.0 - fakeInjectionProbability; }
		else if(inEvent->GetType() == Exposed) { return hidingProbability; }
	}


	if(inEvent->GetType() == Exposed)
	{
		ull trueLocation = inEvent->GetLocationstamp();
		set<ull> locations = set<ull>();

		outEvent->GetLocationstamps(locations);

		set<ull> obfuscatedLocations = set<ull>();
		ObfuscateLocation(trueLocation, obfuscatedLocations);

		if(locations.size() != obfuscatedLocations.size()) { return 0.0; }

		foreach_const(set<ull>, locations, iter)
		{
			ull loc = *iter;

			if(obfuscatedLocations.find(loc) == obfuscatedLocations.end()) { return 0.0; }
		}

		return 1.0 - hidingProbability;
	}

	// event is Actual
	set<ull> locationstamps = set<ull>();
	outEvent->GetLocationstamps(locationstamps);

	// check that locationstamps is valid according to obfucationLevel
	ull numObf = (ull)pow(2, obfuscationLevel);
	ull min = 0; ull max = 0;
	foreach_const(set<ull>, locationstamps, iter)
	{
		if(min == 0) { min = max = *iter; }
		if(*iter < min) { min = *iter; }
		if(*iter > max) { max = *iter; }
	}

	// continuity check
	if(min + (locationstamps.size() - 1) != max) { return 0.0; }

	if(((min - minLoc)  % numObf) != 0 && min != minLoc) { return 0.0; }

	if((((max - minLoc) + 1) % numObf) != 0 && max != maxLoc) { return 0.0; }

	if(locationstamps.size() > numObf) { return 0.0; }

	switch(fakeInjectionAlgorithm)
	{
		case UniformSelection:
			{
				return fakeInjectionProbability * ((double)locationstamps.size() / (double)numLoc);
			}
			break;
		case GeneralStatisticsSelection:
			{
				double* avg = NULL;
				ull tp = Parameters::GetInstance()->LookupTimePeriod(trueTimestamp);
				if(tp == INVALID_TIME_PERIOD)
				{
					SET_ERROR_CODE(ERROR_CODE_INCONSISTENT_TIME_PARTITIONING_USAGE);
					return false;
				}

				ComputeGeneralStatistics(tp, &avg);

				double sum = 0.0;
				foreach_const(set<ull>, locationstamps, iter)
				{
					ull loc = *iter;

					sum += avg[loc - minLoc];
				}

				Free(avg);

				return fakeInjectionProbability * sum;
			}
			break;
		default:
			CODING_ERROR; break;
	}

	return 0.0;

  // Bouml preserved body end 00042591
}

string DefaultLPPMOperation::GetDetailString() 
{
  // Bouml preserved body begin 00095E11

	string fakeAlgo = "";
	switch(fakeInjectionAlgorithm)
	{
		case UniformSelection:	fakeAlgo = STRINGIFY(UniformSelection); break;
		case GeneralStatisticsSelection: fakeAlgo = STRINGIFY(GeneralStatisticsSelection); break;
	}

	stringstream details("");
	details << "LPPMOperation: " << operationName << "(" << obfuscationLevel << ", " << (double)fakeInjectionProbability << ", " << fakeAlgo;
	details << ", " << (double)hidingProbability << ")";

	return details.str();

  // Bouml preserved body end 00095E11
}

} // namespace lpm
