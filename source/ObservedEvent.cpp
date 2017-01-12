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
#include "../include/ObservedEvent.h"

namespace lpm {

ObservedEvent::ObservedEvent(ull nym) 
{
  // Bouml preserved body begin 0003D111

	pseudonym = nym;
	timestamp = set<ull>();
	locationstamp = set<ull>();

  // Bouml preserved body end 0003D111
}

ObservedEvent::~ObservedEvent() 
{
  // Bouml preserved body begin 0003D191

	timestamp.clear();
	locationstamp.clear();

  // Bouml preserved body end 0003D191
}

EventType ObservedEvent::GetType() const 
{
  // Bouml preserved body begin 00039591

	return Observed;

  // Bouml preserved body end 00039591
}

ull ObservedEvent::GetPseudonym() const 
{
  // Bouml preserved body begin 00036091

	return pseudonym;

  // Bouml preserved body end 00036091
}

bool ObservedEvent::AddTimestamp(ull time) 
{
  // Bouml preserved body begin 0003D211

	ull min = 0; ull max = 0;
	DEBUG_VERIFY(Parameters::GetInstance()->GetTimestampsRange(&min, &max) == true);

	if(time < min || time > max)
	{
		SET_ERROR_CODE(ERROR_CODE_INVALID_ARGUMENTS);
		return false;
	}

	if(timestamp.find(time) == timestamp.end())
	{
		timestamp.insert(time);
		return true;
	}

	return true;

  // Bouml preserved body end 0003D211
}

bool ObservedEvent::AddLocationstamp(ull loc) 
{
  // Bouml preserved body begin 0003D291

	ull min = 0; ull max = 0;
	DEBUG_VERIFY(Parameters::GetInstance()->GetLocationstampsRange(&min, &max) == true);

	if(loc < min || loc > max)
	{
		SET_ERROR_CODE(ERROR_CODE_INVALID_ARGUMENTS);
		return false;
	}

	if(locationstamp.find(loc) == locationstamp.end())
	{
		locationstamp.insert(loc);
		return true;
	}

	return true;


  // Bouml preserved body end 0003D291
}

void ObservedEvent::GetTimestamps(set<ull>& ret) const 
{
  // Bouml preserved body begin 00040911

	ret = timestamp;

  // Bouml preserved body end 00040911
}

void ObservedEvent::GetLocationstamps(set<ull>& ret) const 
{
  // Bouml preserved body begin 00040991

	ret = locationstamp;

  // Bouml preserved body end 00040991
}


} // namespace lpm
