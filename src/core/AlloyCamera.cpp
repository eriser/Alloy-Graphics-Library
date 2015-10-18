#include <AlloyCamera.h>
#include "GLShader.h"
#include "AlloyMesh.h"
#include "AlloyUI.h"
#include <string>
#include <list>
#include <fstream>

namespace aly {
const float Camera::sDeg2rad = ALY_PI / 180.0f;
CameraParameters::CameraParameters() :
		changed(true), nearPlane(0.1f), farPlane(10000.0f), Projection(
				float4x4::identity()), View(float4x4::identity()), Model(
				float4x4::identity()), ViewModel(float4x4::identity()), NormalViewModel(
				float4x4::identity()), NormalView(float4x4::identity()), ViewInverse(
				float4x4::identity()), ViewModelInverse(float4x4::identity()) {
}
Camera::Camera() :
		CameraParameters(), Rw(float4x4::identity()), Rm(float4x4::identity()), cameraTrans(
				0, 0, 0), mouseXPos(0), mouseYPos(0), fov(60.0f), eye(
				float3(0.0f, 0.0f, -1.0f)), tumblingSpeed(0.5f), zoomSpeed(
				0.2f), strafeSpeed(0.001f), distanceToObject(1.0), mouseDown(
				false), startTumbling(false), zoomMode(false), needsDisplay(
				true), cameraType(CameraType::Perspective) {
}
float CameraParameters::getScale() const {
	float3x3 U, D, Vt;
	float3x3 A = SubMatrix(View);
	SVD(A, U, D, Vt);
	return std::abs(D(0, 0));
}
void Camera::lookAt(const float3& p, float dist) {
	lookAtPoint = p;
	distanceToObject = dist;
	changed = true;
	needsDisplay = true;
}

void Camera::setSpeed(float zoomSpeed, float strafeSpeed,
		float tumblingSpeed) {
	zoomSpeed = std::max(0.0001f, zoomSpeed);
	strafeSpeed = std::max(0.0001f, strafeSpeed);
	tumblingSpeed = std::max(0.01f, tumblingSpeed);
}

void Camera::aim(const box2px& bounds) {
	float aspectRatio = bounds.dimensions.x / (float) bounds.dimensions.y;
	if (changed) {
		changed = false;
		float4x4 Tinv = float4x4::identity();
		float4x4 Teye = float4x4::identity();
		float4x4 S = float4x4::identity();

		Tinv(0, 3) = -lookAtPoint[0];
		Tinv(1, 3) = -lookAtPoint[1];
		Tinv(2, 3) = -lookAtPoint[2];

		float4x4 T = float4x4::identity();
		T(0, 3) = lookAtPoint[0];
		T(1, 3) = lookAtPoint[1];
		T(2, 3) = lookAtPoint[2];

		Teye(0, 3) = eye[0];
		Teye(1, 3) = eye[1];
		Teye(2, 3) = eye[2];

		S(0, 0) = distanceToObject;
		S(1, 1) = distanceToObject;
		S(2, 2) = distanceToObject;

		float4x4 Tcamera = float4x4::identity();
		Tcamera(0, 3) = cameraTrans[0];
		Tcamera(1, 3) = cameraTrans[1];
		Tcamera(2, 3) = cameraTrans[2];

		if (cameraType == CameraType::Perspective) {
			Projection = Tcamera
					* perspectiveMatrix(fov, aspectRatio, nearPlane, farPlane);
		} else if (cameraType == CameraType::Orthographic) {
			Projection = Tcamera
					* orthographicMatrix(1.0f / aspectRatio, 1.0f, nearPlane,
							farPlane);
		}
		View = Teye * S * Rw * T * Rm;
		ViewModel = View * Model;
		ViewModelInverse = inverse(ViewModel);
		NormalViewModel = transpose(ViewModelInverse);
		ViewInverse = inverse(View);
		NormalView = transpose(ViewInverse);
		needsDisplay = true;
	}
}
void CameraParameters::aim(const box2px& bounds) {
	if (changed) {
		changed = false;
		ViewModel = View * Model;
		ViewModelInverse = inverse(ViewModel);
		NormalViewModel = transpose(ViewModelInverse);
		ViewInverse = inverse(View);
		NormalView = transpose(ViewInverse);
	}
}
void Camera::reset() {
	Rm = float4x4::identity();
	Rw = float4x4::identity();
	distanceToObject = 1.0;
	lookAtPoint = float3(0, 0, 0);
	cameraTrans = float3(0, 0, 0);
}
float2 Camera::computeNormalizedDepthRange(const Mesh& mesh) {
	box3f bbox = mesh.getBoundingBox();
	float4 center = bbox.center().xyzw();
	float4 origin = inverse(ViewModel) * float4(0, 0, 0, 1);
	float3 ray = normalize(center.xyz() - origin.xyz() / origin.w);
	float zMin = getNormalizedDepth(
			center - 0.5f * bbox.dimensions.z * float4(ray, 0));
	float zMax = getNormalizedDepth(
			center + 0.5f * bbox.dimensions.z * float4(ray, 0));
	return float2(zMin, zMax);
}
void Camera::handleKeyEvent(GLFWwindow* win, int key, int action) {
	if ((char) key == 'A') {
		Rm = MakeRotationY((float) (2 * sDeg2rad)) * Rm;
		changed = true;
	} else if ((char) key == 'D') {
		Rm = MakeRotationY((float) (-2 * sDeg2rad)) * Rm;
		changed = true;
	} else if ((char) key == 'S') {
		Rm = MakeRotationX((float) (2 * sDeg2rad)) * Rm;
		changed = true;
	} else if ((char) key == 'W') {
		Rm = MakeRotationX((float) (-2 * sDeg2rad)) * Rm;
		changed = true;
	} else if ((char) key == 'R') {
		reset();
		changed = true;
	} else if (key == GLFW_KEY_UP) {
		cameraTrans[1] += 0.025f;
		changed = true;
	} else if (key == GLFW_KEY_DOWN) {
		cameraTrans[1] -= 0.025f;
		changed = true;
	} else if (key == GLFW_KEY_LEFT) {
		cameraTrans[0] -= 0.025f;
		changed = true;
	} else if (key == GLFW_KEY_RIGHT) {
		cameraTrans[0] += 0.025f;
		changed = true;
	} else if (key == GLFW_KEY_PAGE_UP) {
		distanceToObject = std::max(1E-3f, (1 + zoomSpeed) * distanceToObject);
		changed = true;
	} else if (key == GLFW_KEY_PAGE_DOWN) {
		distanceToObject = std::max(1E-3f, (1 - zoomSpeed) * distanceToObject);
		changed = true;
	} else {
		if (glfwGetKey(win, key) == GLFW_PRESS) {
			switch (key) {
			case GLFW_KEY_SPACE:
				zoomMode = true;
				break;
			}
		} else if (glfwGetKey(win, key) == GLFW_RELEASE) {
			switch (key) {
			case GLFW_KEY_SPACE:
				zoomMode = false;
				break;
			}
		}
	}
	changed = true;
}

void Camera::handleButtonEvent(int button, int action) {
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS)
			mouseDown = true;
		else if (action == GLFW_RELEASE)
			mouseDown = false;
	} else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		if (action == GLFW_PRESS) {
			mouseDown = true;
			zoomMode = true;
		} else if (action == GLFW_RELEASE) {
			mouseDown = false;
			zoomMode = false;
		}
	}
	if (action == GLFW_RELEASE)
		mouseDown = false;
	startTumbling = true;
	changed = true;
}

