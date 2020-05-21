#pragma once

#include "lazy.hpp"
#include "TextureManager.hpp"
#include "ecs/System.hpp"
#include "components/PlayerCameraComponent.hpp"
#include "components/MeshComponent.hpp"
#include "components/TransformComponent.hpp"
#include "components/SkyboxComponent.hpp"
#include "components/PointLightComponent.hpp"
#include "Engine.hpp"
#include "Framebuffer.hpp"
#include "ShaderManager.hpp"
#include <fmt/format.h>
#include <glm/gtx/projection.hpp>

class MeshRendererSystem : public ecs::ComponentSystem
{
private:
	lazy::graphics::Shader _fbShader;
	lazy::graphics::Shader _billboard;

	engine::Framebuffer _framebuffer;
	lazy::graphics::Mesh _quad;

	void initQuad()
	{
		std::array<glm::vec3, 4> pos = {
			glm::vec3(-1.0f, -1.0f, 0.0f),
			glm::vec3( 1.0f, -1.0f, 0.0f),
			glm::vec3( 1.0f,  1.0f, 0.0f),
			glm::vec3(-1.0f,  1.0f, 0.0f),
		};
		std::array<glm::vec2, 4> tex = {
			glm::vec2(0.0f, 0.0f),
			glm::vec2(1.0f, 0.0f),
			glm::vec2(1.0f, 1.0f),
			glm::vec2(0.0f, 1.0f),
		};

		for (auto p : pos) {
			_quad.addPosition(p);
		}
		for (auto t : tex) {
			_quad.addUv(t);
		}
		_quad.addTriangle(glm::uvec3(0, 1, 2));
		_quad.addTriangle(glm::uvec3(0, 2, 3));
		_quad.build();

	}

	void initFramebuffer()
	{
		initQuad();

		_framebuffer.AttachColorBuffer();
		_framebuffer.AttachDepthBuffer();

		_fbShader.addVertexShader("shaders/fb.vs.glsl")
			.addFragmentShader("shaders/fb.fs.glsl")
			.link();

		_fbShader.bind();
		_fbShader.setUniform1i("screen_texture", 0);
		_fbShader.unbind();

		assert(_fbShader.isValid());
	}

	void initBillboard()
	{
		_billboard.addVertexShader("shaders/billboard.vs.glsl")
			.addFragmentShader("shaders/billboard.fs.glsl")
			.link();
	}

public:
	MeshRendererSystem() : _framebuffer(engine::Framebuffer_Type::RW)
	{
		initFramebuffer();
		initBillboard();

		TextureManager::instance().createTexture("light_bulb_icon", "./img/light_bulb_icon.png", {
			{ GL_TEXTURE_WRAP_R, GL_WRAP_BORDER },
			{ GL_TEXTURE_WRAP_S, GL_WRAP_BORDER },
			{ GL_TEXTURE_MIN_FILTER, GL_NEAREST },
			{ GL_TEXTURE_MAG_FILTER, GL_NEAREST },
		}, GL_TEXTURE_2D);

		assert(_framebuffer.IsComplete());
	}

	void OnUpdate(float __unused deltaTime) override
	{
		auto player = GetEntities<PlayerCameraComponent, TransformComponent>();

		if (player.size() == 0) { return ; }

		auto playerCamera = player[0]->Get<PlayerCameraComponent>();
		auto playerTransform = player[0]->Get<TransformComponent>();

		_framebuffer.Bind();

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);

		RenderMeshes(playerCamera, playerTransform);
		RenderSkybox(playerCamera);
		RenderLightBillboard(playerCamera);

		_framebuffer.Unbind();


		glClearColor(1.0f, 0.0, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _framebuffer.GetColorTexture());
		_fbShader.bind();
		_quad.draw();
		_fbShader.unbind();
	}

	void RenderMeshes(PlayerCameraComponent &camera, TransformComponent &playerTransform)
	{
		auto meshes = GetEntities<MeshComponent, TransformComponent>();

		for (auto m : meshes) {
			auto [ meshId, shaderId ] = m->Get<MeshComponent>();
			auto data = engine::Engine::Instance().GetMesh(meshId);
			auto &transform = m->Get<TransformComponent>();

			auto shader = ShaderManager::instance().Get(shaderId).value();
			shader->bind();
			shader->setUniform4x4f("viewProjectionMatrix", camera.viewProjection);
			shader->setUniform4x4f("viewMatrix", camera.view);
			shader->setUniform4x4f("projectionMatrix", camera.projection);
			shader->setUniform3f("viewPos", playerTransform.position);

			UpdateLight(*shader);

			glm::mat4 model(1.0f);
			model = glm::scale(model, transform.scale);
			model = glm::translate(model, transform.position);
			shader->setUniform4x4f("modelMatrix", model);

			// Bind each texture
			size_t currentTexture = 0;
			for (auto &t : data->getTextures()) {
				TextureManager::instance().bind(t, currentTexture);
				currentTexture++;
			}

			data->draw();
			shader->unbind();
		}
	}

	void RenderSkybox(PlayerCameraComponent &camera)
	{
		auto skybox = GetEntities<MeshComponent, SkyboxComponent>();

		if (skybox.size() == 0) { return ; }

		auto [ meshComponent, _ ] = skybox[0]->GetAll();
		auto mesh = engine::Engine::Instance().GetMesh(meshComponent.Id);

		auto shader = ShaderManager::instance().Get(meshComponent.Shader).value();

		shader->bind();
		shader->setUniform4x4f("viewMatrix", glm::mat4(glm::mat3(camera.view)));
		shader->setUniform4x4f("projectionMatrix", camera.projection);

		glDepthMask(GL_FALSE);
		TextureManager::instance().bind("skybox-cubemap", 0);
		mesh->draw();
		glDepthMask(GL_TRUE);

		shader->unbind();
	}

	void UpdateLight(lazy::graphics::Shader &shader)
	{
		auto lights = GetEntities<PointLightComponent, TransformComponent>();

		if (lights.size() == 0) { return ; }

		auto [ light, transform ] = lights[0]->GetAll();

		TransformComponent t(transform);

		t.position = glm::vec3(cos(glfwGetTime()) * 1000.0f, 100.0f, 0.0f);
		lights[0]->Set(t);

		shader.setUniform3f("lightColor", light.Color);
		shader.setUniform3f("lightDir", light.Dir);
		shader.setUniform3f("lightPos", transform.position);
	}

	void RenderLightBillboard(PlayerCameraComponent &camera)
	{
		auto lights = GetEntities<PointLightComponent, TransformComponent>();

		if (lights.size() == 0 ) { return ; }

		auto [ light, transform ] = lights[0]->GetAll();

		_billboard.bind();
		_billboard.setUniform4x4f("viewMatrix", camera.view);
		_billboard.setUniform4x4f("viewProjectionMatrix", camera.viewProjection);
		_billboard.setUniform4x4f("projectionMatrix", camera.projection);
		_billboard.setUniform3f("particlePosition", transform.position);

		TextureManager::instance().bind("light_bulb_icon", 0);
		_quad.draw();

		_billboard.unbind();
	}
};