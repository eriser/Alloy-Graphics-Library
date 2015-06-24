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

#include "CommonShaders.h"
#include "AlloyMesh.h"
#include "AlloyVirtualCamera.h"
namespace aly {
MatcapShader::MatcapShader(const std::string& textureImage,
		std::shared_ptr<AlloyContext> context) :
		GLShader(context), matcapTexture(context) {
	matcapTexture.load(textureImage);
	initialize(std::vector<std::string> { "vp", "vn" },
			R"(#version 330
					layout(location = 0) in vec3 vp; // positions from mesh
					layout(location = 1) in vec3 vn; // normals from mesh
					uniform mat4 ProjMat, ViewMat, ModelMat,ViewModelMat,NormalMat; // proj, view, model matrices
					out vec3 normal;
					void main () {
					  normal = vec3 (NormalMat * vec4 (vn, 0.0));
					  gl_Position = ProjMat * ViewModelMat * vec4 (vp, 1.0);
					})",
			R"(#version 330
					in vec3 normal;
					uniform sampler2D matcapTexture;
					void main() {
					   vec3 normalized_normal = normalize(normal);
					   vec4 c=texture2D(matcapTexture,0.5f*normalized_normal.xy+0.5f);
					   c.w=1.0;
					   gl_FragColor=c;
					 }
					)");
}
void MatcapShader::draw(const Mesh& mesh, VirtualCamera& camera,
		const box2px& bounds) {
	begin().set(camera, bounds).set("matcapTexture", matcapTexture, 0).draw(
			mesh.gl).end();
}
void MatcapShader::draw(const Mesh& mesh, VirtualCamera& camera) {
	draw(mesh, camera, getContext()->getViewport());
}


