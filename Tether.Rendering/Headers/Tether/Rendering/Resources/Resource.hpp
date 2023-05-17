#pragma once

#include <Tether/Common/Defs.hpp>

namespace Tether::Rendering
{
	class Renderer;
}

namespace Tether::Rendering::Resources
{
	class TETHER_EXPORT Resource
	{
		friend Renderer;
	public:
		virtual ~Resource() = default;
		TETHER_NO_COPY(Resource);

		Renderer* GetRenderer();
	protected:
		Resource() = default;

		Renderer* m_pRenderer = nullptr;
	};
}