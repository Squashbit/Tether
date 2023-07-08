#pragma once

#include <Tether/Rendering/Elements/Element.hpp>

namespace Tether::Rendering::Elements
{
	class TETHER_EXPORT ImageView : public Element
	{
	public:
		ImageView(WindowUIManager& windowUI);

		void SetImage(Resources::BufferedImage& image);
		void SetImage(Resources::BufferedImage* pImage);
	private:
		void OnRender(Renderer& renderer) override;

		Resources::BufferedImage* m_pImage = nullptr;
	};
}