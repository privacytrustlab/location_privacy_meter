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
#include "../include/ActualEvent.h"

namespace lpm {

ActualEvent::ActualEvent(ull u, ull t, ull l) 
{
  // Bouml preserved body begin 00034411

	user = u;
	timestamp = t;
	locationstamp = l;

  // Bouml preserved body end 00034411
}

ActualEvent::~ActualEvent() 
{
  // Bouml preserved body begin 00037A91
  // Bouml preserved body end 00037A91
}

ActualEvent::ActualEvent(const ActualEvent& source) 
{
  // Bouml preserved body begin 0003B511

	user = source.user;
	timestamp = source.timestamp;
	locationstamp = source.locationstamp;

  // Bouml preserved body end 0003B511
}

EventType ActualEvent::GetType() const 
{
  // Bouml preserved body begin 00039511

	return Actual;

  // Bouml preserved body end 00039511
}

ull ActualEvent::GetUser() const 
{
  // Bouml preserved body begin 00034211

	return user;

  // Bouml preserved body end 00034211
}

ull ActualEvent::GetTimestamp() const 
{
  // Bouml preserved body begin 00034291

	return timestamp;

  // Bouml preserved body end 00034291
}

ull ActualEvent::GetLocationstamp() const 
{
  // Bouml preserved body begin 00034311

	return locationstamp;

  // Bouml preserved body end 00034311
}

//! 
//! \brief Returns whether the event is exposed (i.e. whether it is an instance of ActualEvent or ExposedEvent)
//!
//! \return true, if the event is exposed, false, otherwise
//!
bool ActualEvent::IsExposed() const 
{
  // Bouml preserved body begin 00034391

	return (GetType() == Exposed);

  // Bouml preserved body end 00034391
}


} // namespace lpm
