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
#include "../include/AttackOperation.h"
#include "../include/MetricOperation.h"
#include "../include/FilterOperation.h"
#include "../include/Context.h"

namespace lpm {

AttackOperation::AttackOperation(string name) : Operation<TraceSet, AttackOutput>(name) 
{
  // Bouml preserved body begin 00049491

	context = NULL;
	applicationPDF = NULL;
	lppmPDF = NULL;

  // Bouml preserved body end 00049491
}

AttackOperation::~AttackOperation() 
{
  // Bouml preserved body begin 00049511

	if(context != NULL) { context->Release(); }

  // Bouml preserved body end 00049511
}

//! 
//! \brief Sets the PDFs given to the attack
//!
//! \param[in] app 	FilterFunction*, an object encompassing the PDF of the application operation.
//! \param[in] lppm 	FilterFunction*, an object encompassing the PDF of the LPPM operation.
//!
//! \return true or false, depending on whether the call is successful
//!
bool AttackOperation::SetPDFs(const FilterFunction* app, const FilterFunction* lppm) 
{
  // Bouml preserved body begin 0004B091

	if(app == NULL || lppm == NULL) { return false; }

	applicationPDF = const_cast<FilterFunction*>(app);
	lppmPDF = const_cast<FilterFunction*>(lppm);

	return true;

  // Bouml preserved body end 0004B091
}

string AttackOperation::GetDetailString() 
{
  // Bouml preserved body begin 0004B291

	return "AttackOperation: " + operationName + "()";

  // Bouml preserved body end 0004B291
}

//! 
//! \brief Sets the context
//!
//! \param[in] newContext 	Context*, the new context
//!
//! \return nothing
//!
void AttackOperation::SetContext(const Context* newContext) 
{
  // Bouml preserved body begin 00052191

	context = const_cast<Context*>(newContext);
	context->AddRef();

  // Bouml preserved body end 00052191
}


} // namespace lpm
