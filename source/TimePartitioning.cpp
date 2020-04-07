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
#include "../include/TimePartitioning.h"
#include "../include/File.h"

namespace lpm {

bool TimePeriod::operator<(const TimePeriod& right) const 
{
  // Bouml preserved body begin 000A5A91

	if(this->start < right.start) { return true; }
	else if(this->start == right.start) { return this->length < right.length; }

	return false;

  // Bouml preserved body end 000A5A91
}

TPNode::TPNode(ull off, ull len)
{
  // Bouml preserved body begin 000A3F91

	slices = vector<TPNode*>();
	groups = vector<TPNode*>(); // guaranteed to be a vector<TPGroup*>!!
	offset = off;
	length = len;
	partitioned = false;
	parent = NULL;

  // Bouml preserved body end 000A3F91
}

TPNode* TPNode::FromFile(File* file)
{
  // Bouml preserved body begin 000AC611

	vector<string> strs = vector<string>();

	string line = "notempty";

	VERIFY(file->IsGood());
	while(file->IsEOF() == false && line.empty() == false)
	{
		VERIFY(file->IsGood());
		VERIFY(file->ReadNextLine(line) == true);
		if(line.empty() == false) { strs.push_back(line); }
	}

	return FromStrings(strs);

  // Bouml preserved body end 000AC611
}

TPNode* TPNode::FromStrings(vector<string>& strings)
{
  // Bouml preserved body begin 000AC591

	string str = strings.at(0);
	strings.erase(strings.begin());

	ull level = 0;
	while(str.at(0) == '\t') { level++; str = str.substr(1); }

	ull nextLevel = 0;
	if(strings.empty() == false)
	{
		string next = strings.at(0);
		while(next.at(0) == '\t') { nextLevel++; next = next.substr(1); }
	}

	// ugly parsing but whatever
	ull offset = 0; ull length = 0;

	size_t pos = str.find(DEFAULT_FIELDS_DELIMITER);
	if(pos == string::npos) { return NULL; }
	{
		stringstream ss(str.substr(0, pos)); 	ss >> offset;
		if(ss.fail() == true) { return NULL; }
	}

	str = str.substr(pos+1);
	pos = str.find(DEFAULT_FIELDS_DELIMITER);
	{
		stringstream ss(str.substr(0, pos)); ss >> length;
		if(ss.fail() == true) { return NULL; }
	}

	stringstream ss("");
	if(pos != string::npos)
	{
		if(nextLevel < level) // we are parsing a leaf
		{
			ull numPeriods = 0; ull id = 0; ull sumLength = 0; ull dummy = 0;
			vector<TimePeriod> periods = vector<TimePeriod>();

			str = str.substr(pos+1);
			pos = str.find(DEFAULT_FIELDS_DELIMITER);
			if(pos == string::npos) { numPeriods = 1; }

			ss.str(str.substr(0, pos));
			ss >> id;

			str = str.substr(pos+1);
			pos = str.find(DEFAULT_FIELD_VALUES_DELIMITER);

			{
				stringstream ss("");
				ss.str(str.substr(0, pos));
				ss >> dummy;
				if(ss.fail() == true) { return NULL; }
			}

			TimePeriod tp; tp.dummy = (dummy == 1 ? true : false); tp.start = offset; tp.length = length; tp.id = id; sumLength += length;
			periods.push_back(tp);

			if(pos != string::npos)
			{
				while(pos != string::npos)
				{
					numPeriods++;
					str = str.substr(pos+1);
					pos = str.find(DEFAULT_FIELDS_DELIMITER);

					if(pos == string::npos) { return NULL; }

					{
						stringstream ss("");
						ss.str(str.substr(0, pos));
						ss >> offset;
						if(ss.fail() == true) { return NULL; }
					}

					str = str.substr(pos+1);
					pos = str.find(DEFAULT_FIELDS_DELIMITER);
					if(pos == string::npos) { return NULL; }


					{
						stringstream ss("");
						ss.str(str.substr(0, pos));
						ss >> length;
						if(ss.fail() == true) { return NULL; }
					}

					str = str.substr(pos+1);
					pos = str.find(DEFAULT_FIELDS_DELIMITER);

					{
						stringstream ss("");
						ss.str(str.substr(0, pos));
						ss >> id;
						if(ss.fail() == true) { return NULL; }
					}

					str = str.substr(pos+1);
					pos = str.find(DEFAULT_FIELD_VALUES_DELIMITER);

					{
						stringstream ss("");
						ss.str(str.substr(0, pos));
						ss >> dummy;
						if(ss.fail() == true) { return NULL; }
					}

					TimePeriod tp; tp.dummy = (dummy == 1 ? true : false); tp.start = offset; tp.length = length; tp.id = id; sumLength += length;
					periods.push_back(tp);
				}
			}


			TPLeaf* leaf = new TPLeaf(sumLength);
			leaf->Partition(periods);

			return leaf;
		}
		else { return NULL; } // error
	}
	else
	{
		TPNode* node = new TPNode(offset, length);
		node->partitioned = false;
		node->slices = vector<TPNode*>();
		if(level == 0) { node->parent = node; } // this is the root

		while(strings.empty() == false && nextLevel > level)
		{
			TPNode* child = FromStrings(strings);
			VERIFY(child != NULL);
			child->parent = node;

			if(dynamic_cast<TPLeaf*>(child) != NULL) { node->partitioned = true; } // if the child is a leaf, then the parent is partitioned

			node->slices.push_back(child);

			nextLevel = 0;
			if(strings.empty() == false)
			{
				string next = strings.at(0);
				while(next.at(0) == '\t') { nextLevel++; next = next.substr(1); }
			}
		}

		return node;
	}

  // Bouml preserved body end 000AC591
}

TPNode::~TPNode() 
{
  // Bouml preserved body begin 000A4011

	// delete all children
	foreach_const(vector<TPNode*>, slices, iter)
	{
		TPNode* node = *iter;
		VERIFY(node != NULL);
		delete node;
	}
	slices.clear();

	foreach_const(vector<TPNode*>, groups, iter)
	{
		TPGroup* group = dynamic_cast<TPGroup*>(*iter);
		VERIFY(group != NULL);
		delete group;
	}
	groups.clear();

  // Bouml preserved body end 000A4011
}

bool TPNode::SliceOut(ull offset, ull length, ull repetitions, TPNode** slice) 
{
  // Bouml preserved body begin 000A2211

	if(partitioned == true || slice == NULL) { return false; } // can't slice out a node that's already been partitioned
	if(repetitions == 0 || length == 0) { return false; }

	if(offset + repetitions * length > this->length)  { return false;  }  // can't splice out if out of bounds

	// check that the new slices (if any) will not intersect the others
	foreach_const(vector<TPNode*>, slices, iter)
	{
		TPNode* node = *iter;
		if(node->offset >= offset && node->offset < offset + repetitions * length) { return false; } // intersection
	}

	// now populate the new slice children's
	vector<TPNode*> groupVec = vector<TPNode*>();
	for(ull i=0; i < repetitions; i++)
	{
		TPNode* newNode = new TPNode(offset + i * length, length);
		newNode->parent = this; // set the parent

		slices.push_back(newNode);
		groupVec.push_back(newNode);
	}
	stable_sort(slices.begin(), slices.end(), TPNode::IsLessTan); // FIXFIX need to sort by explicitly providing the function
	stable_sort(groupVec.begin(), groupVec.end(), TPNode::IsLessTan);

	TPGroup* group = new TPGroup(offset, repetitions * length, groupVec);
	*slice = group;

	groups.push_back(group); // insert this group in our vector
	stable_sort(groups.begin(), groups.end(), TPNode::IsLessTan);

	return true;

  // Bouml preserved body end 000A2211
}

bool TPNode::Partition(vector<TimePeriod>& periods) 
{
  // Bouml preserved body begin 000A2291

	if(partitioned == true || slices.empty() == false) { return false; } // this node has already been spliced -> it cannot be partitioned !

	TPLeaf* partition = new TPLeaf(length);
	partition->parent = this; // set the parent

	bool success = partition->Partition(periods);

	if(success == false) { Free(partition); return false; }

	slices.push_back(partition);

	partitioned = true;

	return true;

  // Bouml preserved body end 000A2291
}

ull TPNode::LookupTimePeriod(ull tm, TimePeriod* absTP, TPNode** leaf) 
{
  // Bouml preserved body begin 000A2491

	ll diff = ((ll)tm - (ll)offset);
	if(diff < 0) { return INVALID_TIME_PERIOD; }

	if(IS_ROOT_NODE == true) // simulate the period time partitioning of the root
	{
		if((tm - offset) >= length)
		{
			tm = (tm - offset) % length + offset;
		}
	}

	foreach_const(vector<TPNode*>, slices, iter)
	{
		TPNode* node = *iter;
		if((tm - offset) >= node->offset && (tm - offset) < node->offset + node->length)
		{
			ull id = node->LookupTimePeriod(tm - offset, absTP, leaf); // recursive lookup

			// If needed, do (part of) the computation for absoluteTP->start
			if(id != INVALID_TIME_PERIOD && absTP != NULL) { absTP->start += offset; }

			return id;
		}
	}

	return INVALID_TIME_PERIOD;

  // Bouml preserved body end 000A2491
}

bool TPNode::GetStringRepresentation(string& str, ull level) 
{
  // Bouml preserved body begin 000AAA91

	if(slices.empty() == true) { return false; }

	stringstream ss("");
	for(ull i=0; i < level; i++) { ss << TIME_PARTITIONING_NODE_DELIMITER; }

	ss << offset << DEFAULT_FIELDS_DELIMITER << length << endl;

	stringstream ss2("");
	foreach_const(vector<TPNode*>, slices, iter)
	{
		TPNode* node = *iter;

		string nstr;
		if(node->GetStringRepresentation(nstr, level + 1) == false) { return false; }
		ss2 << nstr;
	}

	if(ss2.str().empty() == false)	{ str = ss.str() + ss2.str(); } // only output something if at least one of the children did.
	else { str = ""; }
	return true;

  // Bouml preserved body end 000AAA91
}

ull TPNode::GetOffset(bool abs) const 
{
  // Bouml preserved body begin 000B2C11

	if(IS_ROOT_NODE == true || abs == false) { return offset; } // root offset is always absolute
	VERIFY(parent != NULL);

	return offset + parent->GetOffset(true); // absolute

  // Bouml preserved body end 000B2C11
}

ull TPNode::GetLength() const 
{
  // Bouml preserved body begin 000B2C91

	return length;

  // Bouml preserved body end 000B2C91
}

ull TPNode::FillWithDummyTimePeriods(ull nextTPIdx) 
{
  // Bouml preserved body begin 000B4A11

	ull initialNextTPIdx = nextTPIdx;

	if(partitioned == true) { return 0; } // if we're partitioned its all good, no need to fill with dummies

	// VERIFY(groups.empty() == false && slices.empty() == false); // groups can be empty if we read partitioning from file
	VERIFY(slices.empty() == false);

	if(groups.empty()) { return 0; } // don't do anything


	ull currentOffset = 0; // start at 0, since offsets are relative, remember?

	// first look before the first group, then look at each group, finally look after the last group
	foreach_const(vector<TPNode*>, groups, iter)
	{
		TPGroup* group = dynamic_cast<TPGroup*>(*iter);
		VERIFY(group != NULL);

		// before the group
		ull bgOffset = group->offset; VERIFY(bgOffset >= currentOffset);
		ull newPartitionLength = bgOffset - currentOffset;

		if(newPartitionLength != 0) // need to create a dummy
		{
			TPNode* newNode = new TPNode(currentOffset, newPartitionLength);
			newNode->parent = this;

			vector<TimePeriod> vec = vector<TimePeriod>();

			TimePeriod dummy;
			dummy.dummy = true;
			dummy.id = nextTPIdx;
			dummy.start = 0;
			dummy.length = newPartitionLength;

			vec.push_back(dummy);
			VERIFY(newNode->Partition(vec) == true);

			slices.push_back(newNode);

			nextTPIdx++; // increment the counter

			currentOffset += newPartitionLength;
		}

		// inside the group
		ull newTPs = group->FillWithDummyTimePeriods(nextTPIdx);

		nextTPIdx += newTPs;
		currentOffset += group->length;
	}

	// now look at the last group (i.e. the interval between currentOffset and this->length)
	VERIFY(length >= currentOffset);
	ull newPartitionLength = length - currentOffset;

	if(newPartitionLength != 0) // need to create a dummy
	{
		TPNode* newNode = new TPNode(currentOffset, newPartitionLength);
		newNode->parent = this;

		vector<TimePeriod> vec = vector<TimePeriod>();

		TimePeriod dummy;
		dummy.dummy = true;
		dummy.id = nextTPIdx;
		dummy.start = 0;
		dummy.length = newPartitionLength;

		vec.push_back(dummy);
		VERIFY(newNode->Partition(vec) == true);

		slices.push_back(newNode);

		nextTPIdx++; // increment the counter

		currentOffset += newPartitionLength;
	}
	stable_sort(slices.begin(), slices.end(), TPNode::IsLessTan);

	return nextTPIdx - initialNextTPIdx;

  // Bouml preserved body end 000B4A11
}

bool TPNode::IsLessTan(const TPNode* left, const TPNode* right)

{
  // Bouml preserved body begin 000B6491

	if(left->offset < right->offset) { return true; }
	else if(left->offset > right->offset) { return false; }

	return (left->length < right->length);

  // Bouml preserved body end 000B6491
}

TPLeaf::TPLeaf(ull len) : TPNode(0, len) 
{
  // Bouml preserved body begin 000A4091

	periods = NULL;
	length = len;
	numPeriods = 0;

  // Bouml preserved body end 000A4091
}

TPLeaf::~TPLeaf() 
{
  // Bouml preserved body begin 000A4111

	if(periods != NULL) { Free(periods); }
	periods = NULL;
	numPeriods = 0;
	length = 0;

  // Bouml preserved body end 000A4111
}

bool TPLeaf::SliceOut(ull offset, ull length, ull repetitions, vector<TPNode*>& slice) 
{
  // Bouml preserved body begin 000A2311

	CODING_ERROR;
	return false; // can't splice out a leaf!

  // Bouml preserved body end 000A2311
}

bool TPLeaf::Partition(vector<TimePeriod>& periods) 
{
  // Bouml preserved body begin 000A2391

	if(periods.empty() == true) { return false; }

	sort(periods.begin(), periods.end()); // use operator< in TimePeriod

	// check the partition
	ull lastPeriodEnd = 0;
	foreach_const(vector<TimePeriod>, periods, iter)
	{
		TimePeriod tp = *iter;
		if(tp.id == INVALID_TIME_PERIOD) { return false; } // invalid time period id
		if(tp.start != lastPeriodEnd || tp.start + tp.length > length) { return false; } // invalid offsets

		lastPeriodEnd = tp.start + tp.length;
	}

	if(lastPeriodEnd != length) { return false; } // not a complete partition


	// checking done -> copy the partition
	numPeriods = periods.size();
	this->periods = (TimePeriod*)Allocate(numPeriods * sizeof(TimePeriod));

	ull i = 0;
	foreach_const(vector<TimePeriod>, periods, iter)
	{
		TimePeriod tp = *iter;
		this->periods[i++] = tp;
	}

	return true;

  // Bouml preserved body end 000A2391
}

ull TPLeaf::LookupTimePeriod(ull tm, TimePeriod* absTP, TPNode** leaf) 
{
  // Bouml preserved body begin 000A2511

	if(periods == NULL || numPeriods == 0) { return INVALID_TIME_PERIOD; }

	for(ull i=0; i < numPeriods; i++)
	{
		TimePeriod* tp = &periods[i];

		if(tm >= tp->start && tm < tp->start + tp->length)
		{
			if(absTP != NULL)
			{
				absTP->id = tp->id;
				absTP->start += tp->start; // finish the computation (which takes place during the lookup)
				absTP->length = tp->length;
				absTP->dummy = tp->dummy;
			}

			if(leaf != NULL) { *leaf = this; }

			return tp->id;
		}
	}

	return INVALID_TIME_PERIOD;

  // Bouml preserved body end 000A2511
}

bool TPLeaf::GetStringRepresentation(string& str, ull level) 
{
  // Bouml preserved body begin 000AAB11

	if(periods == NULL || numPeriods == 0) { return false; }

	string levelStr = "";
	for(ull i=0; i < level; i++) { levelStr += TIME_PARTITIONING_NODE_DELIMITER; }

	stringstream ss("");
	for(ull i=0; i < numPeriods; i++)
	{
		TimePeriod tp = periods[i];

		ss << tp.start << DEFAULT_FIELDS_DELIMITER;
		ss << tp.length << DEFAULT_FIELDS_DELIMITER;
		ss << tp.id << DEFAULT_FIELDS_DELIMITER;
		ss << (tp.dummy == true ? '1' : '0');

		if(i != numPeriods-1) { ss << DEFAULT_FIELD_VALUES_DELIMITER; }
	}

	if(ss.str().empty()) { str = ""; }
	else
	{
		ss << endl;
		str = levelStr + ss.str();
	}

	return true;

  // Bouml preserved body end 000AAB11
}

ull TPLeaf::FillWithDummyTimePeriods(ull nextTPIdx) 
{
  // Bouml preserved body begin 000B4A91

	CODING_ERROR;
	return false; // can't fill a leaf with dummy TPs

  // Bouml preserved body end 000B4A91
}

TPGroup::TPGroup(ull off, ull len, vector<TPNode*>& members) : TPNode(off, len)
{
  // Bouml preserved body begin 000B4611

	group = members;

  // Bouml preserved body end 000B4611
}

TPGroup::~TPGroup() 
{
  // Bouml preserved body begin 000B4691

	group.clear(); // no need to free the TPNodes, it will be done in the parent

  // Bouml preserved body end 000B4691
}

bool TPGroup::SliceOut(ull offset, ull length, ull repetitions, TPNode** slice) 
{
  // Bouml preserved body begin 000B4711

	vector<TPNode*> groupVec = vector<TPNode*>();
	vector<TPNode*> tmpVec = vector<TPNode*>();
	foreach_const(vector<TPNode*>, group, iter)
	{
		TPNode* node = *iter;
		TPNode* thisSlice = NULL;
		if(node->SliceOut(offset, length, repetitions, &thisSlice) == false)
		{
			foreach_const(vector<TPNode*>, groupVec, iter2) { delete *iter2; }
			foreach_const(vector<TPNode*>, tmpVec, iter2) { delete *iter2; }
			return false;
		}

		tmpVec.push_back(thisSlice);
		TPGroup* thisGroup = dynamic_cast<TPGroup*>(thisSlice);
		groupVec.insert(groupVec.end(), thisGroup->group.begin(), thisGroup->group.end());
	}

	TPGroup* ret = new TPGroup(offset, repetitions * length, groupVec);
	*slice = ret;

	return true;

  // Bouml preserved body end 000B4711
}

bool TPGroup::Partition(vector<TimePeriod>& periods) 
{
  // Bouml preserved body begin 000B4891

	foreach_const(vector<TPNode*>, group, iter)
	{
		TPNode* node = *iter;
		if(node->Partition(periods) == false){ return false; }
	}
	return true;

  // Bouml preserved body end 000B4891
}

ull TPGroup::FillWithDummyTimePeriods(ull nextTPIdx) 
{
  // Bouml preserved body begin 000B4B11

	ull prevNumNewPeriods = 0;
	foreach_const(vector<TPNode*>, group, iter)
	{
		TPNode* node = *iter;
		ull numNewPeriods = node->FillWithDummyTimePeriods(nextTPIdx);

		VERIFY(prevNumNewPeriods == 0 || prevNumNewPeriods == numNewPeriods);
		prevNumNewPeriods = numNewPeriods;
	}
	return prevNumNewPeriods;

  // Bouml preserved body end 000B4B11
}

ull TPGroup::LookupTimePeriod(ull tm, TimePeriod* absTP, TPNode** leaf) 
{
  // Bouml preserved body begin 000B4791

	CODING_ERROR;
	return false; // invalid op. for TPGroups

  // Bouml preserved body end 000B4791
}

bool TPGroup::GetStringRepresentation(string& str, ull level) 
{
  // Bouml preserved body begin 000B4811

	CODING_ERROR;
	return false; // invalid op for TPGroups

  // Bouml preserved body end 000B4811
}

ull TPGroup::GetOffset(bool abs) const 
{
  // Bouml preserved body begin 000B4911

	CODING_ERROR;
	return false; // invalid op. for TPGroups

  // Bouml preserved body end 000B4911
}

ull TPGroup::GetLength() const 
{
  // Bouml preserved body begin 000B4991

	CODING_ERROR;
	return false; // invalid op. for TPGroups

  // Bouml preserved body end 000B4991
}


} // namespace lpm
