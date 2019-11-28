#define GLM_FORCE_RADIANS
#include "Graphics/Render.h"
#include "Graphics/Drawable.h"

namespace sereno
{
    Render::Render() : m_cameraMatrix(1.0f), m_projMatrix(1.0f), m_cameraParams(0.0f, 0.0f, 0.0f, 1.0f)
    {}

    Render::~Render()
    {}

    void Render::render()
    {
        glViewport(m_viewport.x, m_viewport.y, m_viewport.width, m_viewport.height);
        for(Drawable* d : m_currentDrawable)
            d->draw(*this);

        for(Drawable* d : m_currentDrawable)
            d->postDraw(*this);
        m_currentDrawable.clear();
    }

    void Render::addToDraw(Drawable* d)
    {
        m_currentDrawable.push_back(d);
    }

    void Render::setOrthographicMatrix(float left, float right, float bottom, float top, float near, float far, bool rh)
    {
        m_projMatrix     = glm::ortho(left, right, bottom, top, near, far);
        if(!rh)
            glm::value_ptr(m_projMatrix)[10] *= -1.0f;
        m_cameraParams.w = 1.0f;
    }

    void Render::setPerspectiveMatrix(float fovY, float aspect, float near, float far, bool rh)
    {
        if(rh)
            m_projMatrix = glm::perspective(fovY, aspect, near, far);
        else
        {
            float f = cos(fovY/2.0f)/sin(fovY/2.0f);

            float  persp[16] = {f/aspect, 0, 0,               0,
                                0,        f, 0,               0,
                                0,        0, far/(far-near),  1,
                                0,        0, -2*far*near/(far-near),               0};

            float* proj = glm::value_ptr(m_projMatrix);
            for(int i = 0; i < 16; i++)
                proj[i] = persp[i];
        }
        m_cameraParams.w = 0.0f;
    }
}
