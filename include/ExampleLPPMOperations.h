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
#ifndef LPM_EXAMPLELPPMOPERATIONS_H
#define LPM_EXAMPLELPPMOPERATIONS_H

//!
//! \file
//!
//! \brief Provides some implementation of examples LPPM operations by implementing the LPPMOperation class
//!
#include "LPPMOperation.h"
#include <string>
using namespace std;

#include "Defs.h"
#include "Private.h"

namespace lpm { class Context; } 
namespace lpm { class ActualEvent; } 
namespace lpm { class ObservedEvent; } 
namespace lpm { class ExposedEvent; } 
namespace lpm { class Event; } 

namespace lpm {

//! 
//! \brief Default implementation (i.e. a simple example) of a LPPM operation.
//! 
//! This example LPPM operation performs anonymization (using a random time-independent permutation) and distorts events independently using a combination of obfuscation 
//! (controlled by the parameter \a obfLevel), fake injection (controlled by the parameters \a fakeInjectionAlgorithm and \a fakeInjectionProb), and, hiding
//! (controlled by the parameter \a hidingProb).
//!
class DefaultLPPMOperation : public LPPMOperation 
{
  public:
    DefaultLPPMOperation(ushort obfuscationLevel = 0, double fakeInjectionProbability = 0.0, const FakeInjectionAlgorithm fakeInjectionAlgorithm = UniformSelection, double hidingProbability = 0.0);

    virtual ~DefaultLPPMOperation();


  private:
    void ObfuscateLocation(ull location, set<ull>& obfucatedSet) const;

    void ComputeGeneralStatistics(ull tp, double** avg) const;


  public:
    virtual bool Filter(const Context* context, const ActualEvent* inEvent, ObservedEvent** outEvent);

    virtual double PDF(const Context* context, const ActualEvent* inEvent, const ObservedEvent* outEvent) const;


  private:
    ushort obfuscationLevel;

    double fakeInjectionProbability;

    FakeInjectionAlgorithm fakeInjectionAlgorithm;

    double hidingProbability;


  public:
    virtual string GetDetailString();

};

} // namespace lpm
#endif
