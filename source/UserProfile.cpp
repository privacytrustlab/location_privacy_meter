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
#include "../include/UserProfile.h"

namespace lpm {

UserProfile::UserProfile(ull u) 
{
  // Bouml preserved body begin 00045F11

	user = u;
	steadystateVector = NULL;
	transitionMatrix = NULL;

  // Bouml preserved body end 00045F11
}

UserProfile::~UserProfile() 
{
  // Bouml preserved body begin 00049311

	if(steadystateVector != NULL) { Free(steadystateVector); }
	if(transitionMatrix != NULL) { Free(transitionMatrix); }

  // Bouml preserved body end 00049311
}

ull UserProfile::GetUser() const 
{
  // Bouml preserved body begin 00045E91

	return user;

  // Bouml preserved body end 00045E91
}

bool UserProfile::SetTransitionMatrix(const double * matrix) 
{
  // Bouml preserved body begin 00045F91

	if(matrix == NULL) { return false; }
	// TODO: Add more checks

	transitionMatrix = (double*)matrix;

	return true;

  // Bouml preserved body end 00045F91
}

bool UserProfile::SetSteadyStateVector(const double* vector) 
{
  // Bouml preserved body begin 00046011

	if(vector == NULL) { return false; }
	// TODO: Add more checks

	steadystateVector = (double*)vector;

	return true;

  // Bouml preserved body end 00046011
}

bool UserProfile::SetAccuracyInfo(ull samples, const double* variance) 
{
  // Bouml preserved body begin 000C0311

	if(variance == NULL) { return false; }

	numSamples = samples;
	varianceMatrix = (double*)variance;

	return true;

  // Bouml preserved body end 000C0311
}

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
bool UserProfile::GetTransitionMatrix(double** matrix) const 
{
  // Bouml preserved body begin 00049391

	if(matrix == NULL) { return false; }

	*matrix = transitionMatrix;

	return true;

  // Bouml preserved body end 00049391
}

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
bool UserProfile::GetSteadyStateVector(double** vector) const 
{
  // Bouml preserved body begin 00049411

	if(vector == NULL) { return false; }

	*vector = steadystateVector;

	return true;

  // Bouml preserved body end 00049411
}

bool UserProfile::GetAccuracyInfo(ull* samples, double** variance) 
{
  // Bouml preserved body begin 000C0391

	if(samples == NULL || variance == NULL) { return false; }

	*samples = numSamples;
	*variance = varianceMatrix;

	return true;

  // Bouml preserved body end 000C0391
}


} // namespace lpm
