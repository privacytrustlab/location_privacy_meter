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
#ifndef LPM_METRICOPERATION_H
#define LPM_METRICOPERATION_H

//!
//! \file
//!
#include "Operations.h"
#include "AttackOutput.h"
#include "File.h"
#include <string>
using namespace std;

#include "Defs.h"
#include "Private.h"

namespace lpm { class TraceSet; } 

namespace lpm {

//! 
//! \brief Base class for all metric operations.
//! 
//! Metric operations evaluate the performance of the attack.
//!
class MetricOperation : public Operation<AttackOutput, File> 
{
  private:
    MetricType type;


  public:
    explicit MetricOperation(MetricType t);

    virtual ~MetricOperation();

    MetricType GetType() const;

    //! 
    //! \brief Executes the metric operation
    //!
    //! \param[in] input 	AttackOutput*, the result of the attack operation.
    //! \param[in,out] output 	File*, the output file.
    //!
    //! \return true or false, depending on whether the call is successful
    //!
    virtual bool Execute(const AttackOutput* input, File* output) = 0;


  protected:
    TraceSet* actualTraceSet;


  public:
    bool SetActualTrace(const TraceSet* traceSet);

    virtual string GetDetailString();

    virtual string GetTypeString() const = 0;

};
//! 
//! \brief Base class for all metric distance functions (used by distortion-based metrics).
//! 
//! Metric distances implement a single function which computes the distance between two locations.
//!
class MetricDistance 
{
  public:
    //! 
    //! \brief Computes the distance between two locations
    //!
    //! \param[in] firstLocation 	ull, the first location.
    //! \param[in] secondLocation 	ull, the second location.
    //!
    //! \return double, representing the distance between \a firstLocation and \a secondLocation.
    //!
    virtual double ComputeDistance(ull firstLocation, ull secondLocation) const = 0;

};

} // namespace lpm
#endif
