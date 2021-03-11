#pragma once

#include "components/PlayerCameraComponent.hpp"
#include "components/TransformComponent.hpp"
#include "ecs/System.hpp"
#include "opfor/core/Application.hpp"
#include "opfor/core/Input.hpp"
#include "renderer.hpp"
#include <glm/gtc/matrix_transform.hpp>

class CameraMovementSystem : public ecs::ComponentSystem
{
  private:
    const float _BaseSpeed = 100.0f;
    const float _FastSpeed = 5000.0f;

    float _MoveSpeed = _BaseSpeed;

  public:
    void OnUpdate(float deltaTime) override
    {
        auto mouse = opfor::Input::GetMouseButton(opfor::MouseButton::ButtonRight) == opfor::KeyStatus::Pressed;
        if (!mouse)
        {
            return;
        }

        UpdateLook(deltaTime);
        UpdateMovement(deltaTime);
    }

    void UpdateLook(float dt)
    {
        auto playerCamera = GetEntities<PlayerCameraComponent, TransformComponent>();

        if (playerCamera.size() == 0)
        {
            return;
        }

        PlayerCameraComponent camera = playerCamera[0]->Get<PlayerCameraComponent>();
        TransformComponent transform = playerCamera[0]->Get<TransformComponent>();

        if (camera.useInput == false)
            return;

        glm::vec2 vel = opfor::Input::GetMouseRelativePosition() * dt * 10.0f;

        glm::vec3 up(0.0f, 1.0f, 0.0f);
        glm::mat4 view = glm::lookAt(transform.position, transform.position + transform.direction, up);

        transform.yaw += -vel.x;
        transform.pitch += vel.y;

        if (transform.pitch >= 89.0f)
            transform.pitch = 89.0f;
        else if (transform.pitch <= -89.0f)
            transform.pitch = -89.0f;

        transform.direction.x = cos(glm::radians(transform.yaw)) * cos(glm::radians(transform.pitch));
        transform.direction.y = sin(glm::radians(transform.pitch));
        transform.direction.z = sin(glm::radians(transform.yaw)) * cos(glm::radians(transform.pitch));
        ;

        camera.view = view;
        camera.viewProjection = camera.projection * camera.view;
        playerCamera[0]->Set(camera);
        playerCamera[0]->Set(transform);
    }

    void UpdateMovement(float dt)
    {
        auto playerCamera = GetEntities<PlayerCameraComponent, TransformComponent>();

        if (playerCamera.size() == 0)
        {
            return;
        }

        PlayerCameraComponent camera = playerCamera[0]->Get<PlayerCameraComponent>();
        TransformComponent transform = playerCamera[0]->Get<TransformComponent>();

        if (camera.useInput == false)
            return;

        glm::vec3 front(glm::normalize(transform.direction));
        glm::vec3 right(glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f))));

        // Fast Speed?
        _MoveSpeed =
            opfor::Input::GetKey(opfor::KeyCode::LeftShift) == opfor::KeyStatus::Pressed ? _FastSpeed : _BaseSpeed;

        bool fwd = opfor::Input::GetKey(opfor::KeyCode::W) == opfor::KeyStatus::Pressed;
        bool lft = opfor::Input::GetKey(opfor::KeyCode::A) == opfor::KeyStatus::Pressed;
        bool bck = opfor::Input::GetKey(opfor::KeyCode::S) == opfor::KeyStatus::Pressed;
        bool rgt = opfor::Input::GetKey(opfor::KeyCode::D) == opfor::KeyStatus::Pressed;

        transform.position += fwd * dt * _MoveSpeed * front;
        transform.position += bck * dt * _MoveSpeed * -front;
        transform.position += rgt * dt * _MoveSpeed * right;
        transform.position += lft * dt * _MoveSpeed * -right;
        playerCamera[0]->Set(transform);
    }
};
