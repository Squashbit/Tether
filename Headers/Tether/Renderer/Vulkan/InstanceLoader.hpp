#ifndef _TETHER_INSTANCELOADER_HPP
#define _TETHER_INSTANCELOADER_HPP

#include <Tether/Common/Defs.hpp>
#include <Tether/Common/IDisposable.hpp>

namespace Tether::Vulkan
{
	class InstanceLoader : public IDisposable
	{
	public:
		InstanceLoader() = default;
		TETHER_DISPOSE_ON_DESTRUCT(InstanceLoader);
		TETHER_NO_COPY(InstanceLoader);

		bool Load(VkInstance* pInstance);
	private:
		void OnDispose();
	};
}

#endif //_TETHER_INSTANCELOADER_HPP