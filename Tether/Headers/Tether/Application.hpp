#pragma once

#include <Tether/Module.hpp>
#include <Tether/Common/IDisposable.hpp>
#include <Tether/Common/Defs.hpp>
#include <Tether/Common/Types.hpp>
#include <Tether/Common/Logger.hpp>

#include <vector>
#include <optional>

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
        Application();
        
        Storage::AppVarStorage* GetStorage();
        std::vector<Module*>* GetModules();
        Logger& GetLogger();

        static Application& Get();
        static void DisposeApplication();
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
        Logger logger;

        static Application internal;
    };
}