ImageShader::ImageShader(std::shared_ptr<AlloyContext> context) :GLShader(context) {
	initialize(std::vector<std::string> { "vp", "vt" },
			R"(
		 #version 330
		 layout(location = 0) in vec3 vp; 
layout(location = 1) in vec2 vt; 
		 uniform vec4 bounds;
		 uniform ivec4 viewport;
uniform int flip;
out vec2 uv;
		 void main() {
if(flip!=0)uv=vec2(vt.x,1.0-vt.y); else uv=vt;
		 vec2 pos=vp.xy*bounds.zw+bounds.xy;
		 gl_Position = vec4(2*pos.x/viewport.z-1.0,1.0-2*pos.y/viewport.w,0,1);
	})",
			R"(
		 #version 330
in vec2 uv;
		 uniform sampler2D textureImage;

		 void main() {
		 vec4 rgba=texture2D(textureImage,uv);
		 gl_FragColor=rgba;
		 })");
}
DepthAndNormalShader::DepthAndNormalShader(std::shared_ptr<AlloyContext> context) :GLShader(context) {
	initialize(std::vector<std::string> { "vp", "vn" },
			R"(	#version 330
				//layout(location = 0) in vec3 vp;
				layout(location = 1) in vec3 vn;
				layout(location = 3) in vec3 vp0;
				layout(location = 4) in vec3 vp1;
				layout(location = 5) in vec3 vp2;
				layout(location = 6) in vec3 vp3;
				out VS_OUT {
					vec3 p0;
					vec3 p1;
					vec3 p2;
					vec3 p3;
				} vs_out;
				void main(void) {
					vs_out.p0=vp0;
					vs_out.p1=vp1;
					vs_out.p2=vp2;
					vs_out.p3=vp3;
				})",
				R"(	#version 330
					in vec3 normal;
	                in vec3 vert;
					uniform float MIN_DEPTH;
					uniform float MAX_DEPTH;
					void main() {
						vec3 normalized_normal = normalize(normal);
						gl_FragColor = vec4(normalized_normal.xyz,(-vert.z-MIN_DEPTH)/(MAX_DEPTH-MIN_DEPTH));
					}
					)",
				R"(	#version 330
					layout (points) in;
					layout (triangle_strip, max_vertices=4) out;
					in VS_OUT {
						vec3 p0;
						vec3 p1;
						vec3 p2;
						vec3 p3;
					} quad[];
					out vec3 v0, v1, v2, v3;
					out vec3 normal, vert;
					uniform int IS_QUAD;
				uniform mat4 ProjMat, ViewMat, ModelMat,ViewModelMat,NormalMat; 
					void main() {
					  mat4 PVM=ProjMat*ViewModelMat;
					  mat4 VM=ViewModelMat;
					  
					  vec3 p0=quad[0].p0;
					  vec3 p1=quad[0].p1;
					  vec3 p2=quad[0].p2;
                      vec3 p3=quad[0].p3;
					
					  v0 = (VM*vec4(p0,1)).xyz;
					  v1 = (VM*vec4(p1,1)).xyz;
					  v2 = (VM*vec4(p2,1)).xyz;
                      v3 = (VM*vec4(p3,1)).xyz;
					  
					  
if(IS_QUAD!=0){
					  gl_Position=PVM*vec4(p0,1);  
					  vert = v0;
					  normal = (VM*vec4(normalize(cross( p3-p0, p1-p0)),0.0)).xyz;
					  EmitVertex();
} else {	  
					  gl_Position=PVM*vec4(p0,1);  
					  vert = v0;
					  normal = (VM*vec4(normalize(cross( p2-p0, p1-p0)),0.0)).xyz;
					  EmitVertex();
}
					  gl_Position=PVM*vec4(p1,1);  
					  vert = v1;
					  normal = (VM*vec4(normalize(cross( p0-p1, p2-p1)),0.0)).xyz;
					  EmitVertex();

					if(IS_QUAD!=0){
					  gl_Position=PVM*vec4(p3,1);  
					  vert = v3;
					  normal = (VM*vec4(normalize(cross( p2-p3, p0-p3)),0.0)).xyz;
					  EmitVertex();
			          gl_Position=PVM*vec4(p2,1);  
					  vert = v2;
					  normal = (VM*vec4(normalize(cross( p1-p2, p3-p2)),0.0)).xyz;
					  EmitVertex();
					} else {
			          gl_Position=PVM*vec4(p2,1);  
					  vert = v2;
					  normal = (VM*vec4(normalize(cross( p1-p2, p0-p2)),0.0)).xyz;
					  EmitVertex();
					}



					  EndPrimitive();

					 })");
	/*
	initialize(std::vector<std::string> { "vp", "vn" },
			R"(#version 330
				in vec3 vp; // positions from mesh
				in vec3 vn; // normals from mesh
				uniform mat4 ProjMat, ViewMat, ModelMat,ViewModelMat,NormalMat; 
				out vec3 normal;	
out vec4 pos;			
				void main () {
pos=  ViewModelMat *vec4 (vp, 1.0);
				  gl_Position =ProjMat*pos; 
				  normal = vec3 (NormalMat* vec4 (vn, 0.0));
				})",
			R"(	#version 330
				in vec3 normal;
in vec4 pos;
				uniform float MIN_DEPTH;
				uniform float MAX_DEPTH;
				void main() {
					vec3 normalized_normal = normalize(normal);
					gl_FragColor = vec4(normalized_normal.xyz,(-pos.z-MIN_DEPTH)/(MAX_DEPTH-MIN_DEPTH));
				}
				)");
	*/
}
void DepthAndNormalShader::draw(const Mesh& mesh, VirtualCamera& camera,
		GLFrameBuffer& frameBuffer) {
	frameBuffer.begin();
	glDisable(GL_BLEND);
	glDisable(GL_BLEND);
	if(mesh.quadIndexes.size()>0){
		begin().set("MIN_DEPTH",camera.getNearPlane()).set("IS_QUAD",1).set("MAX_DEPTH",camera.getFarPlane()).set(camera,frameBuffer.getViewport()).draw(mesh,GLMesh::PrimitiveType::QUADS).end();
	}
	begin().set("MIN_DEPTH",camera.getNearPlane()).set("IS_QUAD",0).set("MAX_DEPTH",camera.getFarPlane()).set(camera,frameBuffer.getViewport()).draw(mesh,GLMesh::PrimitiveType::TRIANGLES).end();

	glEnable(GL_BLEND);
	glEnable(GL_BLEND);
	frameBuffer.end();
}
EdgeDepthAndNormalShader::EdgeDepthAndNormalShader(std::shared_ptr<AlloyContext> context) :GLShader(context) {
	initialize(std::vector<std::string> { "vp", "vn" },
			R"(	#version 330
				//layout(location = 0) in vec3 vp;
				layout(location = 1) in vec3 vn;
				layout(location = 3) in vec3 vp0;
				layout(location = 4) in vec3 vp1;
				layout(location = 5) in vec3 vp2;
				layout(location = 6) in vec3 vp3;
				out VS_OUT {
					vec3 p0;
					vec3 p1;
					vec3 p2;
					vec3 p3;
				} vs_out;
				void main(void) {
					vs_out.p0=vp0;
					vs_out.p1=vp1;
					vs_out.p2=vp2;
					vs_out.p3=vp3;
				})",
			R"(	#version 330
				in vec3 v0, v1, v2, v3;
				in vec3 normal, vert;
				uniform float MIN_DEPTH;uniform float MAX_DEPTH;uniform float DISTANCE_TOL;
				uniform mat4 ProjMat, ViewMat, ModelMat,ViewModelMat,NormalMat; 
				uniform float SCALE;
				uniform int IS_QUAD;
				
				vec3 slerp(vec3 p0, vec3 p1, float t){
				  p0=normalize(p0);
				  p1=normalize(p1);
				  float dotp = dot(p0,p1);
				  if ((dotp > 0.9999) || (dotp<-0.9999)){
					if (t<=0.5)return p0;
					return p1;
				  }
				  float theta = acos(dotp * 3.141596535/180.0);
				  return ((p0*sin((1-t)*theta) + p1*sin(t*theta)) / sin(theta));
				}

				void main(void) {
				  vec3 line, vec, proj;
				  float dists[4];
				  vec3 tan[4];
				  // compute minimum distance from current interpolated 3d vertex to triangle edges
				  // edge v1-v0
				  
				  vec = vert - v0;
				  line = normalize(v1 - v0);
				  proj = dot(vec, line) * line;
				  dists[0] = length (vec - proj);
				  tan[0]=cross(line,normal);
				  
				  vec = vert - v1;
                  line = normalize(v2 - v1);
				  proj = dot(vec, line) * line;
				  dists[1] = length (vec - proj);
				  tan[1]=cross(line,normal);

				if(IS_QUAD!=0){
                  vec = vert - v2;
				  line = normalize(v3 - v2); 
				  proj = dot(vec, line) * line;
				  dists[2] = length (vec - proj);
				  tan[2]=cross(line,normal);

				  line = normalize(v0 - v3); 
                  vec = vert - v3;
				  proj = dot(vec, line) * line;
				  dists[3] = length (vec - proj);
				  tan[3]=cross(line,normal);
				} else {
                  vec = vert - v2;
				  line = normalize(v0 - v2); 
				  proj = dot(vec, line) * line;
				  dists[2] = length (vec - proj);
				  tan[2]=cross(line,normal);
				}

				  vec3 outNorm=normalize(normal);
                  
                  float minDist=1E30;
                  for(int n=0;n<3+IS_QUAD;n++){
                     if(dists[n]<minDist){
				       outNorm=slerp(tan[n],normal,clamp(dists[n]/DISTANCE_TOL,0.0,1.0));
                       minDist=dists[n];
                     }
					}
				if (minDist <DISTANCE_TOL){
					gl_FragColor = vec4(outNorm,minDist);
				} else {
					gl_FragColor = vec4(outNorm,minDist);
				}
				})",
				R"(	#version 330
					layout (points) in;
					layout (triangle_strip, max_vertices=4) out;
					in VS_OUT {
						vec3 p0;
						vec3 p1;
						vec3 p2;
						vec3 p3;
					} quad[];
					out vec3 v0, v1, v2, v3;
					out vec3 normal, vert;
					uniform int IS_QUAD;
				uniform mat4 ProjMat, ViewMat, ModelMat,ViewModelMat,NormalMat; 
					void main() {
					  mat4 PVM=ProjMat*ViewModelMat;
					  mat4 VM=ViewModelMat;
					  
					  vec3 p0=quad[0].p0;
					  vec3 p1=quad[0].p1;
					  vec3 p2=quad[0].p2;
                      vec3 p3=quad[0].p3;
					
					  v0 = (VM*vec4(p0,1)).xyz;
					  v1 = (VM*vec4(p1,1)).xyz;
					  v2 = (VM*vec4(p2,1)).xyz;
                      v3 = (VM*vec4(p3,1)).xyz;
					  
					  
if(IS_QUAD!=0){
					  gl_Position=PVM*vec4(p0,1);  
					  vert = v0;
					  normal = (VM*vec4(normalize(cross( p3-p0, p1-p0)),0.0)).xyz;
					  EmitVertex();
} else {	  
					  gl_Position=PVM*vec4(p0,1);  
					  vert = v0;
					  normal = (VM*vec4(normalize(cross( p2-p0, p1-p0)),0.0)).xyz;
					  EmitVertex();
}
					  gl_Position=PVM*vec4(p1,1);  
					  vert = v1;
					  normal = (VM*vec4(normalize(cross( p0-p1, p2-p1)),0.0)).xyz;
					  EmitVertex();

					if(IS_QUAD!=0){
					  gl_Position=PVM*vec4(p3,1);  
					  vert = v3;
					  normal = (VM*vec4(normalize(cross( p2-p3, p0-p3)),0.0)).xyz;
					  EmitVertex();
					  gl_Position=PVM*vec4(p2,1);  
					   vert = v2;
					  normal = (VM*vec4(normalize(cross( p1-p2, p3-p2)),0.0)).xyz;
					  EmitVertex();

					} else {
			          gl_Position=PVM*vec4(p2,1);  
					  vert = v2;
					  normal = (VM*vec4(normalize(cross( p1-p2, p0-p2)),0.0)).xyz;
					  EmitVertex();
					}
					EndPrimitive();
	

					 })");
}
/*
EdgeDepthAndNormalShader::EdgeDepthAndNormalShader(std::shared_ptr<AlloyContext> context) :GLShader(context) {
	initialize(std::vector<std::string> { "vp", "vn" },
			R"(	#version 330
				in vec3 vp;
				in vec3 vn;
				void main(void) {
				  gl_Position = vec4(vp,1.0);
				})",
			R"(	#version 330
				in vec3 v0, v1, v2;
				in vec3 normal, vert;
				uniform float MIN_DEPTH;uniform float MAX_DEPTH;uniform float DISTANCE_TOL;
				uniform mat4 ProjMat, ViewMat, ModelMat,ViewModelMat,NormalMat;
				uniform float SCALE;
uniform int IS_QUAD;

vec3 slerp(vec3 p0, vec3 p1, float t){
  p0=normalize(p0);
  p1=normalize(p1);
  float dotp = dot(p0,p1);
  if ((dotp > 0.9999) || (dotp<-0.9999)){
    if (t<=0.5)return p0;
    return p1;
  }
  float theta = acos(dotp * 3.141596535/180.0);
  return ((p0*sin((1-t)*theta) + p1*sin(t*theta)) / sin(theta));
}

				void main(void) {
				  vec3 line, vec, proj;
				  float dist1,dist2,dist3,dist;
				  float w1,w2,w3;
				  vec3 tan1,tan2,tan3;
				  // compute minimum distance from current interpolated 3d vertex to triangle edges
				  // edge v1-v0

				  vec = vert - v0;
				  line = normalize(v1 - v0);
				  proj = dot(vec, line) * line;
				  dist1 = length (vec - proj);
				  tan1=cross(line,normal);

				  line = normalize(v0 - v2);
				  proj = dot(vec, line) * line;
				  dist2 = length (vec - proj);
				  tan2=cross(line,normal);

				  line = normalize(v2 - v1);
vec = vert - v1;
				  proj = dot(vec, line) * line;
				  dist3 = length (vec - proj);
				  tan3=cross(line,normal);

				  vec3 outNorm=normalize(normal);
				  w1=clamp(dist1/DISTANCE_TOL,0.0,1.0);
				  w2=clamp(dist2/DISTANCE_TOL,0.0,1.0);
w3=clamp(dist3/DISTANCE_TOL,0.0,1.0);
					if(dist1<dist2){
						if(IS_QUAD>0||dist1<dist3){
						dist=dist1;
						outNorm=slerp(tan1,normal,w1);
						} else {
						dist=dist3;
						outNorm=slerp(tan3,normal,w3);
						}
					} else {
						if(IS_QUAD>0||dist2<dist3){
						dist=dist2;
						outNorm=slerp(tan2,normal,w2);
						} else {
						dist=dist3;
						outNorm=slerp(tan3,normal,w3);
						}
					}
    if (dist <DISTANCE_TOL){
      gl_FragColor = vec4(outNorm,dist);
    } else {
gl_FragColor = vec4(outNorm,dist);
    }
				})",
				R"(	#version 330
					layout (triangles) in;
					layout (triangle_strip, max_vertices=3) out;
					out vec3 v0, v1, v2;
					out vec3 normal, vert;
				uniform mat4 ProjMat, ViewMat, ModelMat,ViewModelMat,NormalMat;
					void main() {
					  mat4 PVM=ProjMat*ViewModelMat;
					  mat4 VM=ViewModelMat;

					  vec3 v01 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
					  vec3 v02 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
					  vec3 fn =  normalize(cross( v01, v02 ));
					  vec4 p0=gl_in[0].gl_Position;
					  vec4 p1=gl_in[1].gl_Position;
					  vec4 p2=gl_in[2].gl_Position;

					  v0 = (VM*p0).xyz;
					  v1 = (VM*p1).xyz;
					  v2 = (VM*p2).xyz;


					  gl_Position=PVM*gl_in[0].gl_Position;
					  vert = v0;
					  normal = (VM*vec4(fn,0.0)).xyz;
					  EmitVertex();


					  gl_Position=PVM*gl_in[1].gl_Position;
					  vert = v1;
					  normal = (VM*vec4(fn,0.0)).xyz;
					  EmitVertex();


					  gl_Position=PVM*gl_in[2].gl_Position;
					  vert = v2;
					  normal = (VM*vec4(fn,0.0)).xyz;
					  EmitVertex();

					  EndPrimitive();
					 })");
}
*/
EdgeEffectsShader::EdgeEffectsShader(std::shared_ptr<AlloyContext> context) :GLShader(context) {
	initialize(std::vector<std::string> { "vp", "vt" },
			R"(
#version 330
layout(location = 0) in vec3 vp; 
layout(location = 1) in vec2 vt; 
uniform vec4 bounds;
uniform ivec4 viewport;
out vec2 uv;
void main() {
uv=vt;
vec2 pos=vp.xy*bounds.zw+bounds.xy;
gl_Position = vec4(2*pos.x/viewport.z-1.0,1.0-2*pos.y/viewport.w,0,1);
})",
			R"(
#version 330
in vec2 uv;
uniform ivec2 imageSize;
uniform int KERNEL_SIZE;
uniform sampler2D textureImage;
const int SCALE=2;
void main() {
vec4 rgba=texture2D(textureImage,uv);
vec4 nrgba;
if(rgba.w>0){
float minDistance=KERNEL_SIZE*KERNEL_SIZE;
for(int i=-KERNEL_SIZE;i<=KERNEL_SIZE;i++){
	for(int j=-KERNEL_SIZE;j<=KERNEL_SIZE;j++){
      nrgba=texture2D(textureImage,uv+SCALE*vec2(i/float(imageSize.x),j/float(imageSize.y)));
if(nrgba.w<=0.0){
      minDistance=min(minDistance,i*i+j*j);
}
	}
}
rgba=vec4(1.0-sqrt(minDistance)/KERNEL_SIZE,0.0,0.0,1.0);
} else {
float minDistance=KERNEL_SIZE*KERNEL_SIZE;
for(int i=-KERNEL_SIZE;i<=KERNEL_SIZE;i++){
	for(int j=-KERNEL_SIZE;j<=KERNEL_SIZE;j++){
      nrgba=texture2D(textureImage,uv+SCALE*vec2(i/float(imageSize.x),j/float(imageSize.y)));
if(nrgba.w>0.0){
      minDistance=min(minDistance,i*i+j*j);
}
	}
}
rgba=vec4(0.0,1.0-sqrt(minDistance)/KERNEL_SIZE,0.0,1.0);
}
gl_FragColor=rgba;
})");
}
void EdgeEffectsShader::draw(const GLTextureRGBAf& imageTexture, const box2px& bounds){
	begin().set("KERNEL_SIZE",4).set("textureImage", imageTexture, 0).set("bounds",
			bounds).set("imageSize",imageTexture.bounds.dimensions).set("viewport", context->viewport).draw(
			imageTexture).end();
}
void EdgeEffectsShader::draw(const GLTextureRGBAf& imageTexture, const float2& location,const float2& dimensions){
	draw(imageTexture,box2px(location,dimensions));
}


