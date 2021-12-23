#include <Link/Graphics/Shader.hpp>
#include <Link/Graphics/OpenGL.hpp>

using namespace Link::Graphics;

void Shader::Create(uint32_t type)
{
	if (initialized)
		return;
	
	shader = glCreateShader(type);

	initialized = true;
}

void Shader::ShaderSource(std::string source)
{
	const char* sourceCstr = source.c_str();
	glShaderSource(shader, 1, &sourceCstr, NULL);
}

void Shader::CompileShader()
{
	glCompileShader(shader);
}

int Shader::GetCompileStatus()
{
	int status = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

	return status;
}

std::string Shader::GetInfoLog()
{
	char shaderInfoLog[512];
	glGetShaderInfoLog(shader, 512, NULL, shaderInfoLog);

	return std::string(shaderInfoLog);
}

uint32_t Shader::GetShader() const
{
	return shader;
}

void Shader::OnDispose()
{
	glDeleteShader(shader);
}