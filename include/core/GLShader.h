/*
 * Copyright(C) 2014, Blake C. Lucas, Ph.D. (img.science@gmail.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef GLSHADER_H_
#define GLSHADER_H_
#include "AlloyContext.h"
#include "GLTexture.h"
#include "AlloyVirtualCamera.h"
#include <vector>
#include <string>
namespace aly {
class GLShader {
private:
	GLuint mVertexShaderHandle;
	GLuint mFragmentShaderHandle;
	GLuint mGeometryShaderHandle;
	GLuint mProgramHandle;

public:
	// Default constructor.
	GLShader();
	virtual ~GLShader();
	// Initialization function to compile the shader.
	void initialize(const std::vector<std::string>& attributes,
			const std::string& pVertexShaderString,
			const std::string& pFragmentShaderString,
			const std::string& pGeomShaderString);
	void initialize(const std::vector<std::string>& attributes,
			const std::string& pVertexShaderString,
			const std::string& pFragmentShaderString){
		initialize(attributes,pVertexShaderString,pFragmentShaderString,"");
	}

	GLShader(const std::vector<std::string>& attributes,
			const std::string& pVertexShaderString,
			const std::string& pFragmentShaderString,
			const std::string& pGeomShaderString) :
			GLShader() {
		initialize(attributes, pVertexShaderString, pFragmentShaderString,
				pGeomShaderString);
	}
	inline GLShader& set(const std::string& variable, float value) {
		glUniform1f(glGetUniformLocation(mProgramHandle, variable.c_str()),
				value);
		return *this;
	}
	inline GLShader& set(const std::string& variable, int value) {
		glUniform1i(glGetUniformLocation(mProgramHandle, variable.c_str()),
				value);
		return *this;
	}
	inline GLShader& set(const std::string& variable, uint32_t value) {
		glUniform1ui(glGetUniformLocation(mProgramHandle, variable.c_str()),
				value);
		return *this;
	}
	inline GLShader& set(const std::string& variable, float1 value) {
		glUniform1f(glGetUniformLocation(mProgramHandle, variable.c_str()),
				value.x);
		return *this;
	}
	inline GLShader& set(const std::string& variable, int1 value) {
		glUniform1i(glGetUniformLocation(mProgramHandle, variable.c_str()),
				value.x);
		return *this;
	}
	inline GLShader& set(const std::string& variable, uint1 value) {
		glUniform1ui(glGetUniformLocation(mProgramHandle, variable.c_str()),
				value.x);
		return *this;
	}
	inline GLShader& set(const std::string& variable, float2 value) {
		glUniform2f(glGetUniformLocation(mProgramHandle, variable.c_str()),
				value.x, value.y);
		return *this;
	}
	inline GLShader& set(const std::string& variable, int2 value) {
		glUniform2i(glGetUniformLocation(mProgramHandle, variable.c_str()),
				value.x, value.y);
		return *this;
	}
	inline GLShader& set(const std::string& variable, uint2 value) {
		glUniform2ui(glGetUniformLocation(mProgramHandle, variable.c_str()),
				value.x, value.y);
		return *this;
	}

	inline GLShader& set(const std::string& variable, float3 value) {
		glUniform3f(glGetUniformLocation(mProgramHandle, variable.c_str()),
				value.x, value.y, value.z);
		return *this;
	}
	inline GLShader& set(const std::string& variable, int3 value) {
		glUniform3i(glGetUniformLocation(mProgramHandle, variable.c_str()),
				value.x, value.y, value.z);
		return *this;
	}
	inline GLShader& set(const std::string& variable, uint3 value) {
		glUniform3ui(glGetUniformLocation(mProgramHandle, variable.c_str()),
				value.x, value.y, value.z);
		return *this;
	}

	inline GLShader& set(const std::string& variable, float4 value) {
		glUniform4f(glGetUniformLocation(mProgramHandle, variable.c_str()),
				value.x, value.y, value.z, value.w);
		return *this;
	}
	inline GLShader& set(const std::string& variable, int4 value) {
		glUniform4i(glGetUniformLocation(mProgramHandle, variable.c_str()),
				value.x, value.y, value.z, value.w);
		return *this;
	}
	inline GLShader& set(const std::string& variable, uint4 value) {
		glUniform4ui(glGetUniformLocation(mProgramHandle, variable.c_str()),
				value.x, value.y, value.z, value.w);
		return *this;
	}

	inline GLShader& set(const std::string& variable, const float4x4& value) {
		glUniformMatrix4fv(
		glGetUniformLocation(mProgramHandle, variable.c_str()), 1, false,
				value.ptr());
		return *this;
	}
	inline GLShader& set(const std::string& variable, const float3x3& value) {
		glUniformMatrix3fv(
		glGetUniformLocation(mProgramHandle, variable.c_str()), 1, false,
				value.ptr());
		return *this;
	}
	inline GLShader& set(const std::string& variable, const float2x2& value) {
		glUniformMatrix2fv(
		glGetUniformLocation(mProgramHandle, variable.c_str()), 1, false,
				value.ptr());
		return *this;
	}
	inline GLShader& set(VirtualCamera& camera,const box2px& bounds){
		camera.aim(bounds);
	    set("ProjMat",camera.mProjection);
	    set("ViewMat",camera.mView);
	    set("ModelMat",camera.mModel);
		return *this;
	}
	inline GLShader& set(const std::string& variable, const box2f& value) {
		glUniform4f(glGetUniformLocation(mProgramHandle, variable.c_str()),value.position.x, value.position.y, value.dimensions.x, value.dimensions.y);
		return *this;
	}
	inline GLShader& set(const std::string& variable, const box2i& value) {
		glUniform4i(glGetUniformLocation(mProgramHandle, variable.c_str()),value.position.x, value.position.y, value.dimensions.x, value.dimensions.y);
		return *this;
	}
	template<class T, int C, ImageType I> void set(const std::string& variable,
			const GLTexture<T, C, I>& value, int id) {
		glUniform1i(glGetUniformLocation(mProgramHandle, variable.c_str()), id);
		glActiveTexture(GL_TEXTURE0 + id);
		glBindTexture(GL_TEXTURE_2D, value.textureId);
	}
	virtual GLShader& begin();
	virtual void end();
	inline GLuint GetProgramHandle() const {
		return mProgramHandle;
	}
};

}
#endif /* SHADER_H_ */
