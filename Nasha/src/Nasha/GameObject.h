#pragma once

#include "Model.h"

#include <glm/gtc/matrix_transform.hpp>

#include <memory>

namespace Nasha{
    struct TransformComponent{
        glm::vec3 translation{};
        glm::vec3 scale{1.f, 1.f, 1.f};
        glm::vec3 rotation{};

        glm::mat4 mat4() {
            const float c3 = glm::cos(rotation.z);
            const float s3 = glm::sin(rotation.z);
            const float c2 = glm::cos(rotation.x);
            const float s2 = glm::sin(rotation.x);
            const float c1 = glm::cos(rotation.y);
            const float s1 = glm::sin(rotation.y);
            return glm::mat4{
                    {
                            scale.x * (c1 * c3 + s1 * s2 * s3),
                                    scale.x * (c2 * s3),
                                                   scale.x * (c1 * s2 * s3 - c3 * s1),
                                                                  0.0f,
                    },
                    {
                            scale.y * (c3 * s1 * s2 - c1 * s3),
                                    scale.y * (c2 * c3),
                                                   scale.y * (c1 * c3 * s2 + s1 * s3),
                                                                  0.0f,
                    },
                    {
                            scale.z * (c2 * s1),
                                    scale.z * (-s2),
                                                   scale.z * (c1 * c2),
                                                                  0.0f,
                    },
                    {translation.x, translation.y, translation.z, 1.0f}};
        }
    };

    class GameObject{
    public:
        using id_t = unsigned int;
        static GameObject creteGameObject(){
            static id_t currentID = 0;
            return GameObject(currentID++);
        };

        GameObject(const GameObject &) = delete;
        GameObject &operator=(const GameObject &) = delete;
        GameObject(GameObject &&) = default;
        GameObject &operator=(GameObject &&) = default;

        [[nodiscard]] id_t getID() const { return id; }

        std::shared_ptr<Model> m_model{};
        glm::vec3 m_color{};
        TransformComponent m_transform{};

    private:
        explicit GameObject(id_t objID)
            :id{objID} {};
        id_t id;

    public:
    private:
    };
}