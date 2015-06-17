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
#version 330
in vec3 normal;
in float mag;
uniform sampler2D matcapTexture;
uniform sampler2D colormapTexture;
uniform float maxVelocity;
uniform float minVelocity;
uniform float colorMapValue;
uniform int transparent;
void main() {
   vec3 normalized_normal = normalize(normal);
   vec4 c=texture2D(matcapTexture,0.5f*normalized_normal.xy+0.5f);
   if(maxVelocity>0.0&&maxVelocity-minVelocity>1E-6f){
      float hue=clamp(mag,0.0,1.0);
      vec4 colormap=texture2D(colormapTexture,vec2(((colorMapValue<0)?1.0-hue:hue),abs(colorMapValue)));
      c=mix(c,colormap,0.5);
    }
    if(transparent>0){
    	float w=dot(normalized_normal.xy,normalized_normal.xy);
    	c.w=w;
    	gl_FragColor=c;
    } else {
    	c.w=1.0;
	    gl_FragColor=c;
    }
 }
