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
#include "../include/Parameters.h"

namespace lpm {

Parameters::Parameters() 
{
  // Bouml preserved body begin 0002F211

	minTimestamp = 1;
	minLocationstamp = 1;

	maxTimestamp = PARAMETERS_DEFAULT_MAX_TIMESTAMPS;
	maxLocationstamp = PARAMETERS_DEFAULT_MAX_LOCATIONSTAMPS;

	usersRanges = vector<pair<ull, ull> >();

  // Bouml preserved body end 0002F211
}

Parameters::~Parameters() 
{
  // Bouml preserved body begin 000B1291

	if(tpInfo.propTPVector != NULL) { Free(tpInfo.propTPVector); }
	if(tpInfo.propTransMatrix != NULL) { Free(tpInfo.propTransMatrix); }
	if(tpInfo.partitioning != NULL) { delete tpInfo.partitioning; }

  // Bouml preserved body end 000B1291
}

bool Parameters::GetUsersSet(set<ull>& users) 
{
  // Bouml preserved body begin 000A0691

	users.clear();

	pair_foreach_const(vector<pair<ull, ull> >, usersRanges, rangesIter)
	{
		ull min = rangesIter->first;
		ull max = rangesIter->second;

		for(ull i = min; i <= max; i++) { users.insert(i); }
	}

	return true;
  // Bouml preserved body end 000A0691
}

ull Parameters::GetUsersCount() 
{
  // Bouml preserved body begin 000A0711

	ull count = 0;

	pair_foreach_const(vector<pair<ull, ull> >, usersRanges, rangesIter)
	{
		ull min = rangesIter->first;
		ull max = rangesIter->second;
		DEBUG_VERIFY(max >= min && min >= 1);

		count += max - min + 1;
	}

	return count;

  // Bouml preserved body end 000A0711
}

bool Parameters::UserExists(ull user) 
{
  // Bouml preserved body begin 0009EC11

	pair_foreach_const(vector<pair<ull, ull> >, usersRanges, rangesIter)
	{
		ull min = rangesIter->first;
		ull max = rangesIter->second;

		if(user >= min && user <= max) { return true; }
	}

	return false;

  // Bouml preserved body end 0009EC11
}

bool Parameters::AddUsersRange(ull min, ull max) 
{
  // Bouml preserved body begin 0002EF91

	if(min < 1 || max < min) { return false; }

	pair_foreach(vector<pair<ull, ull> >, usersRanges, rangesIter)
	{
		ull rangeMin = rangesIter->first;
		ull rangeMax = rangesIter->second;

		if(min == rangeMin && max == rangeMax) { return true; } // no need to do anything

		if(!((rangeMax + 1 < min) || (rangeMin - 1 > max))) // if ranges are not disjoint ("merge" the two)
		{
			ull newMin = MIN(rangeMin, min);
			ull newMax = MAX(rangeMax, max);

			usersRanges.erase(rangesIter); // remove the existing range

			return AddUsersRange(newMin, newMax);
		}
	}

	// if we could not find a range to merge (add the new range to the vector)
	usersRanges.insert(usersRanges.end(), pair<ull, ull>(min, max));

	return true;

  // Bouml preserved body end 0002EF91
}

bool Parameters::RemoveUsersRange(ull min, ull max) 
{
  // Bouml preserved body begin 0009EC91

	if(min < 1 || max < min) { return false; }

	pair_foreach(vector<pair<ull, ull> >, usersRanges, rangesIter)
	{
		ull rangeMin = rangesIter->first;
		ull rangeMax = rangesIter->second;

		if(min <= rangeMin && max >= rangeMax)
		{
			usersRanges.erase(rangesIter); // remove the whole range (it is included in the other one)
			return RemoveUsersRange(min, max); // call ourselves recursively
		}

		if(!((rangeMax + 1 < min) || (rangeMin - 1 > max))) // if ranges are not disjoint ("shrink" the existing range)
		{
			if(rangeMin < min && rangeMax > max) // need to split the range in two:
			{
				usersRanges.erase(rangesIter); // remove this range
				ull lowerMin = rangeMin;
				ull lowerMax = min - 1; // ranges are inclusive

				usersRanges.insert(usersRanges.end(), pair<ull, ull>(lowerMin, lowerMax));

				ull upperMin = max + 1; // ranges are inclusive
				ull upperMax = rangeMax;

				usersRanges.insert(usersRanges.end(), pair<ull, ull>(upperMin, upperMax));

				return RemoveUsersRange(min, max); // call ourselves recursively
			}

			if(rangeMin < min) // need to decrease upper bound
			{
				rangesIter->second = min - 1;
			}
			else // need to increase lower bound
			{
				rangesIter->first = max + 1;
			}
			// note: we can't return or break here, because we may have to shrink other ranges (=> need to iterate on all ranges)
		}
	}

	return true;

  // Bouml preserved body end 0009EC91
}

void Parameters::ClearUsersSet() 
{
  // Bouml preserved body begin 0009ED11

	usersRanges.clear();

  // Bouml preserved body end 0009ED11
}

bool Parameters::GetTimestampsRange(ull* min, ull* max) const 
{
  // Bouml preserved body begin 0002F011

	if(min == NULL || max == NULL) { return false; }

	*min = minTimestamp;
	*max = maxTimestamp;

	return true;

  // Bouml preserved body end 0002F011
}

bool Parameters::SetTimestampsRange(ull min, ull max) 
{
  // Bouml preserved body begin 0002F091

	if(min < 1 || max <= min) { return false; }

	minTimestamp = min;
	maxTimestamp = max;

	return true;

  // Bouml preserved body end 0002F091
}

bool Parameters::GetLocationstampsRange(ull* min, ull* max) const 
{
  // Bouml preserved body begin 0002F111

	if(min == NULL || max == NULL) { return false; }

	*min = minLocationstamp;
	*max = maxLocationstamp;

	return true;

  // Bouml preserved body end 0002F111
}

bool Parameters::SetLocationstampsRange(ull min, ull max) 
{
  // Bouml preserved body begin 0002F191

	if(min < 1 || max <= min) { return false; }

	minLocationstamp = min;
	maxLocationstamp = max;

	return true;

  // Bouml preserved body end 0002F191
}

TPNode* Parameters::CreateTimePartitioning(ull offset, ull length) 
{
  // Bouml preserved body begin 000A7411

	if(offset == 0 || length == 0) { return NULL; }

	TPNode* root = new TPNode(offset, length);
	root->parent = root; // this is the root (its parent is itself)

	return root;

  // Bouml preserved body end 000A7411
}

bool Parameters::SetTimePartitioning(TPNode* partitioning) 
{
  // Bouml preserved body begin 000A7491

	if(partitioning == NULL) { return false; }

	InitializeTPInfo(partitioning);

	return true;

  // Bouml preserved body end 000A7491
}

ull Parameters::LookupTimePeriod(ull tm, bool inclDummies, TimePeriod* absTP, TPNode** partParentNode) 
{
  // Bouml preserved body begin 000AA991

	if(tpInfo.partitioning == NULL)
	{
		if((tm >= minTimestamp && tm <= maxTimestamp))
		{
			if(absTP != NULL) { absTP->id = tpInfo.minPeriod; absTP->start = minTimestamp; absTP->length = maxTimestamp - minTimestamp + 1; }
			if(partParentNode != NULL) { *partParentNode = tpInfo.partitioning; }
			return tpInfo.minPeriod;
		}

		return INVALID_TIME_PERIOD;
	}

	TPNode* leaf = NULL;
	ull tp = tpInfo.partitioning->LookupTimePeriod(tm, absTP, &leaf);

	VERIFY(tp == INVALID_TIME_PERIOD || tp <= tpInfo.numPeriodsInclDummies);
	if(tp > tpInfo.maxPeriod && inclDummies == false) { tp = INVALID_TIME_PERIOD; }

	if(partParentNode != NULL && leaf != NULL)
	{
		// algorithmically this is simple, we go up the tree starting from the leaf until all time periods (including the dummies)
		// are somewhere in the sub-tree. To make it efficient we do sort of a partial depth first search exploration of tree by going up from a leaf.
		// for each node we encounter, we compute the set of time period ids contained in the subtree
		set<TPNode*> visitedNodes = set<TPNode*>();

		// construct the candidate set
		set<TPNode*> partParentCandidates = set<TPNode*>();
		{
			TPNode* node = leaf;

			do // while not at the root
			{
				partParentCandidates.insert(node);
				node = node->parent;
			}
			while(node != node->parent);
		}

		vector<TPNode*> stack = vector<TPNode*>();
		set<ull>* tpsInSubTree = NULL;

		map<TPNode*, set<ull> > mapTPsInSubTree = map<TPNode*, set<ull> >(); // map of the set of tp ids in the subtree rooted at each node
		stack.push_back(leaf->parent);
		visitedNodes.insert(leaf);
		visitedNodes.insert(leaf->parent);

		TPNode* node = NULL;
		while(stack.empty() == false)
		{
			node = stack.back();
			stack.pop_back();

			TPNode* parent = node->parent;

			if(visitedNodes.find(parent) == visitedNodes.end()) { stack.push_back(parent); visitedNodes.insert(parent); } // add the parent

			// include siblings of node
			foreach_const(vector<TPNode*>, parent->slices, iter)
			{
				TPNode* child = *iter;
				if(child == node) { continue; }

				if(visitedNodes.find(child) == visitedNodes.end())
				{
					stack.push_back(child); // add the siblings
					visitedNodes.insert(child);

					foreach_const(vector<TPNode*>, child->slices, iter)
					{
						TPNode* child2 = *iter;
						if(visitedNodes.find(child2) == visitedNodes.end()) { stack.push_back(child2); visitedNodes.insert(child2); }
					}
				}
			}


			map<TPNode*, set<ull> >::iterator mapIter = mapTPsInSubTree.find(node);
			if(mapIter == mapTPsInSubTree.end())
			{
				mapTPsInSubTree.insert(pair<TPNode*, set<ull> >(node, set<ull>()));
				mapIter = mapTPsInSubTree.find(node);
			}

			tpsInSubTree = &(mapIter->second);

			if(node->partitioned == true) // this is a leaf -> add the partition
			{
				TPLeaf* leaf = dynamic_cast<TPLeaf*>(node->slices[0]);
				if(leaf == NULL) { CODING_ERROR;  return INVALID_TIME_PERIOD; }

				for(ull i=0; i < leaf->numPeriods; i++)
				{
					TimePeriod tp = leaf->periods[i];
					ull id = tp.id;
					tpsInSubTree->insert(id);
				}
			}
			else // this is a node -> compute the union of the children's sets
			{
				foreach_const(vector<TPNode*>, node->slices, iter)
				{
					TPNode* child = *iter;

					map<TPNode*, set<ull> >::iterator mapIter2 = mapTPsInSubTree.find(child);
					VERIFY(mapIter2 != mapTPsInSubTree.end()); // otherwise, we have a problem, since we always visit the tree going up

					set<ull> childSet = mapIter2->second;
					foreach_const(set<ull>, childSet, iterChild) { tpsInSubTree->insert(*iterChild); } // add the child's contribution
				}
			}

			VERIFY(tpsInSubTree->size() <= tpInfo.numPeriodsInclDummies);

			if(tpsInSubTree->size() == tpInfo.numPeriodsInclDummies	&& partParentCandidates.find(node) != partParentCandidates.end())
			{ break; } // we are done, the node we're looking for is 'node'.
		}

		VERIFY(tpsInSubTree->size() == tpInfo.numPeriodsInclDummies && node != NULL);
		*partParentNode = node;
	}

	return tp;

  // Bouml preserved body end 000AA991
}

bool Parameters::GetTimePeriodInfo(ull* numPeriods, TPInfo* tpInfo) 
{
  // Bouml preserved body begin 000AAA11

	if(numPeriods == NULL && tpInfo == NULL) { return false; }

	if(tpInfo != NULL) { *tpInfo = this->tpInfo; }
	if(numPeriods != NULL) { *numPeriods = this->tpInfo.numPeriods; }

	return true;

  // Bouml preserved body end 000AAA11
}

bool Parameters::InitializeTPInfo(TPNode* partitioning) 
{
  // Bouml preserved body begin 000B7E11

	set<ull> periodIds = set<ull>();
	set<ull> dummyPeriodIds = set<ull>();

	vector<ull> periodSizes = vector<ull>();
	periodSizes.push_back(0); // periodSizes[0] = 0;

	// check that the partitioning is valid
	{
		vector<TPNode*> queue = vector<TPNode*>();
		queue.push_back(partitioning);
		while(queue.empty() == false)
		{
			TPNode* node = queue.at(0);
			queue.erase(queue.begin());

			if(node->partitioned == true)
			{
				TPLeaf* leaf = dynamic_cast<TPLeaf*>(node->slices[0]);
				if(leaf == NULL) { CODING_ERROR;  return false; }

				for(ull i=0; i < leaf->numPeriods; i++)
				{
					TimePeriod tp = leaf->periods[i];
					ull id = tp.id;
					if(tp.dummy == false) { periodIds.insert(id); }
					else { dummyPeriodIds.insert(id); }

					while(periodSizes.size() < id + 1) { periodSizes.push_back(0); } // extend the vector...
					periodSizes.at(id) += tp.length;
				}
			}
			else
			{
				foreach_const(vector<TPNode*>, node->slices, iter)
				{
					TPNode* child = *iter;
					queue.push_back(child);
				}
				if(node->slices.empty() == true) // invalid
				{
					SET_ERROR_CODE(ERROR_CODE_INVALID_TIME_PARTITIONING);
					return false;
				}
			}
		}
	}

	ull minPeriod = 1;
	ull nextPeriodId = minPeriod;
	foreach_const(set<ull>, periodIds, iter)
	{
		ull id = *iter;
		if(nextPeriodId != id)
		{
			SET_ERROR_CODE(ERROR_CODE_INVALID_TIME_PARTITIONING);
			return false;
		}
		nextPeriodId++;
	}

	ull numDummies = 0; ull tmp = nextPeriodId;
	foreach_const(set<ull>, dummyPeriodIds, iter)
	{
		ull id = *iter;
		if(tmp != id)
		{
			SET_ERROR_CODE(ERROR_CODE_INVALID_TIME_PARTITIONING);
			return false;
		}
		tmp++;
		numDummies++;
	}


	if(tpInfo.partitioning != NULL) { delete tpInfo.partitioning; }
	tpInfo.partitioning = partitioning;


	// compute the number of time periods
	ull numPeriods = tpInfo.numPeriods = periodIds.size();
	// VERIFY(periodIds.size() == periodSizes.size() - 1);

	tpInfo.minPeriod = minPeriod;
	tpInfo.maxPeriod = numPeriods + tpInfo.minPeriod - 1;

	// let's add the dummy periods
	ull nextTPIdx = tpInfo.maxPeriod + 1;
	numDummies += partitioning->FillWithDummyTimePeriods(nextTPIdx + numDummies);

	tpInfo.numPeriodsInclDummies = numPeriods + numDummies;

	// retrieve the canonical partition parent node
	VERIFY(LookupTimePeriod(partitioning->offset, true, NULL, &tpInfo.canonicalPartitionParentNode) != INVALID_TIME_PERIOD);

	// compute the fraction of time within each time period
	if(tpInfo.propTPVector != NULL) { Free(tpInfo.propTPVector); }
	double* propTPVector = (double*)Allocate(numPeriods * sizeof(double));

	double sum = 0;
	for(ull idx=0; idx < numPeriods; idx++)
	{
		ull size = periodSizes.at(minPeriod + idx);
		propTPVector[idx] = size;
		sum += size;
	}

	for(ull idx=0; idx < numPeriods; idx++) { propTPVector[idx] /= sum; }

	tpInfo.propTPVector = propTPVector;


	// compute the proportion of transitions (between time periods) matrix
	if(tpInfo.propTransMatrix != NULL) { Free(tpInfo.propTransMatrix); }
	ull numPeriodsInclDummies = tpInfo.numPeriodsInclDummies;
	double* propTransMatrix = (double*)Allocate(numPeriodsInclDummies * numPeriodsInclDummies * sizeof(double));

	// initialization
	for(ull tp1Idx = 0; tp1Idx < numPeriodsInclDummies; tp1Idx++) { for(ull tp2Idx = 0; tp2Idx < numPeriodsInclDummies; tp2Idx++) { propTransMatrix[GET_INDEX(tp1Idx, tp2Idx, numPeriodsInclDummies)] = 0.0; } }

	ull starttm = tpInfo.partitioning->offset;
	ull endtm = tpInfo.partitioning->offset + tpInfo.partitioning->length - 1;
	ull previd = INVALID_TIME_PERIOD;
	for(ull tm = starttm; tm <= endtm + 1;) // note: 1. no tm++		2. end at tm = endtm+1 (so that we wrap around)
	{
		TimePeriod tp; memset(&tp, 0, sizeof(tp));
		ull id = tpInfo.partitioning->LookupTimePeriod(tm, &tp);

		VERIFY(id != INVALID_TIME_PERIOD); // should not happen since we have added dummy tps

		// first count the transition from previd to id
		if(previd != INVALID_TIME_PERIOD)
		{
			ull index = GET_INDEX((previd - minPeriod), (id - minPeriod), numPeriodsInclDummies);
			propTransMatrix[index]++;
		}

		VERIFY(tm > endtm || tm == tp.start); // this should always be the case because of the way we iterate (and the semantics of time periods), remember tp.start is absolute
		tm += tp.length;

		// now account for tp.length - 1 new transitions from id to id
		ull index = GET_INDEX((id - minPeriod), (id - minPeriod), numPeriodsInclDummies);
		propTransMatrix[index] += tp.length - 1;

		previd = id;
	}

	// normalize
	for(ull tp1Idx = 0; tp1Idx < numPeriodsInclDummies; tp1Idx++)
	{
		double sum = 0.0;
		for(ull tp2Idx = 0; tp2Idx < numPeriodsInclDummies; tp2Idx++)
		{
			ull index = GET_INDEX(tp1Idx, tp2Idx, numPeriodsInclDummies);
			ull count = propTransMatrix[index];
			sum += count;

			if(tp1Idx == tp2Idx && count == 0) // problem: resulting markov chain will not be aperiodic
			{
				SET_ERROR_CODE(ERROR_CODE_INVALID_TIME_PARTITIONING);
				return false;
			}
		}

		VERIFY(sum != 0.0);// If sum == 0, the partitioning can't possibly be valid (there are no transitions from tp1Idx to any other time period)

		for(ull tp2Idx = 0; tp2Idx < numPeriodsInclDummies; tp2Idx++)
		{
			ull index = GET_INDEX(tp1Idx, tp2Idx, numPeriodsInclDummies);
			propTransMatrix[index] /= sum;
		}
	}

	tpInfo.propTransMatrix = propTransMatrix;

	return true;

  // Bouml preserved body end 000B7E11
}


} // namespace lpm
