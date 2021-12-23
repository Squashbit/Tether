#include <Link/Graphics/VAO.hpp>
#include <Link/Graphics/OpenGL.hpp>

using namespace Link::Graphics;

void VAO::Init()
{
	if (initialized)
		return;
	
	glGenVertexArrays(1, &vao);

	initialized = true;
}

void VAO::Bind()
{
	if (!initialized)
		return;
	
	glBindVertexArray(vao);
}

uint32_t VAO::GetVertexArray() const
{
	return vao;
}

void VAO::OnDispose()
{
	glDeleteVertexArrays(1, &vao);
}