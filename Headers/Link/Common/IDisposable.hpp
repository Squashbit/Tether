/*
 * IDisposable is an abstract class that is extended when something needs to
 * be disposed when it is destroyed.
 * 
 * It comes with a Dispose function that will fire off the OnDispose virtual
 * function only if the object's initialized field is set to true.
 */

#ifndef _LINK_IDISPOSABLE_HPP
#define _LINK_IDISPOSABLE_HPP

namespace Link
{
    class IDisposable
    {
    public:
        // Calls the Dispose function
        ~IDisposable();

        /**
         * @brief Disposes the object
         */
        void Dispose();
    protected:
        // This is the fuction that should be overridden for object disposal.
        virtual void OnDispose() {}

        // This field must be set to true in order for OnDispose to be called.
        // After dispose is called, this field will automatically be set to false.
        bool initialized = false;
    };
}

#endif //_LINK_IDISPOSABLE_HPP