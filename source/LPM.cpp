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
#include "../include/LPM.h"
#include "../include/Context.h"
#include "../include/TraceSet.h"
#include "../include/FilterOperation.h"
#include "../include/AttackOperation.h"
#include "../include/AttackOutput.h"
#include "../include/File.h"
#include "../include/CreateContextOperation.h"
#include "../include/ContextAnalysisSchedule.h"
#include "../include/TraceGeneratorOperation.h"

namespace lpm {

LPM::LPM() 
{
  // Bouml preserved body begin 00098311

	contextFactory = new ContextFactory();

  // Bouml preserved body end 00098311
}

LPM::~LPM() 
{
  // Bouml preserved body begin 00098391

	if(contextFactory != NULL) { delete contextFactory; }

  // Bouml preserved body end 00098391
}

bool LPM::SetContextFactory(ContextFactory* factory) 
{
  // Bouml preserved body begin 00098291

	if(factory == NULL) { return false; }

	if(contextFactory != NULL) { delete contextFactory; }

	contextFactory = factory;

	return true;

  // Bouml preserved body end 00098291
}

//! 
//! \brief Runs the specified schedule
//!
//! \param[in] schedule 	Schedule*, the schedule to run.
//! \param[in] actualTraceFile 	File*, the \a actual trace file.
//! \param[in,out] outputPrefix string, the prefix to use when naming output files.
//!
//! \return true or false, depending on whether the call is successful (i.e. whether the schedule executes successfully)
//!
bool LPM::RunSchedule(const Schedule* schedule, const File* actualTraceFile, const char* outputPrefix) const 
{
  // Bouml preserved body begin 0002B311

	Log::GetInstance()->Append("Entering LPM::RunSchedule");

	if(schedule == NULL || schedule->IsValid() == false)
	{
		SET_ERROR_CODE(ERROR_CODE_INVALID_SCHEDULE);
		Log::GetInstance()->Append("LPM::RunSchedule: Invalid schedule!", Log::errorLevel);
		return false;
	}

	if(actualTraceFile == NULL || actualTraceFile->IsGood() == false)
	{
		SET_ERROR_CODE(ERROR_CODE_INVALID_INPUT_FILE);
		Log::GetInstance()->Append("LPM::RunSchedule: Invalid input file!", Log::errorLevel);
		return false;
	}

	Log::GetInstance()->Append("Entered LPM::RunSchedule()!");

	LogParameters();

	// log schedule
	stringstream info("");
	info << "Running " << schedule->GetDetailString();
	Log::GetInstance()->Append(info.str());

	ull stateByteSize = sizeof(LPM::State);
	LPM::State* state = (LPM::State*)Allocate(stateByteSize);
	memset(state, 0, stateByteSize);

	state->input = (void*)actualTraceFile;
	state->currentPosition = ScheduleBeginning;
	state->outputPrefix = const_cast<char*>(outputPrefix);
	state->context = NULL;
	state->appPDF = NULL;
	state->lppmPDF = NULL;
	state->actualTraceSet = NULL;
	state->attackOperation = NULL;
	state->attackOutput = NULL;
	state->anonymizationMap = map<ull, ull>();

	bool success = RunSubschedule(schedule, state);

	Free(state);

	if(success == false) { return false; }

	Log::GetInstance()->Append("LPM::RunSchedule exiting successfully!");

	return true;

  // Bouml preserved body end 0002B311
}

bool LPM::RunSubschedule(const Schedule* subschedule, LPM::State* state) const 
{
  // Bouml preserved body begin 00066B91

	const Schedule* schedule = subschedule;
	void* currentInput = state->input;
	VERIFY(schedule != NULL && currentInput != NULL);

	SchedulePosition currentPosition = state->currentPosition;
	SchedulePosition endPosition = schedule->endPosition;
	VERIFY(currentPosition <= endPosition);

	Context* context = state->context;

	File* traceFile = NULL;
	TraceSet* actualTraceSet = state->actualTraceSet;
	TraceSet* currentTraceSet = NULL;

	FilterFunction* appPDF = state->appPDF;
	FilterFunction* lppmPDF = state->lppmPDF;

	stringstream tmp("");
	if(state->outputPrefix != NULL) { tmp << state->outputPrefix; }
	string outputPrefix = tmp.str();

	AttackOperation* attackOperation = state->attackOperation;
	AttackOutput* attackOutput = state->attackOutput;
	map<ull, ull> anonymizationMap = state->anonymizationMap;

	switch(currentPosition)
	{
		case ScheduleBeforeInputs: traceFile = static_cast<File*>(currentInput); break;
		case ScheduleBeforeApplicationOperation: currentTraceSet = static_cast<TraceSet*>(currentInput); break;
		case ScheduleBeforeLPPMOperation: currentTraceSet = static_cast<TraceSet*>(currentInput); break;
		case ScheduleBeforeAttackOperation: currentTraceSet = static_cast<TraceSet*>(currentInput); break;
		case ScheduleBeforeMetricOperation: currentTraceSet = static_cast<TraceSet*>(currentInput); break; // metric has no input TraceSet, but we need to free memory
		default:
			Log::GetInstance()->Append("LPM::RunSubschedule: Unknown Schedule Position!", Log::errorLevel);
			CODING_ERROR;
			return false;
		break;
	}

	while(currentPosition < endPosition || subschedule->branchingPosition == currentPosition)
	{
		// branch if needed
		if(subschedule->branchingPosition == currentPosition)
		{
			state->actualTraceSet = actualTraceSet;
			state->currentPosition = currentPosition;
			state->context = context;
			state->input = currentInput;
			state->appPDF = appPDF;
			state->lppmPDF = lppmPDF;
			state->attackOperation = attackOperation;
			state->attackOutput = attackOutput;
			state->anonymizationMap = anonymizationMap;

			for(ull branch = 0; branch < schedule->branches.size(); branch++)
			{
				Schedule* subsched = schedule->branches[branch];
				VERIFY(subsched != NULL);

				stringstream newOutputPrefix("");
				newOutputPrefix << outputPrefix << "-branch" << branch;
				string tmp = newOutputPrefix.str();
				state->outputPrefix = strdup(tmp.c_str());

				if(actualTraceSet != NULL) { actualTraceSet->AddRef(); }
				if(currentTraceSet != NULL) { currentTraceSet->AddRef(); }
				if(state->context != NULL) { state->context->AddRef(); }
				if(state->attackOutput != NULL) { state->attackOutput->AddRef(); }

				ull stateByteSize = sizeof(LPM::State);
				LPM::State* stateCopy = (LPM::State*)Allocate(stateByteSize);
				memcpy(stateCopy, state, stateByteSize);

				if(RunSubschedule(subsched, stateCopy) == false)
				{
					stringstream ss("LPM::RunSubschedule: Failed while branching (branch ");
					ss << branch << ")!";
					Log::GetInstance()->Append(ss.str(), Log::errorLevel);

					Free(stateCopy);
					free(state->outputPrefix);  // the memory was allocated with strdup
					state->outputPrefix = NULL;
					return false;
				}

				Free(stateCopy);
				free(state->outputPrefix); // the memory was allocated with strdup
				state->outputPrefix = NULL;
			}

			VERIFY(state->actualTraceSet != NULL && currentTraceSet != NULL && state->context != NULL);

			state->actualTraceSet->Release();
			currentTraceSet->Release();
			state->context->Release();
			if(state->attackOutput != NULL) { state->attackOutput->Release(); }

			return true;
		}

		switch(currentPosition)
		{
			case ScheduleBeforeInputs:
			{
				// Read actual trace file
				InputOperation* inputOperation = schedule->actualTraceInputOperation;

				File* actualTraceFile = traceFile;
				if(schedule->afterInputPosition != ScheduleInvalidPosition) // if we are not starting from the beginning
				{
					actualTraceFile = schedule->actualTraceFile;
				}

				VERIFY(inputOperation != NULL);
				state->actualTraceSet = actualTraceSet = currentTraceSet = new TraceSet(ActualTrace);
				if(inputOperation->Execute(actualTraceFile, actualTraceSet) == false) { return false; } /* the Execute() method sets the error. */

				Log::GetInstance()->Append("LPM::RunSchedule: Executed Actual Trace InputOperation::Execute successfully!");

				// Load context
				VERIFY(context == NULL);
				state->context = context = contextFactory->NewContext();

				VERIFY(context != NULL);
				VERIFY(schedule->contextFile != NULL && schedule->loadContextOperation != NULL);

				LoadContextOperation* loadContextOperation = schedule->loadContextOperation;
				File* contextFile = schedule->contextFile;

				if(loadContextOperation->Execute(contextFile, context) == false) { return false; }

				// check that we have a well-defined time partitioning
				Parameters* params = Parameters::GetInstance();
				ull minTime = 0; ull maxTime = 0;
				VERIFY(params->GetTimestampsRange(&minTime, &maxTime) == true);
				for(ull tm = minTime; tm <= maxTime; tm++)
				{
					if(params->LookupTimePeriod(tm, false, NULL, NULL) == INVALID_TIME_PERIOD)
					{
						SET_ERROR_CODE(ERROR_CODE_INCONSISTENT_TIME_PARTITIONING_USAGE);
						Log::GetInstance()->Append("LPM::RunSchedule: The time partitioning and the timestamps range are inconsistent!", Log::errorLevel);
						return false;
					}
				}

				Log::GetInstance()->Append("LPM::RunSchedule: Executed LoadContextOperation::Execute successfully!");

				TraceType type = ActualTrace;
				switch(schedule->afterInputPosition)
				{
				case ScheduleBeforeLPPMOperation:
					{
						currentPosition = schedule->afterInputPosition;
						type = ExposedTrace;
						appPDF = schedule->appPDF;
						lppmPDF = NULL;
					}
					break;
				case ScheduleBeforeAttackOperation:
					{
						currentPosition = schedule->afterInputPosition;
						type = ObservedTrace;
						appPDF = schedule->appPDF;
						lppmPDF = schedule->lppmPDF;
					}
					break;
				default:
					{
						currentPosition = ScheduleBeforeApplicationOperation; // advance current position anyways
						type = ActualTrace;
					}
					break;
				}

				if(schedule->afterInputPosition != ScheduleInvalidPosition) // if we are not starting from the beginning
				{
					VERIFY(schedule->actualTraceInputOperation != schedule->inputOperation);

					// read the input trace
					inputOperation = schedule->inputOperation;
					VERIFY(inputOperation != NULL);
					currentTraceSet = new TraceSet(type);
					if(inputOperation->Execute(traceFile, currentTraceSet) == false) { return false; } /* the Execute() method sets the error. */

					Log::GetInstance()->Append("LPM::RunSchedule: Executed InputOperation::Execute successfully!");

					// give the context to the PDFs
					if(appPDF != NULL) { dynamic_cast<ApplicationOperation*>(appPDF)->SetContext(context); }
					if(lppmPDF != NULL) { dynamic_cast<LPPMOperation*>(lppmPDF)->SetContext(context); }
				}

				LogParameters();

				if(DoOutput(schedule, currentPosition, outputPrefix, currentTraceSet) == false) { return false; }
			}
			break;
			case ScheduleBeforeApplicationOperation:
			{
				VERIFY(currentTraceSet != NULL && currentTraceSet->GetTraceType() == ActualTrace);
				VERIFY(context != NULL && schedule->applicationOperation != NULL);

				ApplicationOperation* applicationOperation = schedule->applicationOperation;
				applicationOperation->SetContext(context);
				appPDF = (FilterFunction*)applicationOperation;

				TraceSet* exposedTraceSet = new TraceSet(ExposedTrace);
				VERIFY(exposedTraceSet != NULL);
				if(applicationOperation->Execute(currentTraceSet, exposedTraceSet) == false) { return false; }

				Log::GetInstance()->Append("LPM::RunSchedule: Executed ApplicationOperation::Execute successfully!");

				if(currentTraceSet != actualTraceSet) { currentTraceSet->Release(); }
				currentTraceSet = exposedTraceSet;

				currentPosition = ScheduleBeforeLPPMOperation;

				if(DoOutput(schedule, currentPosition, outputPrefix, currentTraceSet) == false) { return false; }
			}
			break;
			case ScheduleBeforeLPPMOperation:
			{
				VERIFY(currentTraceSet != NULL && currentTraceSet->GetTraceType() == ExposedTrace);
				VERIFY(context != NULL && schedule->lppmOperation != NULL);

				LPPMOperation* lppmOperation = schedule->lppmOperation;
				lppmOperation->SetContext(context);
				lppmPDF = (FilterFunction*)lppmOperation;

				TraceSet* observedTraceSet = new TraceSet(ObservedTrace);
				VERIFY(observedTraceSet != NULL);
				if(lppmOperation->Execute(currentTraceSet, observedTraceSet) == false) { return false; }

				Log::GetInstance()->Append("LPM::RunSchedule: Executed LPPMOperation::Execute successfully!");

				lppmOperation->GetAnonymizationMap(anonymizationMap);

				currentTraceSet->Release();
				currentTraceSet = observedTraceSet;

				currentPosition = ScheduleBeforeAttackOperation;

				if(DoOutput(schedule, currentPosition, outputPrefix, currentTraceSet) == false) { return false; }
			}
			break;
			case ScheduleBeforeAttackOperation:
			{
				VERIFY(currentTraceSet != NULL && currentTraceSet->GetTraceType() == ObservedTrace);
				VERIFY(context != NULL && schedule->attackOperation != NULL);

				attackOperation = schedule->attackOperation;
				VERIFY(attackOperation != NULL);
				attackOperation->SetContext(context);
				VERIFY(attackOperation->SetPDFs(appPDF, lppmPDF) == true);

				attackOutput = new AttackOutput();
				VERIFY(attackOutput != NULL);
				if(attackOperation->Execute(currentTraceSet, attackOutput) == false) { return false; }

				stringstream info("");
				info << "LPM::RunSchedule: Executed AttackOperation::Execute successfully!";
				Log::GetInstance()->Append(info.str());

				currentPosition = ScheduleBeforeMetricOperation;
			}
			break;
			case ScheduleBeforeMetricOperation:
			{
				MetricType metricType = schedule->metricType;
				MetricOperation* metricOperation = NULL;

				VERIFY(attackOperation != NULL);
				if(attackOperation->CreateMetric(metricType, &metricOperation) == false) { return false; }
				VERIFY(metricOperation != NULL);

				stringstream info("");
				info << "LPM::RunSchedule: Created the MetricOperation successfully!";
				Log::GetInstance()->Append(info.str());

				VERIFY(metricOperation->SetActualTrace(actualTraceSet) == true);
				switch(metricType)
				{
					case Distortion:
					case MostLikelyLocationDistortion:
					case MostLikelyTraceDistortion:
						{
							MetricDistance* distance = schedule->distanceFunction;
							if(distance == NULL) // create the default
							{ distance = new DefaultMetricDistance(); }

							(dynamic_cast<DistortionMetricOperation*>(metricOperation))->SetDistanceFunction(distance);
						}
						break;
					case Anonymity:
						{
							(dynamic_cast<AnonymityMetricOperation*>(metricOperation))->SetAnonymizationMap(anonymizationMap);
						}
						break;
					default:
						break;
				}

				info.str("");
				info << outputPrefix << "-metric_" << metricOperation->GetTypeString();
				File outputFile(info.str(), false);

				if(outputFile.IsGood() == false)
				{
					SET_ERROR_CODE(ERROR_CODE_INVALID_ARGUMENTS);
					return false;
				}

				if(metricOperation->Execute(attackOutput, &outputFile) == false) { return false; }

				// release the metricOperation object here since we instantiated it (it is not part of the Schedule like the rest)
				metricOperation->Release();

				attackOutput->Release();

				info.str("");
				info << "LPM::RunSchedule: Executed MetricOperation::Execute successfully!";
				Log::GetInstance()->Append(info.str());

				currentPosition = ScheduleEnd;
				VERIFY(schedule->endPosition == ScheduleEnd);
			}
			break;
			default:
				Log::GetInstance()->Append("LPM::RunSubschedule: Unknown Schedule Position!", Log::errorLevel);
				CODING_ERROR;
				return false;
			break;
		}

		currentInput = static_cast<void*>(currentTraceSet);
	}

	if(state->actualTraceSet != NULL) { state->actualTraceSet->Release(); }

	if(currentTraceSet != NULL) { currentTraceSet->Release(); }

	if(state->context != NULL) { state->context->Release(); }

	return true;

  // Bouml preserved body end 00066B91
}

bool LPM::DoOutput(const Schedule* schedule, SchedulePosition currentPosition, string outputFilePath, const TraceSet* currentTraceSet) const 
{
  // Bouml preserved body begin 00032711

	OutputOperation* outputOperation = schedule->GetOutputOperationAt(currentPosition);
	if(outputOperation != NULL)
	{
		string tmp = "LPM::DoOutput: Outputting something at position ";
		string pos = "-";
		switch(currentPosition)
		{
		case ScheduleBeforeApplicationOperation: pos += "input"; break;
		case ScheduleBeforeLPPMOperation: pos += "app"; break;
		case ScheduleBeforeAttackOperation: pos += "lppm"; break;

		default:
			CODING_ERROR; break;
		}

		tmp += pos;
		Log::GetInstance()->Append(tmp);

		outputFilePath += pos;
		File outputFile(outputFilePath, false);

		if(outputFile.IsGood() == false)
		{
			SET_ERROR_CODE(ERROR_CODE_INVALID_ARGUMENTS);
			return false;
		}

		if(outputOperation->Execute(currentTraceSet, &outputFile) == false) { return false; }

		Log::GetInstance()->Append("LPM::DoOutput: Output successfully written in " + outputFilePath + "!");
	}

	return true;

  // Bouml preserved body end 00032711
}

void LPM::LogParameters() const 
{
  // Bouml preserved body begin 00096891

	Log* log = Log::GetInstance();
	Parameters* params = Parameters::GetInstance();

	// GUR: ### ull minUser = 0; ull maxUser = 0;
	// GUR: ### VERIFY(params->GetUsersRange(&minUser, &maxUser) == true);

	set<ull> users = set<ull>();
	VERIFY(params->GetUsersSet(users) == true);

	stringstream info("");
	info << "Users set is {";
	ull i = 0;
	foreach_const(set<ull>, users, iter)
	{
		info << *iter;

		if(i == users.size() - 1) { info << "} !"; }
		else { info << ", "; }

		i++;
	}
	log->Append(info.str());


	ull minTime = 0; ull maxTime = 0;
	VERIFY(params->GetTimestampsRange(&minTime, &maxTime) == true);
	ull numTimes = maxTime - minTime + 1;

	info.str("");
	info << "Time parameters are (minTime, maxTime, numTimes) = (" << minTime << ", " << maxTime << ", " << numTimes << ")!";
	log->Append(info.str());

	// logging
	ull minLoc = 0; ull maxLoc = 0;
	VERIFY(params->GetLocationstampsRange(&minLoc, &maxLoc) == true);
	ull numLoc = maxLoc - minLoc + 1;

	info.str("");
	info << "Location parameters are  (minLoc, maxLoc, numLoc) = (" << minLoc << ", " << maxLoc << ", " << numLoc << ")!";
	log->Append(info.str());

  // Bouml preserved body end 00096891
}

//! 
//! \brief Runs the knowledge construction
//!
//! \param[in] knowledgeFiles 	KnowledgeInput*, the files to use for the construction.
//! \param[in] outputFile 	File*, the output file.
//! \param[in] maxGSIterationsPerUser [optional] ull, the maximum number of Gibbs sampling iterations, for each user.
//! \param[in] maxSecondsPerUser [optional] ull, the maximum number of seconds to spend in the Gibbs sampling procedure, for each user.
//!
//! \return true or false, depending on whether the call is successful (i.e. whether the knowledge is constructed successfully)
//!
bool LPM::RunKnowledgeConstruction(const KnowledgeInput* knowledgeFiles, File* outputFile, ull maxGSIterationsPerUser, ull maxSecondsPerUser) const 
{
  // Bouml preserved body begin 00067091

	if(knowledgeFiles == NULL || outputFile == NULL || outputFile->IsGood() == false)
	{
		SET_ERROR_CODE(ERROR_CODE_INVALID_ARGUMENTS);
		return false;
	}

	vector<File*> files = knowledgeFiles->learningTraceFilesVector;
	foreach_const(vector<File*>, files, iter)
	{
		File* file = *iter;

		if(file == NULL || file->IsGood() == false)
		{
			string details = "";
			if(file->IsGood() == false) { details = "one of the provided learning file was not found"; }

			SET_ERROR_CODE_DETAILS(ERROR_CODE_INVALID_ARGUMENTS, details);
			return false;
		}
	}

	Log::GetInstance()->Append("Entered LPM::RunKnowledgeConstruction()!");

	CreateContextOperation* createContextOperation = new CreateContextOperation("CreateContextOperation");
	StoreContextOperation* storeContextOperation = new StoreContextOperation("StoreContextOperation");

	Context* context = contextFactory->NewContext();

	bool success = createContextOperation->SetLimits(maxGSIterationsPerUser, maxSecondsPerUser);

	if(success == true) { if(createContextOperation->Execute(knowledgeFiles, context) == false) { success = false; } }

	if(success == true) { if(storeContextOperation->Execute(context, outputFile) == false) { success = false; } }

	context->Release();
	createContextOperation->Release();
	storeContextOperation->Release();

	Log::GetInstance()->Append((success == true) ? "Exited LPM::RunKnowledgeConstruction() successfully!" : "LPM::RunKnowledgeConstruction() failed!");

	return success;

  // Bouml preserved body end 00067091
}

bool LPM::RunContextAnalysisSchedule(ContextAnalysisSchedule* schedule, const File* contextFile, string outputFileName) const 
{
  // Bouml preserved body begin 000BB611

	File output(outputFileName, false);

	if(contextFile == NULL || schedule == NULL || output.IsGood() == false)
	{
		SET_ERROR_CODE(ERROR_CODE_INVALID_ARGUMENTS);
		return false;
	}

	Log::GetInstance()->Append("Entered LPM::RunContextAnalysisSchedule()!");

	LoadContextOperation* loadContextOperation = schedule->loadContextOperation;
	Context* context = contextFactory->NewContext();

	bool success = loadContextOperation->Execute(contextFile, context);

	if(success != false)
	{
		success = schedule->contextAnalysisOperation->Execute(context, &output);
	}

	Log::GetInstance()->Append((success == true) ? "Exited LPM::RunContextAnalysisSchedule() successfully!" : "LPM::RunContextAnalysisSchedule() failed!");

	return success;

  // Bouml preserved body end 000BB611
}

//! 
//! \brief Generates an artificial actual trace by sampling from the provided background knowledge
//!
//! \param[in] knowledge 	File*, the knowledge file to use.
//! \param[in] output 	File*, the ouput trace file.
//!
//! \return true or false, depending on whether the call is successful (i.e. whether the trace was generated successfully)
//!
bool LPM::GenerateTracesFromKnowledge(const File* knowledge, File* output) const 
{
  // Bouml preserved body begin 00096791

	if(knowledge == NULL || output == NULL || output->IsGood() == false)
	{
		SET_ERROR_CODE(ERROR_CODE_INVALID_ARGUMENTS);
		return false;
	}

	Log::GetInstance()->Append("Entered LPM::GenerateTracesFromKnowledge()!");

	LoadContextOperation* loadContextOperation = new LoadContextOperation();
	Context* context = contextFactory->NewContext();

	KnowledgeSamplingTraceGeneratorOperation* generatorOperation = new KnowledgeSamplingTraceGeneratorOperation();

	bool success = loadContextOperation->Execute(knowledge, context);

	if(success != false)
	{
		// check that we have a well-defined time partitioning
		Parameters* params = Parameters::GetInstance();
		ull minTime = 0; ull maxTime = 0;
		VERIFY(params->GetTimestampsRange(&minTime, &maxTime) == true);
		for(ull tm = minTime; tm <= maxTime; tm++)
		{
			if(params->LookupTimePeriod(tm, false, NULL, NULL) == INVALID_TIME_PERIOD)
			{
				SET_ERROR_CODE(ERROR_CODE_INCONSISTENT_TIME_PARTITIONING_USAGE);
				Log::GetInstance()->Append("LPM::RunSchedule: The time partitioning and the timestamps range are inconsistent!", Log::errorLevel);
				return false;
			}
		}

		KnowledgeTraceGeneratorInput generatorInput = KnowledgeTraceGeneratorInput(context);
		success = GenerateTraces(generatorOperation, &generatorInput, output);
	}

	loadContextOperation->Release();
	context->Release();
	generatorOperation->Release();

	Log::GetInstance()->Append((success == true) ? "Exited LPM::GenerateTracesFromKnowledge() successfully!" : "LPM::GenerateTracesFromKnowledge() failed!");

	return success;

  // Bouml preserved body end 00096791
}

//! 
//! \brief Generates an artificial actual trace using the provided trace generator
//!
//! \param[in] generator 	TraceGeneratorOperation*, the generator to use.
//! \param[in] input 	TraceGeneratorInput*, the generator's input.
//! \param[in] output 	File*, the ouput trace file.
//!
//! \return true or false, depending on whether the call is successful (i.e. whether the trace was generated successfully)
//!
bool LPM::GenerateTraces(TraceGeneratorOperation* generator, const TraceGeneratorInput* input, File* output) const 
{
  // Bouml preserved body begin 00096811

	if(generator == NULL || input == NULL || output == NULL || output->IsGood() == false)
	{
		SET_ERROR_CODE(ERROR_CODE_INVALID_ARGUMENTS);
		return false;
	}

	Log::GetInstance()->Append("Entered LPM::GenerateTraces()!");

	bool success = generator->Execute(input, output);

	Log::GetInstance()->Append((success == true) ? "Exited LPM::GenerateTraces() successfully!" : "LPM::GenerateTraces() failed!");

	return success;

  // Bouml preserved body end 00096811
}


} // namespace lpm
