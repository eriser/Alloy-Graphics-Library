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

#include "AlloyContext.h"
#include "AlloyMath.h"
#include "GLTexture.h"

namespace aly {
template<class T,int C,ImageType I> GLuint GLImage<T,C,I>::vao=0;
template<class T,int C,ImageType I> GLuint GLImage<T,C,I>::mPositionBuffer=0;
template<class T,int C,ImageType I> GLuint GLImage<T,C,I>::mUVBuffer=0;
template<class T,int C,ImageType I> std::unique_ptr<GLShader> GLImage<T,C,I>::defaultShader;

template<class T,int C,ImageType I> const float2 GLImage<T,C,I>::TextureCoords[4]={float2(1.0f,1.0f),float2(0.0f,1.0f),float2(0.0f,0.0f),float2(1.0f,0.0f)};
template<class T,int C,ImageType I> const float3 GLImage<T,C,I>::PositionCoords[4]={float3(1.0f,1.0f,0.0f),float3(0.0f,1.0f,0.0f),float3(0.0f,0.0f,0.0f),float3(1.0f,0.0f,0.0f)};

} /* namespace imagesci */
