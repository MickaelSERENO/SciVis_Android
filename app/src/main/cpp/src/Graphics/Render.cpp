#include "Graphics/Render.h"
#include "Graphics/Drawable.h"

namespace sereno
{
    Render::Render() : m_cameraMatrix(1.0f)
    {}

    Render::~Render()
    {}

    void Render::render()
    {
        for(Drawable* d : m_currentDrawable)
            d->draw(m_cameraMatrix);
    }

    void Render::addToDraw(Drawable* d)
    {
        m_currentDrawable.push_back(d);
    }
}
