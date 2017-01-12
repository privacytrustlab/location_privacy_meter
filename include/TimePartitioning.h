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
#ifndef LPM_TIMEPARTITIONING_H
#define LPM_TIMEPARTITIONING_H

//!
//! \file
//!
#include <vector>
using namespace std;
#include <string>
using namespace std;

#include "Defs.h"
#define INVALID_TIME_PERIOD 0
#define TIME_PARTITIONING_NODE_DELIMITER '\t'

#define IS_ROOT_NODE 	((this == parent) ? true : false)

#include "Memory.h"

namespace lpm { class File; } 

namespace lpm {

struct TimePeriod 
{
    ull start;

    ull length;

    ull id;

    bool dummy;

    bool operator<(const TimePeriod& right) const;

};
class TPNode 
{
  protected:
    TPNode* parent;

    ull offset;

    ull length;


  private:
    vector<TPNode*> slices;

    vector<TPNode*> groups;

    bool partitioned;

    TPNode(ull off, ull len);


  public:
    static TPNode* FromFile(File* file);


  private:
    static TPNode* FromStrings(vector<string>& strings);


  public:
    virtual ~TPNode();

    virtual bool SliceOut(ull offset, ull length, ull repetitions, TPNode** slice);

    virtual bool Partition(vector<TimePeriod>& periods);

    virtual ull LookupTimePeriod(ull tm, TimePeriod* absTP = NULL, TPNode** leaf = NULL);

    virtual bool GetStringRepresentation(string& str, ull level = 0);

    ull GetOffset(bool abs) const;

    ull GetLength() const;


  private:
    virtual ull FillWithDummyTimePeriods(ull nextTPIdx);


  public:
    static bool IsLessTan(const TPNode* left, const TPNode* right);


friend class Parameters;
friend class TPGroup;
friend class TPLeaf;
};
class TPLeaf : public TPNode 
{
  private:
    TimePeriod* periods;

    ull numPeriods;

    TPLeaf(ull len);


  public:
    virtual ~TPLeaf();

    virtual bool SliceOut(ull offset, ull length, ull repetitions, vector<TPNode*>& slice);

    virtual bool Partition(vector<TimePeriod>& periods);

    virtual ull LookupTimePeriod(ull tm, TimePeriod* absTP = NULL, TPNode** leaf = NULL);

    virtual bool GetStringRepresentation(string& str, ull level = 0);


  private:
    virtual ull FillWithDummyTimePeriods(ull nextTPIdx);


friend class TPNode;
friend class Parameters;
};
class TPGroup : public TPNode 
{
  private:
    vector<TPNode*> group;

    TPGroup(ull off, ull len, vector<TPNode*>& members);


  public:
    virtual ~TPGroup();

    virtual bool SliceOut(ull offset, ull length, ull repetitions, TPNode** slice);

    virtual bool Partition(vector<TimePeriod>& periods);


  private:
    virtual ull FillWithDummyTimePeriods(ull nextTPIdx);

    virtual ull LookupTimePeriod(ull tm, TimePeriod* absTP = NULL, TPNode** leaf = NULL);

    virtual bool GetStringRepresentation(string& str, ull level = 0);

    ull GetOffset(bool abs) const;

    ull GetLength() const;


friend class TPNode;
};

} // namespace lpm
#endif
