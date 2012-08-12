#ifndef SMARTPTR_H
#define SMARTPTR_H

#include <cstdlib>
#include <Utilities/SmartPtrPolicies.h>
#include <Debug/Exception.h>
#include <Debug/New.h>


namespace Agmd
{
    template <class, template <class> class> class SmartPtr;
    template <class T, template <class> class Ownership> T*& GetPtr(SmartPtr<T, Ownership>& Ptr);

    template <class T, template <class> class Ownership = RefCount>
    class SmartPtr : public Ownership<T>
    {
    public :


        SmartPtr();

        SmartPtr(const SmartPtr& sptrcpy);

        SmartPtr(T* ptr);

        ~SmartPtr();

        T& operator * () const;
        T* operator ->() const;

        const SmartPtr& operator =(const SmartPtr& sptr);

        const SmartPtr& operator =(T* ptr);

        operator T*() const;

    private :

        friend T*& GetPtr<T, Ownership>(SmartPtr<T, Ownership>& sptr);

        void Swap(SmartPtr& sptr);

        T* m_Data;
    };
    #include "SmartPtr.inl"
    #include <Debug/NewOff.h>
}

#endif // SMARTPTR_H
