#pragma once

#include <Tether/Common/IDisposable.hpp>
#include <Tether/Module/Rendering/Common/Defs.hpp>

#include <vector>

namespace Tether::Rendering::Objects
{
	class TETHER_EXPORT Object : public IDisposable
	{
	public:
		Object() = default;
		TETHER_DISPOSE_ON_DESTROY(Object);
		TETHER_NO_COPY(Object);
	};
}