#pragma once

#include "Model.h"

#include <memory>

namespace Nasha{
    struct Transform2DComponent{
        glm::vec2 translation{};
        glm::vec2 scale{1.f, 1.f};
        float rotation;

        glm::mat2 mat2() {
            const float s = glm::sin(rotation);
            const float c = glm::cos(rotation);
            glm::mat2 rotMatrix{{c, s}, {-s, c}};
            glm::mat2 scaleMat{{scale.x, 0.0f}, {0.0f, scale.y}};
//            return rotMatrix * scaleMat;
            return scaleMat * rotMatrix;
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
        Transform2DComponent m_transform2D{};

    private:
        explicit GameObject(id_t objID)
            :id{objID} {};
        id_t id;

    public:
    private:
    };
}