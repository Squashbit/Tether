#ifndef _LINK_SIMPLESHADER_HPP
#define _LINK_SIMPLESHADER_HPP

#include <Link/Common/Types.hpp>
#include <Link/Common/IDisposable.hpp>
#include <Link/Graphics/ShaderProgram.hpp>

#include <iostream>

namespace Link::Graphics
{
    class SimpleShader : public IDisposable
    {
    public:
		SimpleShader() {}
		
		SimpleShader(const SimpleShader&) = delete;
		SimpleShader(SimpleShader&&) = delete;
		SimpleShader& operator=(const SimpleShader&) = delete;
		SimpleShader& operator=(SimpleShader&&) = delete;

		void Init();
		bool LoadVertex(std::string path);
		bool LoadFragment(std::string path);
		bool Link();

        void SetUniformInt(std::string name, int value);
		void SetUniformFloat(std::string name, float value);
		void SetUniformDouble(std::string name, double value);
		void SetUniform2f(std::string name, const Vector2f& vec);
		void SetUniform3f(std::string name, const Vector3f& vec);
		void SetUniform4f(std::string name, const Vector4f& vec);
		void SetUniformMatrix4fv(std::string name, float* value);

		void Use();

		uint32_t GetProgram();
	protected:
		void OnDispose();
	private:
		ShaderProgram shaderProgram;
    };
}

#endif //_LINK_SIMPLESHADER_HPP