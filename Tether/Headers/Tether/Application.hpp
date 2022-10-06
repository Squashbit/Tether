#ifndef _TETHER_APPLICATION_HPP
#define _TETHER_APPLICATION_HPP

#include <Tether/Module.hpp>
#include <Tether/Common/IDisposable.hpp>
#include <Tether/Common/Defs.hpp>
#include <Tether/Common/Types.hpp>

#include <vector>

namespace Tether
{
    namespace Storage
    {
        struct AppVarStorage;
    }

    namespace Devices
    {
        class DeviceManager;
    }

    class TETHER_EXPORT Application : public IDisposable
    {
        friend Module;
        friend class SimpleWindow;
        friend Devices::DeviceManager;
    public:
        TETHER_NO_COPY(Application);
        TETHER_DISPOSE_ON_DESTRUCT(Application);
        Application() {}

        bool Init();

        Storage::AppVarStorage* GetStorage();
        
        static Application& Get();
    protected:
        void RegisterModule(Module* pModule);

        Storage::AppVarStorage* storage = nullptr;
    private:
        bool OnInit();
        void OnAppDispose();
        
        void* LoadLibrary(const char* path);
        void* LoadFunction(void* handle, const char* funcName);
        void LoadLibraries();
        void LoadFunctions();

        void FreeLibrary(void* library);
        void FreeLibraries();

        void CreateKeyLUT();

        void OnDispose();

        std::vector<Module*> modules;

        static Application internal;
    };
}

#endif //_TETHER_APPLICATION_HPP