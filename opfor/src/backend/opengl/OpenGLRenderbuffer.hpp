#pragma once

#include "opfor/renderer/Renderbuffer.hpp"

namespace opfor
{

class OpenGLRenderbuffer : public Renderbuffer
{
  private:
    uint32_t _RendererID;

  public:
    OpenGLRenderbuffer();
    virtual ~OpenGLRenderbuffer();

    void Bind() const override;
    void Unbind() const override;

    void AttachDepthComponent(std::array<unsigned int, 2> size) override;

    void Resize(unsigned int width, unsigned int height) override;

    uint32_t GetRawHandle() const override
    {
        return _RendererID;
    }
};

} // namespace opfor
