#include "GLSurfaceViewData.h"

namespace sereno
{
    GLSurfaceViewData::GLSurfaceViewData(const std::string& _dataPath) : dataPath(_dataPath), renderer(this)
    {
        m_mutex = PTHREAD_MUTEX_INITIALIZER;
    }

    GLSurfaceViewData::~GLSurfaceViewData()
    {
        pthread_mutex_destroy(&m_mutex);
        for(Event* ev : m_events)
            delete ev;
    }

    void GLSurfaceViewData::changeSurfaceSize(uint32_t width, uint32_t height)
    {
        Event* ev            = new Event;
        ev->type             = RESIZE;
        ev->sizeEvent.width  = width;
        ev->sizeEvent.height = height;
        addEvent(ev);
    }

    Event* GLSurfaceViewData::pollEvent()
    {
        Event* ev = NULL;
        pthread_mutex_lock(&m_mutex);
        {
            if(m_events.size() > 0)
            {
                ev = m_events.front();
                m_events.pop_front();
            }
        }
        pthread_mutex_unlock(&m_mutex);

        return ev;
    }

    void GLSurfaceViewData::addEvent(Event* ev)
    {
        pthread_mutex_lock(&m_mutex);
            m_events.push_back(ev);
        pthread_mutex_unlock(&m_mutex);
    }
}
