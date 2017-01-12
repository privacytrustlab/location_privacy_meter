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
#include "../include/File.h"

namespace lpm {

//! 
//! \brief Constructs a File object given a \a path.
//!
//! \param[in] path 	string, the filepath.
//! \param[in] readOnly 	[optional] bool, the access mode to the file (read-only, if \a readOnly = \a true, read-write, otherwise)
//!
//! \note If \a readOnly = \a false (i.e. the access mode is read-write), the filepath need not point to an existing file. 
//! If it does not, the file will be created. In either case, the directory structure must exist.
//!
File::File(string path, bool readOnly) 
{
  // Bouml preserved body begin 0002B091

	this->readOnly = readOnly;
	this->path = path;
	stream.open(path.c_str(), readOnly == true ? fstream::in : fstream::out);

  // Bouml preserved body end 0002B091
}

File::~File() 
{
  // Bouml preserved body begin 0002B111

	if(stream.is_open()) { stream.close(); }

  // Bouml preserved body end 0002B111
}

std::fstream& File::GetStream() const 
{
  // Bouml preserved body begin 0002B211

	return const_cast<fstream&>(stream);

  // Bouml preserved body end 0002B211
}

//! 
//! \brief Returns the status of the file
//!
//! \return true or false, depending on whether the file has been opened successfully and can be accessed as expected
//!
bool File::IsGood() const 
{
  // Bouml preserved body begin 0002B191

	return stream.good();

  // Bouml preserved body end 0002B191
}

//! 
//! \brief Returns whether the end of the file has been reached
//!
//! \return true or false, depending on whether the end of the file has been reached
//!
bool File::IsEOF() const 
{
  // Bouml preserved body begin 00082011

	return stream.eof();

  // Bouml preserved body end 00082011
}

//! 
//! \brief Rewinds the file so that the next read starts at the beginning
//!
//! \note It does not make sense to Rewind an output file.
//!
//! \return true or false, depending on whether the operation was successful.
//!
bool File::Rewind() const 
{
  // Bouml preserved body begin 0006F491

	if(stream.is_open() == false || readOnly == false) { return false; }

	const_cast<fstream&>(stream).clear();
	const_cast<fstream&>(stream).seekg(ios::beg);

	return IsGood();

  // Bouml preserved body end 0006F491
}

string File::GetFilePath() const 
{
  // Bouml preserved body begin 00032691

	return path;

  // Bouml preserved body end 00032691
}

//! 
//! \brief Reads the next line in the file
//!
//! \param[in,out] line 	string, the output line.
//!
//! \return true or false, depending on whether the call is successful
//!
bool File::ReadNextLine(string& line) const 
{
  // Bouml preserved body begin 00081B91

	if(IsGood() == false || IsEOF() == true) { return false; }

	getline(GetStream(), line);

	ull lastPos = line.length() - 1;
	if(line.empty() == false && line.at(lastPos) == '\r')
	{
		line = line.substr(0, lastPos);
	}

	// check for empty line
	// if the line contains only white spaces and end line chars, it is considered as empty
	bool empty = true;
	const char* l = line.c_str();
	char c = *l;
	while(c != '\0')
	{
		if(isspace(c) == false && c != '\r' && c != '\n') { empty = false; break;}

		l++;
		c = *l;
	}

	if(empty == true) { line = ""; }

	return true;


  // Bouml preserved body end 00081B91
}

//!
//! \brief Writes the given line to the end of the file
//!
//! \param[in] line 	string, the line to append to the file.
//!
//! \return true or false, depending on whether the call is successful
//!
bool File::WriteLine(string line) const 
{
  // Bouml preserved body begin 00081C11

	if(IsGood() == false) { return false; }

	GetStream() << line << endl;

	return true;

  // Bouml preserved body end 00081C11
}


} // namespace lpm
