//#include <Windows.h>
#include "OpenGLFramebuffer.hpp"
#include "OpenGLConversions.hpp"
#include "opfor/core/Application.hpp"
#include "opfor/renderer/Framebuffer.hpp"
#include "opfor/renderer/Renderbuffer.hpp"
#include "opfor/renderer/Texture.hpp"

namespace opfor
{

OpenGLFramebuffer::OpenGLFramebuffer()
{
    glGenFramebuffers(1, &_RendererID);
}

OpenGLFramebuffer::~OpenGLFramebuffer()
{
    glDeleteFramebuffers(1, &_RendererID);
}

void OpenGLFramebuffer::Bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, _RendererID);
}

void OpenGLFramebuffer::Unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLFramebuffer::AttachTexture(SharedPtr<Texture> texture, FramebufferAttachment attachment)
{
    glBindFramebuffer(GL_FRAMEBUFFER, _RendererID);

    switch (attachment)
    {
    case FramebufferAttachment::ColorAttachment0:
    case FramebufferAttachment::ColorAttachment1:
    case FramebufferAttachment::ColorAttachment2:
    case FramebufferAttachment::ColorAttachment3:
    case FramebufferAttachment::ColorAttachment4:
    case FramebufferAttachment::ColorAttachment5:
    case FramebufferAttachment::ColorAttachment6:
    case FramebufferAttachment::ColorAttachment7:
    case FramebufferAttachment::ColorAttachment8:
    case FramebufferAttachment::ColorAttachment9:
    case FramebufferAttachment::ColorAttachment10:
    case FramebufferAttachment::ColorAttachment11:
    case FramebufferAttachment::ColorAttachment12:
    case FramebufferAttachment::ColorAttachment13:
    case FramebufferAttachment::ColorAttachment14:
    case FramebufferAttachment::ColorAttachment15:
        glFramebufferTexture2D(GL_FRAMEBUFFER, ToGlColorAttachmentEnum(attachment), GL_TEXTURE_2D,
                               texture->GetRawHandle(), 0);
        _Attachments.insert(attachment);
        UpdateDrawBuffers();
        break;
    case FramebufferAttachment::DepthAttachment:
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture->GetRawHandle(), 0);
        // glReadBuffer(GL_NONE);
        // glDrawBuffer(GL_NONE);
        break;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    _Textures.push_back(texture);
}

void OpenGLFramebuffer::AttachRenderbuffer(SharedPtr<Renderbuffer> renderbuffer, RenderbufferAttachment attachment)
{
    glBindFramebuffer(GL_FRAMEBUFFER, _RendererID);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, ToGlRenderbufferAttachment(attachment), GL_RENDERBUFFER,
                              renderbuffer->GetRawHandle());

    _Renderbuffers.push_back(renderbuffer);
}

void OpenGLFramebuffer::UpdateDrawBuffers() const
{
    std::vector<GLuint> attachments;

    attachments.reserve(_Attachments.size());

    for (auto const &attach : _Attachments)
    {
        if (attach >= FramebufferAttachment::ColorAttachment0 && attach <= FramebufferAttachment::ColorAttachment15)
            attachments.push_back(ToGlColorAttachmentEnum(attach));
    }

    if (attachments.size() > 0)
    {
        glDrawBuffers(static_cast<GLsizei>(attachments.size()), attachments.data());
    }
}

bool OpenGLFramebuffer::IsComplete() const
{
    return glCheckFramebufferStatus(GL_FRAMEBUFFER);
}

void OpenGLFramebuffer::CopyToDefault(CopyTarget target)
{
    GLint readBuffer = 0;
    GLint drawBuffer = 0;

    // Save currently bound framebuffers
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &readBuffer);
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &drawBuffer);

    if (target == CopyTarget::DepthBufferBit)
    {

        int width = Application::Get().GetWindow()->GetWidth();
        int height = Application::Get().GetWindow()->GetHeight();

        glBindFramebuffer(GL_READ_FRAMEBUFFER, _RendererID);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    }

    // Restore framebuffers
    glBindFramebuffer(GL_READ_FRAMEBUFFER, readBuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, drawBuffer);
}

void OpenGLFramebuffer::CopyTo(CopyTarget target, Framebuffer &dst)
{
    GLint readBuffer = 0;
    GLint drawBuffer = 0;

    // Save currently bound framebuffers
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &readBuffer);
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &drawBuffer);

    if (target == CopyTarget::DepthBufferBit)
    {

        int width = Application::Get().GetWindow()->GetWidth();
        int height = Application::Get().GetWindow()->GetHeight();

        glBindFramebuffer(GL_READ_FRAMEBUFFER, _RendererID);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst.GetRawHandle());
        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    }

    // Restore framebuffers
    glBindFramebuffer(GL_READ_FRAMEBUFFER, readBuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, drawBuffer);
}

void OpenGLFramebuffer::CopyDefault(CopyTarget target)
{
    GLint readBuffer = 0;
    GLint drawBuffer = 0;

    // Save currently bound framebuffers
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &readBuffer);
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &drawBuffer);

    int width = Application::Get().GetWindow()->GetWidth();
    int height = Application::Get().GetWindow()->GetHeight();

    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, _RendererID);

    if (target == CopyTarget::DepthBufferBit)
    {
        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    }
    else if (target == CopyTarget::ColorBufferBit)
    {
        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    }

    // Restore framebuffers
    glBindFramebuffer(GL_READ_FRAMEBUFFER, readBuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, drawBuffer);
}

} // namespace opfor
