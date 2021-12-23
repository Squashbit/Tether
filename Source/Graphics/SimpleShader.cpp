#include <Link/Graphics/SimpleShader.hpp>
#include <Link/Graphics/OpenGL.hpp>
#include <Link/Common/FileLoader.hpp>

using namespace Link::Graphics;

void SimpleShader::Init()
{
	if (initialized)
		return;
	
    shaderProgram.Init();

	initialized = true;
}

bool SimpleShader::LoadVertex(std::string path)
{
    Graphics::Shader shader;
	
	std::string shaderSource;
	if (!FileLoader::LoadFromFile(
		path, &shaderSource))
		return false;
			
	shader.Create(GL_VERTEX_SHADER);
	shader.ShaderSource(shaderSource);
	shader.CompileShader();
			
	if (!shader.GetCompileStatus())
		return false;

    shaderProgram.AttachShader(shader);

    return true;
}

bool SimpleShader::LoadFragment(std::string path)
{
    Graphics::Shader shader;
	
	std::string shaderSource;
	if (!FileLoader::LoadFromFile(
		path, &shaderSource))
		return false;
			
	shader.Create(GL_FRAGMENT_SHADER);
	shader.ShaderSource(shaderSource);
	shader.CompileShader();
			
	if (!shader.GetCompileStatus())
		return false;

    shaderProgram.AttachShader(shader);

    return true;
}

bool SimpleShader::Link()
{
    shaderProgram.LinkProgram();
	if (!shaderProgram.GetLinkStatus())
		return false;

    return true;
}

void SimpleShader::SetUniformInt(std::string name, int value)
{
    shaderProgram.SetUniformInt(name, value);
}

void SimpleShader::SetUniformFloat(std::string name, float value)
{
    shaderProgram.SetUniformFloat(name, value);
}

void SimpleShader::SetUniformDouble(std::string name, double value)
{
    shaderProgram.SetUniformDouble(name, value);
}

void SimpleShader::SetUniform2f(std::string name, const Vector2f& value)
{
    shaderProgram.SetUniform2f(name, value);
}

void SimpleShader::SetUniform3f(std::string name, const Vector3f& value)
{
    shaderProgram.SetUniform3f(name, value);
}

void SimpleShader::SetUniform4f(std::string name, const Vector4f& value)
{
    shaderProgram.SetUniform4f(name, value);
}

void SimpleShader::SetUniformMatrix4fv(std::string name, float* value)
{
    shaderProgram.SetUniformMatrix4fv(name, value);
}

void SimpleShader::Use()
{
    shaderProgram.Use();
}

uint32_t SimpleShader::GetProgram()
{
    return shaderProgram.GetProgram();
}

void SimpleShader::OnDispose()
{
	shaderProgram.Dispose();
}