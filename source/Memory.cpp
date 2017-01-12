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
#include "../include/Memory.h"

namespace lpm {

Memory::Memory() 
{
  // Bouml preserved body begin 00090F11

	references = map<void*, ull>();
	chunks = map<void*, string>();

  // Bouml preserved body end 00090F11
}

Memory::~Memory() 
{
  // Bouml preserved body begin 00090F91

	Report();

	references.clear();
	chunks.clear();

  // Bouml preserved body end 00090F91
}

void* Memory::AllocateChunk(ull bytes, const char* file, int line) 
{
  // Bouml preserved body begin 00091011

	stringstream ss("");
	ss << (size_t)bytes << " bytes allocated in "<< file << ":" << line;

	string details = ss.str();

	if(((size_t)bytes) != bytes) // overflow on size_t
	{
		SET_ERROR_CODE(ERROR_CODE_SIZE_T_OVERFLOW);
	}

	void* ret = malloc(bytes);

	if(ret == NULL)
	{
		SET_ERROR_CODE(ERROR_CODE_MEMORY_ALLOCATION_FAILURE);
	}
	else
	{
		// register the allocation
		chunks.insert(pair<void*, string>(ret, details));
	}

	return ret;

  // Bouml preserved body end 00091011
}

void Memory::FreeChunk(void* chunk) 
{
  // Bouml preserved body begin 00091111

	stringstream ss("");
	ull pointer = (ull)chunk;

	map<void*, string>::iterator iter = chunks.find(chunk);
	if(iter != chunks.end()) // found entry
	{
		string details = iter->second;

		/* ss << "Freeing memory at 0x" << hex << pointer << " (" << details << ")!";
		Log::GetInstance()->Append(ss.str()); */

		chunks.erase(iter);
	}
	else // not found
	{
		ss << "Attempting to Free possibly unallocated memory at 0x" << hex << pointer << "!";
		Log::GetInstance()->Append(ss.str(), Log::warningLevel);
	}

	free(chunk);

  // Bouml preserved body end 00091111
}

void Memory::UpdateReference(void* object, ull newCount) 
{
  // Bouml preserved body begin 00091091

	//ull pointer = (ull)object;

	map<void*, ull>::iterator iter = references.find(object);
	if(iter != references.end()) // found entry
	{
		if(newCount == 0)
		{
			references.erase(iter);

			/* stringstream ss("");
			ss << "Releasing reference to object at 0x" << hex << pointer << "!";
			Log::GetInstance()->Append(ss.str()); */
		}
		else
		{
			iter->second = newCount;
		}
	}
	else // not found
	{
		VERIFY(newCount == 1);

		references.insert(pair<void*, ull>(object, newCount));

		/* stringstream ss("");
		ss << "Adding references to object at 0x" << hex << pointer << "!";
		Log::GetInstance()->Append(ss.str()); */

		VERIFY(newCount == 1);
	}

  // Bouml preserved body end 00091091
}

void Memory::Report() 
{
  // Bouml preserved body begin 00092A91

	stringstream ss("");

	ull refs = references.size();
	ull chunksCount = chunks.size();

	if(refs == 0 && chunksCount == 0)
	{
		ss << "All references and chunks were Freed: no memory leaks found!";
		Log::GetInstance()->Append(ss.str());
	}
	else
	{
		ss << "Memory leaks found: " << refs << " references, " << chunksCount << " chunks not Freed!";
		Log::GetInstance()->Append(ss.str(), Log::warningLevel);

		pair_foreach_const(map<void*, ull>, references, iter)
		{
			ull pointer = (ull)iter->first;
			ull refCount = iter->second;

			ss.str("");
			ss << "Reference at 0x" << hex << pointer << " was not released (refCount = " << dec << refCount << ")!";
			Log::GetInstance()->Append(ss.str(), Log::warningLevel);
		}

		pair_foreach_const(map<void*, string>, chunks, iter)
		{
			ull pointer = (ull)iter->first;
			string details = iter->second;

			ss.str("");
			ss << "Memory at 0x" << hex << pointer << " was not released (" << details << ")!";
			Log::GetInstance()->Append(ss.str(), Log::warningLevel);
		}
	}

  // Bouml preserved body end 00092A91
}


} // namespace lpm
