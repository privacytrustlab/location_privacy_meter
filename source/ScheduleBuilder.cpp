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
#include "../include/ScheduleBuilder.h"
#include "../include/Schedule.h"
#include "../include/OutputOperation.h"
#include "../include/File.h"
#include "../include/FilterOperation.h"
#include "../include/InputOperation.h"
#include "../include/LoadContextOperation.h"
#include "../include/ApplicationOperation.h"
#include "../include/LPPMOperation.h"
#include "../include/AttackOperation.h"
#include "../include/MetricOperation.h"

namespace lpm {

//! 
//! \brief Constructs a schedule builder object
//!
//! \param[in] name 	[optional] string, the name of the schedule that will be built by the schedule builder.
//!
//! \note Naming a schedule builder names the constructed schedules.
//! \note Even though naming is optional (the default name is "unnamed schedule") it is good practice to do so.
//!
ScheduleBuilder::ScheduleBuilder(string name) 
{
  // Bouml preserved body begin 00064911

	schedule = NULL;
	NewSchedule(name);

  // Bouml preserved body end 00064911
}

ScheduleBuilder::~ScheduleBuilder() 
{
  // Bouml preserved body begin 00064991

	schedule = NULL;

  // Bouml preserved body end 00064991
}

//! 
//! \brief Creates a new schedule associated with the builder
//!
//! \param[in] name 	[optional] string, the name of the schedule that will be built by the schedule builder.
//!
//! \note NewSchedule() is called automatically by the ScheduleBuilder constructor. 
//! Therefore, it should only be called when creating several schedules using the same builder.
//!
//! \note Even though naming is optional (the default name is "unnamed schedule") it is good practice to do so.
//!
void ScheduleBuilder::NewSchedule(string name) 
{
  // Bouml preserved body begin 00064B91

	if(schedule != NULL)
	{ schedule->Release(); }

	inputDone = false;
	wasForked = false;

	schedule = new Schedule(name);
	schedule->builder = static_cast<void*>(this);
	schedule->startPosition = ScheduleBeginning;
	schedule->branchingPosition = ScheduleInvalidPosition;
	schedule->endPosition = ScheduleInvalidPosition;

	currentPosition = ScheduleBeginning;
	lastOutputPosition = ScheduleInvalidPosition;

  // Bouml preserved body end 00064B91
}

//! 
//! \brief Returns a pointer to the schedule built
//!
//! Allows to retrieve the Schedule object built so far.
//!
//! \note This effectively severs the tie between the Schedule built and the ScheduleBuilder. 
//! Therefore, after the call, the schedule object cannot be modified through the builder. 
//! If a new builder is required to build another schedule, the NewSchedule() method should be called.
//!
Schedule* ScheduleBuilder::GetSchedule() const 
{
  // Bouml preserved body begin 00064C11

	schedule->endPosition = currentPosition;

	for(ull i = 0; i < schedule->branches.size(); i++)
	{
		Schedule* subsched = schedule->branches[i];

		if(subsched->builder != NULL)
		{
			ScheduleBuilder* builder = static_cast<ScheduleBuilder*>(subsched->builder);
			builder->GetSchedule(); // invoke GetSchedule() recursively so endPosition gets set!
			delete builder;
		}

	}

	schedule->builder = NULL;

	return schedule;

  // Bouml preserved body end 00064C11
}

//! 
//! \brief Forks the schedule constructed so far at the current position
//!
//! This method provides the simplest way to use branching.
//!
//! \param[in] branches 	[optional] ushort, the number of branches (the default value is 2).
//!
//! \note This method can only be called once per schedule position (i.e. one cannot use this to branch several time at the current position).
//!
//! \note If branching is successful, the sub-schedules for each branch can be built/specified using the GetBranchSchedulerBuilder() and SetBranchSchedulerBuilder() methods.
//!
//! \see GetBranchSchedulerBuilder(), SetBranchSchedulerBuilder()
//!
//! \return true or false, depending on whether the call is successful
//!
bool ScheduleBuilder::ForkSchedule(ushort branches) 
{
  // Bouml preserved body begin 00064D91

	VERIFY(schedule != NULL);
	if(branches < 2 || wasForked == true || currentPosition == ScheduleEnd) { return false; }

	wasForked = true;

	schedule->branchingPosition = currentPosition;

	schedule->branches = vector<Schedule*>();
	schedule->branches.reserve(branches);

	for(ull i = 0; i < branches; i++)
	{
		string name = schedule->name;

		stringstream ss("");
		ss << name << "_branch" << i;

		ScheduleBuilder* builder =  new ScheduleBuilder(ss.str());
		builder->currentPosition = currentPosition;
		builder->inputDone = inputDone;

		builder->schedule->builder = static_cast<void*>(builder);
		builder->schedule->startPosition = currentPosition;
		schedule->branches.push_back(builder->schedule);
		builder->schedule->root = false; // set the schedule as non-root
	}

	return true;

  // Bouml preserved body end 00064D91
}

//! 
//! \brief Retrieves the ScheduleBuilder object of the specified branch
//!
//! \param[in] branchIdx 	ushort, the index of the branch whose builder object to retrieve.
//!
//! \note branches indices start at 0
//!
//! \return a pointer to the builder of the specified branch, if successful, NULL, otherwise
//!
ScheduleBuilder* ScheduleBuilder::GetBranchScheduleBuilder(const ushort branchIdx) const 
{
  // Bouml preserved body begin 00064F11

	if(wasForked == false || schedule->branches.size() <= branchIdx) { return NULL; }

	Schedule* branch = schedule->branches[branchIdx];
	ScheduleBuilder* builder = static_cast<ScheduleBuilder*>(branch->builder);
	branch->endPosition = builder->currentPosition;

	return builder;

  // Bouml preserved body end 00064F11
}

//! 
//! \brief Sets the ScheduleBuilder object of the specified branch
//!
//! \param[in] branchIdx 	ushort, the branch whose builder object to set.
//! \param[in] builder ScheduleBuilder*, the builder to use
//!
//! \note branches indices start at 0
//!
//! \return true or false, depending on whether the call is successful
//!
bool ScheduleBuilder::SetBranchScheduleBuilder(const ushort branchIdx, const ScheduleBuilder* builder) 
{
  // Bouml preserved body begin 00064F91

	if(builder == NULL || wasForked == false || schedule->branches.size() <= branchIdx) { return false; }

	Schedule* sched = schedule->branches[branchIdx];
	sched->builder = NULL;

	sched->Release();

	schedule->branches[branchIdx] = builder->schedule;

	return true;

  // Bouml preserved body end 00064F91
}

//! 
//! \brief Inserts the default (intermediary) output operation at the current schedule position
//!
//! \return true or false, depending on whether the call is successful
//!
bool ScheduleBuilder::InsertOutputOperation() 
{
  // Bouml preserved body begin 0006A391

	if(inputDone == false || lastOutputPosition == currentPosition) { return false; }

	OutputOperation* output = new OutputOperation("DefaultOutputOperation");
	VERIFY(InsertOutputOperation(output) == true);

	// ensure refCount == 1
	output->Release();

	return true;

  // Bouml preserved body end 0006A391
}

//! 
//! \brief Inserts the specified (intermediary) output operation at the current schedule position
//!
//! \param[in] output 	OutputOperation*, the OutputOperation object to use.
//!
//! \return true or false, depending on whether the call is successful
//!
bool ScheduleBuilder::InsertOutputOperation(const OutputOperation* output) 
{
  // Bouml preserved body begin 00065191

	if(output == NULL || inputDone == false || lastOutputPosition == currentPosition) { return false; }

	switch(currentPosition)
	{
	case ScheduleBeforeApplicationOperation:
	case ScheduleBeforeLPPMOperation:
	case ScheduleBeforeAttackOperation:
		break;
	default:
		return false;
	}

	lastOutputPosition = currentPosition;

	const_cast<OutputOperation*>(output)->AddRef();
	return schedule->AddOutputOperation(output, currentPosition);

  // Bouml preserved body end 00065191
}

//! 
//! \brief Sets the inputs of the schedule: the context file containing the background knowledge (the default input operation and default load context operation will be used)
//!
//! \param[in] contextFile 	File*, the context file.
//!
//! \return true or false, depending on whether the call is successful
//!
bool ScheduleBuilder::SetInputs(const File* contextFile) 
{
  // Bouml preserved body begin 0008A291

	if(wasForked == true || inputDone == true || currentPosition != ScheduleBeginning) { return false; }
	if(contextFile == NULL) { return false; }

	VERIFY(schedule->inputOperation == NULL && schedule->actualTraceInputOperation == NULL && schedule->actualTraceFile == NULL &&
			schedule->contextFile == NULL && schedule->loadContextOperation == NULL);

	inputDone = true;

	schedule->contextFile = const_cast<File*>(contextFile);

	InputOperation* input = new InputOperation("DefaultInputOperation");
	LoadContextOperation* loadContext = new LoadContextOperation("DefaultLoadContextOperation");

	schedule->inputOperation = schedule->actualTraceInputOperation = input;
	schedule->loadContextOperation = loadContext;

	currentPosition = ScheduleBeforeApplicationOperation;

	return true;

  // Bouml preserved body end 0008A291
}

//! 
//! \brief Sets the inputs of the schedule: the context file containing the background knowledge, the actual trace file, and, the application and LPPM PDFs if applicable (the default input operation and default load context operation will be used)
//!
//! Additionally, the start position of the schedule can be specified. If specified, it must be consistent with the application and LPPM PDFs given. That is, if the start position is before the LPPM stage (\a ScheduleBeforeLPPMOperation),
//! then application PDF must be a valid pointer to a filter function of the PDF used to produce the input, and, lppmPDF must be \a NULL. If on the other hand, the start position is before the attack (\a ScheduleBeforeAttackOperation), then
//! both the application and LPPM PDFs must be valid pointers (not \a NULL).
//!
//! If the start position and the PDFs are not consistent, the call will fail. 
//!
//! \note if the caller does not wish to specify the start position,
//! it will be inferred from the application and LPPM PDFs. The start position can be verified by setting the start position pointer 
//! to a SchedulePosition equal to \a ScheduleInvalidPosition, in which case, the function will set the startPosition inferred (provided the call is successful). 
//!
//! \param[in] contextFile 	File*, the context file.
//! \param[in] actualTraceFile 	File*, the actual trace file.
//! \param[in,out] startPosition 	[optional] SchedulePosition*, the position at which the schedule will start.
//! \param[in] appPDF 	FilterFunction*, the application PDF (can't  be \a NULL).
//! \param[in] lppmPDF 	[optional] FilterFunction*, the LPPM PDF (the default is \a NULL, this imply a schedule starting before the LPPM stage).
//!
//! \return true or false, depending on whether the call is successful
//!
bool ScheduleBuilder::SetInputs(File* contextFile, File* actualTraceFile, SchedulePosition* startPosition, FilterFunction* appPDF, FilterFunction* lppmPDF) 
{
  // Bouml preserved body begin 0006A411

	if(wasForked == true || inputDone == true || currentPosition != ScheduleBeginning) { return false; }

	InputOperation* input = new InputOperation("DefaultInputOperation");
	LoadContextOperation* loadContext = new LoadContextOperation("DefaultLoadContextOperation");

	bool status = SetInputs(contextFile, actualTraceFile, input, loadContext, startPosition, appPDF, lppmPDF);

	// ensure refCount == 1
	loadContext->Release();
	input->Release();

	return status;

  // Bouml preserved body end 0006A411
}

//! 
//! \brief Sets the inputs of the schedule: the context file containing the background knowledge, the actual trace file, and, the application and LPPM PDFs (using the specified input operation and load context operation)
//!
//! Additionally, the start position of the schedule can be specified. If specified, it must be consistent with the application and LPPM PDFs given. That is, if the start position is before the LPPM stage (\a ScheduleBeforeLPPMOperation),
//! then application PDF must be a valid pointer to a filter function of the PDF used to produce the input, and, lppmPDF must be \a NULL. If on the other hand, the start position is before the attack (\a ScheduleBeforeAttackOperation), then
//! both the application and LPPM PDFs must be valid pointers (not \a NULL).
//!
//! If the start position and the PDFs are not consistent, the call will fail. 
//!
//! \note if the caller does not wish to specify the start position,
//! it will be inferred from the application and LPPM PDFs. The start position can be verified by setting the start position pointer 
//! to a SchedulePosition equal to \a ScheduleInvalidPosition, in which case, the function will set the startPosition inferred (provided the call is successful). 
//!
//! \param[in] contextFile 	File*, the context file.
//! \param[in] actualTraceFile 	File*, the actual trace file.
//! \param[in] input 	InputOperation*, the input operation to use to read the trace file.
//! \param[in] loadContext 	LoadContextOperation*, the load context operation to use when loading \a contextFile.
//! \param[in,out] startPosition 	[optional] SchedulePosition*, the position at which the schedule will start.
//! \param[in] appPDF 	FilterFunction*, the application PDF (can't  be \a NULL).
//! \param[in] lppmPDF 	[optional] FilterFunction*, the LPPM PDF (the default is \a NULL, this imply a schedule starting before the LPPM stage).
//!
//! \return true or false, depending on whether the call is successful
//!
bool ScheduleBuilder::SetInputs(File* contextFile, File* actualTraceFile, InputOperation* input, LoadContextOperation* loadContext, SchedulePosition* startPosition, FilterFunction* appPDF, FilterFunction* lppmPDF) 
{
  // Bouml preserved body begin 00064C91

	if(wasForked == true || inputDone == true || currentPosition != ScheduleBeginning) { return false; }
	if(contextFile == NULL || actualTraceFile == NULL || loadContext == NULL || appPDF == NULL) { return false; }

	VERIFY(schedule->inputOperation == NULL && schedule->actualTraceInputOperation == NULL && schedule->actualTraceFile == NULL &&
			schedule->contextFile == NULL && schedule->loadContextOperation == NULL);

	if(startPosition != NULL && *startPosition != ScheduleInvalidPosition)
	{
		switch(*startPosition)
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
		default: return false; // can only start before lppm, attack
		}
	}

