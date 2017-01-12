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
#include "../include/FilterOperation.h"
#include "../include/Context.h"
#include "../include/Event.h"

namespace lpm {

FilterOperation::FilterOperation(string name) : Operation<TraceSet, TraceSet>(name)
{
  // Bouml preserved body begin 00021B91

	context = NULL;

  // Bouml preserved body end 00021B91
}

FilterOperation::~FilterOperation() 
{
  // Bouml preserved body begin 00021C11

	if(context != NULL) { context->Release(); }

  // Bouml preserved body end 00021C11
}

//! 
//! \brief Executes the filter operation
//!
//! \param[in] input 	TraceSet* whose events are to be filtered.
//! \param[in,out] output 	TraceSet* the filtered output object.
//!
//! \return true or false, depending on whether the call is successful
//!
bool FilterOperation::Execute(const TraceSet* input, TraceSet* output) 
{
  // Bouml preserved body begin 00021F91

	TraceSet* inputTraceSet = const_cast<TraceSet*>(input);
	TraceSet* outputTraceSet = const_cast<TraceSet*>(output);

	map<ull, Trace*> mapping = map<ull, Trace*>();
	inputTraceSet->GetMapping(mapping);

	pair_foreach_const(map<ull, Trace*>, mapping, mapIter)
	{
		Trace* trace = mapIter->second;
		vector<Event*> events = vector<Event*>();
		trace->GetEvents(events);

		foreach_const(vector<Event*>, events, iter)
		{
			Event* inEvent = *iter;
			Event* outEvent = NULL;

			if(Filter(context, inEvent, &outEvent) == true && outEvent != NULL)
			{
				if(outputTraceSet->AddEvent(outEvent) == false)
				{
					outEvent->Release();
					return false;
				}
			}
			VERIFY(outEvent != NULL);

			if(outEvent != NULL) { outEvent->Release(); }
		}
	}

	return true;

  // Bouml preserved body end 00021F91
}

//! 
//! \brief Sets the context (background knowledge of the adversary)
//!
//! \param[in] newContext 	Context*, the new context
//!
//! \return nothing
//!
void FilterOperation::SetContext(Context* newContext) 
{
  // Bouml preserved body begin 00047991

	context = newContext;
	context->AddRef();

  // Bouml preserved body end 00047991
}


} // namespace lpm
