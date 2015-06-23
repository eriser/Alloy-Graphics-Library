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
class Mesh;
class GLShader {
protected:
	GLuint mVertexShaderHandle;
	GLuint mFragmentShaderHandle;
	GLuint mGeometryShaderHandle;
	GLuint mProgramHandle;
	bool shaderEnabled = false;
	std::shared_ptr<AlloyContext> context;
	void enableCheck() {
		if (!shaderEnabled)
			throw std::runtime_error(
					"Could not set shader parameter because shader is not enabled. Call shader.begin() first.");
	}
public:
	std::shared_ptr<AlloyContext>& getContext() {
		return context;
	}
	// Default constructor.
	GLShader(std::shared_ptr<AlloyContext>& context=AlloyDefaultContext());
	GLShader();
	virtual ~GLShader();
	// Initialization function to compile the shader.
	void initialize(const std::vector<std::string>& attributes,
			const std::string& pVertexShaderString,
			const std::string& pFragmentShaderString,
			const std::string& pGeomShaderString);
	void initialize(const std::vector<std::string>& attributes,
			const std::string& pVertexShaderString,
			const std::string& pFragmentShaderString) {
		initialize(attributes, pVertexShaderString, pFragmentShaderString, "");
	}

	GLShader(std::shared_ptr<AlloyContext>& context,
			const std::vector<std::string>& attributes,
			const std::string& pVertexShaderString,
			const std::string& pFragmentShaderString,
			const std::string& pGeomShaderString) :
			GLShader(context) {
		initialize(attributes, pVertexShaderString, pFragmentShaderString,
				pGeomShaderString);
	}
	inline GLShader& set(const std::string& variable, float value) {
		enableCheck();
		glUniform1f(glGetUniformLocation(mProgramHandle, variable.c_str()),
				value);
		return *this;
	}
	inline GLShader& set(const std::string& variable, int value) {
		enableCheck();
		glUniform1i(glGetUniformLocation(mProgramHandle, variable.c_str()),
				value);
		return *this;
	}
	inline GLShader& set(const std::string& variable, uint32_t value) {
		enableCheck();
		glUniform1ui(glGetUniformLocation(mProgramHandle, variable.c_str()),
				value);
		return *this;
	}
	inline GLShader& set(const std::string& variable, float1 value) {
		enableCheck();
		glUniform1f(glGetUniformLocation(mProgramHandle, variable.c_str()),
				value.x);
		return *this;
	}
	inline GLShader& set(const std::string& variable, int1 value) {
		enableCheck();
		glUniform1i(glGetUniformLocation(mProgramHandle, variable.c_str()),
				value.x);
		return *this;
	}
	inline GLShader& set(const std::string& variable, uint1 value) {
		enableCheck();
		glUniform1ui(glGetUniformLocation(mProgramHandle, variable.c_str()),
				value.x);
		return *this;
	}
	inline GLShader& set(const std::string& variable, float2 value) {
		enableCheck();
		glUniform2f(glGetUniformLocation(mProgramHandle, variable.c_str()),
				value.x, value.y);
		return *this;
	}
	inline GLShader& set(const std::string& variable, int2 value) {
		enableCheck();
		glUniform2i(glGetUniformLocation(mProgramHandle, variable.c_str()),
				value.x, value.y);
		return *this;
	}
	inline GLShader& set(const std::string& variable, uint2 value) {
		enableCheck();
		glUniform2ui(glGetUniformLocation(mProgramHandle, variable.c_str()),
				value.x, value.y);
		return *this;
	}

	inline GLShader& set(const std::string& variable, float3 value) {
		enableCheck();
		glUniform3f(glGetUniformLocation(mProgramHandle, variable.c_str()),
				value.x, value.y, value.z);
		return *this;
	}
	inline GLShader& set(const std::string& variable, int3 value) {
		enableCheck();
		glUniform3i(glGetUniformLocation(mProgramHandle, variable.c_str()),
				value.x, value.y, value.z);
		return *this;
	}
	inline GLShader& set(const std::string& variable, uint3 value) {
		enableCheck();
		glUniform3ui(glGetUniformLocation(mProgramHandle, variable.c_str()),
				value.x, value.y, value.z);
		return *this;
	}

	inline GLShader& set(const std::string& variable, float4 value) {
		enableCheck();
		glUniform4f(glGetUniformLocation(mProgramHandle, variable.c_str()),
				value.x, value.y, value.z, value.w);
		return *this;
	}
	inline GLShader& set(const std::string& variable, int4 value) {
		enableCheck();
		glUniform4i(glGetUniformLocation(mProgramHandle, variable.c_str()),
				value.x, value.y, value.z, value.w);
		return *this;
	}
	inline GLShader& set(const std::string& variable, uint4 value) {
		enableCheck();
		glUniform4ui(glGetUniformLocation(mProgramHandle, variable.c_str()),
				value.x, value.y, value.z, value.w);
		return *this;
	}

	inline GLShader& set(const std::string& variable, const float4x4& value) {
		enableCheck();
		glUniformMatrix4fv(
		glGetUniformLocation(mProgramHandle, variable.c_str()), 1, false,
				value.ptr());
		return *this;
	}
	inline GLShader& set(const std::string& variable, const float3x3& value) {
		enableCheck();
		glUniformMatrix3fv(
		glGetUniformLocation(mProgramHandle, variable.c_str()), 1, false,
				value.ptr());
		return *this;
	}
	inline GLShader& set(const std::string& variable, const float2x2& value) {
		enableCheck();
		glUniformMatrix2fv(
		glGetUniformLocation(mProgramHandle, variable.c_str()), 1, false,
				value.ptr());
		return *this;
	}
	inline GLShader& set(const std::string& variable, const aly::Color& value) {
		enableCheck();
		glUniform4f(glGetUniformLocation(mProgramHandle, variable.c_str()),
				value.r, value.g, value.b, value.a);
		return *this;
	}
	inline GLShader& set(VirtualCamera& camera, const box2px& bounds) {
		enableCheck();
		camera.aim(bounds);
		set("ProjMat", camera.mProjection);
		set("ViewMat", camera.mView);
		set("ModelMat", camera.mModel);
		set("ViewModelMat", camera.mViewModel);
		set("NormalMat", camera.mNormal);

		return *this;
	}
	inline GLShader& set(const std::string& variable, const box2f& value) {
		enableCheck();
		glUniform4f(glGetUniformLocation(mProgramHandle, variable.c_str()),
				value.position.x, value.position.y, value.dimensions.x,
				value.dimensions.y);
		return *this;
	}
	inline GLShader& set(const std::string& variable, const box2i& value) {
		enableCheck();
		glUniform4i(glGetUniformLocation(mProgramHandle, variable.c_str()),
				value.position.x, value.position.y, value.dimensions.x,
				value.dimensions.y);
		return *this;
	}
	template<class T, int C, ImageType I> GLShader& set(
			const std::string& variable, const GLTexture<T, C, I>& value,
			int id) {
		enableCheck();
		glUniform1i(glGetUniformLocation(mProgramHandle, variable.c_str()), id);
		glActiveTexture(GL_TEXTURE0 + id);
		glBindTexture(GL_TEXTURE_2D, value.textureId);
		return *this;
	}

	GLShader& begin();
	GLShader& draw(const GLComponent& comp);
	GLShader& draw(const Mesh& mesh);

	void end();
	inline GLuint GetProgramHandle() const {
		return mProgramHandle;
	}
};

}
#endif /* SHADER_H_ */
