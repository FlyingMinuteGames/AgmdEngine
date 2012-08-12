#ifndef GLSHADERPROGRAM_H
#define GLSHADERPROGRAM_H

#include <Config\Fwd.h>
#include <Core\BaseShaderProgram.h>
#include <Renderer\OpenGL\GlShader.h>
#include <AgmdDefines.h>
#include <string>
#include <map>

namespace Agmd
{
	class GLShaderProgram : public BaseShaderProgram
	{
	public:

		enum ParameterType
		{
			PARAMETER_NOFOUND,
			PARAMETER_UNIFORM,
			PARAMETER_ATTRIBUT
		};

		struct Parameter
		{
			uint32 loc;
			uint32 size;
			uint32 typeValue;
			ParameterType type;
		};

		typedef std::map<std::string,Parameter> ParameterMap; 


		GLShaderProgram(uint32 id);
		virtual ~GLShaderProgram();

		virtual void SetParameter(std::string name, float value);
		virtual void SetParameter(std::string name, vec2 value);
		virtual void SetParameter(std::string name, vec3 value);
		virtual void SetParameter(std::string name, vec4 value);

		virtual void SetParameter(std::string name, int value);
		virtual void SetParameter(std::string name, ivec2 value);
		virtual void SetParameter(std::string name, ivec3 value);
		virtual void SetParameter(std::string name, ivec4 value);

		virtual void SetParameter(std::string name, mat2 value);
		virtual void SetParameter(std::string name, mat3 value);
		virtual void SetParameter(std::string name, mat4 value);

		virtual void Use(bool);
	private:
		void SetupShader();
		Parameter GetParameter(std::string name);

		ParameterMap m_UniformMap;
		ParameterMap m_AttributMap;
		uint32 m_id;
	};
}

#endif