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
#include "../include/LPPMOperation.h"
#include "../include/Context.h"
#include "../include/Event.h"
#include "../include/ActualEvent.h"
#include "../include/ObservedEvent.h"
#include "../include/TraceSet.h"
#include "../include/ExposedEvent.h"

namespace lpm {

void LPPMOperation::ComputeAnonymizationMap() 
{
  // Bouml preserved body begin 00074211

	VERIFY(anonymization.empty() == true);

	// ull min = 0; ull max = 0;
	// GUR: ### VERIFY(Parameters::GetInstance()->GetUsersRange(&min, &max) == true);

	set<ull> usersSet = set<ull>();
	VERIFY(Parameters::GetInstance()->GetUsersSet(usersSet) == true);
	VERIFY(usersSet.size() > 0);

	ull numUsers = usersSet.size();
	ull byteSize = numUsers * sizeof(ull);

	ull* users = (ull*)Allocate(byteSize);
	ull* pseudonyms = (ull*)Allocate(byteSize);

	VERIFY(users != NULL);
	VERIFY(pseudonyms != NULL);

	memset(users, 0, byteSize);
	memset(pseudonyms, 0, byteSize);

	ull i = 0;
	foreach_const(set<ull>, usersSet, iter) { users[i] = *iter; i++; }

	RNG::GetInstance()->RandomPermutation(users, numUsers, pseudonyms);

	for(ull i = 0; i < numUsers; i++)
	{
		ull user = users[i];
		ull pseudonym = pseudonyms[i];

		VERIFY(user != 0 && pseudonym != 0);

		anonymization.insert(pair<ull, ull>(user, pseudonym));
	}

	Free(users);
	Free(pseudonyms);

  // Bouml preserved body end 00074211
}

LPPMOperation::LPPMOperation(string name) : FilterOperation(name) 
{
  // Bouml preserved body begin 00022211

	anonymization = map<ull, ull>();

  // Bouml preserved body end 00022211
}

LPPMOperation::~LPPMOperation() 
{
  // Bouml preserved body begin 00022291

	anonymization.clear();

  // Bouml preserved body end 00022291
}

bool LPPMOperation::Filter(const Context* context, const Event* inEvent, Event** outEvent) 
{
  // Bouml preserved body begin 0003B611

	if(anonymization.empty())
	{
		ComputeAnonymizationMap();

#define LOG_ANONYMIZATION 1
#ifdef LOG_ANONYMIZATION

	Log::GetInstance()->Append("LPPM Anonymization Mapping (user -> pseudonym):");
	pair_foreach_const(map<ull, ull>, anonymization, iter)
	{
		stringstream ss("");
		ss << iter->first << " -> " << iter->second;
		Log::GetInstance()->Append(ss.str());
	}

#endif

	}

	return const_cast<LPPMOperation*>(this)->Filter(context, (const ActualEvent*)inEvent, (ObservedEvent**)outEvent);

  // Bouml preserved body end 0003B611
}

double LPPMOperation::PDF(const Context* context, const Event* inEvent, const Event* outEvent) const 
{
  // Bouml preserved body begin 00027911

	// VERIFY(anonymization.empty() == false);

	return PDF(context, (const ActualEvent*)inEvent, (const ObservedEvent*)outEvent);

  // Bouml preserved body end 00027911
}

string LPPMOperation::GetDetailString() 
{
  // Bouml preserved body begin 0002EE91

	return "LPPMOperation: " + operationName;

  // Bouml preserved body end 0002EE91
}

ull LPPMOperation::GetPseudonym(ull user) const 
{
  // Bouml preserved body begin 0003D011

	VERIFY(anonymization.empty() == false);

	map<ull, ull>::const_iterator iter = anonymization.find(user);
	VERIFY(iter != anonymization.end());

	return iter->second;

  // Bouml preserved body end 0003D011
}

void LPPMOperation::GetAnonymizationMap(map<ull, ull>& map) const 
{
  // Bouml preserved body begin 00075B91

	map = anonymization;

  // Bouml preserved body end 00075B91
}


} // namespace lpm
