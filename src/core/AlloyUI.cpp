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
#include "AlloyUI.h"
namespace aly{
	Region::Region():position(coord_px(0,0)),dimensions(percent(1,1)){

	}
	void Region::pack(const int2& dims,const double2& dpmm){
		bounds.position=position.toPixles(dims,dpmm);
		bounds.dimensions=dimensions.toPixles(dims,dpmm);
		for(std::shared_ptr<Region>& region:children){
			region->pack(dims,dpmm);
		}
	}
	void Region::pack(AlloyContext* context){
		bounds.position=position.toPixles(context->viewport.dimensions,context->dpmm);
		bounds.dimensions=dimensions.toPixles(context->viewport.dimensions,context->dpmm);
		for(std::shared_ptr<Region>& region:children){
			region->pack(bounds.dimensions,context->dpmm);
		}
	}
	void Region::pack(){
		pack(Application::getContext());
	}
};



