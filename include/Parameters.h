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
#ifndef LPM_PARAMETERS_H
#define LPM_PARAMETERS_H

//!
//! \file
//!
#include <map>
using namespace std;
#include "Singleton.h"
#include <vector>
using namespace std;

#include "Defs.h"

#ifndef WIN32
#include "TimePartitioning.h"
#endif

#define PARAMETERS_DEFAULT_MAX_USERS (1 << 6) // 64
#define PARAMETERS_DEFAULT_MAX_TIMESTAMPS (1 << 4) // 16
#define PARAMETERS_DEFAULT_MAX_LOCATIONSTAMPS (1 << 4) // 16

namespace lpm { class TPNode; } 
namespace lpm { struct TimePeriod; } 

namespace lpm {

struct TPInfo 
{
    ull numPeriods;

    ull minPeriod;

    ull maxPeriod;

    ull numPeriodsInclDummies;

    TPNode* partitioning;

    TPNode* canonicalPartitionParentNode;

    map<ull, TPNode*> partParentNodeMap;

    double* propTPVector;

    double* propTransMatrix;

};
//!
//! \brief Encompasses the parameters of the simulation
//!
//! Singleton class which stores the parameters of the simulation (i.e. min and max user IDs, timestamps, locationstamps).
//!
class Parameters : public Singleton<Parameters> 
{
  private:
    ull minTimestamp;

    ull maxTimestamp;

    ull minLocationstamp;

    ull maxLocationstamp;

    vector<pair<ull, ull> > usersRanges;

    TPInfo tpInfo;


  public:
    Parameters();

    ~Parameters();

    bool GetUsersSet(set<ull>& users);

    ull GetUsersCount();

    bool UserExists(ull user);

    bool AddUsersRange(ull min, ull max);

    bool RemoveUsersRange(ull min, ull max);

    void ClearUsersSet();

    bool GetTimestampsRange(ull* min, ull* max) const;

    bool SetTimestampsRange(ull min, ull max);

    bool GetLocationstampsRange(ull* min, ull* max) const;

    bool SetLocationstampsRange(ull min, ull max);

    TPNode* CreateTimePartitioning(ull offset, ull length);

    bool SetTimePartitioning(TPNode* partitioning);

    ull LookupTimePeriod(ull tm, bool inclDummies = false, TimePeriod* absTP = NULL, TPNode** partParentNode = NULL);

    bool GetTimePeriodInfo(ull* numPeriods, TPInfo* tpInfo = NULL);


  private:
    bool InitializeTPInfo(TPNode* partitioning);

};

} // namespace lpm
#endif
