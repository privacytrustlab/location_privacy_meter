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
#include "../include/Context.h"
#include "../include/UserProfile.h"

namespace lpm {

Context::Context() 
{
  // Bouml preserved body begin 0002D191

	userProfiles = map<ull, UserProfile*>();

  // Bouml preserved body end 0002D191
}

Context::~Context() 
{
  // Bouml preserved body begin 0002D211

	ClearProfiles();

  // Bouml preserved body end 0002D211
}

bool Context::AddProfile(const UserProfile* profile) 
{
  // Bouml preserved body begin 00045D11

	const_cast<UserProfile*>(profile)->AddRef();

	userProfiles.insert(pair<ull, UserProfile*>(profile->GetUser(), const_cast<UserProfile*>(profile)));

	return true;

  // Bouml preserved body end 00045D11
}

bool Context::GetProfiles(map<ull, UserProfile*>& profiles) const 
{
  // Bouml preserved body begin 00045D91

	profiles = userProfiles;

	return true;

  // Bouml preserved body end 00045D91
}

bool Context::GetUserProfile(ull user, UserProfile** profile) const 
{
  // Bouml preserved body begin 0004ED91

	if(profile == NULL) { return false; }

	map<ull, UserProfile*>::const_iterator iter = userProfiles.find(user);
	if(iter != userProfiles.end()) // if we find the user profile
	{
		*profile = iter->second;

		return true;
	}

	return false;

  // Bouml preserved body end 0004ED91
}

bool Context::RemoveUserProfile(ull user) 
{
  // Bouml preserved body begin 0008F591

	map<ull, UserProfile*>::iterator iter = userProfiles.find(user);
	if(iter != userProfiles.end()) // if we find the user profile
	{
		UserProfile* profile = iter->second;
		profile->Release();

		userProfiles.erase(iter);

		return true;
	}

	return false;

  // Bouml preserved body end 0008F591
}

void Context::ClearProfiles() 
{
  // Bouml preserved body begin 00085411

	pair_foreach_const(map<ull, UserProfile*>, userProfiles, iter)
	{
		UserProfile* profile = iter->second;
		profile->Release();
	}

	userProfiles.clear();

  // Bouml preserved body end 00085411
}

ContextFactory::ContextFactory() 
{
  // Bouml preserved body begin 00096611
  // Bouml preserved body end 00096611
}

ContextFactory::~ContextFactory() 
{
  // Bouml preserved body begin 00096691
  // Bouml preserved body end 00096691
}

Context* ContextFactory::NewContext() const 
{
  // Bouml preserved body begin 00096511

	return new Context(); // default implementation

  // Bouml preserved body end 00096511
}


} // namespace lpm
