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
namespace aly
{
struct Mesh;
enum class CameraType
{
    Perspective, Orthographic
};
class VirtualCamera: public EventHandler
{
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
        CameraType cameraType;
        void handleKeyEvent(GLFWwindow* win, int key, int action);
        void handleButtonEvent(int button, int action);
        void handleCursorEvent(float x, float y);
        void handleScrollEvent(int pos);
    public:

        float4x4 Projection, View, Model;
        float4x4 ViewModel, NormalViewModel, NormalView;
        float4x4 ViewInverse, ViewModelInverse;
        VirtualCamera();
        void aim(const aly::box2px& bounds);
        void setPose(const float4x4& m)
        {
            Model = m;
        }
        void setCameraType(const CameraType& type)
        {
            cameraType = type;
            changed = true;
        }
        inline float2 getFocalLength() const
        {
            return float2(Projection(0, 0), Projection(1, 1));
        }
        float4x4 getPose() const
        {
            return Model;
        }
        bool isDirty() const
        {
            return needsDisplay || changed;
        }
        void setDirty(bool d)
        {
            needsDisplay = d;
            if (d)
                changed = true;
        }
        float getScale() const
        {
            return distanceToObject;
        }
        virtual bool onEventHandler(AlloyContext* context,
                                    const InputEvent& event) override;
        void setNearFarPlanes(float n, float f)
        {
            nearPlane = n;
            farPlane = f;
            changed = true;
        }
        void setFieldOfView(float degrees)
        {
            fov = degrees;
            changed = true;
        }
        void setSpeed(float zoomSpeed, float strafeSpeed, float tumblingSpeed);
        void lookAt(const float3& p, float dist);
        void lookAt(const float3& p)
        {
            lookAtPoint = p;
            changed = true;
        }
        float getNearPlane() const
        {
            return nearPlane;
        }
        float2 getZRange() const
        {
            return float2(nearPlane, farPlane);
        }
        float getNormalizedDepth(const float4& pt) const
        {
            float4 out = ViewModel * pt;
            return (-out.z - nearPlane) / (farPlane - nearPlane);
        }
        float2 computeNormalizedDepthRange(const Mesh& mesh);
        float getFarPlane() const
        {
            return farPlane;
        }
        inline float3 transformWorldToScreen(const float3& pt) const
        {
            float4 ptp(pt[0], pt[1], pt[2], 1.0f);
            float4 p = Projection * ViewModel * ptp;
            return float3(p[0] / p[3], p[1] / p[3], p[2] / p[3]);
        }

        inline float3 transformWorldToNormalizedImage(const float3& pt) const
        {
            float4 ptp(pt[0], pt[1], pt[2], 1.0f);
            float4 p = Projection * ViewModel * ptp;
            return float3(0.5f * (p[0] / p[3] + 1.0f),
                          0.5f * (1.0f - p[1] / p[3]),
                          p[2] / p[3]);
        }
        inline float3 transformWorldToImage(const float3& pt,
                                            int w,
                                            int h) const
        {
            float4 ptp(pt[0], pt[1], pt[2], 1.0f);
            float4 p = Projection * ViewModel * ptp;
            return float3(w * 0.5f * (p[0] / p[3] + 1.0f),
                          h * 0.5f * (1.0f - p[1] / p[3]),
                          p[2] / p[3]);
        }
        inline float3 transformNormalizedImageToWorld(const float3& pt) const
        {
            float4 ptp;
            float A = Projection(2, 2);
            float B = Projection(2, 3);
            float C = Projection(3, 2);
            float fx = Projection(0, 0);
            float fy = Projection(1, 1);
            ptp.z = B / (C * pt.z - A);
            ptp.x = (2.0f * pt.x - 1.0f) * ptp.z * C / fx;
            ptp.y = (1.0f - 2.0f * pt.y) * ptp.z * C / fy;
            ptp.w = 1.0f;
            ptp = ViewModelInverse * ptp;
            return ptp.xyz() / ptp.w;
        }
        inline float3 transformImageToWorld(const float3& pt,
                                            int w,
                                            int h) const
        {
            float4 ptp;
            float A = Projection(2, 2);
            float B = Projection(2, 3);
            float C = Projection(3, 2);
            float fx = Projection(0, 0);
            float fy = Projection(1, 1);
            ptp.z = B / (C * pt.z - A);
            ptp.x = (2.0f * pt.x / (float) w - 1.0f) * ptp.z * C / fx;
            ptp.y = (1.0f - 2.0f * pt.y / (float) h) * ptp.z * C / fy;
            ptp.w = 1.0f;
            ptp = ViewModelInverse * ptp;
            return ptp.xyz() / ptp.w;
        }
        inline float3 transformScreenToWorld(const float3& pt) const
        {
            float4 ptp;
            float A = Projection(2, 2);
            float B = Projection(2, 3);
            float C = Projection(3, 2);
            float fx = Projection(0, 0);
            float fy = Projection(1, 1);
            ptp.z = B / (C * pt.z - A);
            ptp.x = pt.x * ptp.z * C / fx;
            ptp.y = pt.y * ptp.z * C / fy;
            ptp.w = 1.0f;
            ptp = ViewModelInverse * ptp;
            return ptp.xyz() / ptp.w;
        }
        inline float3 transformScreenDepthToWorld(const float3& pt) const
        {
            float4 ptp;
            float A = Projection(2, 2);
            float B = Projection(2, 3);
            float C = Projection(3, 2);
            float fx = Projection(0, 0);
            float fy = Projection(1, 1);
            ptp.z = -(pt.z * (farPlane - nearPlane) + nearPlane);
            ptp.x = pt.x * ptp.z * C / fx;
            ptp.y = pt.y * ptp.z * C / fy;
            ptp.w = 1.0f;
            ptp = ViewModelInverse * ptp;
            return ptp.xyz() / ptp.w;
        }
        inline float3 transformNormalizedImageDepthToWorld(const float3& pt) const
        {
            float4 ptp;
            float A = Projection(2, 2);
            float B = Projection(2, 3);
            float C = Projection(3, 2);
            float fx = Projection(0, 0);
            float fy = Projection(1, 1);
            ptp.z = -(pt.z * (farPlane - nearPlane) + nearPlane);
            ptp.x = (2.0f * pt.x - 1.0f) * ptp.z * C / fx;
            ptp.y = (1.0f - 2.0f * pt.y) * ptp.z * C / fy;
            ptp.w = 1.0f;
            ptp = ViewModelInverse * ptp;
            return ptp.xyz() / ptp.w;
        }
        inline float3 transformImageDepthToWorld(const float3& pt,
                                                 int w,
                                                 int h) const
        {
            float4 ptp;
            float A = Projection(2, 2);
            float B = Projection(2, 3);
            float C = Projection(3, 2);
            float fx = Projection(0, 0);
            float fy = Projection(1, 1);
            ptp.z = -(pt.z * (farPlane - nearPlane) + nearPlane);
            ptp.x = (2.0f * pt.x / (float) w - 1.0f) * ptp.z * C / fx;
            ptp.y = (1.0f - 2.0f * pt.y / (float) h) * ptp.z * C / fy;
            ptp.w = 1.0f;
            ptp = ViewModelInverse * ptp;
            return ptp.xyz() / ptp.w;
        }
        void resetTranslation()
        {
            cameraTrans = float3(0, 0, 0);
            lookAtPoint = float3(0, 0, 0);
            changed = true;
        }

        void setZoom(float z)
        {
            distanceToObject = z;
            changed = true;
        }
        static const float sDeg2rad;
};
// class Camera
}
#endif /* ALLOYCAMERA_H_ */
