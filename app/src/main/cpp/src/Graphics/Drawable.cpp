#include "Graphics/Drawable.h"
#include "Graphics/Render.h"

namespace sereno
{
    Drawable::Drawable(GLRenderer* glRenderer, Material* mtl) : m_glRenderer(glRenderer), m_mtl(mtl)
    {}

    Drawable::~Drawable()
    {}

    void Drawable::draw(const Render& render)
    {}

    void Drawable::postDraw(const Render& render)
    {}

    void Drawable::setMaterial(Material* mtl)
    {
        m_mtl = mtl;
    }
}