bool Camera::onEventHandler(AlloyContext* context,
		const InputEvent& event) {
	if(context->getGlassPanel()->isVisible())return false;
	if (!event.isAltDown() && !event.isShiftDown() && !event.isControlDown()) {
		switch (event.type) {
		case InputType::Cursor:
			handleCursorEvent(event.cursor.x, event.cursor.y);
			return true;
		case InputType::MouseButton:
			handleButtonEvent(event.button, event.action);
			return true;
		case InputType::Scroll:
			handleScrollEvent((int) event.scroll.y);
			return true;
		case InputType::Key:
			handleKeyEvent(context->window, event.key, event.action);
			return true;
		default:
			return false;
		}
	} else {
		return false;
	}
}
void Camera::handleCursorEvent(float x, float y) {
	if (startTumbling) {
		mouseXPos = x;
		mouseYPos = y;
		startTumbling = false;
	}
	float dx, dy;
	dx = x - mouseXPos;
	dy = y - mouseYPos;
	if (mouseDown && !zoomMode) {
		Rw = MakeRotationY((float) (dx * tumblingSpeed * sDeg2rad))
				* MakeRotationX((float) (dy * tumblingSpeed * sDeg2rad)) * Rw;
		changed = true;
	} else if (mouseDown && zoomMode) {
		float3 mUp = Rw.row(1).xyz();
		float3 mRight = Rw.row(0).xyz();
		lookAtPoint += (mRight * dx - dy * mUp) * (float) (strafeSpeed);
		changed = true;
	}
	mouseXPos = x;
	mouseYPos = y;
}
void Camera::handleScrollEvent(int pos) {
	distanceToObject = std::max(1E-3f,
			(1 - pos * zoomSpeed) * distanceToObject);
	changed = true;
}
}
