#ifndef _LINK_TEXTURE_HPP
#define _LINK_TEXTURE_HPP

#include <iostream>

#include <Link/Common/IDisposable.hpp>

namespace Link::Graphics
{
	class Texture2D : public IDisposable
	{
	public:
		Texture2D() {}
		~Texture2D();

		Texture2D(const Texture2D&) = delete;
		Texture2D(Texture2D&&) = delete;
		Texture2D& operator=(const Texture2D&) = delete;
		Texture2D& operator=(Texture2D&&) = delete;

		bool Load(uint8_t* data, uint64_t width, uint64_t height, 
			int channels);
		bool Load(std::string filePath);

        void TexParameteri(uint32_t pname, int param);

		void Bind(uint8_t activeTexture);
	protected:
		void OnDispose();
    private:
        uint32_t texture = 0;
	};
}

#endif //_LINK_TEXTURE_HPP