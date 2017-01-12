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
#ifndef LPM_LOG_H
#define LPM_LOG_H

//!
//! \file
//!
#include "Singleton.h"
#include <string>
using namespace std;

#include "Defs.h"

namespace lpm {

//!
//! \brief Provides the logging facilities
//!
//! Singleton class which provides convenient methods to log information, warnings and errors to file.
//!

class Log : public Singleton<Log> 
{
  public:
    Log();

    virtual ~Log();

    //! 
    //! \brief Appends a message to the log
    //
    //! Adds a message at the end of the current log file. 
    //! The seriousness level of the message (info, warning, or error) can be specified.
    //
    //! \param[in] message 	string to append to the log file.
    //! \param[in] level 	[optional] ushort specifying the seriousness of the message.
    //!	This can be one of Log::infoLevel (default), Log::warningLevel, or, Log::errorLevel.
    //
    //! \return nothing
    //!
    void Append(string message, ushort level = Log::infoLevel);


  private:
    ofstream logFile;

    ofstream crashLogFile;

    ofstream errorLogFile;

    bool enabled;


  public:
    static const ushort warningLevel;

    static const ushort errorLevel;

    static const ushort infoLevel;

    //!
    //! \brief Sets the output file name
    //!
    //! \param[in] filename 	string to be use as base file name (if not present, the '.log' extension will be used).
    //
    //! \return nothing
    void SetOutputFileName(string filename);

    //!
    //! \brief Enables/Disables the logging facilities
    //
    //! \param[in] state 	bool determining whether to enable or disable logging.
    //!
    //! \note Crash logging cannot be disabled
    //
    //! \return nothing
    void SetEnabled(bool state);

    //!
    //! \brief Registers an error
    //
    //! \param[in] message 	string detailing the error.
    //!
    //! \return nothing
    void RegisterError(string message);

    //!
    //! \brief Registers a crash
    //!
    //! \param[in] message 	string detailing the crash.
    //
    //! \note the program will terminate before it returns from the call
    //!
    //! \return nothing
    void RegisterCrash(string message);


  private:
    bool GetTimeString(string& timeString) const;

};

} // namespace lpm
#endif