	inputDone = true;

	schedule->contextFile = contextFile;

	schedule->inputOperation = input;
	schedule->inputOperation->AddRef();

	schedule->actualTraceFile = actualTraceFile;
	schedule->actualTraceInputOperation = new InputOperation("DefaultInputOperation");

	schedule->loadContextOperation = loadContext;
	schedule->loadContextOperation->AddRef();

	schedule->appPDF = appPDF;
	schedule->lppmPDF = lppmPDF;

	if(startPosition != NULL && *startPosition == ScheduleInvalidPosition)
	{
		*startPosition = (lppmPDF == NULL) ? ScheduleBeforeLPPMOperation : ScheduleBeforeAttackOperation;
	}

	if(startPosition == NULL)
	{
		SchedulePosition tmp = ScheduleInvalidPosition;
		startPosition = &tmp;

		if(appPDF != NULL && lppmPDF == NULL) { *startPosition = ScheduleBeforeLPPMOperation; }
		else if(appPDF != NULL && lppmPDF != NULL) { *startPosition = ScheduleBeforeAttackOperation; }
		else { CODING_ERROR; }
	}

	schedule->afterInputPosition = *startPosition;
	currentPosition = *startPosition;

	return true;

  // Bouml preserved body end 00064C91
}

//! 
//! \brief Sets the application
//!
//! \param[in] application 	ApplicationOperation*, the application operation to use.
//!
//! \return true or false, depending on whether the call is successful
//!
bool ScheduleBuilder::SetApplicationOperation(const ApplicationOperation* application) 
{
  // Bouml preserved body begin 00065011

	if(wasForked == true || inputDone == false || currentPosition != ScheduleBeforeApplicationOperation) { return false; }
	if(application == NULL) { return false; }

	schedule->applicationOperation = const_cast<ApplicationOperation*>(application);
	schedule->applicationOperation->AddRef();

	currentPosition = ScheduleBeforeLPPMOperation;

	return true;

  // Bouml preserved body end 00065011
}

