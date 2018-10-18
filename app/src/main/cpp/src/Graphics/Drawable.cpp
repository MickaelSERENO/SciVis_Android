#include "Graphics/Drawable.h"
#include "Graphics/Render.h"

namespace sereno
{
    Drawable::Drawable(GLRenderer* glRenderer, Material* mtl) : m_glRenderer(glRenderer), m_mtl(mtl)
    {}

    Drawable::~Drawable()
    {}

    void Drawable::draw(const glm::mat4& cameraMat)
    {}

    void Drawable::postDraw(const glm::mat4& cameraMat)
    {}
}
