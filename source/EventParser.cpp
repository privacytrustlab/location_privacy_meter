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
#include "../include/EventParser.h"
#include "../include/ActualEvent.h"
#include "../include/ObservedEvent.h"

namespace lpm {

bool EventParser::ParseActualEvent(string line, ActualEvent** event) const 
{
  // Bouml preserved body begin 00034191

	VERIFY(event != NULL);

	bool exposed = false;
	size_t pos = 0;
	vector<ull> values = vector<ull>();

	// try parse 4 (exposed event)
	bool parse4 = LineParser<ull>::GetInstance()->ParseFields(line, values, 4, &pos);

	if(parse4 == false)
	{
		values.clear();
		if(LineParser<ull>::GetInstance()->ParseFields(line, values, 3, &pos) == false || pos != string::npos)
		{
			SET_ERROR_CODE(ERROR_CODE_INVALID_FORMAT);
			return false;
		}
	}
	else if(pos != string::npos || (values[3] != 0 && values[3] != 1)) // parse4 but more than 4 values
	{
		SET_ERROR_CODE(ERROR_CODE_INVALID_FORMAT);
		return false;
	}

	ull user = values[0];
	ull timestamp = values[1];
	ull locationstamp = values[2];

	if(parse4 == true){ exposed = (values[3] == 0) ? false : true; }

	*event = (exposed == true) ? new ExposedEvent(user, timestamp, locationstamp) : new ActualEvent(user, timestamp, locationstamp);

	return true;

  // Bouml preserved body end 00034191
}

bool EventParser::ParseObservedEvent(string line, ObservedEvent** event) 
{
  // Bouml preserved body begin 0003B211

	VERIFY(event != NULL && line.empty() == false);

	ull nym = 0; size_t pos = 0;
	if(LineParser<ull>::GetInstance()->ParseValue(line, &nym, &pos) == false || pos == string::npos) { return false; }

	size_t lastPos = line.rfind(DEFAULT_FIELDS_DELIMITER);
	size_t firstPos = line.rfind(DEFAULT_FIELDS_DELIMITER, lastPos - 1);

	pos = line.find(DEFAULT_FIELDS_DELIMITER);

	if(line.find(DEFAULT_FIELDS_DELIMITER) != firstPos || firstPos == string::npos || lastPos == string::npos || pos == string::npos)
	{
		SET_ERROR_CODE(ERROR_CODE_INVALID_FORMAT);
		return false;
	}

	string timestampsString = line.substr(pos + 1, lastPos);
	string locationsString = line.substr(lastPos + 1);

	vector<ull> timestamps = vector<ull>();
	if(LineParser<ull>::GetInstance()->ParseFields(timestampsString, timestamps, ANY_NUMBER_OF_FIELDS, NULL, DEFAULT_FIELD_VALUES_DELIMITER) == false) { return false; }

	vector<ull> locations = vector<ull>();
	if(locationsString.empty() == false && LineParser<ull>::GetInstance()->ParseFields(locationsString, locations, ANY_NUMBER_OF_FIELDS, NULL, DEFAULT_FIELD_VALUES_DELIMITER) == false) { return false; }

	ObservedEvent* observedEvent = new ObservedEvent(nym);

	foreach_const(vector<ull>, timestamps, iter)
	{
		if(observedEvent->AddTimestamp(*iter) == false)
		{
			SET_ERROR_CODE(ERROR_CODE_INVALID_FORMAT);
			observedEvent->Release();

			return false;
		}
	}

	foreach_const(vector<ull>, locations, iter)
	{
		if(observedEvent->AddLocationstamp(*iter) == false)
		{
			SET_ERROR_CODE(ERROR_CODE_INVALID_FORMAT);
			observedEvent->Release();

			return false;
		}
	}

	*event = observedEvent;

	return true;

  // Bouml preserved body end 0003B211
}


} // namespace lpm
