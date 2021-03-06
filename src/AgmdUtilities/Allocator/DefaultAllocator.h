/*
============================================================================
AgmdUtilities - Utilities Lib
Author : Cyril Basset (basset.cyril@gmail.com - https://github.com/Agamand)
https://github.com/Agamand/AgmdEngine
============================================================================
*/

#ifndef _DEFAULTALLOCATOR_H_
#define _DEFAULTALLOCATOR_H_

#include <Allocator/Allocator.h>

namespace AgmdUtilities
{
    template <typename T>
    class DefaultAllocator : public Allocator<T>
    {
    public:
        DefaultAllocator();
        ~DefaultAllocator();

        virtual T* Allocate(size_t count);

        virtual void UnAllocate(T* ptr, bool isArray = false);
    };
    #include "DefaultAllocator.inl"
}


#endif /* _DEFAULTALLOCATOR_H_ */
