#include <Link/Graphics/Buffer.hpp>
#include <Link/Graphics/OpenGL.hpp>

using namespace Link::Graphics;

void Buffer::Init(const VAO* pVao)
{
	if (initialized)
		return;
	
	glBindVertexArray(pVao->GetVertexArray());
	glGenBuffers(1, &buffer);

	initialized = true;
}

void Buffer::SetTarget(uint32_t target)
{
	this->target = target;
}

void Buffer::BufferData(uint64_t size, const void* data, 
	uint32_t usage)
{
	if (!initialized)
		return;

	glBindBuffer(target, buffer);
	glBufferData(target, size, data, usage);
}

void Buffer::VertexAttribPointer(uint32_t index, uint32_t size,
	uint32_t type, bool normalized, uint64_t stride, void* pointer)
{
	if (!initialized)
		return;
	
	glBindBuffer(target, buffer);
	glVertexAttribPointer(index, size, type, normalized, stride, pointer);
	glEnableVertexAttribArray(index);
}

uint32_t Buffer::GetBuffer() const
{
	return buffer;
}

void Buffer::OnDispose()
{
	glDeleteBuffers(1, &buffer);
}