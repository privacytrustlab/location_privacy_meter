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
#ifndef LPM_ERRORS_H
#define LPM_ERRORS_H

//!
//! \file
//!
#include "Singleton.h"
#include <string>
using namespace std;

#include "Defs.h"
#include "Log.h"

#define NO_ERROR 0

#define MAKE_WARNING_CODE(_code) (((ull)0x1) << 62 | (_code))
#define MAKE_ERROR_CODE(_code) (((ull)0x1) << 63 | (_code))

#define SUCCESS(_code) ((((_code) >> 63) == 0) ? true : false)

#define EXTRACT_CODE(_code) ((_code) & 0xFFFFFFFF)

#define ERROR_CODE_INVALID_ARGUMENTS MAKE_ERROR_CODE(0x00000001UL)
#define ERROR_CODE_INVALID_FORMAT MAKE_ERROR_CODE(0x00000002UL)
#define ERROR_CODE_INVALID_OPERATION MAKE_ERROR_CODE(0x00000010UL)
#define ERROR_CODE_INVALID_POSITION MAKE_ERROR_CODE(0x00000020UL)
#define ERROR_CODE_INVALID_SCHEDULE MAKE_ERROR_CODE(0x00000030UL)

#define ERROR_CODE_INVALID_INPUT_FILE MAKE_ERROR_CODE(0x00000040UL)
#define ERROR_CODE_TRACE_INPUT_FILE_MISMATCH MAKE_ERROR_CODE(0x00000041UL)
#define ERROR_CODE_INCOMPLETE_INPUT_FILE MAKE_ERROR_CODE(0x00000042UL)
#define ERROR_CODE_EMPTY_TRACE MAKE_ERROR_CODE(0x00000043UL)
#define ERROR_CODE_INCOMPATIBLE_INPUT_FILES MAKE_ERROR_CODE(0x00000044UL)
#define ERROR_CODE_DUPLICATE_ENTRIES MAKE_ERROR_CODE(0x00000045UL)
#define ERROR_CODE_IMPOSSIBLE_TRACE MAKE_ERROR_CODE(0x00000046UL)

#define ERROR_CODE_INVALID_TIME_PARTITIONING MAKE_ERROR_CODE(0x00000050UL)
#define ERROR_CODE_INCONSISTENT_TIME_PARTITIONING_USAGE MAKE_ERROR_CODE(0x00000051UL)

#define ERROR_CODE_SIZE_T_OVERFLOW MAKE_ERROR_CODE(0x00000102UL)
#define ERROR_CODE_MEMORY_ALLOCATION_FAILURE MAKE_ERROR_CODE(0x00000201UL)

#define ERROR_CODE_INVALID_METHOD_CALL MAKE_ERROR_CODE(0x00000301UL)

#define SET_ERROR_CODE(_code) Errors::GetInstance()->SetErrorCode((_code), __FILE__, __LINE__)
#define SET_ERROR_CODE_DETAILS(_code, _dets) Errors::GetInstance()->SetErrorCode((_code), __FILE__, __LINE__, (_dets))

namespace lpm {

//!
//! \brief Provides the error reporting facilities
//!
//! Singleton class which provides convenient methods to report errors.
//!

class Errors : public Singleton<Errors> 
{
  public:
    Errors();


  private:
    ull lastErrorCode;

    string lastErrorFile;

    int lastErrorLine;

    string lastErrorDetails;


  public:
    //! 
    //! \brief Returns the error code of the last error (the most recent one)
    //!
    //! \note If no error occurred since the start of the program, a value of \a 0 is returned. 
    //!
    //! \return ull, the error code
    //!
    //!
    ull GetLastErrorCode() const;

    //! 
    //! \brief Sets the error code
    //!
    //! \note This method is not meant to be used (i.e. called) directly. Rather, users should use the SET_ERROR_CODE macro.
    //!
    //! \param[in] errorCode 	ull, the error code.
    //! \param[in,out] file 	char*, the name of the source file whose code encountered the error.
    //! \param[in,out] line int, the line number in the source file at which the error occurred (or was detected).
    //!
    //! \return nothing
    //!
    void SetErrorCode(ull errorCode, const char* file, int line, string details = "");

    //! 
    //! \brief Returns a message detailing the last error (the most recent one) that occurred
    //!
    //! \return string, the message detailing the last error
    //!
    string GetLastErrorMessage() const;

};

} // namespace lpm
#endif
