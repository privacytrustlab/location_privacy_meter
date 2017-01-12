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
#include "../include/EventFormatter.h"
#include "../include/ActualEvent.h"
#include "../include/ObservedEvent.h"

namespace lpm {

bool EventFormatter::FormatActualEvent(const ActualEvent* event, string& line, bool forceExposedOutputFormat) 
{
  // Bouml preserved body begin 0003B291

	if(event == NULL)
	{
		SET_ERROR_CODE(ERROR_CODE_INVALID_ARGUMENTS);
		return false;
	}

	stringstream ss("");

	ss << event->GetUser() << DEFAULT_FIELDS_DELIMITER << ' ';
	ss << event->GetTimestamp() << DEFAULT_FIELDS_DELIMITER << ' ';
	ss << event->GetLocationstamp();

	if(event->GetType() == Exposed) { ss << DEFAULT_FIELDS_DELIMITER  << ' ' << '1'; }
	else if(forceExposedOutputFormat == true) { ss << DEFAULT_FIELDS_DELIMITER  << ' ' << '0'; }

	line = ss.str();

	return true;

  // Bouml preserved body end 0003B291
}

bool EventFormatter::FormatObservedEvent(const ObservedEvent* event, string& line) 
{
  // Bouml preserved body begin 0003B311

	if(event == NULL)
	{
		SET_ERROR_CODE(ERROR_CODE_INVALID_ARGUMENTS);
		return false;
	}

	stringstream ss("");

	ss << event->GetPseudonym() << DEFAULT_FIELDS_DELIMITER;

	set<ull> timestamps = set<ull>();
	event->GetTimestamps(timestamps);

	foreach_const(set<ull>, timestamps, iter)
	{
		set<ull>::const_iterator next = iter;
		next++;

		ss << *iter;
		if(next != timestamps.end()) { ss << DEFAULT_FIELD_VALUES_DELIMITER; }
	}

	ss << DEFAULT_FIELDS_DELIMITER;

	set<ull> locationstamps = set<ull>();
	event->GetLocationstamps(locationstamps);

	foreach_const(set<ull>, locationstamps, iter)
	{
		set<ull>::const_iterator next = iter;
		next++;

		ss << *iter;
		if(next != locationstamps.end()) { ss << DEFAULT_FIELD_VALUES_DELIMITER; }
	}

	line = ss.str();

	return true;

  // Bouml preserved body end 0003B311
}


} // namespace lpm
