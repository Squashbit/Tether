#include <Link/Graphics/ShaderProgram.hpp>
#include <Link/Graphics/OpenGL.hpp>

using namespace Link::Graphics;

void ShaderProgram::Init()
{
	if (initialized)
		return;
	
	program = glCreateProgram();

	initialized = true;
}

void ShaderProgram::AttachShader(const Shader& shader)
{
	glAttachShader(program, shader.GetShader());
}

void ShaderProgram::LinkProgram()
{
	glLinkProgram(program);
}

int ShaderProgram::GetLinkStatus()
{
	int status = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &status);

	return status;
}

std::string ShaderProgram::GetProgramInfoLog()
{
	char programInfoLog[512];
	glGetProgramInfoLog(program, 512, NULL, programInfoLog);

	return std::string(programInfoLog);
}

void ShaderProgram::SetUniformInt(std::string name, int value)
{
	int uniformLocation = glGetUniformLocation(program, name.c_str());
	glUniform1i(uniformLocation, value);
}

void ShaderProgram::SetUniformFloat(std::string name, float value)
{
	int uniformLocation = glGetUniformLocation(program, name.c_str());
	glUniform1f(uniformLocation, value);
}

void ShaderProgram::SetUniformDouble(std::string name, double value)
{
	int uniformLocation = glGetUniformLocation(program, name.c_str());
	glUniform1d(uniformLocation, value);
}

void ShaderProgram::SetUniform2f(std::string name, const Vector2f& value)
{
	int uniformLocation = glGetUniformLocation(program, name.c_str());
	glUniform2f(uniformLocation, value.x, value.y);
}

void ShaderProgram::SetUniform3f(std::string name, const Vector3f& value)
{
	int uniformLocation = glGetUniformLocation(program, name.c_str());
	glUniform3f(uniformLocation, value.x, value.y, value.z);
}

void ShaderProgram::SetUniform4f(std::string name, const Vector4f& value)
{
	int uniformLocation = glGetUniformLocation(program, name.c_str());
	glUniform4f(uniformLocation, value.x, value.y, value.z, value.w);
}

void ShaderProgram::SetUniformMatrix4fv(std::string name, float* value)
{
	int uniformLocation = glGetUniformLocation(program, name.c_str());
	glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, value);
}

void ShaderProgram::Use()
{
	glUseProgram(program);
}

uint32_t ShaderProgram::GetProgram() const
{
	return program;
}

void ShaderProgram::OnDispose()
{
	glDeleteProgram(program);
}
