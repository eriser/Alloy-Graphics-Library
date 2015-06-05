/*
 * Copyright(C) 2015, Blake C. Lucas, Ph.D. (img.science@gmail.com)
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
#include "AlloyDrawUtil.h"
namespace aly{
	std::list<float4> NVG_SCISSOR_STACK;
	int pushScissor(NVGcontext* ctx,float x,float y,float w,float h){
		NVG_SCISSOR_STACK.push_back(float4(x,y,w,h));
		nvgScissor(ctx,x,y,w,h);
		return  NVG_SCISSOR_STACK.size();
	}
	int popScissor(NVGcontext* ctx,float x,float y,float w,float h){
		if(NVG_SCISSOR_STACK.size()==0)throw std::runtime_error("Cannot pop an empty scissor stack.");
		float4  back=NVG_SCISSOR_STACK.back();
		NVG_SCISSOR_STACK.pop_back();
		nvgScissor(ctx,back.x,back.y,back.z,back.w);
		return NVG_SCISSOR_STACK.size();
	}

	float drawText(NVGcontext* nvg, float x, float y, const char* txt,const FontStyle& style,const Color& foreground,const Color& background,const char* end){
		float ret;
		const float shift = 1.0f;
		nvgFillColor(nvg, background);
		if(style==FontStyle::Outline){
			nvgText(nvg, x + shift,
					y, txt, nullptr);
			nvgText(nvg, x - shift,
					y, txt, nullptr);
			nvgText(nvg, x,
					y + shift, txt, nullptr);
			nvgText(nvg, x,
					y - shift, txt, nullptr);
			nvgText(nvg, x + shift,
					y - shift, txt, nullptr);
			nvgText(nvg, x + shift,
					y + shift, txt, nullptr);
			nvgText(nvg, x + shift,
					y - shift, txt, nullptr);
			nvgText(nvg, x + shift,
					y + shift, txt, nullptr);
			nvgText(nvg, x - shift,
					y - shift, txt, nullptr);
			nvgText(nvg, x - shift,
					y + shift, txt, nullptr);
			nvgText(nvg, x - shift,
					y - shift, txt, nullptr);
			nvgText(nvg, x - shift,
					y + shift, txt, nullptr);
		} else if(style==FontStyle::Shadow){
			nvgText(nvg, x,y + shift, txt, nullptr);
		} else if(style==FontStyle::Glow){
			throw std::runtime_error("Not supported yet");
		}
		nvgFillColor(nvg,foreground);
		ret=nvgText(nvg, x, y,txt, nullptr);
		return ret;
	}
}



