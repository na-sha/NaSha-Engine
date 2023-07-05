#pragma once

#include "Model.h"

#include <glm/gtc/matrix_transform.hpp>

#include <memory>
#include <unordered_map>

namespace Nasha{
    struct TransformComponent{
        glm::vec3 translation{};
        glm::vec3 scale{1.f, 1.f, 1.f};
        glm::vec3 rotation{};

        glm::mat4 mat4();
        glm::mat3 normalMatrix();
    };

    class GameObject{
    public:
        using id_t = unsigned int;
        using Map = std::unordered_map<id_t, GameObject>;

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