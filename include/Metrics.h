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
#ifndef LPM_METRICS_H
#define LPM_METRICS_H

//!
//! \file
//!
#include "MetricOperation.h"
#include <string>
using namespace std;
#include <map>
using namespace std;

#include "Defs.h"
#include "Private.h"

namespace lpm { class AttackOutput; } 
namespace lpm { class File; } 

namespace lpm {

//! 
//! \brief Base class for distortion based metric operations.
//! 
//! Evalutes the location-privacy of users at each timestamp using a distance function.
//!
class DistortionMetricOperation : public MetricOperation 
{
  public:
    DistortionMetricOperation(MetricType type = Distortion);

    ~DistortionMetricOperation();

    virtual bool Execute(const AttackOutput* input, File* output);


  protected:
    MetricDistance* distanceFunction;


  public:
    void SetDistanceFunction(MetricDistance* function);

    virtual string GetTypeString() const;

};
class EntropyMetricOperation : public MetricOperation 
{
  public:
    EntropyMetricOperation();

    virtual ~EntropyMetricOperation();

    virtual bool Execute(const AttackOutput* input, File* output);

    virtual string GetTypeString() const;

};
class MostLikelyLocationDistortionMetricOperation : public DistortionMetricOperation 
{
  public:
    MostLikelyLocationDistortionMetricOperation();

    virtual ~MostLikelyLocationDistortionMetricOperation();

    virtual bool Execute(const AttackOutput* input, File* output);

    virtual string GetTypeString() const;

};
class AnonymityMetricOperation : public MetricOperation 
{
  public:
    AnonymityMetricOperation();

    virtual ~AnonymityMetricOperation();

    virtual bool Execute(const AttackOutput* input, File* output);


  private:
    map<ull, ull> anonymizationMap;


  public:
    void SetAnonymizationMap(const map<ull,ull>& map);

    virtual string GetTypeString() const;

};
//!
//! \brief Default implementation of the distance function.
//!
//! The implementation defines the distance between two different locations to be 1, and 0 otherwise (if the locations are the same).
//!
class DefaultMetricDistance : public MetricDistance 
{
  public:
    virtual double ComputeDistance(ull firstLocation, ull secondLocation) const;

};
class MostLikelyTraceDistortionMetricOperation : public DistortionMetricOperation 
{
  public:
    MostLikelyTraceDistortionMetricOperation();

    ~MostLikelyTraceDistortionMetricOperation();

    virtual bool Execute(const AttackOutput* input, File* output);

    virtual string GetTypeString() const;

};
class DensityMetricOperation : public MetricOperation 
{
  public:
    DensityMetricOperation();

    ~DensityMetricOperation();

    virtual bool Execute(const AttackOutput* input, File* output);

    virtual string GetTypeString() const;

};
class MeetingDisclosureMetricOperation : public MetricOperation 
{
  public:
    MeetingDisclosureMetricOperation();

    ~MeetingDisclosureMetricOperation();

    virtual bool Execute(const AttackOutput* input, File* output);

    virtual string GetTypeString() const;

};

} // namespace lpm
#endif
