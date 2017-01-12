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
#include "../include/Metrics.h"
#include "../include/AttackOutput.h"
#include "../include/File.h"

namespace lpm {

DistortionMetricOperation::DistortionMetricOperation(MetricType type) : MetricOperation(type)
{
  // Bouml preserved body begin 0004D111

	distanceFunction = NULL;
	operationName = "DistortionMetricOperation";

  // Bouml preserved body end 0004D111
}

DistortionMetricOperation::~DistortionMetricOperation() 
{
  // Bouml preserved body begin 0004D191
  // Bouml preserved body end 0004D191
}

bool DistortionMetricOperation::Execute(const AttackOutput* input, File* output) 
{
  // Bouml preserved body begin 0004D211

	Log::GetInstance()->Append("Entering DistortionMetricOperation::Execute");

	VERIFY(input != NULL && output != NULL);

	ull minTime = 0; ull maxTime = 0;
	VERIFY(Parameters::GetInstance()->GetTimestampsRange(&minTime, &maxTime) == true);
	ull numTimes = maxTime - minTime + 1;

	ull minLoc = 0; ull maxLoc = 0;
	VERIFY(Parameters::GetInstance()->GetLocationstampsRange(&minLoc, &maxLoc) == true);
	ull numLoc = maxLoc - minLoc + 1;

	map<ull, Trace*> mapping = map<ull, Trace*>();
	actualTraceSet->GetMapping(mapping);

	double* localizationDistribution = NULL;
	VERIFY(input->GetProbabilityDistribution(&localizationDistribution) == true);
	VERIFY(localizationDistribution != NULL);

	VERIFY(distanceFunction != NULL);

	ull userIndex = 0;
	pair_foreach_const(map<ull, Trace*>, mapping, userIter)
	{
		ull user = userIter->first;
		Trace* actualTrace = userIter->second;

		vector<Event*> events = vector<Event*>();
		actualTrace->GetEvents(events);

		VERIFY(numTimes == events.size());

		stringstream info("");
		info << "Computing average error for user " << user <<" userIndex " << userIndex;

		Log::GetInstance()->Append(info.str());

		stringstream ss("");
		double avgError = 0.0;
		ss << user << ": ";

		foreach_const(vector<Event*>, events, eventsIter)
		{
			ActualEvent* actualEvent = dynamic_cast<ActualEvent*>(*eventsIter);

			ull timestamp = actualEvent->GetTimestamp();
			ull locationstamp = actualEvent->GetLocationstamp();

			double probError = 0;
			for(ull loc = minLoc; loc <= maxLoc; loc++)
			{
				double distance = distanceFunction->ComputeDistance(locationstamp, loc);

				if(distance != 0.0)
				{
					ull index = GET_INDEX_3D(userIndex, (timestamp - minTime), (loc - minLoc), numTimes, numLoc);
					probError += distance * localizationDistribution[index];
				}
			}
			// output file

			ss << probError;
			if((eventsIter + 1) != events.end())
			{
				ss << ", ";
			}

			avgError += probError;
		}

		avgError /= numTimes;

		double locPrivacy = avgError;

		// output to file
		output->WriteLine(ss.str());

		// log
		ss.str("");
		ss << user << ", " << locPrivacy;
		Log::GetInstance()->Append("Location privacy: " + ss.str());

		userIndex++;
	}

	return true;

  // Bouml preserved body end 0004D211
}

void DistortionMetricOperation::SetDistanceFunction(MetricDistance* function) 
{
  // Bouml preserved body begin 00075F11

	distanceFunction = function;

  // Bouml preserved body end 00075F11
}

string DistortionMetricOperation::GetTypeString() const 
{
  // Bouml preserved body begin 00076091

	return "distortion";

  // Bouml preserved body end 00076091
}

EntropyMetricOperation::EntropyMetricOperation() :  MetricOperation(Entropy)
{
  // Bouml preserved body begin 00070E11
  // Bouml preserved body end 00070E11
}

EntropyMetricOperation::~EntropyMetricOperation() 
{
  // Bouml preserved body begin 00070E91
  // Bouml preserved body end 00070E91
}

bool EntropyMetricOperation::Execute(const AttackOutput* input, File* output) 
{
  // Bouml preserved body begin 00070F11

	Log::GetInstance()->Append("Entering EntropyMetricOperation::Execute");

	VERIFY(input != NULL && output != NULL);

	ull minTime = 0; ull maxTime = 0;
	VERIFY(Parameters::GetInstance()->GetTimestampsRange(&minTime, &maxTime) == true);
	ull numTimes = maxTime - minTime + 1;

	ull minLoc = 0; ull maxLoc = 0;
	VERIFY(Parameters::GetInstance()->GetLocationstampsRange(&minLoc, &maxLoc) == true);
	ull numLoc = maxLoc - minLoc + 1;

	const double maxEntropy = log(numLoc);

	map<ull, Trace*> mapping = map<ull, Trace*>();
	actualTraceSet->GetMapping(mapping);

	double* localizationDistribution = NULL;
	VERIFY(input->GetProbabilityDistribution(&localizationDistribution) == true);
	VERIFY(localizationDistribution != NULL);

	ull userIndex = 0;
	pair_foreach_const(map<ull, Trace*>, mapping, userIter)
	{
		ull user = userIter->first;
		Trace* actualTrace = userIter->second;

		vector<Event*> events = vector<Event*>();
		actualTrace->GetEvents(events);

		VERIFY(numTimes == events.size());

		stringstream info("");
		info << "Computing entropy for user " << user << " userIndex " << userIndex;

		Log::GetInstance()->Append(info.str());

		stringstream ss("");

		ss << user << ": ";

		foreach_const(vector<Event*>, events, eventsIter)
		{
			ActualEvent* actualEvent = dynamic_cast<ActualEvent*>(*eventsIter);

			ull timestamp = actualEvent->GetTimestamp();

			double sum = 0.0;
			double entropy = 0.0;
			for(ull loc = minLoc; loc <= maxLoc; loc++)
			{
				ull index = GET_INDEX_3D(userIndex, (timestamp - minTime), (loc - minLoc), numTimes, numLoc);

				double p = localizationDistribution[index];

				if(p != 0.0)
				{
					VERIFY(p > 0.0 && p <= 1.0);

					sum += p * log(p);
				}
			}
			entropy = (sum == 0.0) ? 0.0 : (-sum / maxEntropy);

			// output file
			ss << entropy;
			if((eventsIter + 1) != events.end())
			{
				ss << ", ";
			}
		}

		// output to file
		output->WriteLine(ss.str());

		// log
		Log::GetInstance()->Append("Entropy: " + ss.str());

		userIndex++;
	}

	return true;

  // Bouml preserved body end 00070F11
}

string EntropyMetricOperation::GetTypeString() const 
{
  // Bouml preserved body begin 00076191

	return "entropy";

  // Bouml preserved body end 00076191
}

MostLikelyLocationDistortionMetricOperation::MostLikelyLocationDistortionMetricOperation() : DistortionMetricOperation(MostLikelyLocationDistortion)
{
  // Bouml preserved body begin 00075C11
  // Bouml preserved body end 00075C11
}

MostLikelyLocationDistortionMetricOperation::~MostLikelyLocationDistortionMetricOperation() 
{
  // Bouml preserved body begin 00075C91
  // Bouml preserved body end 00075C91
}

bool MostLikelyLocationDistortionMetricOperation::Execute(const AttackOutput* input, File* output) 
{
  // Bouml preserved body begin 00075D11

	Log::GetInstance()->Append("Entering MostLikelyLocationDistortionMetricOperation::Execute");

	VERIFY(input != NULL && output != NULL);

	ull minTime = 0; ull maxTime = 0;
	VERIFY(Parameters::GetInstance()->GetTimestampsRange(&minTime, &maxTime) == true);
	ull numTimes = maxTime - minTime + 1;

	ull minLoc = 0; ull maxLoc = 0;
	VERIFY(Parameters::GetInstance()->GetLocationstampsRange(&minLoc, &maxLoc) == true);
	ull numLoc = maxLoc - minLoc + 1;

	map<ull, Trace*> mapping = map<ull, Trace*>();
	actualTraceSet->GetMapping(mapping);

	double* localizationDistribution = NULL;
	VERIFY(input->GetProbabilityDistribution(&localizationDistribution) == true);
	VERIFY(localizationDistribution != NULL);

	ull Nusers = mapping.size();

	// most likely location
	ull mostLikelyLocationByteSize = Nusers * numTimes * sizeof(ull);
	ull* mostLikelyLocation = (ull*)Allocate(mostLikelyLocationByteSize);
	VERIFY(mostLikelyLocation != NULL);
	memset(mostLikelyLocation, 0, mostLikelyLocationByteSize);

	// for each user
	for(ull userIndex = 0; userIndex < Nusers; userIndex++)
	{
		for (ull tm = minTime; tm <= maxTime; tm++)
		{
			double sum = 0.0;

			// most likely location
			ull mostLikelyLoc = minLoc;
			double mostLikelyLocProb = 0.0;

			for (ull loc = minLoc; loc <= maxLoc; loc++)
			{
				ull index = GET_INDEX_3D(userIndex, (tm - minTime), (loc - minLoc), numTimes, numLoc);

				double prob = localizationDistribution[index];


				if(prob > mostLikelyLocProb) // update most likely location for that time
				{
					mostLikelyLoc = loc;
					mostLikelyLocProb = prob;
				}

				stringstream info2("");
				info2 << "       sum " << userIndex << " " << tm << " " << loc << " | " << sum;
				Log::GetInstance()->Append(info2.str());

			}

			// set most likely location for time tm
			mostLikelyLocation[GET_INDEX(userIndex, (tm - minTime), numTimes)] = mostLikelyLoc;
		}
	}

	ull userIndex = 0;
	pair_foreach_const(map<ull, Trace*>, mapping, userIter)
	{
		ull user = userIter->first;

		stringstream info("");
		info << "Computing most likely locations for user " << user << ", userIndex " << userIndex;

		Log::GetInstance()->Append(info.str());

		// output file
		stringstream ss("");
		ss << "Most likely location for user " << user << ": ";

		for(ull tm = minTime; tm <= maxTime; tm++)
		{
			ss << mostLikelyLocation[GET_INDEX(userIndex, (tm - minTime), numTimes)];
			if (tm != maxTime) { ss << ", "; }
		}

		output->WriteLine(ss.str());

		// log
		Log::GetInstance()->Append(ss.str());

		userIndex++;
	}

	output->WriteLine(""); // empty line

	VERIFY(distanceFunction != NULL);

	userIndex = 0;
	pair_foreach_const(map<ull, Trace*>, mapping, userIter)
	{
		ull user = userIter->first;
		Trace* actualTrace = userIter->second;

		vector<Event*> events = vector<Event*>();
		actualTrace->GetEvents(events);

		VERIFY(numTimes == events.size());

		stringstream info("");
		info << "Computing average error for user " << user <<" userIndex " << userIndex;

		Log::GetInstance()->Append(info.str());

		stringstream ss("");
		double avgError = 0.0;
		ss << user << ": ";

		foreach_const(vector<Event*>, events, eventsIter)
		{
			ActualEvent* actualEvent = dynamic_cast<ActualEvent*>(*eventsIter);

			ull timestamp = actualEvent->GetTimestamp();
			ull locationstamp = actualEvent->GetLocationstamp();

			ull loc = mostLikelyLocation[GET_INDEX(userIndex, (timestamp - minTime), numTimes)];
			double error = distanceFunction->ComputeDistance(locationstamp, loc);

			// output file
			ss << error;
			if((eventsIter + 1) != events.end())
			{
				ss << ", ";
			}

			avgError += error;
		}

		avgError /= numTimes;

		double locPrivacy = avgError;

		// output to file
		output->WriteLine(ss.str());

		// log
		ss.str("");
		ss << user << ", " << locPrivacy;
		Log::GetInstance()->Append("Location privacy: " + ss.str());

		userIndex++;
	}

	Free(mostLikelyLocation);

	return true;

  // Bouml preserved body end 00075D11
}

string MostLikelyLocationDistortionMetricOperation::GetTypeString() const 
{
  // Bouml preserved body begin 00076111

	return "most_likely_location_distortion";

  // Bouml preserved body end 00076111
}

AnonymityMetricOperation::AnonymityMetricOperation() : MetricOperation(Anonymity)
{
  // Bouml preserved body begin 00075D91
  // Bouml preserved body end 00075D91
}

AnonymityMetricOperation::~AnonymityMetricOperation() 
{
  // Bouml preserved body begin 00075E11
  // Bouml preserved body end 00075E11
}

bool AnonymityMetricOperation::Execute(const AttackOutput* input, File* output) 
{
  // Bouml preserved body begin 00075E91

	Log::GetInstance()->Append("Entering AnonymityMetricOperation::Execute");

	VERIFY(input != NULL && output != NULL);

	map<ull, ull> reconstructedAnonymizationMap = map<ull, ull>();
	input->GetAnonymizationMap(reconstructedAnonymizationMap);
	VERIFY(reconstructedAnonymizationMap.empty() == false);

	pair_foreach_const(map<ull, ull>, reconstructedAnonymizationMap, iter)
	{
		ull user = iter->first;
		ull nym = iter->second;

		stringstream ss("");
		ss << user << ": ";

		map<ull, ull>::const_iterator iterRec = anonymizationMap.find(user);

		// FIX rev. 285
		// if the adversary de-anonymized that user then no anonymity (i.e. 0), otherwise 1
		if(nym == iterRec->second) { ss << "0"; }
		else { ss << "1"; }

		output->WriteLine(ss.str());

	}

	return true;

  // Bouml preserved body end 00075E91
}

void AnonymityMetricOperation::SetAnonymizationMap(const map<ull,ull>& map) 
{
  // Bouml preserved body begin 00075F91

	anonymizationMap = map;

  // Bouml preserved body end 00075F91
}

string AnonymityMetricOperation::GetTypeString() const 
{
  // Bouml preserved body begin 00076211

	return "anonymity";

  // Bouml preserved body end 00076211
}

double DefaultMetricDistance::ComputeDistance(ull firstLocation, ull secondLocation) const 
{
  // Bouml preserved body begin 00077D11

	return (firstLocation == secondLocation) ? 0.0 : 1.0;

  // Bouml preserved body end 00077D11
}

MostLikelyTraceDistortionMetricOperation::MostLikelyTraceDistortionMetricOperation() : DistortionMetricOperation(MostLikelyTraceDistortion)
{
  // Bouml preserved body begin 0008BC11
  // Bouml preserved body end 0008BC11
}

MostLikelyTraceDistortionMetricOperation::~MostLikelyTraceDistortionMetricOperation() 
{
  // Bouml preserved body begin 0008BC91
  // Bouml preserved body end 0008BC91
}

bool MostLikelyTraceDistortionMetricOperation::Execute(const AttackOutput* input, File* output) 
{
  // Bouml preserved body begin 0008BD11

	Log::GetInstance()->Append("Entering MostLikelyTraceDistortionMetricOperation::Execute");

	VERIFY(input != NULL && output != NULL);

	ull minTime = 0; ull maxTime = 0;
	VERIFY(Parameters::GetInstance()->GetTimestampsRange(&minTime, &maxTime) == true);
	ull numTimes = maxTime - minTime + 1;

	ull minLoc = 0; ull maxLoc = 0;
	VERIFY(Parameters::GetInstance()->GetLocationstampsRange(&minLoc, &maxLoc) == true);

	ull* mostLikelyTrace = NULL;
	VERIFY(input->GetMostLikelyTrace(&mostLikelyTrace) == true);
	VERIFY(mostLikelyTrace != NULL);

	map<ull, Trace*> mapping = map<ull, Trace*>();
	actualTraceSet->GetMapping(mapping);

	// output most likely trace
	ull userIndex = 0;
	pair_foreach_const(map<ull, Trace*>, mapping, userIter)
	{
		ull user = userIter->first;

		stringstream ss("");
		ss << "Most likely trace for user " << user << ": ";

		for(ull tm = minTime; tm <= maxTime; tm++)
		{
			ull index = GET_INDEX(userIndex, (tm - minTime), numTimes);
			ull loc = mostLikelyTrace[index];

			ss << loc;

			if(tm != maxTime) { ss << ", "; }
		}

		output->WriteLine(ss.str());

		userIndex++;
	}

	output->WriteLine(""); // empty line

	VERIFY(distanceFunction != NULL);

	userIndex = 0;
	pair_foreach_const(map<ull, Trace*>, mapping, userIter)
	{
		ull user = userIter->first;
		Trace* actualTrace = userIter->second;

		vector<Event*> events = vector<Event*>();
		actualTrace->GetEvents(events);

		VERIFY(numTimes == events.size());

		stringstream info("");
		info << "Computing average error for user " << user <<" userIndex " << userIndex;

		Log::GetInstance()->Append(info.str());

		stringstream ss("");
		double avgError = 0.0;
		ss << user << ": ";

		foreach_const(vector<Event*>, events, eventsIter)
		{
			ActualEvent* actualEvent = dynamic_cast<ActualEvent*>(*eventsIter);

			ull timestamp = actualEvent->GetTimestamp();
			ull locationstamp = actualEvent->GetLocationstamp();

			ull loc = mostLikelyTrace[GET_INDEX(userIndex, (timestamp - minTime), numTimes)];
			double error = distanceFunction->ComputeDistance(locationstamp, loc);

			// output file
			ss << error;
			if((eventsIter + 1) != events.end())
			{
				ss << ", ";
			}

			avgError += error;
		}

		avgError /= numTimes;

		double locPrivacy = avgError;

		// output to file
		output->WriteLine(ss.str());

		// log
		ss.str("");
		ss << user << ", " << locPrivacy;
		Log::GetInstance()->Append("Location privacy: " + ss.str());

		userIndex++;
	}

	return true;

  // Bouml preserved body end 0008BD11
}

string MostLikelyTraceDistortionMetricOperation::GetTypeString() const 
{
  // Bouml preserved body begin 0008BD91

	return "most_likely_trace_distortion";

  // Bouml preserved body end 0008BD91
}

DensityMetricOperation::DensityMetricOperation() : MetricOperation(Density)
{
  // Bouml preserved body begin 0008BE11
  // Bouml preserved body end 0008BE11
}

DensityMetricOperation::~DensityMetricOperation() 
{
  // Bouml preserved body begin 0008BE91
  // Bouml preserved body end 0008BE91
}

bool DensityMetricOperation::Execute(const AttackOutput* input, File* output) 
{
  // Bouml preserved body begin 0008BF11

	Log::GetInstance()->Append("Entering DensityMetricOperation::Execute");

	VERIFY(input != NULL && output != NULL);

	ull minTime = 0; ull maxTime = 0;
	VERIFY(Parameters::GetInstance()->GetTimestampsRange(&minTime, &maxTime) == true);
	ull numTimes = maxTime - minTime + 1;

	ull minLoc = 0; ull maxLoc = 0;
	VERIFY(Parameters::GetInstance()->GetLocationstampsRange(&minLoc, &maxLoc) == true);
	ull numLoc = maxLoc - minLoc + 1;

	map<ull, Trace*> mapping = map<ull, Trace*>();
	actualTraceSet->GetMapping(mapping);

	double* localizationDistribution = NULL;
	VERIFY(input->GetProbabilityDistribution(&localizationDistribution) == true);
	VERIFY(localizationDistribution != NULL);

	ull Nusers = mapping.size();

	ull actualPresenceByteSize = numLoc * numTimes * sizeof(double);
	double* actualPresence = (double*)Allocate(actualPresenceByteSize);
	VERIFY(actualPresence != NULL);
	memset(actualPresence, 0, actualPresenceByteSize);

	stringstream info("");
	info << "Computing actual user presence!";

	Log::GetInstance()->Append(info.str());

	pair_foreach_const(map<ull, Trace*>, mapping, userIter)
	{
		Trace* actualTrace = userIter->second;

		vector<Event*> events = vector<Event*>();
		actualTrace->GetEvents(events);

		VERIFY(numTimes == events.size());

		foreach_const(vector<Event*>, events, eventsIter)
		{
			ActualEvent* actualEvent = dynamic_cast<ActualEvent*>(*eventsIter);

			ull timestamp = actualEvent->GetTimestamp();
			ull locationstamp = actualEvent->GetLocationstamp();

			ull index = GET_INDEX((locationstamp - minLoc), (timestamp - minTime), numTimes);

			actualPresence[index]++;
		}
	}

	ull reconstructedPresenceByteSize = numLoc * numTimes * sizeof(double);
	double* reconstructedPresence = (double*)Allocate(reconstructedPresenceByteSize);
	VERIFY(reconstructedPresence != NULL);
	memset(reconstructedPresence, 0, reconstructedPresenceByteSize);

	info.str("");
	info << "Computing reconstructed user presence!";

	Log::GetInstance()->Append(info.str());

	for(ull userIndex = 0; userIndex < Nusers; userIndex++)
	{
		for(ull loc = minLoc; loc <= maxLoc; loc++)
		{
			for(ull tm = minTime; tm <= maxTime; tm++)
			{
				ull probIndex = GET_INDEX_3D(userIndex, (loc - minLoc), (tm - minTime), numLoc, numTimes);
				ull presenceIndex = GET_INDEX((loc - minLoc), (tm - minTime), numTimes);

				reconstructedPresence[presenceIndex] += localizationDistribution[probIndex];
			}
		}
	}

	info.str("");
	info << "Outputting density!";

	Log::GetInstance()->Append(info.str());

	stringstream ss("");

	for(ull loc = minLoc; loc <= maxLoc; loc++)
	{
		for(ull tm = minTime; tm <= maxTime; tm++)
		{
			ull index = GET_INDEX((loc - minLoc), (tm - minTime), numTimes);
			double abs = ABS(reconstructedPresence[index] - actualPresence[index]);

			ss << abs;

			if(tm != maxTime) { ss << ", "; }
		}

		output->WriteLine(ss.str());
		ss.str("");
	}

	Free(actualPresence);
	Free(reconstructedPresence);

	return true;

  // Bouml preserved body end 0008BF11
}

string DensityMetricOperation::GetTypeString() const 
{
  // Bouml preserved body begin 0008BF91

	return "density";

  // Bouml preserved body end 0008BF91
}

MeetingDisclosureMetricOperation::MeetingDisclosureMetricOperation() : MetricOperation(MeetingDisclosure)
{
  // Bouml preserved body begin 0008C011
  // Bouml preserved body end 0008C011
}

MeetingDisclosureMetricOperation::~MeetingDisclosureMetricOperation() 
{
  // Bouml preserved body begin 0008C091
  // Bouml preserved body end 0008C091
}

bool MeetingDisclosureMetricOperation::Execute(const AttackOutput* input, File* output) 
{
  // Bouml preserved body begin 0008C111

	Log::GetInstance()->Append("Entering MeetingDisclosureMetricOperation::Execute");

	VERIFY(input != NULL && output != NULL);

	ull minTime = 0; ull maxTime = 0;
	VERIFY(Parameters::GetInstance()->GetTimestampsRange(&minTime, &maxTime) == true);
	ull numTimes = maxTime - minTime + 1;

	ull minLoc = 0; ull maxLoc = 0;
	VERIFY(Parameters::GetInstance()->GetLocationstampsRange(&minLoc, &maxLoc) == true);
	ull numLoc = maxLoc - minLoc + 1;

	map<ull, Trace*> mapping = map<ull, Trace*>();
	actualTraceSet->GetMapping(mapping);

	double* localizationDistribution = NULL;
	VERIFY(input->GetProbabilityDistribution(&localizationDistribution) == true);
	VERIFY(localizationDistribution != NULL);

	ull Nusers = mapping.size();

	VERIFY(Nusers > 1); // makes no sense to use this metric if there is a single user.

	map<ull, ull> usersPairs = map<ull, ull>();
	map<ull, ull> usersIndexPairs = map<ull, ull>();

	ull firstUserIndex = 0;
	pair_foreach_const(map<ull, Trace*>, mapping, userIter)
	{
		ull firstUser = userIter->first;

		ull secondUserIndex = 0;
		pair_foreach_const(map<ull, Trace*>, mapping, userIter2)
		{
			ull secondUser = userIter2->first;

			if(firstUser < secondUser) // consider unordered pairs of distinct users
			{
				usersPairs.insert(pair<ull, ull>(firstUser, secondUser));
				usersIndexPairs.insert(pair<ull, ull>(firstUserIndex, secondUserIndex));
			}

			secondUserIndex++;
		}

		firstUserIndex++;
	}

	ull pairs = usersIndexPairs.size();
	VERIFY(pairs >= 1);

	ull actualMeetingsByteSize = pairs * sizeof(double);
	double* actualMeetings = (double*)Allocate(actualMeetingsByteSize);
	VERIFY(actualMeetings != NULL);
	memset(actualMeetings, 0, actualMeetingsByteSize);

	ull pairIndex = 0;
	pair_foreach_const(map<ull, ull>, usersPairs, pairsIter)
	{
		ull firstUser = pairsIter->first;
		ull secondUser = pairsIter->second;

		VERIFY(mapping.find(firstUser) != mapping.end());
		Trace* firstTrace = (mapping.find(firstUser))->second;

		vector<Event*> firstEvents = vector<Event*>();
		firstTrace->GetEvents(firstEvents);

		VERIFY(mapping.find(secondUser) != mapping.end());
		Trace* secondTrace = (mapping.find(secondUser))->second;

		vector<Event*> secondEvents = vector<Event*>();
		secondTrace->GetEvents(secondEvents);

		VERIFY(firstEvents.size() == secondEvents.size());

		vector<Event*>::const_iterator secondIter = secondEvents.begin();
		foreach_const(vector<Event*>, firstEvents, firstIter)
		{
			ActualEvent* firstEvent = dynamic_cast<ActualEvent*>(*firstIter);
			ActualEvent* secondEvent = dynamic_cast<ActualEvent*>(*secondIter);

			ull tm = firstEvent->GetTimestamp();
			VERIFY(tm == secondEvent->GetTimestamp());

			ull firstLoc = firstEvent->GetLocationstamp();
			ull secondLoc = secondEvent->GetLocationstamp();

			if(firstLoc == secondLoc) // meeting
			{
				actualMeetings[pairIndex]++;
			}

			secondIter++;
		}

		pairIndex++;
	}


	ull reconstructedMeetingsByteSize = pairs * sizeof(double);
	double* reconstructedMeetings = (double*)Allocate(reconstructedMeetingsByteSize);
	VERIFY(reconstructedMeetings != NULL);
	memset(reconstructedMeetings, 0, reconstructedMeetingsByteSize);

	pairIndex = 0;
	pair_foreach_const(map<ull, ull>, usersIndexPairs, indexPairsIter)
	{
		ull firstUserIndex = indexPairsIter->first;
		ull secondUserIndex = indexPairsIter->second;

		for(ull tm = minTime; tm <= maxTime; tm++)
		{
			for(ull loc = minLoc; loc <= maxLoc; loc++)
			{
				ull firstProbIndex = GET_INDEX_3D(firstUserIndex, (loc - minLoc), (tm - minTime), numLoc, numTimes);
				ull secondProbIndex = GET_INDEX_3D(secondUserIndex, (loc - minLoc), (tm - minTime), numLoc, numTimes);

				reconstructedMeetings[pairIndex] += (localizationDistribution[firstProbIndex] * localizationDistribution[secondProbIndex]);
			}
		}

		pairIndex++;
	}

	stringstream info("");
	info << "Outputting meeting disclosure!";

	Log::GetInstance()->Append(info.str());


	pairIndex = 0;
	pair_foreach_const(map<ull, ull>, usersPairs, pairsIter)
	{
		ull firstUser = pairsIter->first;
		ull secondUser = pairsIter->second;

		stringstream ss("");
		double abs = ABS(actualMeetings[pairIndex] - reconstructedMeetings[pairIndex]);
		ss << firstUser << ", " << secondUser << ": " << abs;

		output->WriteLine(ss.str());

		pairIndex++;
	}

	Free(actualMeetings);
	Free(reconstructedMeetings);

	return true;

  // Bouml preserved body end 0008C111
}

string MeetingDisclosureMetricOperation::GetTypeString() const 
{
  // Bouml preserved body begin 0008C191

	return "meeting_disclosure";

  // Bouml preserved body end 0008C191
}


} // namespace lpm
