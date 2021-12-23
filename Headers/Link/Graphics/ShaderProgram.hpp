#ifndef _LINK_SHADERPROGRAM_HPP
#define _LINK_SHADERPROGRAM_HPP

#include <Link/Common/Types.hpp>
#include <Link/Common/IDisposable.hpp>
#include <Link/Graphics/Shader.hpp>

#include <iostream>

namespace Link::Graphics
{
	class ShaderProgram : public IDisposable
	{
	public:
		ShaderProgram() {}
		
		ShaderProgram(const ShaderProgram&) = delete;
		ShaderProgram(ShaderProgram&&) = delete;
		ShaderProgram& operator=(const ShaderProgram&) = delete;
		ShaderProgram& operator=(ShaderProgram&&) = delete;

		void Init();

		/*
		 * Wraps:
		 * glAttachShader
		*/
		void AttachShader(const Shader& shader);
		/*
		 * Wraps:
		 * glLinkProgram
		*/
		void LinkProgram();

		int GetLinkStatus();
		std::string GetProgramInfoLog();

		void SetUniformInt(std::string name, int value);
		void SetUniformFloat(std::string name, float value);
		void SetUniformDouble(std::string name, double value);
		void SetUniform2f(std::string name, const Vector2f& vec);
		void SetUniform3f(std::string name, const Vector3f& vec);
		void SetUniform4f(std::string name, const Vector4f& vec);
		void SetUniformMatrix4fv(std::string name, float* value);

		/*
		 * Wraps:
		 * glUseProgram
		*/
		void Use();

		uint32_t GetProgram() const;
	protected:
		void OnDispose();
	private:
		uint32_t program = 0;
	};
}

#endif //_LINK_SHADERPROGRAM_HPP