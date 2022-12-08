/*
 * IDisposable is an abstract class that is extended when something needs to
 * be disposed when it is destroyed.
 * 
 * It comes with a Dispose function that will fire off the OnDispose virtual
 * function only if the object's initialized field is set to true.
 */

#pragma once

#include <Tether/Common/Defs.hpp>

#define TETHER_DISPOSE_ON_DESTROY(className) \
    ~className() \
    { \
        Dispose(); \
    }

namespace Tether
{
    class TETHER_EXPORT IDisposable
    {
    public:
        /**
         * @returns True if the object is initialized; otherwise, false.
         */
        bool IsInitialized() const;

        /**
         * @brief Disposes the object
         */
        void Dispose();
    protected:
        // This is the function that should be overridden for object disposal.
        virtual void OnDispose() {}

        // This field must be set to true in order for OnDispose to be called.
        // After dispose is called, this field will automatically be set to false.
        bool initialized = false;
    };
}
