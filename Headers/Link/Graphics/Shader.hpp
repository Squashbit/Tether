#ifndef _LINK_SHADER_HPP
#define _LINK_SHADER_HPP

#include <iostream>
#include <Link/Common/IDisposable.hpp>

namespace Link::Graphics
{
	class Shader : public IDisposable
	{
	public:
		Shader() {}
		~Shader();

		Shader(const Shader&) = delete;
		Shader(Shader&&) = delete;
		Shader& operator=(const Shader&) = delete;
		Shader& operator=(Shader&&) = delete;
		
		/*
		 * Creates the shader with the specified type.
		 *
		 * Params:
		 * type - The type of the shader. Example: GL_VERTEX_SHADER
		 * 
		 * Wraps:
		 * glCompileShader
		*/
		void Create(uint32_t type);

		/*
		 * Sets the source of the shader
		 *
		 * Params:
		 * source - The shader source as a string
		 * 
		 * Wraps:
		 * glShaderSource
		*/
		void ShaderSource(std::string source);
		/*
		 * Wraps:
		 * glCompileShader
		*/
		void CompileShader();

		/*
		 * Wraps:
		 * glGetShaderiv
		*/
		int GetCompileStatus();

		/*
		 * Wraps:
		 * glGetShaderInfoLog
		*/
		std::string GetInfoLog();

		uint32_t GetShader() const;
	protected:
		void OnDispose();
	private:
		uint32_t shader = 0;
		std::string infoLog;
	};
}

#endif //_LINK_SHADER_HPP