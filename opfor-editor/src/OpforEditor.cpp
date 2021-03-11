#include "SpheresRotateSystem.hpp"
#include "components/ModelComponent.hpp"
#include "components/SkyboxComponent.hpp"
#include "components/TransformComponent.hpp"
#include "opfor.hpp"
#include "opfor/renderer/Model.hpp"
#include "opfor/renderer/ShaderManager.hpp"

#include "editor/EditorInspector.hpp"
#include "editor/EditorLog.hpp"
#include "editor/EditorMenuBar.hpp"
#include "editor/EditorSceneHierarchy.hpp"
#include "editor/EditorViewport.hpp"

class OpforEditor : public opfor::Application
{
  private:
    void LoadModel(std::string const &filename, ModelComponent &modelComponent)
    {
        opfor::Model model{};

        model.LoadFromGLTF(filename);

        auto const &meshes = model.GetMeshes();
        if (!meshes.empty())
        {
            modelComponent.meshes.reserve(meshes.size());
            modelComponent.meshes.insert(modelComponent.meshes.end(), meshes.begin(), meshes.end());
        }
    }

  public:
    OpforEditor() : opfor::Application()
    {
        // opfor::Application::Get().LoadLevel("assets/levels/pbr.level");

        ImGuiLayer::Get().OpenWindow<EditorViewport>();
        ImGuiLayer::Get().OpenWindow<EditorMenuBar>();
        ImGuiLayer::Get().OpenWindow<EditorLog>();
        ImGuiLayer::Get().OpenWindow<EditorSceneHierarchy>();
        ImGuiLayer::Get().OpenWindow<EditorInspector>();

        CreateComponentSystem<SpheresRotateSystem>();

        auto *skyBox = CreateEntity<SkyboxComponent>();
        skyBox->SetName("SkyBox");

        auto *player = opfor::Application::Get().CreateEntity<TransformComponent>();
        player->SetName("Default Player");

        auto [spheresShaderID, spheresShader] = ShaderManager::Get().Create("shaders/basic.glsl");

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
    }
};

namespace opfor
{

UniquePtr<Application> CreateApplication()
{
    return MakeUnique<OpforEditor>();
}

} // namespace opfor
