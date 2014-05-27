#include "ShaderHandler.h"

#include <vector>
#include <assert.h>


std::string ShaderHandler::errorMsg = "";


bool ShaderHandler::CreateShaderProgram(RenderObjHandle & out_handle)
{
	out_handle = glCreateProgram();

	if (out_handle == 0)
	{
		errorMsg = "Couldn't create shader program.";
		return false;
	}

	return true;
}

bool ShaderHandler::CreateShader(RenderObjHandle shaderProgram, RenderObjHandle& out_handle, const Char* shaderText, Shaders shaderType)
{
    GLenum glShaderType;
    switch (shaderType)
    {
        case Shaders::SH_Fragment_Shader: glShaderType =  GL_FRAGMENT_SHADER; break;
        case Shaders::SH_Vertex_Shader: glShaderType = GL_VERTEX_SHADER; break;
        case Shaders::SH_GeometryShader: glShaderType = GL_GEOMETRY_SHADER; break;

        default: assert(false);
    }

    out_handle = glCreateShader(glShaderType);

	if (out_handle == 0)
	{
        errorMsg = "Couldn't create shader type.";
		return false;
	}

	const Char* p[1];
	p[0] = shaderText;
	UniformLocation lengths[1];
	lengths[0] = strlen(shaderText);
	glShaderSource(out_handle, 1, p, lengths);

	glCompileShader(out_handle);

	UniformLocation success;
	glGetShaderiv(out_handle, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		Char infoLog[1024];
		glGetShaderInfoLog(out_handle, sizeof(infoLog), NULL, infoLog);
        errorMsg = infoLog;
		return false;
	}

	glAttachShader(shaderProgram, out_handle);

	return true;
}

bool ShaderHandler::FinalizeShaders(RenderObjHandle shaderProgram, bool validate)
{
    ClearAllRenderingErrors();

	UniformLocation success;
	char error[1024];

	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (success == 0)
	{
		glGetProgramInfoLog(shaderProgram, sizeof(error), NULL, error);
		errorMsg = error;
		return false;
	}

	if (validate)
	{
		glValidateProgram(shaderProgram);
		glGetProgramiv(shaderProgram, GL_VALIDATE_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shaderProgram, sizeof(error), NULL, error);
			errorMsg = error;
			return false;
		}
	}

    errorMsg = GetCurrentRenderingError();
	return errorMsg.empty();
}