//! 
//! \brief Sets the LPPM
//!
//! \param[in] lppm 	LPPMOperation*, the LPPM operation to use.
//!
//! \return true or false, depending on whether the call is successful
//!
bool ScheduleBuilder::SetLPPMOperation(const LPPMOperation* lppm) 
{
  // Bouml preserved body begin 00065091

	if(wasForked == true || inputDone == false || currentPosition != ScheduleBeforeLPPMOperation) { return false; }
	if(lppm == NULL) { return false; }

	schedule->lppmOperation = const_cast<LPPMOperation*>(lppm);
	schedule->lppmOperation->AddRef();

	currentPosition = ScheduleBeforeAttackOperation;

	return true;

  // Bouml preserved body end 00065091
}

//! 
//! \brief Sets the attack
//!
//! \param[in] attack 	AttackOperation*, the attack operation to use.
//!
//! \return true or false, depending on whether the call is successful
//!
bool ScheduleBuilder::SetAttackOperation(const AttackOperation* attack) 
{
  // Bouml preserved body begin 00065111

	if(wasForked == true || inputDone == false || currentPosition != ScheduleBeforeAttackOperation) { return false; }
	if(attack == NULL) { return false; }

	schedule->attackOperation = const_cast<AttackOperation*>(attack);
	schedule->attackOperation->AddRef();

	currentPosition = ScheduleBeforeMetricOperation;

	return true;

  // Bouml preserved body end 00065111
}

//! 
//! \brief Sets the metric
//!
//! \param[in] type 	MetricType, the type of metric to use.
//! \param[in] distance 	[optional] MetricDistance*, the metric distance to use (default value is \a NULL which corresponds to the DefaultMetricDistance object).
//!
//! \note The second (optional) parameter only make sense for \a Distortion based metrics. 
//! It should not be provided, otherwise.
//!
//! \return true or false, depending on whether the call is successful
//!
bool ScheduleBuilder::SetMetricType(MetricType type, MetricDistance* distance) 
{
  // Bouml preserved body begin 00065211

	if(wasForked == true || inputDone == false || currentPosition != ScheduleBeforeMetricOperation) { return false; }

	schedule->metricType = type;

	schedule->distanceFunction = distance;

	currentPosition = ScheduleEnd;

	return true;

  // Bouml preserved body end 00065211
}


} // namespace lpm
