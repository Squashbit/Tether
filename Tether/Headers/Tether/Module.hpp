#ifndef _TETHER_MODULE_HPP
#define _TETHER_MODULE_HPP

#include <Tether/Common/Defs.hpp>
#include <Tether/Common/IDisposable.hpp>

namespace Tether
{
	class TETHER_EXPORT Module
	{
		friend class Application;
	public:
		TETHER_NO_COPY(Module);
		/**
		 * @brief
		 * The module constructor. Just initializes this module with the given
		 * information.
		 * 
		 * @param pThisModule The pointer to the child module class. Should just be
		 * "this."
		 * @param moduleName The formal name of the module.
		 */
		Module(Module* pThisModule, const char* moduleName);
		
		const char* GetModuleName();
	protected:
		virtual bool OnInit() { return true; }
		virtual void OnDispose() {}
	private:
		const char* moduleName = nullptr;
	};
}

#endif //_TETHER_MODULE_HPP