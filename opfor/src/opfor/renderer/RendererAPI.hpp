#pragma once

#include "opfor/core/base.hpp"
#include <array>
#include <glm/glm.hpp>
#include <optional>

namespace opfor
{

// Forward Declarations
// ====================
class VertexArray;
class Framebuffer;
class Texture;
class Shader;

enum class RendererCaps;
enum class CopyTarget;
enum class TextureUnit;
enum class ClearFlag;

class RendererAPI
{
  public:
    enum class API
    {
        None = 0,
        OpenGL = 1,
    };

  private:
    static API _API;

  public:
    virtual ~RendererAPI() = default;

    virtual void PushViewport(glm::uvec2 pos, glm::uvec2 size) = 0;
    virtual void PopViewport() = 0;

    virtual void Clear(ClearFlag) = 0;
    virtual void SetClearColor(std::array<float, 4>) = 0;

    virtual void DrawIndexed(SharedPtr<VertexArray> const &) = 0;

    virtual void PushFramebuffer(SharedPtr<Framebuffer> const &) = 0;
    virtual void PopFramebuffer() = 0;

    virtual void CopyFramebufferToDefaultFramebuffer(SharedPtr<Framebuffer> const &, CopyTarget) const = 0;
    virtual void CopyFramebufferToFramebuffer(SharedPtr<Framebuffer> const &dst, SharedPtr<Framebuffer> &src,
                                              CopyTarget) const = 0;
    virtual void CopyDefaultFramebufferTo(SharedPtr<Framebuffer> const &, CopyTarget) const = 0;

    virtual void PushCapability(RendererCaps, bool enable) = 0;
    virtual void PopCapability(RendererCaps) = 0;

    virtual void PushTexture(SharedPtr<Texture> const &, TextureUnit) = 0;
    virtual void PopTexture(TextureUnit) = 0;

    virtual void PushShader(SharedPtr<Shader> const &) = 0;
    virtual void PopShader() = 0;

    virtual void SetUniform(std::string const &name, int32_t value) = 0;
    virtual void SetUniform(std::string const &name, uint32_t value) = 0;
    virtual void SetUniform(std::string const &name, float value) = 0;
    virtual void SetUniform(std::string const &name, glm::vec3 value) = 0;
    virtual void SetUniform(std::string const &name, glm::vec4 value) = 0;
    virtual void SetUniform(std::string const &name, glm::mat3 value) = 0;
    virtual void SetUniform(std::string const &name, glm::mat4 value) = 0;
    virtual void SetUniform(std::string const &name, std::vector<glm::mat4> matrices,
                            std::optional<size_t> size = std::nullopt) = 0;
    virtual void SetUniform(std::string const &name, std::vector<glm::vec3> vectors,
                            std::optional<size_t> size = std::nullopt) = 0;

    virtual void SetDepthMask(bool val) = 0;

    inline static API GetAPI()
    {
        return _API;
    }
};

} // namespace opfor
