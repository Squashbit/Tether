#ifndef _TETHER_APPLICATION_HPP
#define _TETHER_APPLICATION_HPP

#include <Tether/Common/IDisposable.hpp>
#include <Tether/Common/Defs.hpp>
#include <Tether/Common/Types.hpp>

namespace Tether
{
    namespace Storage
    {
        struct AppVarStorage;
        struct VulkanNative;
    }

    namespace Devices
    {
        class DeviceManager;
    }

    class Application : public IDisposable
    {
        friend class SimpleWindow;
        friend Devices::DeviceManager;
    public:
        TETHER_NO_COPY(Application);
        TETHER_DISPOSE_ON_DESTRUCT(Application);
        Application() {}

        bool Init();

        // Vulkan functions
        bool LoadVulkan();
        bool InitVulkan(bool validationLayers = false);

        Storage::AppVarStorage* GetStorage();
        Storage::VulkanNative* GetVulkanNative();

        bool IsVulkanLoaded();
        bool IsVulkanInitialized();

        static Application& Get();
    protected:
        Storage::AppVarStorage* storage = nullptr;
        Storage::VulkanNative* vulkan = nullptr;
    private:
        bool OnInit();
        void OnAppDispose();
        
        void DisposeVulkan();

        void* LoadLibrary(const char* path);
        void* LoadFunction(void* handle, const char* funcName);
        void LoadLibraries();
        void LoadFunctions();

        void FreeLibrary(void* library);
        void FreeLibraries();

        void CreateKeyLUT();

        void OnDispose();

        static Application internal;
    };
}

#endif //_TETHER_APPLICATION_HPP