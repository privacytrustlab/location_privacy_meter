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
#include "../include/Trace.h"
#include "../include/Event.h"

namespace lpm {

Trace::Trace(ull u) 
{
  // Bouml preserved body begin 00023D11

	user = u;

  // Bouml preserved body end 00023D11
}

Trace::~Trace() 
{
  // Bouml preserved body begin 00023D91

	foreach_const(list<Event*>, events, iter)
	{
		(*iter)->Release();
	}

	events.clear();

  // Bouml preserved body end 00023D91
}

bool Trace::AddEvent(const Event* event) 
{
  // Bouml preserved body begin 00023C91

	if(event == NULL)
	{
		SET_ERROR_CODE(ERROR_CODE_INVALID_ARGUMENTS);
		return false;
	}

	// TODO: add check for duplicate events (depending on how this is done, it may be inefficient)

	events.push_back(const_cast<Event*>(event));

	const_cast<Event*>(event)->AddRef();

	return true;

  // Bouml preserved body end 00023C91
}

bool Trace::RemoveEvent(const Event* event) 
{
  // Bouml preserved body begin 00043F91

	if(event == NULL)
	{
		SET_ERROR_CODE(ERROR_CODE_INVALID_ARGUMENTS);
		return false;
	}

	events.remove(const_cast<Event*>(event));
	const_cast<Event*>(event)->Release();

	return true;

  // Bouml preserved body end 00043F91
}

ull Trace::GetUser() const 
{
  // Bouml preserved body begin 00035D91

	return user;

  // Bouml preserved body end 00035D91
}

void Trace::GetEvents(vector<Event*>& ret) const 
{
  // Bouml preserved body begin 00035F11

	ret.resize(events.size());
	copy(events.begin(), events.end(), ret.begin());

  // Bouml preserved body end 00035F11
}


} // namespace lpm
