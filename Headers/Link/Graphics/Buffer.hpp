#ifndef _LINK_BUFFER_HPP
#define _LINK_BUFFER_HPP

#include <Link/Graphics/VAO.hpp>
#include <Link/Common/IDisposable.hpp>

#include <iostream>

namespace Link::Graphics
{
	class Buffer : public IDisposable
	{
	public:
		Buffer() {}
		
		Buffer(const Buffer&) = delete;
		Buffer(Buffer&&) = delete;
		Buffer& operator=(const Buffer&) = delete;
		Buffer& operator=(Buffer&&) = delete;

		void Init(const VAO* pVao);
		
		/*
		 * Params:
		 * target - The type of buffer. Example: GL_ARRAY_BUFFER
		*/
		void SetTarget(uint32_t target);
		
		/*
		 * Sets the buffer data.
		 *
		 * Params:
		 * size - The size of the data
		 * data - uh.. the data
		 * usage - Example: GL_STATIC_DRAW
		 *
		 * Wraps:
		 * glBindBuffer
		 * glBufferData
		*/
		void BufferData(uint64_t size, const void* data, uint32_t usage);

		/*
		 * Tells OpenGL where to find the data in the buffer.
		 *
		 * Params:
		 * index - The attribute index
		 * size - The size of the attribute
		 * type - The type of the attribute. Example: GL_FLOAT
		 * normalized - I don't fucking know
		 * stride - The space between attributes in bytes
		 * pointer - The offset of where the data begins in the buffer
		*/
		void VertexAttribPointer(uint32_t index, uint32_t size,
			uint32_t type, bool normalized, uint64_t stride, void* pointer);
		
		uint32_t GetBuffer() const;
	protected:
		void OnDispose();
	private:
		uint32_t buffer = 0;
		uint32_t target = 0;

		uint32_t vao = 0;
	};
}

#endif //_LINK_BUFFER_HPP