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
#ifndef LPM_FILE_H
#define LPM_FILE_H

//!
//! \file
//!
#include <string>
using namespace std;

#include "Defs.h"

namespace lpm {

//!
//! \brief Represents a file
//!
//! Files, either input, or, output (i.e. read-only, or, read-write) are encapsulated by this class, which
//! additionally provides convenient methods to read/write from/to them. 
//!
//! \note After an object of type File is created, it is good practice to ensure that IsGood() returns \a true.
//! This allows to make sure that the library will able to read/write the file.
//! 
//! \see IsGood(), ReadNextLine(), WriteLine()
//!
class File 
{
  public:
    //! 
    //! \brief Constructs a File object given a \a path.
    //!
    //! \param[in] path 	string, the filepath.
    //! \param[in] readOnly 	[optional] bool, the access mode to the file (read-only, if \a readOnly = \a true, read-write, otherwise)
    //!
    //! \note If \a readOnly = \a false (i.e. the access mode is read-write), the filepath need not point to an existing file. 
    //! If it does not, the file will be created. In either case, the directory structure must exist.
    //!
    File(string path, bool readOnly = true);

    virtual ~File();


  private:
    string path;

    std::fstream stream;

    bool readOnly;

    std::fstream& GetStream() const;


  public:
    //! 
    //! \brief Returns the status of the file
    //!
    //! \return true or false, depending on whether the file has been opened successfully and can be accessed as expected
    //!
    bool IsGood() const;

    //! 
    //! \brief Returns whether the end of the file has been reached
    //!
    //! \return true or false, depending on whether the end of the file has been reached
    //!
    bool IsEOF() const;

    //! 
    //! \brief Rewinds the file so that the next read starts at the beginning
    //!
    //! \note It does not make sense to Rewind an output file.
    //!
    //! \return true or false, depending on whether the operation was successful.
    //!
    bool Rewind() const;

    string GetFilePath() const;

    //! 
    //! \brief Reads the next line in the file
    //!
    //! \param[in,out] line 	string, the output line.
    //!
    //! \return true or false, depending on whether the call is successful
    //!
    bool ReadNextLine(string& line) const;

    //!
    //! \brief Writes the given line to the end of the file
    //!
    //! \param[in] line 	string, the line to append to the file.
    //!
    //! \return true or false, depending on whether the call is successful
    //!
    bool WriteLine(const string line) const;

};

} // namespace lpm
#endif
