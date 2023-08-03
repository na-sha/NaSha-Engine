#pragma once

#define GLM_FORCE_RADIANS //deprecated -- uses radians as default
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace nasha{
    class Camera{
    public:
        [[maybe_unused]]void setOrthographicProjection(float left, float right,
                                                       float top, float bottom,
                                                       float near, float far);
        void setPerspectiveProjection(float fovy, float aspect, float near, float far);

        void setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3{0.f, -1.f, 0.f});
        void setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3{0.f, -1.f, 0.f});
        void setViewYXZ(glm::vec3 position, glm::vec3 rotation);

        [[nodiscard]] const glm::mat4& getProjection() const { return m_projectionMatrix; }
        [[nodiscard]] const glm::mat4& getView() const { return m_viewMatrix; }
        [[nodiscard]] const glm::mat4& getInverseView() const { return m_inverseViewMatrix; }
        [[nodiscard]] const glm::vec3 getPosition() const { return m_inverseViewMatrix[3]; }
    private:
    public:
    private:
        glm::mat4 m_projectionMatrix{1.0f};
        glm::mat4 m_viewMatrix{1.0f};
        glm::mat4 m_inverseViewMatrix{1.0f};
    };
}