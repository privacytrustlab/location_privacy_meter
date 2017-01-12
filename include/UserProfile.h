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
#ifndef LPM_USERPROFILE_H
#define LPM_USERPROFILE_H

//!
//! \file
//!
#include "Reference.h"

#include "Defs.h"

namespace lpm {

//!
//! \brief Represents the mobility profile a given user
//!
//! The user profile of a given user contains a transition matrix and a steady-state vector.
//!
//! \see Context
//!
class UserProfile : public Reference<UserProfile> 
{
  private:
    ull user;

    double* transitionMatrix;

    double* steadystateVector;

    ull numSamples;

    double* varianceMatrix;


  public:
    explicit UserProfile(ull u);

    virtual ~UserProfile();

    ull GetUser() const;

    bool SetTransitionMatrix(const double * matrix);

    bool SetSteadyStateVector(const double* vector);

    bool SetAccuracyInfo(ull samples, const double* variance);

    //! 
    //! \brief Returns the transition matrix
    //!
    //! \param[out] matrix 	double**, a pointer which will point to the output transition matrix (if the call is successful).
    //!
    //! \note The transition matrix is a two dimensional array of doubles (of size \a numLoc x \a numLoc, 
    //! where \a numLoc := \a maxLoc - \a minLoc + 1).
    //! \note \a minLoc and \a maxLoc can be retrieved using the \a GetLocationstampsRange() method of the Parameters singleton class.
    //!
    //! \return true or false, depending on whether the call is successful.
    //!
    bool GetTransitionMatrix(double** matrix) const;

    //! 
    //! \brief Returns the steady-state vector
    //!
    //! \param[out] vector 	double**, a pointer which will point to the output vector (if the call is successful).
    //!
    //! \note The steady-state vector an array of doubles (of size \a numLoc, where \a numLoc := \a maxLoc - \a minLoc + 1).
    //! \note \a minLoc and \a maxLoc can be retrieved using the \a GetLocationstampsRange() method of the Parameters singleton class.
    //!
    //! \return true or false, depending on whether the call is successful.
    //!
    bool GetSteadyStateVector(double** vector) const;


  private:
    bool GetAccuracyInfo(ull* samples, double** variance);

};

} // namespace lpm
#endif
