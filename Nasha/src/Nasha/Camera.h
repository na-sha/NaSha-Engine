#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace Nasha{
    class Camera{
    public:
        void setOrthographicProjection(float left, float right, float top, float bottom, float near, float far);
        void setPerspectiveProjection(float fovy, float aspect, float near, float far);

        const glm::mat4 getProjectionMatrix() const { return projectionMatrix; }

    private:


    public:
    private:
        glm::mat4 projectionMatrix{1.f};
    };
}