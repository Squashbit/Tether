#pragma once

#include <Tether/Common/Defs.hpp>
#include <Tether/Common/Ref.hpp>
#include <Tether/Common/Types.hpp>

#include <vector>
#include <optional>

namespace Tether
{
	namespace Storage
	{
		struct AppVarStorage;
	}

	class TETHER_EXPORT Application
    {
    public:
        ~Application();
        TETHER_NO_COPY(Application);
        
        Storage::AppVarStorage* GetStorage();
        int16_t*const GetKeycodes();
        int16_t*const GetScancodes();
        
        static Application& Get();
    private:
        Application();

        bool OnInit();
        void OnAppDispose();

		void LoadLibraries();
		void LoadFunctions();
        void FreeLibraries();

        void CreateKeyLUT();

		int16_t keycodes[512];
		int16_t scancodes[Keycodes::KEY_LAST + 1];

        Scope<Storage::AppVarStorage> storage;

        inline static std::unique_ptr<Application> internal = nullptr;
    };
}
