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
namespace aly{
class VirtualCamera: public EventHandler{
protected:
    // Camera parameters
    float4x4 mRw,mRm;
    float3 mCameraTrans;
    float mMouseXPos;
    float mMouseYPos;
    float mFov, mNearPlane, mFarPlane;
    float3 mLookAt, mEye;
    float mTumblingSpeed, mZoomSpeed, mStrafeSpeed;
    float mDistanceToObject;
    bool mMouseDown, mStartTumbling, mZoomMode, mChanged, mNeedsDisplay;

    void handleKeyEvent(GLFWwindow* win,int key, int action);
    void handleButtonEvent(int button, int action);
    void handleCursorEvent(float x, float y);
    void handleScrollEvent(int pos);
public:

    float4x4 mProjection,mView,mModel;
    float4x4 mViewModel,mNormal;
	VirtualCamera();
    void aim(const aly::box2px& bounds);
    void setPose(const float4x4& m){
    	mModel=m;
    }
    float4x4& getPose(){
    	return mModel;
    }
    double getScale(){
    	return mModel(0,0)*mDistanceToObject;
    }
    virtual bool onEventHandler(AlloyContext* context, const InputEvent& event) override;
    void setNearFarPlanes(double n, double f) { mNearPlane = n; mFarPlane = f; }
    void setFieldOfView(double degrees) { mFov = degrees; }
    void setSpeed(double zoomSpeed, double strafeSpeed, double tumblingSpeed);
    void lookAt(const float3& p, double dist);
    void setDistanceToObject(double distance){
    	mDistanceToObject=distance;
    	mChanged = true;
    	mNeedsDisplay=true;
    }
    void setLookAt(double x,double y, double z){
        	mLookAt[0]=x;
        	mLookAt[1]=y;
        	mLookAt[2]=z;
        	mChanged = true;
        	mNeedsDisplay=true;
        }
    float nearPlane(){return mNearPlane;}
    float farPlane(){return mFarPlane;}
    bool needsDisplay() const { return mNeedsDisplay; }
    void setNeedsDisplay(bool val){
    	mNeedsDisplay=val;
    }
    float3 transform(float3& pt){
    	float4 ptp(pt[0],pt[1],pt[2],1.0f);
    	float4 p=mProjection*mView*mModel*ptp;
    	return float3(p[0]/p[3],p[1]/p[3],p[2]/p[3]);
    }
    void resetTranslation(){
    	mCameraTrans=float3(0,0,0);
    	mLookAt=float3(0,0,0);
    }

    void setZoom(double z){
    	mDistanceToObject=z;
    }
    static const float sDeg2rad;
}; // class Camera
}
#endif /* ALLOYCAMERA_H_ */
