#include <Tether/Rendering/Elements/ImageView.hpp>

namespace Tether::Rendering::Elements
{
	ImageView::ImageView(WindowUIManager& windowUI)
		:
		Element(windowUI)
	{}

	void ImageView::SetImage(Resources::BufferedImage& image)
	{
		m_pImage = &image;
	}

	void ImageView::SetImage(Resources::BufferedImage* pImage)
	{
		m_pImage = pImage;
	}

	void ImageView::OnRender(Renderer& renderer)
	{
		if (!m_pImage)
			return;

		renderer.DrawImage(m_X, m_Y, m_Width, m_Height, *m_pImage);
	}
}