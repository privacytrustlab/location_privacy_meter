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
#ifndef LPM_LINEFORMATTER_H
#define LPM_LINEFORMATTER_H

//!
//! \file
//!
#include "Singleton.h"
#include <string>
using namespace std;
#include <vector>
using namespace std;

#include "Defs.h"
#include "NoDepend.h"

namespace lpm {

//!
//! \brief Formats some values into a line with a specific format
//!
//! Singleton class which provides convenient methods to format values.
//!

template<typename T>
class LineFormatter : public Singleton<LineFormatter<T> > 
{
  public:
    //! 
    //! \brief Format the given value into a vector (a line of type string) in the specified format 
    //!
    //! \param[in] vector 	T*, the array of type \a T* (template) whose elements will be formatted.
    //! \param[in] vectorLength ull, the number of elements in \a vector.
    //! \param[in,out] outputLine string, the output line which will receive the formatted values.
    //! \param[in] delimiter [optional] char, the fields delimiter to use (the default value is a comma: ',').
    //!
    //! \return true or false, depending on whether the call is successful (i.e. whether the line was parsed successfully)
    //!
    bool FormatVector(const T* vector, ull vectorLength, string& outputLine, char delimiter = DEFAULT_FIELDS_DELIMITER);

    //! 
    //! \brief Format the given values into a matrix (a vector of lines each of type string) in the specified format 
    //!
    //! \param[in] matrix 	T*, the two-dimensional array of type \a T* (template) whose elements will be formatted.
    //! \param[in] rows ull, the number of rows in \a matrix.
    //! \param[in] columns ull, the number of columns in \a matrix.
    //! \param[in,out] outputLines vector<string>, the output vector of lines which will receive the formatted matrix.
    //! \param[in] delimiter [optional] char, the fields delimiter to use (the default value is a comma: ',').
    //!
    //! \return true or false, depending on whether the call is successful (i.e. whether the line was parsed successfully)
    //!
    bool FormatMatrix(const T* matrix, ull rows, ull columns, vector<string>& outputLines, char delimiter = DEFAULT_FIELDS_DELIMITER);

};
//! 
//! \brief Format the given value into a vector (a line of type string) in the specified format 
//!
//! \param[in] vector 	T*, the array of type \a T* (template) whose elements will be formatted.
//! \param[in] vectorLength ull, the number of elements in \a vector.
//! \param[in,out] outputLine string, the output line which will receive the formatted values.
//! \param[in] delimiter [optional] char, the fields delimiter to use (the default value is a comma: ',').
//!
//! \return true or false, depending on whether the call is successful (i.e. whether the line was parsed successfully)
//!
template<typename T>
bool LineFormatter<T>::FormatVector(const T* vector, ull vectorLength, string& outputLine, char delimiter) 
{
  // Bouml preserved body begin 00083991

	if(vector == NULL || vectorLength == 0)
	{
		SET_ERROR_CODE(ERROR_CODE_INVALID_ARGUMENTS);
		return false;
	}

	stringstream ss("");
	for(ull i = 0; i < vectorLength; i++)
	{
		ss << vector[i];
		if(i != vectorLength - 1) { ss << delimiter; }
	}

	outputLine = ss.str();

	return true;

  // Bouml preserved body end 00083991
}

//! 
//! \brief Format the given values into a matrix (a vector of lines each of type string) in the specified format 
//!
//! \param[in] matrix 	T*, the two-dimensional array of type \a T* (template) whose elements will be formatted.
//! \param[in] rows ull, the number of rows in \a matrix.
//! \param[in] columns ull, the number of columns in \a matrix.
//! \param[in,out] outputLines vector<string>, the output vector of lines which will receive the formatted matrix.
//! \param[in] delimiter [optional] char, the fields delimiter to use (the default value is a comma: ',').
//!
//! \return true or false, depending on whether the call is successful (i.e. whether the line was parsed successfully)
//!
template<typename T>
bool LineFormatter<T>::FormatMatrix(const T* matrix, ull rows, ull columns, vector<string>& outputLines, char delimiter) 
{
  // Bouml preserved body begin 00083A11

	if(matrix == NULL || rows == 0 || columns == 0)
	{
		SET_ERROR_CODE(ERROR_CODE_INVALID_ARGUMENTS);
		return false;
	}

	outputLines.clear();
	for(ull i = 0; i < rows; i++)
	{
		stringstream ss("");
		for(ull j = 0; j < columns; j++)
		{
			ss << matrix[GET_INDEX(i, j, columns)];
			if(j != columns - 1) { ss << delimiter; }
		}

		string outputLine = ss.str();
		outputLines.push_back(outputLine);
	}

	return true;

  // Bouml preserved body end 00083A11
}


} // namespace lpm
#endif
