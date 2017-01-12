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
#ifndef LPM_SCHEDULE_H
#define LPM_SCHEDULE_H

//!
//! \file
//!
#include <string>
using namespace std;
#include <vector>
using namespace std;
#include <map>
using namespace std;
#include "Reference.h"

#include "Defs.h"
#include "Private.h"

namespace lpm { class InputOperation; } 
namespace lpm { class File; } 
namespace lpm { class LoadContextOperation; } 
namespace lpm { class ApplicationOperation; } 
namespace lpm { class LPPMOperation; } 
namespace lpm { class AttackOperation; } 
namespace lpm { class MetricDistance; } 
namespace lpm { class OutputOperation; } 
namespace lpm { class FilterFunction; } 

namespace lpm {

//!
//! \brief Defines a positional scheme for the stages of a schedule.
//!
enum SchedulePosition 
{
  ScheduleBeginning = 0, 
  ScheduleBeforeInputs = ScheduleBeginning, 
  ScheduleBeforeApplicationOperation = ScheduleBeforeInputs + 1, 
  ScheduleBeforeLPPMOperation = ScheduleBeforeApplicationOperation + 1, 
  ScheduleBeforeAttackOperation = ScheduleBeforeLPPMOperation + 1, 
  ScheduleBeforeMetricOperation = ScheduleBeforeAttackOperation + 1, 
  ScheduleEnd = ScheduleBeforeMetricOperation + 1, 
  ScheduleInvalidPosition = -1 

};
//!
//! \brief Models a schedule
//!
//! \note The IsValid() method can be called to verify that the Schedule* object is valid (i.e. makes sense with respect to the library and the framework)
//!
class Schedule : public Reference<Schedule> 
{
  private:
    string name;

    void* builder;

    SchedulePosition startPosition;

    SchedulePosition endPosition;

    explicit Schedule(string name);


  public:
    virtual ~Schedule();


  private:
    bool root;

    InputOperation* inputOperation;

    File* actualTraceFile;

    InputOperation* actualTraceInputOperation;

    File* contextFile;

    LoadContextOperation* loadContextOperation;

    ApplicationOperation* applicationOperation;

    LPPMOperation* lppmOperation;

    AttackOperation* attackOperation;

    MetricType metricType;

    MetricDistance* distanceFunction;

    vector<Schedule*> branches;

    SchedulePosition branchingPosition;

    map<SchedulePosition, OutputOperation*> outputOperations;


  public:
    bool IsValid() const;

    string GetDetailString(ull level = 0) const;


  private:
    void ClearOutputOperations();

    bool AddOutputOperation(const OutputOperation* outputOperation, SchedulePosition position = ScheduleEnd);

    OutputOperation* GetOutputOperationAt(SchedulePosition pos) const;

    SchedulePosition afterInputPosition;

    FilterFunction* appPDF;

    FilterFunction* lppmPDF;


friend class LPM;
friend class ScheduleBuilder;
};

} // namespace lpm
#endif