OutlineShader::OutlineShader(std::shared_ptr<AlloyContext> context) :GLShader(context) {
	initialize(std::vector<std::string> { "vp", "vt" },
			R"(
#version 330
layout(location = 0) in vec3 vp; 
layout(location = 1) in vec2 vt; 
uniform vec4 bounds;
uniform ivec4 viewport;

out vec2 uv;
void main() {
uv=vt;
vec2 pos=vp.xy*bounds.zw+bounds.xy;
gl_Position = vec4(2*pos.x/viewport.z-1.0,1.0-2*pos.y/viewport.w,0,1);
})",
			R"(
#version 330
in vec2 uv;
uniform ivec2 imageSize;
uniform int KERNEL_SIZE;
uniform sampler2D textureImage;
uniform vec4 innerColor,outerColor,edgeColor;
float w;
void main() {
vec4 rgba=texture2D(textureImage,uv);
vec4 nrgba;
if(rgba.w>0){
float minDistance=KERNEL_SIZE*KERNEL_SIZE;
for(int i=-KERNEL_SIZE;i<=KERNEL_SIZE;i++){
	for(int j=-KERNEL_SIZE;j<=KERNEL_SIZE;j++){
      nrgba=texture2D(textureImage,uv+vec2(i/float(imageSize.x),j/float(imageSize.y)));
if(nrgba.w<=0.0){
      minDistance=min(minDistance,i*i+j*j);
}
	}
}
w=sqrt(minDistance)/KERNEL_SIZE;
if(w>0.99999)discard;
rgba=mix(edgeColor,innerColor,w);
} else {
float minDistance=KERNEL_SIZE*KERNEL_SIZE;
for(int i=-KERNEL_SIZE;i<=KERNEL_SIZE;i++){
	for(int j=-KERNEL_SIZE;j<=KERNEL_SIZE;j++){
      nrgba=texture2D(textureImage,uv+vec2(i/float(imageSize.x),j/float(imageSize.y)));
if(nrgba.w>0.0){
      minDistance=min(minDistance,i*i+j*j);
}
	}
}
w=sqrt(minDistance)/KERNEL_SIZE;
if(w>0.99999)discard;
rgba=mix(edgeColor,outerColor,w);
}
gl_FragColor=rgba;
})");
}
void OutlineShader::draw(const GLTextureRGBAf& imageTexture,const box2px& bounds){
	begin().set("KERNEL_SIZE",kernelSize).set("innerColor",innerGlowColor).set("outerColor",outerGlowColor).set("edgeColor",edgeColor).set("textureImage", imageTexture, 0).set("bounds",
			bounds).set("imageSize",imageTexture.bounds.dimensions).set("viewport", context->viewport).draw(
			imageTexture).end();
}
void OutlineShader::draw(const GLTextureRGBAf& imageTexture,const float2& location,const float2& dimensions){
	draw(imageTexture,box2px(location,dimensions));
}
NormalColorShader::NormalColorShader(std::shared_ptr<AlloyContext> context) :GLShader(context) {
	initialize(std::vector<std::string> { "vp", "vt" },
			R"(
#version 330
layout(location = 0) in vec3 vp; 
layout(location = 1) in vec2 vt; 
uniform vec4 bounds;
uniform ivec4 viewport;
out vec2 uv;
void main() {
uv=vt;
vec2 pos=vp.xy*bounds.zw+bounds.xy;
gl_Position = vec4(2*pos.x/viewport.z-1.0,1.0-2*pos.y/viewport.w,0,1);
})",
			R"(
#version 330
in vec2 uv;
const float PI=3.1415926535;
uniform sampler2D textureImage;
void main() {
vec4 rgba=texture2D(textureImage,uv);
if(rgba.w>0){
float lum=clamp(abs(rgba.w),0.0f,1.0f);
rgba=vec4(rgba.x*0.5+0.5,rgba.y*0.5+0.5,rgba.z,1.0);

} else {
rgba=vec4(0.0,0.0,0.0,1.0);
}
gl_FragColor=rgba;
})");
}
void NormalColorShader::draw(const GLTextureRGBAf& imageTexture, const box2px& bounds){
	begin().set("textureImage", imageTexture, 0).set("bounds",
			bounds).set("viewport", context->viewport).draw(
			imageTexture).end();
}
void NormalColorShader::draw(const GLTextureRGBAf& imageTexture, const float2& location,const float2& dimensions){
	draw(imageTexture,box2px(location,dimensions));
}

