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
#ifndef LPM_FILTEROPERATION_H
#define LPM_FILTEROPERATION_H

//!
//! \file
//!
#include "Operations.h"
#include <string>
using namespace std;

#include "Defs.h"
#include "Trace.h"
#include "TraceSet.h"

namespace lpm { class Context; } 
namespace lpm { class Event; } 

namespace lpm {

//!
//! \brief Represents the filter function, i.e. the pdf of a filter opreration
//!
class FilterFunction 
{
  public:
    virtual double PDF(const Context* context, const Event* inEvent, const Event* outEvent) const = 0;

};
//!
//! \brief Represents a filter operation
//!
//! A special kind of operation which takes a TraceSet as input and produces a TraceSet as output.
//! Filter operations treat every event in the input TraceSet independently. 
//! For this purpose, it defines a pure virtual Filter() method.
//!
//

class FilterOperation : public Operation<TraceSet, TraceSet>, public FilterFunction 
{
  public:
    FilterOperation(string name);

    virtual ~FilterOperation();

    //! 
    //! \brief Executes the filter operation
    //!
    //! \param[in] input 	TraceSet* whose events are to be filtered.
    //! \param[in,out] output 	TraceSet* the filtered output object.
    //!
    //! \return true or false, depending on whether the call is successful
    //!
    virtual bool Execute(const TraceSet* input, TraceSet* output);

    //! 
    //! \brief Sets the context (background knowledge of the adversary)
    //!
    //! \param[in] newContext 	Context*, the new context
    //!
    //! \return nothing
    //!
    void SetContext(Context* newContext);

    //! 
    //! \brief Filters the input event
    //!
    //! \param[in] context 	Context*, the context.
    //! \param[in] inEvent 	Event*, the event to filter.
    //! \param[in] outEvent	Event*, the filtered output event.
    //!
    //! \return true or false, depending on whether the call is successful
    //!
    virtual bool Filter(const Context* context, const Event* inEvent, Event** outEvent) = 0;

    //! 
    //! \brief The probability density function (pdf) of the filter operation
    //!
    //! Computes and returns the probability that \a outEvent is the filtered output event conditional on the input event \a inEvent and the \a context.
    //!
    //! \param[in] context 	Context*, the context.
    //! \param[in] inEvent 	Event*, the event to filter.
    //! \param[in] outEvent	Event*, the filtered output event.
    //!
    //! \return the value of the pdf as a double
    //!
    virtual double PDF(const Context* context, const Event* inEvent, const Event* outEvent) const = 0;

    virtual string GetDetailString() = 0;


  protected:
    Context* context;

};

} // namespace lpm
#endif
