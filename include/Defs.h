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
#ifndef LPM_DEFS_H
#define LPM_DEFS_H

//!
//! \file
//!
// language includes
#include <cassert>
#include <climits>
#include <stdint.h>

#include <cstring>
#include <memory>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <cfloat>

#include <iostream>
#include <fstream>
#include <sstream>

#include <set>
#include <vector>
#include <map>
#include <list>
#include <algorithm>
#include <cerrno>

using namespace std;

#define ASSERT assert

#ifdef DEBUG
	#define VERIFY ASSERT
#else
	#define VERIFY RELEASE_VERIFY
#endif

#define RELEASE_VERIFY RUNTIME_VERIFY
#define DEBUG_VERIFY ASSERT

#define CODING_ERROR ASSERT(0 == 1)

#ifdef FORCE_RELEASE
	#ifdef DEBUG
		#define Allocate(_s) Memory::GetInstance()->AllocateChunk((_s), __FILE__, __LINE__)
		#define Free Memory::GetInstance()->FreeChunk
	#else
		#define Allocate malloc
		#define Free free
	#endif
#else
	#define Allocate(_s) Memory::GetInstance()->AllocateChunk((_s), __FILE__, __LINE__)
	#define Free Memory::GetInstance()->FreeChunk
#endif

#define foreach(T, _c, i) \
	for(T::iterator i = (_c).begin(); i != (_c).end(); ++i)

#define foreach_const(T, _c, i) \
	for(T::const_iterator i = (_c).begin(); i != (_c).end(); ++i)

#define foreach_const_reverse(T, _c, i) \
	for(T::const_reverse_iterator i = (_c).rbegin(); i != (_c).rend(); ++i)

#define pair_foreach(T1, T2, _c, i) \
	for(T1, T2::iterator i = (_c).begin(); i != (_c).end(); ++i)

#define pair_foreach_const(T1, T2, _c, i) \
	for(T1, T2::const_iterator i = (_c).begin(); i != (_c).end(); ++i)

#define GET_INDEX(_i, _j, _numCol) ((_i) * (_numCol) + (_j))

#define GET_INDEX_3D(_i, _j, _k, _numRow, _numCol) (((_i) * (_numRow) * (_numCol)) + (_j) * (_numCol) + (_k))

#define GET_INDEX_4D(_i, _j, _k, _l, _numPage, _numRow, _numCol) (((_i) * (_numPage) * (_numRow) * (_numCol)) + (_j) * (_numRow) * (_numCol) + (_k) * (_numCol) + (_l))

#define DEFAULT_FIELDS_DELIMITER ','
#define DEFAULT_FIELD_VALUES_DELIMITER '|'

#define STRINGIFY(_sth) #_sth

#define ABS(_a) (((_a) >= 0) ? (_a) : -(_a))
#define MIN(_a, _b) (((_a) < (_b)) ? (_a) : (_b))
#define MAX(_a, _b) (((_a) < (_b)) ? (_b) : (_a))

#define EPSILON 0.0001

#define SQRT_DBL_MIN 1.4916681462400413e-154

#define NORMALIZE_VECTOR(_pvec, _n) { double sum = 0.0; for(ull normIdx=0; normIdx < (_n); normIdx++) { sum += (_pvec)[normIdx]; } for(ull normIdx=0; normIdx < (_n); normIdx++) { (_pvec)[normIdx] /= sum; } }

namespace lpm {

typedef int32_t int32;
typedef uint32_t uint32;
typedef int64_t int64, ll;
typedef uint64_t uint64, ull;
typedef unsigned long ulong;
typedef unsigned int uint;
typedef unsigned short ushort;

//! 
//! \brief Defines trace types (actual, exposed, observed) 
//! 
//! \see EventType
//! 
enum TraceType 
{
  ActualTrace = 0, 
  ExposedTrace = ActualTrace + 1, 
  ObservedTrace = ExposedTrace + 1 

};
//!
//! \brief Defines events types (actual, exposed, observed)
//! 
//! \see TraceType
//!
enum EventType 
{
  Actual = ActualTrace, 
  Exposed = ExposedTrace, 
  Observed = ObservedTrace 

};
//!
//! \brief Defines metric types (distortion, anonymity, entropy, etc.)
//!

enum MetricType 
{
  Distortion = 0, 
  MostLikelyLocationDistortion = Distortion + 1, 
  MostLikelyTraceDistortion = MostLikelyLocationDistortion + 1, 
  Entropy = MostLikelyTraceDistortion + 1, 
  Density = Entropy + 1, 
  MeetingDisclosure = Density + 1, 
  Anonymity = MeetingDisclosure + 1 

};
//!
//! \brief Algorithm used for fake injection (either \a UniformSelection, or \a GeneralStatisticsSelection)
//!

enum FakeInjectionAlgorithm 
{
  UniformSelection = 0, 
  GeneralStatisticsSelection = UniformSelection + 1 

};
//!
//! \brief Defines application types
//!

enum ApplicationType 
{
  Basic = 0, 
  LocalSearch = Basic + 1 

};
//!
//! \brief Defines adversary's power (weak, strong)
//!

enum AttackType 
{
  Weak = 0, 
  Strong = Weak + 1 

};

} // namespace lpm
#endif
