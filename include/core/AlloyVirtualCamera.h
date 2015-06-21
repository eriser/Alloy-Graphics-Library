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

#ifndef ALLOYCAMERA_H_
#define ALLOYCAMERA_H_

#include "AlloyMath.h"
#include "AlloyContext.h"
#include <fstream>
namespace aly {
class VirtualCamera: public EventHandler {
protected:
	// Camera parameters
	float4x4 Rw, Rm;
	float3 cameraTrans;
	float mouseXPos;
	float mouseYPos;
	float fov, nearPlane, farPlane;
	float3 lookAtPoint, eye;
	float tumblingSpeed, zoomSpeed, strafeSpeed;
	float distanceToObject;
	bool mouseDown, startTumbling, zoomMode, changed, needsDisplay;

	void handleKeyEvent(GLFWwindow* win, int key, int action);
	void handleButtonEvent(int button, int action);
	void handleCursorEvent(float x, float y);
	void handleScrollEvent(int pos);
public:

	float4x4 mProjection, mView, mModel;
	float4x4 mViewModel, mNormal;
	VirtualCamera();
	void aim(const aly::box2px& bounds);
	void setPose(const float4x4& m) {
		mModel = m;
	}
	float4x4& getPose() {
		return mModel;
	}
	float getScale() {
		return mModel(0, 0) * distanceToObject;
	}
	virtual bool onEventHandler(AlloyContext* context, const InputEvent& event)
			override;
	void setNearFarPlanes(float n, float f) {
		nearPlane = n;
		farPlane = f;
	}
	void setFieldOfView(float degrees) {
		fov = degrees;
	}
	void setSpeed(float zoomSpeed, float strafeSpeed, float tumblingSpeed);
	void lookAt(const float3& p, float dist);
	void setDistanceToObject(float distance) {
		distanceToObject = distance;
		changed = true;
		needsDisplay = true;
	}
	void lookAt(const float3& p) {
		lookAtPoint = p;
		changed = true;
		needsDisplay = true;
	}
	float getNearPlane() {
		return nearPlane;
	}
	float getFarPlane() {
		return farPlane;
	}
	float3 transform(float3& pt) {
		float4 ptp(pt[0], pt[1], pt[2], 1.0f);
		float4 p = mProjection * mView * mModel * ptp;
		return float3(p[0] / p[3], p[1] / p[3], p[2] / p[3]);
	}
	void resetTranslation() {
		cameraTrans = float3(0, 0, 0);
		lookAtPoint = float3(0, 0, 0);
	}

	void setZoom(float z) {
		distanceToObject = z;
	}
	static const float sDeg2rad;
};
// class Camera
}
#endif /* ALLOYCAMERA_H_ */
