#pragma once

#include "Model.h"

#include <glm/gtc/matrix_transform.hpp>

#include <memory>
#include <unordered_map>

namespace nasha{
//    struct Transform2DComponent{
//        glm::vec2 translation{};
//        glm::vec2 scale{1.f, 1.f};
//        float rotation;
//
//        glm::mat2 mat2() {
//            const float s = glm::sin(rotation);
//            const float c = glm::cos(rotation);
//            glm::mat2 rotMatrix{{c, s}, {-s, c}};
//
//            glm::mat2 scaleMat{{scale.x, .0f}, {.0f, scale.y}};
//            return rotMatrix * scaleMat;
//        }
//    };

    struct TransformComponent{
        glm::vec3 translation{};
        glm::vec3 scale{1.0f, 1.0f, 1.0f};
        glm::vec3 rotation{};

        [[nodiscard]] glm::mat4 mat4() const;
        [[nodiscard]] glm::mat3 normalMatrix() const;
    };

    struct PointLightComponent {
        float lightIntensity = 1.0f;
    };

    class GameObject {
    public:
        using id_t = unsigned int;
        using Map = std::unordered_map<id_t, GameObject>;

        static GameObject createGameObject(){
            static id_t currentId = 0;
            return GameObject{currentId++};
        }

        static GameObject makePointLight(float intensity = 10.0f,
                                         float radius = 0.1f,
                                         glm::vec3 color = glm::vec3(1.0f));

        GameObject(const GameObject&) = delete;
        GameObject &operator=(const GameObject&) = delete;
        GameObject (GameObject&&) = default;
        GameObject &operator=(GameObject&&) = default;

        [[nodiscard]] id_t gameId() const { return m_id; }

    private:
        explicit GameObject(id_t id) : m_id{id} {}

    public:
        glm::vec3 color{};
//        Transform2DComponent transform2D{};
        TransformComponent transform{};

        std::shared_ptr<Model> model{};
        std::unique_ptr<PointLightComponent> pointLight = nullptr;

    private:
        id_t m_id;
    };
}
