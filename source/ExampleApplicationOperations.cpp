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
//! \brief Provides some examples of application operations by implementing the ApplicationOperation class
//!
#include "../include/ExampleApplicationOperations.h"
#include "../include/Context.h"
#include "../include/ActualEvent.h"

namespace lpm {

DefaultApplicationOperation::DefaultApplicationOperation(double mu, ApplicationType type) : ApplicationOperation("Default ApplicationOperation")
{
  // Bouml preserved body begin 0003EE11

	this->mu = mu;
	this->appType = type;

  // Bouml preserved body end 0003EE11
}

DefaultApplicationOperation::~DefaultApplicationOperation() 
{
  // Bouml preserved body begin 0003EE91
  // Bouml preserved body end 0003EE91
}

bool DefaultApplicationOperation::Filter(const Context* context, const ActualEvent* inEvent, ActualEvent** outEvent) 
{
  // Bouml preserved body begin 0003EC11

	if(context == NULL || inEvent == NULL || outEvent == NULL)
	{
		SET_ERROR_CODE(ERROR_CODE_INVALID_ARGUMENTS);
		return false;
	}

	ExposedEvent* exposedEvent = new ExposedEvent(*inEvent);

	if(RNG::GetInstance()->GetUniformRandomDouble() <= PDF(context, inEvent, exposedEvent))
	{
		*outEvent = exposedEvent; // expose
	}
	else
	{
		exposedEvent->Release();

		*outEvent = new ActualEvent(*inEvent); // do not expose
	}

	return true;

  // Bouml preserved body end 0003EC11
}

double DefaultApplicationOperation::PDF(const Context* context, const ActualEvent* inEvent, const ActualEvent* outEvent) const 
{
  // Bouml preserved body begin 00042491

	VERIFY(outEvent != NULL);

	if(inEvent->GetUser() != outEvent->GetUser() ||
		inEvent->GetTimestamp() != outEvent->GetTimestamp() ||
		inEvent->GetLocationstamp() != outEvent->GetLocationstamp())
	{
		return 0.0;
	}

	if(appType == Basic)
	{
		return (outEvent->GetType() == Exposed) ? mu : 1.0 - mu;
	}
	else if(appType == LocalSearch)
	{
		ull user = inEvent->GetUser();
		ull location = inEvent->GetLocationstamp();
		ull timestamp = inEvent->GetTimestamp();


		ull minLoc = 0; ull maxLoc = 0;
		VERIFY(Parameters::GetInstance()->GetLocationstampsRange(&minLoc, &maxLoc) == true);

		ull tp = Parameters::GetInstance()->LookupTimePeriod(timestamp);
		if(tp == INVALID_TIME_PERIOD)
		{
			SET_ERROR_CODE(ERROR_CODE_INCONSISTENT_TIME_PARTITIONING_USAGE);
			return false;
		}


		UserProfile* profile = NULL;
		VERIFY(context->GetUserProfile(user, &profile) == true);
		VERIFY(profile != NULL);

		double* steadyStateVector = NULL;
		VERIFY(profile->GetSteadyStateVector(&steadyStateVector) == true);

		// get the proper sub-chain steady-state vector according to the time period of the event
		double* subChainSteadyStateVector = NULL;
		VERIFY(Algorithms::GetSteadyStateVectorOfSubChain(steadyStateVector, tp, &subChainSteadyStateVector) == true);

		double probExposure = mu * (1.0 - subChainSteadyStateVector[location - minLoc]);

		Free(subChainSteadyStateVector); // free the sub-chain steady-state vector

		return (outEvent->GetType() == Exposed) ? probExposure : 1.0 - probExposure;
	}
	else
	{
		CODING_ERROR;
		return 0.0;
	}

  // Bouml preserved body end 00042491
}

string DefaultApplicationOperation::GetDetailString() 
{
  // Bouml preserved body begin 00095E91

	string type = "";
	switch(appType)
	{
		case Basic:	type = STRINGIFY(Basic); break;
		case LocalSearch: type = STRINGIFY(LocalSearch); break;
	}

	stringstream details("");
	details << "ApplicationOperation: " << operationName << "(" << mu << ", " << type << ")";

	return details.str();

  // Bouml preserved body end 00095E91
}


} // namespace lpm
