#ifndef _LINK_VAO_HPP
#define _LINK_VAO_HPP

#include <iostream>

#include <Link/Common/IDisposable.hpp>

namespace Link::Graphics
{
	class VAO : public IDisposable
	{
	public:
		VAO() {}
        
		void Init();

        VAO(const VAO&) = delete;
		VAO(VAO&&) = delete;
		VAO& operator=(const VAO&) = delete;
		VAO& operator=(VAO&&) = delete;

        void Bind();

		uint32_t GetVertexArray() const;
	protected:
		void OnDispose();
	private:
		uint32_t vao = 0;
	};
}

#endif //_LINK_VAO_HPP