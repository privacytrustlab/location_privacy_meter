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
#ifndef LPM_REFERENCE_H
#define LPM_REFERENCE_H

//!
//! \file
//!
#include "Defs.h"
#include "Memory.h"

namespace lpm {

//!
//! \brief Implements a reference counting framework using templates
//!
//!
//! Base class from which all reference counted classes derive (e.g. Operation, Schedule, TraceSet, Trace, Event etc.).
//! A Reference counted object is a dynamically allocated object (e.g. using the \a new operator) whose resources are deallocated
//! when they are no longer needed (i.e. when no other object maintains a reference to it). 
//!
//! After obtaining a reference to a reference counted object one can use the \a AddRef() method as notification that the object is being used.
//! When the object is no longer used, \a Release() should be called.
//!
//! \note Newly created reference counted objects (e.g. just after a \a new) have \a refCount = 0.
//! \note If \a AddRef() has been called and \a refCount = 1, we say the caller \a owns (exclusively) the object.
//!
//! \see AddRef(), Release()
//!

template<typename T>
class Reference 
{
  private:
    T* referencedObject;

    unsigned long refCount;


  public:
    Reference();

    virtual ~Reference();

    //! 
    //! \brief Increases the reference count (\a refCount) by 1
    //!
    //! \return nothing
    //!
    void AddRef();

    //! 
    //! \brief Decreases the reference count (\a refCount) by 1
    //!
    //! If after decrementing, the reference count is 0, the resources allocated are released (i.e. the \a delete operator is called on the referenced object).
    //!
    //! \note Naturally, if a call causes the object to be released (i.e. its resources are deallocated), the latter should no longer be used.
    //!
    //! \return nothing
    //!
    void Release();

};
template<typename T>
Reference<T>::Reference() 
{
  // Bouml preserved body begin 0001F591

#ifdef UGLY_TRICK
	ll referencePartOffset = (ll)((T*)1) - (ll)(Reference<T>*)((T*)1);
	referencedObject = (T*)((ull)this + referencePartOffset);
#else
	referencedObject = static_cast<T*>(this); // <- should be portable
#endif

	// VERIFY((void*)referencedObject != (void*)this);

	refCount = 1;

	Memory::GetInstance()->UpdateReference(static_cast<void*>(referencedObject), refCount);

  // Bouml preserved body end 0001F591
}

template<typename T>
Reference<T>::~Reference() 
{
  // Bouml preserved body begin 0006BE11

	DEBUG_VERIFY(refCount == 0);
	Memory::GetInstance()->UpdateReference(static_cast<void*>(referencedObject), refCount);

  // Bouml preserved body end 0006BE11
}

//! 
//! \brief Increases the reference count (\a refCount) by 1
//!
//! \return nothing
//!
template<typename T>
void Reference<T>::AddRef() 
{
  // Bouml preserved body begin 0001F611

	DEBUG_VERIFY(refCount > 0);
	refCount++;

	Memory::GetInstance()->UpdateReference(static_cast<void*>(referencedObject), refCount);

  // Bouml preserved body end 0001F611
}

//! 
//! \brief Decreases the reference count (\a refCount) by 1
//!
//! If after decrementing, the reference count is 0, the resources allocated are released (i.e. the \a delete operator is called on the referenced object).
//!
//! \note Naturally, if a call causes the object to be released (i.e. its resources are deallocated), the latter should no longer be used.
//!
//! \return nothing
//!
template<typename T>
void Reference<T>::Release() 
{
  // Bouml preserved body begin 0001F691

	DEBUG_VERIFY(refCount > 0);

	Memory::GetInstance()->UpdateReference(static_cast<void*>(referencedObject), refCount);

	refCount--;
	if(refCount == 0)
	{
		delete referencedObject;
	}

  // Bouml preserved body end 0001F691
}


} // namespace lpm
#endif
