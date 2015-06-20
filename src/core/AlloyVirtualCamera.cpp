#include "AlloyVirtualCamera.h"

#include "GLShader.h"
#include <cmath>
#include <string>
#include <list>
#include <fstream>

namespace aly {
const float VirtualCamera::sDeg2rad = M_PI / 180.0;
VirtualCamera::VirtualCamera()
    : mFov(60.0)
	,mRw(float4x4::identity())
	,mRm(float4x4::identity())
	,mCameraTrans(0,0,0)
    , mNearPlane(0.1)
    , mFarPlane(10000.0)
    , mEye(double3(0.0, 0.0, -1.0))
    , mTumblingSpeed(0.5)
    , mZoomSpeed(0.2)
    , mStrafeSpeed(0.001)
    , mDistanceToObject(1.0)
    , mMouseDown(false)
    , mStartTumbling(false)
    , mZoomMode(false)
    , mChanged(true)
    , mNeedsDisplay(true)
	, mProjection(float4x4::identity())
	, mView(float4x4::identity())
	, mModel(float4x4::identity())
	, mMouseXPos(0)
	, mMouseYPos(0)
{
}


void
VirtualCamera::lookAt(const float3& p, double dist)
{
    mLookAt = p;
    mDistanceToObject = dist;
    mChanged=true;
    mNeedsDisplay = true;
}



void
VirtualCamera::setSpeed(double zoomSpeed, double strafeSpeed, double tumblingSpeed)
{
    mZoomSpeed = std::max(0.0001, zoomSpeed);
    mStrafeSpeed = std::max(0.0001, strafeSpeed);
    mTumblingSpeed = std::max(0.01, tumblingSpeed);
}




void VirtualCamera::aim(const box2px& bounds){
	float aspectRatio = bounds.dimensions.x / (float)bounds.dimensions.y;
    if (mChanged) {
        mChanged = false;
        float4x4 Tinv=float4x4::identity();
        float4x4 Teye=float4x4::identity();
        float4x4 S=float4x4::identity();

        Tinv(0,3)=-mLookAt[0];
        Tinv(1,3)=-mLookAt[1];
        Tinv(2,3)=-mLookAt[2];

        float4x4 T=float4x4::identity();
        T(0,3)=mLookAt[0];
        T(1,3)=mLookAt[1];
        T(2,3)=mLookAt[2];

        Teye(0,3)=mEye[0];
        Teye(1,3)=mEye[1];
        Teye(2,3)=mEye[2];

        S(0,0)=mDistanceToObject;
        S(1,1)=mDistanceToObject;
        S(2,2)=mDistanceToObject;

        float4x4 Tcamera=float4x4::identity();
        Tcamera(0,3)=mCameraTrans[0];
        Tcamera(1,3)=mCameraTrans[1];
        Tcamera(2,3)=mCameraTrans[2];

        mProjection=Tcamera*transpose(perspectiveMatrix(mFov,aspectRatio,mNearPlane,mFarPlane));
        mView=Teye*S*mRw*T*mRm;
    }

    mNeedsDisplay = false;
}

void VirtualCamera::handleKeyEvent(GLFWwindow* win,int key, int action)
{
	if((char)key=='A'){
		mRm=MakeRotationY((float)(2*sDeg2rad))*mRm;
		mChanged=true;
	} else if((char)key=='D'){
		mRm=MakeRotationY((float)(-2*sDeg2rad))*mRm;
		mChanged=true;
	} else if((char)key=='S'){
		mRm=MakeRotationX((float)(2*sDeg2rad))*mRm;
		mChanged=true;
	} else if((char)key=='W'){
		mRm=MakeRotationX((float)(-2*sDeg2rad))*mRm;
		mChanged=true;
	} else if((char)key=='R'){
		mRm=float4x4::identity();
		mRw=float4x4::identity();
		mDistanceToObject=1.0;
		mLookAt=float3(0,0,0);
		mCameraTrans=float3(0,0,0);
		mChanged=true;
	} else if(key==GLFW_KEY_UP){
		mCameraTrans[1]-=0.025;
		mChanged=true;
	} else if(key==GLFW_KEY_DOWN){
		mCameraTrans[1]+=0.025;
		mChanged=true;
	} else if(key==GLFW_KEY_LEFT){
		mCameraTrans[0]-=0.025;
		mChanged=true;
	} else if(key==GLFW_KEY_RIGHT){
		mCameraTrans[0]+=0.025;
		mChanged=true;
	} else if(key==GLFW_KEY_PAGE_UP){
		mDistanceToObject =(1+mZoomSpeed)*mDistanceToObject;
		mChanged=true;
	} else if(key==GLFW_KEY_PAGE_DOWN){
		mDistanceToObject =(1-mZoomSpeed)*mDistanceToObject;
		mChanged=true;
	} else{
		if (glfwGetKey(win,key) == GLFW_PRESS) {
			switch(key) {
				case GLFW_KEY_SPACE:
					mZoomMode = true;
					break;
			}
		} else if (glfwGetKey(win,key) == GLFW_RELEASE) {
			switch(key) {
				case GLFW_KEY_SPACE:
					mZoomMode = false;
					break;
			}
		}
	}
    mChanged = true;
}

void
VirtualCamera::handleButtonEvent(int button, int action)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) mMouseDown = true;
        else if (action == GLFW_RELEASE) mMouseDown = false;
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            mMouseDown = true;
            mZoomMode = true;
        } else if (action == GLFW_RELEASE) {
            mMouseDown = false;
            mZoomMode = false;
        }
    }
    if (action == GLFW_RELEASE) mMouseDown = false;

    mStartTumbling = true;
    mChanged = true;
}

bool VirtualCamera::onEventHandler(AlloyContext* context, const InputEvent& event){

	pixel2 lastPt=context->getCursorDownPosition();
	switch(event.type){
		case InputType::Cursor:
			handleCursorEvent(event.cursor.x,event.cursor.y);
			return  true;
		case InputType::MouseButton:
			handleButtonEvent(event.button,event.action);
			return true;
		case InputType::Scroll:
			handleScrollEvent(event.scroll.y);
			return true;
		case InputType::Key:
			handleKeyEvent(context->window,event.key,event.action);
			return true;
		default:
			return false;
	}
}
void
VirtualCamera::handleCursorEvent(float x, float y)
{
    if (mStartTumbling) {
    	mMouseXPos=x;
    	mMouseYPos=y;
        mStartTumbling = false;
    }
    float dx, dy;
    dx = x - mMouseXPos;
    dy = y - mMouseYPos;
    if (mMouseDown && !mZoomMode) {
        mNeedsDisplay = true;
		mRw=
				MakeRotationY((float)(dx*mTumblingSpeed*sDeg2rad))*
				MakeRotationX((float)(dy*mTumblingSpeed*sDeg2rad))*mRw;
    } else if (mMouseDown && mZoomMode) {
        mNeedsDisplay = true;
        float3 mUp=mRw.row(1).xyz();
        float3 mRight=mRw.row(0).xyz();
        mLookAt +=(mRight*dx-dy*mUp) * (float)(mStrafeSpeed);
    }
	mMouseXPos=x;
	mMouseYPos=y;
    mChanged = true;
}
void VirtualCamera::handleScrollEvent(int pos)
{
    mDistanceToObject =(1-pos*mZoomSpeed)*mDistanceToObject;
    mChanged = true;
    mNeedsDisplay = true;
}
}
