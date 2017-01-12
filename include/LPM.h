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
#ifndef LPM_LPM_H
#define LPM_LPM_H

//!
//! \file
//!
#include "Singleton.h"
#include "Schedule.h"
#include <map>
using namespace std;
#include <string>
using namespace std;

#include "Defs.h"
#include "Private.h"
#include "Metrics.h"
#include "ExampleTraceGeneratorOperations.h"

namespace lpm { class ContextFactory; } 
namespace lpm { class Context; } 
namespace lpm { class TraceSet; } 
namespace lpm { class FilterFunction; } 
namespace lpm { class AttackOperation; } 
namespace lpm { class AttackOutput; } 
namespace lpm { class Schedule; } 
namespace lpm { class File; } 
namespace lpm { struct KnowledgeInput; } 
namespace lpm { class ContextAnalysisSchedule; } 
namespace lpm { class TraceGeneratorOperation; } 
namespace lpm { class TraceGeneratorInput; } 

namespace lpm {

//! 
//! \brief Main class of the library
//! 
//! The LPM singleton class can be thought of as the engine of the library. As such, it is responsible for running arbitrary schedules
//! and performing procedures such as the knowledge construction.
//!
class LPM : public Singleton<LPM> 
{
  private:
    ContextFactory* contextFactory;


  public:
    struct State 
    {
        SchedulePosition currentPosition;

        void* input;

        Context* context;

        TraceSet* actualTraceSet;

        char* outputPrefix;

        FilterFunction* appPDF;

        FilterFunction* lppmPDF;

        AttackOperation* attackOperation;

        AttackOutput* attackOutput;

        map<ull, ull> anonymizationMap;

    };
    
    LPM();

    ~LPM();

    bool SetContextFactory(ContextFactory* factory);

    //! 
    //! \brief Runs the specified schedule
    //!
    //! \param[in] schedule 	Schedule*, the schedule to run.
    //! \param[in] actualTraceFile 	File*, the \a actual trace file.
    //! \param[in,out] outputPrefix string, the prefix to use when naming output files.
    //!
    //! \return true or false, depending on whether the call is successful (i.e. whether the schedule executes successfully)
    //!
    bool RunSchedule(const Schedule* schedule, const File* actualTraceFile, const char* outputPrefix) const;


  private:
    bool RunSubschedule(const Schedule* subschedule, State* state) const;

    bool DoOutput(const Schedule* schedule, SchedulePosition currentPosition, string outputFilePath, const TraceSet* currentTraceSet) const;

    void LogParameters() const;


  public:
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
    bool RunKnowledgeConstruction(const KnowledgeInput* knowledgeFiles, File* outputFile, ull maxGSIterationsPerUser = KC_DEFAULT_GS_ITERATIONS, ull maxSecondsPerUser = KC_NO_LIMITS) const;

    bool RunContextAnalysisSchedule(ContextAnalysisSchedule* schedule, const File* contextFile, string outputFileName) const;

    //! 
    //! \brief Generates an artificial actual trace by sampling from the provided background knowledge
    //!
    //! \param[in] knowledge 	File*, the knowledge file to use.
    //! \param[in] output 	File*, the ouput trace file.
    //!
    //! \return true or false, depending on whether the call is successful (i.e. whether the trace was generated successfully)
    //!
    bool GenerateTracesFromKnowledge(const File* knowledge, File* output) const;

    //! 
    //! \brief Generates an artificial actual trace using the provided trace generator
    //!
    //! \param[in] generator 	TraceGeneratorOperation*, the generator to use.
    //! \param[in] input 	TraceGeneratorInput*, the generator's input.
    //! \param[in] output 	File*, the ouput trace file.
    //!
    //! \return true or false, depending on whether the call is successful (i.e. whether the trace was generated successfully)
    //!
    bool GenerateTraces(TraceGeneratorOperation* generator, const TraceGeneratorInput* input, File* output) const;

};

} // namespace lpm
#endif
