#include "opfor.hpp"
#include "opfor/renderer/Model.hpp"
#include "opfor/renderer/ShaderManager.hpp"
#include "components/TransformComponent.hpp"
#include "components/SkyboxComponent.hpp"
#include "components/ModelComponent.hpp"
#include "SpheresRotateSystem.hpp"

#include "editor/EditorViewport.hpp"
#include "editor/EditorMenuBar.hpp"
#include "editor/EditorLog.hpp"
#include "editor/EditorSceneHierarchy.hpp"
#include "editor/EditorInspector.hpp"

struct Container : ecs::IComponent {};

class ContainerSystem : public ecs::ComponentSystem
{
private:
	float totalTime = 0.0f;
	
public:
	void OnUpdate(float deltaTime) override
	{
		totalTime += deltaTime;
		
		auto containers = GetEntities<TransformComponent, Container>();

		for (auto c : containers)
		{
			auto &t = c->Get<TransformComponent>();

			t.position.x = sinf(totalTime) * 1000;
		}
	}
};

class OpforEditor : public opfor::Application
{
private:
	void LoadModel(std::string const &filename, ModelComponent &modelComponent)
	{
		opfor::Model model{};

		model.LoadFromGLTF(filename);

		auto const &meshes = model.GetMeshes();
		if (!meshes.empty()) {
			modelComponent.meshes.reserve(meshes.size());
			modelComponent.meshes.insert(modelComponent.meshes.end(), meshes.begin(), meshes.end());
		}
	}

public:
	OpforEditor() : opfor::Application()
	{
		//opfor::Application::Get().LoadLevel("assets/levels/pbr.level");

		ImGuiLayer::Get().OpenWindow<EditorViewport>();
		ImGuiLayer::Get().OpenWindow<EditorMenuBar>();
		ImGuiLayer::Get().OpenWindow<EditorLog>();
		ImGuiLayer::Get().OpenWindow<EditorSceneHierarchy>();
		ImGuiLayer::Get().OpenWindow<EditorInspector>();

		CreateComponentSystem<SpheresRotateSystem>();
		CreateComponentSystem<ContainerSystem>();

		auto *skyBox = CreateEntity<SkyboxComponent>();
		skyBox->SetName("SkyBox");

		auto *player = opfor::Application::Get().CreateEntity<TransformComponent>();
		player->SetName("Default Player");

		auto [ spheresShaderID, spheresShader ] = ShaderManager::Get().Create("shaders/basic.glsl");

		auto *spheres = CreateEntity<SphereComponent, TransformComponent, ModelComponent>();
		auto &sphereModel = spheres->Get<ModelComponent>();
			LoadModel("./assets/models/metal-sphere/metal-sphere.gltf", sphereModel);
			sphereModel.shader = spheresShaderID;
			spheresShader.Bind();
			spheresShader.SetUniform("material.albedo", 0);
			spheresShader.SetUniform("material.metallicRoughness", 1);
			spheresShader.SetUniform("material.normal", 2);
			spheresShader.SetUniform("material.metallicFactor", 1.0f);
			spheresShader.SetUniform("material.roughnessFactor", 1.0f);
			spheresShader.Unbind();
		spheres->SetName("Spheres");

		auto *container = CreateEntity<TransformComponent, Container>();
		container->SetName("Container");

		auto &containerTransform = container->Get<TransformComponent>();
		spheres->Get<TransformComponent>().parent = &containerTransform;
	}
};

namespace opfor {

UniquePtr<Application> CreateApplication()
{
	return MakeUnique<OpforEditor>();
}

}
