#pragma once

#include "core/base.hpp"

#include "ecs/Component.hpp"
#include <glm/gtc/quaternion.hpp>
#include <glm/vec3.hpp>

struct TransformComponent : ecs::IComponentBase
{
    opfor::String name;

    glm::vec3 position{0.0f, 0.0f, 0.0f};
    glm::quat rotation{1.0f, 0.0f, 0.0f, 0.0f};
    glm::vec3 scale{1.0f, 1.0f, 1.0f};
};