DepthColorShader::DepthColorShader(std::shared_ptr<AlloyContext> context) :GLShader(context) {
	initialize(std::vector<std::string> { "vp", "vt" },
			R"(
#version 330
layout(location = 0) in vec3 vp; 
layout(location = 1) in vec2 vt; 
uniform vec4 bounds;
uniform ivec4 viewport;
out vec2 uv;
void main() {
uv=vt;
vec2 pos=vp.xy*bounds.zw+bounds.xy;
gl_Position = vec4(2*pos.x/viewport.z-1.0,1.0-2*pos.y/viewport.w,0,1);
})",
			R"(
#version 330
in vec2 uv;
const float PI=3.1415926535;
uniform sampler2D textureImage;
uniform float zMin;
uniform float zMax;
void main() {
vec4 rgba=texture2D(textureImage,uv);
if(rgba.w>0){
	float lum=clamp((rgba.w-zMin)/(zMax-zMin),0.0f,1.0f);
	float r=max(0.0,1.0f-lum*2.0);
	float b=min(2*lum-1.0,1.0);
	float g=max(0.0,1-2.0*abs(2.0*lum-1.0));
	rgba=vec4(r,g,b,1.0);
} else {
rgba=vec4(0.0,0.0,0.0,1.0);
}
gl_FragColor=rgba;
})");
}
void DepthColorShader::draw(const GLTextureRGBAf& imageTexture,float2 zRange, const box2px& bounds){
	begin().set("textureImage", imageTexture, 0).set("zMin",zRange.x),set("zMax",zRange.y).set("bounds",
			bounds).set("viewport", context->viewport).draw(
			imageTexture).end();
}
void DepthColorShader::draw(const GLTextureRGBAf& imageTexture,float2 zRange,  const float2& location,const float2& dimensions){
	draw(imageTexture,zRange,box2px(location,dimensions));
}


