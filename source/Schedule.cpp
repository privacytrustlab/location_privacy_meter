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
#include "../include/Schedule.h"
#include "../include/InputOperation.h"
#include "../include/File.h"
#include "../include/LoadContextOperation.h"
#include "../include/ApplicationOperation.h"
#include "../include/LPPMOperation.h"
#include "../include/AttackOperation.h"
#include "../include/MetricOperation.h"
#include "../include/OutputOperation.h"
#include "../include/FilterOperation.h"

namespace lpm {

Schedule::Schedule(string name) 
{
  // Bouml preserved body begin 00021911

	this->name = name;

	builder = NULL;

	startPosition = ScheduleBeginning;
	endPosition = ScheduleEnd;

	afterInputPosition = ScheduleInvalidPosition;
	appPDF = NULL;
	lppmPDF = NULL;

	root = true;

	inputOperation = NULL;
	actualTraceFile = NULL;
	actualTraceInputOperation = NULL;
	contextFile = NULL;
	loadContextOperation = NULL;
	applicationOperation = NULL;
	lppmOperation = NULL;
	attackOperation = NULL;

	metricType = Distortion;

	distanceFunction = NULL;

	branches = vector<Schedule*>();
	branchingPosition = ScheduleInvalidPosition;

	outputOperations = map<SchedulePosition, OutputOperation*>();

  // Bouml preserved body end 00021911
}

Schedule::~Schedule() 
{
  // Bouml preserved body begin 00021991

	if(actualTraceInputOperation != inputOperation) { actualTraceInputOperation->Release(); }
	if(inputOperation != NULL){ inputOperation->Release(); }
	if(loadContextOperation != NULL){ loadContextOperation->Release(); }
	if(applicationOperation != NULL){ applicationOperation->Release(); }
	if(lppmOperation != NULL){ lppmOperation->Release(); }
	if(attackOperation != NULL){ attackOperation->Release(); }

	if(distanceFunction != NULL) { delete distanceFunction; }

	if(branchingPosition != ScheduleInvalidPosition)
	{
		for(ull branch = 0; branch < branches.size(); branch++)
		{
			Schedule* sched = branches[branch];
			sched->Release();
		}
	}

	ClearOutputOperations();

  // Bouml preserved body end 00021991
}

bool Schedule::IsValid() const 
{
  // Bouml preserved body begin 00030B91

	if(startPosition > endPosition || endPosition == ScheduleInvalidPosition || startPosition == ScheduleInvalidPosition) { return false; }
	if(branchingPosition != ScheduleInvalidPosition && endPosition != branchingPosition) { return false; }
	if(startPosition == branchingPosition && startPosition != ScheduleBeginning) { return false; }

	// every root schedule must have a context file
	if(root == true && contextFile == NULL) { return false; }

	// every root schedule 'technically' starts at the beginning
	if(root == true && startPosition != ScheduleBeginning) { return false; }

	// every schedule which does not start at the beginning must have a trace file
	if(root == true && afterInputPosition != ScheduleInvalidPosition && actualTraceFile == NULL) { return false; }
	if(root == true && afterInputPosition != ScheduleInvalidPosition)
	{
		switch(afterInputPosition)
		{
		case ScheduleBeforeLPPMOperation:
			{
				if(appPDF == NULL || lppmPDF != NULL) { return false; }
			}
			break;
		case ScheduleBeforeAttackOperation:
			{
				if(appPDF == NULL || lppmPDF == NULL) { return false; }
			}
			break;
		default: return false;
		}
	}

	if(branchingPosition != ScheduleInvalidPosition)
	{
		if(branches.size() < 2) { return false; }

		for(ull branch = 0; branch < branches.size(); branch++)
		{
			if(branches[branch]->IsValid() == false) { return false; }
		}
	}

	switch(startPosition)
	{
		case ScheduleBeginning:
		{
			if(branchingPosition != ScheduleBeginning)
			{
				if(inputOperation == NULL || actualTraceInputOperation == NULL) { return false; }
				if(loadContextOperation == NULL) { return false; }
			}
		}
		break;
		case ScheduleBeforeApplicationOperation:
		{
			if(applicationOperation == NULL) { return false; }
		}
		break;
		case ScheduleBeforeLPPMOperation:
		{
			if(lppmOperation == NULL) { return false; }
		}
		break;
		case ScheduleBeforeAttackOperation:
		{
			if(attackOperation == NULL) { return false; }
		}
		break;
		case ScheduleBeforeMetricOperation:
		{
			// nothing to check here
		}
		break;
		case ScheduleEnd:
		default:
			return false;
		break;
	}

	if(endPosition == afterInputPosition) { return true; }

	switch(endPosition)
	{
		case ScheduleBeginning:
		{
			if(branchingPosition != endPosition) { return false; }
		}
		break;
		case ScheduleBeforeApplicationOperation:
		{
			if(branchingPosition != endPosition) { return false; }
		}
		break;
		case ScheduleBeforeLPPMOperation:
		{
			if(applicationOperation == NULL) { return false; }
		}
		break;
		case ScheduleBeforeAttackOperation:
		{
			if(lppmOperation == NULL) { return false; }
			if(afterInputPosition == ScheduleInvalidPosition && startPosition < ScheduleBeforeLPPMOperation)
			{
				if(applicationOperation == NULL) { return false; }
			}
		}
		break;
		case ScheduleBeforeMetricOperation:
		{
			if(attackOperation == NULL) { return false; }
			if(afterInputPosition == ScheduleInvalidPosition && startPosition < ScheduleBeforeAttackOperation)
			{
				if(lppmOperation == NULL) { return false; }
			}
			if(afterInputPosition == ScheduleInvalidPosition && startPosition < ScheduleBeforeLPPMOperation)
			{
				if(applicationOperation == NULL) { return false; }
			}
		}
		break;
		case ScheduleEnd:
		{
			// nothing to check
		}
		break;
		default:
			return false;
		break;
	}

	return true;

  // Bouml preserved body end 00030B91
}

string Schedule::GetDetailString(ull level) const 
{
  // Bouml preserved body begin 0002B391

	string indent = "";
	for(ull i = 0; i < level; i++)
	{
		indent += "\t";
	}

	string ret = indent + "Schedule: " + name + "\n";

	if(inputOperation != NULL) { ret += indent + inputOperation->GetDetailString() + "\n"; }

	map<SchedulePosition, OutputOperation*>::const_iterator iter = outputOperations.find(ScheduleBeforeApplicationOperation);
	if(iter != outputOperations.end()) { ret += indent + iter->second->GetDetailString() + "\n"; }
	if(applicationOperation != NULL) { ret += indent + applicationOperation->GetDetailString() + "\n"; }

	iter = outputOperations.find(ScheduleBeforeLPPMOperation);
	if(iter != outputOperations.end()) { ret += indent + iter->second->GetDetailString() + "\n"; }
	if(lppmOperation != NULL) { ret += indent + lppmOperation->GetDetailString() + "\n"; }

	iter = outputOperations.find(ScheduleBeforeAttackOperation);
	if(iter != outputOperations.end()) { ret += indent + iter->second->GetDetailString() + "\n"; }
	if(attackOperation != NULL) { ret += indent + attackOperation->GetDetailString() + "\n"; }

	if(endPosition == ScheduleEnd)
	{
		string metricString = "";
		switch(metricType)
		{
		case Distortion:
			metricString = STRINGIFY(Distortion);
			break;
		case MostLikelyLocationDistortion:
			metricString = STRINGIFY(MostLikelyLocationDistortion);
			break;
		case MostLikelyTraceDistortion:
			metricString = STRINGIFY(MostLikelyTraceDistortion);
			break;
		case Entropy:
			metricString = STRINGIFY(Entropy);
			break;
		case Density:
			metricString = STRINGIFY(Density);
			break;
		case MeetingDisclosure:
			metricString = STRINGIFY(MeetingDisclosure);
			break;
		case Anonymity:
			metricString = STRINGIFY(Anonymity);
			break;
		default:
			metricString = "Unknown metric";
			break;
		}

		ret += indent + "Metric: " + metricString + "\n";
	}

	if(branchingPosition != ScheduleInvalidPosition)
	{
		for(ull branch = 0; branch < branches.size(); branch++)
		{
			ret += branches[branch]->GetDetailString(level + 1);
		}
	}

	return ret;

  // Bouml preserved body end 0002B391
}

void Schedule::ClearOutputOperations() 
{
  // Bouml preserved body begin 0002B611

	pair_foreach_const(map<SchedulePosition, OutputOperation*>, outputOperations, iter)
	{
		if(iter->second != NULL) { iter->second->Release(); }
	}

	outputOperations.clear();

  // Bouml preserved body end 0002B611
}

bool Schedule::AddOutputOperation(const OutputOperation* outputOperation, SchedulePosition position) 
{
  // Bouml preserved body begin 0002B691

	if(outputOperation == NULL)
	{
		SET_ERROR_CODE(ERROR_CODE_INVALID_OPERATION);
		return false;
	}

	if(position == ScheduleEnd || outputOperations.find(position) != outputOperations.end())
	{
		SET_ERROR_CODE(ERROR_CODE_INVALID_POSITION);
		return false;
	}

	OutputOperation* outputOp = const_cast<OutputOperation*>(outputOperation);
	outputOp->AddRef();
	pair<SchedulePosition, OutputOperation*> newOutputOperation(position, outputOp);
	outputOperations.insert(newOutputOperation);

	return true;

  // Bouml preserved body end 0002B691
}

OutputOperation* Schedule::GetOutputOperationAt(SchedulePosition pos) const 
{
  // Bouml preserved body begin 00068A11

	map<SchedulePosition, OutputOperation*>::const_iterator iter = outputOperations.find(pos);

	if(iter != outputOperations.end()) { return iter->second; }

	return NULL;

  // Bouml preserved body end 00068A11
}


} // namespace lpm
