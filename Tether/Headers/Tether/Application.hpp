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
        TETHER_DISPOSE_ON_DESTRUCT(Application);
        Application() {}

        bool Init();

        Storage::AppVarStorage* GetStorage();
        std::vector<Module*>* GetModules();
        
        static Application& Get();
    protected:
        void RegisterModule(Module* pModule);

        Storage::AppVarStorage* storage = nullptr;
    private:
        bool OnInit();
        void OnAppDispose();

		void LoadLibraries();
		void LoadFunctions();
        void FreeLibraries();

        void CreateKeyLUT();

        void OnDispose();

        std::vector<Module*> modules;

        static Application internal;
    };
}

#endif //_TETHER_APPLICATION_HPP