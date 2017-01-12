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
#ifndef LPM_SCHEDULEBUILDER_H
#define LPM_SCHEDULEBUILDER_H

//!
//! \file
//!
#include "Schedule.h"
#include <string>
using namespace std;

#include "Defs.h"
#include "Private.h"

namespace lpm { class Schedule; } 
namespace lpm { class OutputOperation; } 
namespace lpm { class File; } 
namespace lpm { class FilterFunction; } 
namespace lpm { class InputOperation; } 
namespace lpm { class LoadContextOperation; } 
namespace lpm { class ApplicationOperation; } 
namespace lpm { class LPPMOperation; } 
namespace lpm { class AttackOperation; } 
namespace lpm { class MetricDistance; } 

namespace lpm {

//!
//! \brief Implements a schedule building framework
//!
//! Provides convenient and powerful methods to build fully customized schedules by taking advantage of all available features (e.g. intermediary outputs, branching, etc.). 
//!
//! The methods to specified the operations to use need to be called in the logical order of their position (e.g. SetInputs() must be called first, SetApplicationOperation() if called, must be called before SetLPPMOperation()).
//!
//! \note The \a Quick \a Start \a Guide explains the best practices when using schedule builders
//!
//!
class ScheduleBuilder 
{
  private:
    Schedule* schedule;

    SchedulePosition currentPosition;


  public:
    //! 
    //! \brief Constructs a schedule builder object
    //!
    //! \param[in] name 	[optional] string, the name of the schedule that will be built by the schedule builder.
    //!
    //! \note Naming a schedule builder names the constructed schedules.
    //! \note Even though naming is optional (the default name is "unnamed schedule") it is good practice to do so.
    //!
    explicit ScheduleBuilder(string name);

    ~ScheduleBuilder();

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
    void NewSchedule(string name);

    //! 
    //! \brief Returns a pointer to the schedule built
    //!
    //! Allows to retrieve the Schedule object built so far.
    //!
    //! \note This effectively severs the tie between the Schedule built and the ScheduleBuilder. 
    //! Therefore, after the call, the schedule object cannot be modified through the builder. 
    //! If a new builder is required to build another schedule, the NewSchedule() method should be called.
    //!
    Schedule* GetSchedule() const;

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
    bool ForkSchedule(ushort branches = 2);

    //! 
    //! \brief Retrieves the ScheduleBuilder object of the specified branch
    //!
    //! \param[in] branchIdx 	ushort, the index of the branch whose builder object to retrieve.
    //!
    //! \note branches indices start at 0
    //!
    //! \return a pointer to the builder of the specified branch, if successful, NULL, otherwise
    //!
    ScheduleBuilder* GetBranchScheduleBuilder(const ushort branchIdx) const;

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
    bool SetBranchScheduleBuilder(const ushort branchIdx, const ScheduleBuilder* builder);

    //! 
    //! \brief Inserts the default (intermediary) output operation at the current schedule position
    //!
    //! \return true or false, depending on whether the call is successful
    //!
    bool InsertOutputOperation();

    //! 
    //! \brief Inserts the specified (intermediary) output operation at the current schedule position
    //!
    //! \param[in] output 	OutputOperation*, the OutputOperation object to use.
    //!
    //! \return true or false, depending on whether the call is successful
    //!
    bool InsertOutputOperation(const OutputOperation* output);

    //! 
    //! \brief Sets the inputs of the schedule: the context file containing the background knowledge (the default input operation and default load context operation will be used)
    //!
    //! \param[in] contextFile 	File*, the context file.
    //!
    //! \return true or false, depending on whether the call is successful
    //!
    bool SetInputs(const File* contextFile);

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
    bool SetInputs(File* contextFile, File* actualTraceFile, SchedulePosition* startPosition, FilterFunction* appPDF, FilterFunction* lppmPDF = NULL);

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
    bool SetInputs(File* contextFile, File* actualTraceFile, InputOperation* input, LoadContextOperation* loadContext, SchedulePosition* startPosition, FilterFunction* appPDF, FilterFunction* lppmPDF);

    //! 
    //! \brief Sets the application
    //!
    //! \param[in] application 	ApplicationOperation*, the application operation to use.
    //!
    //! \return true or false, depending on whether the call is successful
    //!
    bool SetApplicationOperation(const ApplicationOperation* application);

    //! 
    //! \brief Sets the LPPM
    //!
    //! \param[in] lppm 	LPPMOperation*, the LPPM operation to use.
    //!
    //! \return true or false, depending on whether the call is successful
    //!
    bool SetLPPMOperation(const LPPMOperation* lppm);

    //! 
    //! \brief Sets the attack
    //!
    //! \param[in] attack 	AttackOperation*, the attack operation to use.
    //!
    //! \return true or false, depending on whether the call is successful
    //!
    bool SetAttackOperation(const AttackOperation* attack);

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
    bool SetMetricType(MetricType type, MetricDistance* distance = NULL);


  private:
    bool wasForked;

    bool inputDone;

    SchedulePosition lastOutputPosition;

};

} // namespace lpm
#endif