WireframeShader::WireframeShader(std::shared_ptr<AlloyContext> context) :GLShader(context) {
	initialize(std::vector<std::string> { "vp", "vt" },
			R"(
#version 330
layout(location = 0) in vec3 vp; 
layout(location = 1) in vec2 vt; 
uniform vec4 bounds;
uniform ivec4 viewport;
out vec2 uv;
void main() {
uv=vt;
vec2 pos=vp.xy*bounds.zw+bounds.xy;
gl_Position = vec4(2*pos.x/viewport.z-1.0,1.0-2*pos.y/viewport.w,0,1);
})",
			R"(
#version 330
in vec2 uv;
const float PI=3.1415926535;
uniform sampler2D textureImage;
uniform float zMin;
uniform float zMax;
uniform vec4 edgeColor;
uniform vec4 faceColor;
void main() {
vec4 rgba=texture2D(textureImage,uv);
if(rgba.w>0){
	float lum=clamp((rgba.w-zMin)/(zMax-zMin),0.0f,1.0f);
	rgba=mix(edgeColor,faceColor,smoothstep(0.0,1.0,lum>0.5?1.0:0.0));
} else {
rgba=vec4(0.0,0.0,0.0,1.0);
}
gl_FragColor=rgba;
})");
}
void WireframeShader::draw(const GLTextureRGBAf& imageTexture,float2 zRange, const box2px& bounds){
	begin().set("textureImage", imageTexture, 0).set("edgeColor",edgeColor).set("faceColor",faceColor).set("zMin",zRange.x),set("zMax",zRange.y).set("bounds",
			bounds).set("viewport", context->viewport).draw(
			imageTexture).end();
}
void WireframeShader::draw(const GLTextureRGBAf& imageTexture,float2 zRange,  const float2& location,const float2& dimensions){
	draw(imageTexture,zRange,box2px(location,dimensions));
}
void EdgeDepthAndNormalShader::draw(const Mesh& mesh, VirtualCamera& camera,
		GLFrameBuffer& frameBuffer) {
	frameBuffer.begin();
		glDisable(GL_BLEND);
		if(mesh.quadIndexes.size()>0){
			begin().set("DISTANCE_TOL",camera.getScale()).set("IS_QUAD",1),set("MIN_DEPTH",camera.getNearPlane()).set("MAX_DEPTH",camera.getFarPlane()).set(camera, frameBuffer.getViewport()).draw(mesh,GLMesh::PrimitiveType::QUADS).end();
		}
		begin().set("DISTANCE_TOL",camera.getScale()).set("IS_QUAD",0),set("SCALE",camera.getScale()),set("MIN_DEPTH",camera.getNearPlane()).set("MAX_DEPTH",camera.getFarPlane()).set(camera, frameBuffer.getViewport()).draw(mesh,GLMesh::PrimitiveType::TRIANGLES).end();

		glEnable(GL_BLEND);
	frameBuffer.end();
}


}

