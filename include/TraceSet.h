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
#ifndef LPM_TRACESET_H
#define LPM_TRACESET_H

//!
//! \file
//!
#include <map>
using namespace std;
#include "Reference.h"

#include "Defs.h"
#include "Event.h"
#include "ActualEvent.h"
#include "ExposedEvent.h"
#include "ObservedEvent.h"

namespace lpm { class Trace; } 
namespace lpm { class Event; } 

namespace lpm {

//!
//! \brief Represents a set of traces
//!
//! \see Trace
//!

class TraceSet : public Reference<TraceSet> 
{
  private:
    TraceType type;


  public:
    explicit TraceSet(TraceType traceType);

    virtual ~TraceSet();


  private:
    map<ull, Trace*> mapping;


  public:
    //! 
    //! \brief Adds the specified event to the trace set
    //!
    //! \param[in] event 	Event*, the event to add.
    //!
    //! \return true or false, depending on whether the call is successful
    //!
    bool AddEvent(const Event* event);

    TraceType GetTraceType() const;

    //! 
    //! \brief Retrieves the mapping of usersID to their traces
    //!
    //! Returns the mapping (user -> trace) that is contained in the TraceSet.
    //!
    //! \param[in,out] ret map<ull, Trace*>, a map which will contain the mapping after the call returns.
    //!
    //! \return nothing
    //!
    void GetMapping(map<ull, Trace*>& ret) const;

    //! 
    //! \brief Returns whether the trace set is empty (i.e. contains no events)
    //!
    //! \return true, if the trace set is empty, false, otherwise
    //!
    bool IsEmpty() const;

};

} // namespace lpm
#endif
