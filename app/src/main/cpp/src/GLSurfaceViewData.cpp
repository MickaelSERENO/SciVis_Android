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

    void GLSurfaceViewData::touchEvent(int action, int finger, float x, float y)
    {
        //COmpute the new event
        Event* ev = new Event;
        switch(action)
        {
            case TOUCH_TYPE_DOWN:
                ev->type = TOUCH_DOWN;
                break;
            case TOUCH_TYPE_UP:
                ev->type = TOUCH_UP;
                break;
            case TOUCH_TYPE_MOVE:
                ev->type = TOUCH_MOVE;
                break;
            default:
                ev->type = UNKNOWN;
        }
        ev->touchEvent.id = finger;
        ev->touchEvent.x  = x;
        ev->touchEvent.y = y;

        //Save touch state (old, start position)
        TouchCoord* tc = getTouchCoord(finger);
        if(tc)
        {
            m_touchCoords[finger].type = (action <= 2) ? (TouchType)action : TOUCH_TYPE_DOWN;

            if(action == TOUCH_TYPE_DOWN)
            {
                tc->startX = x;
                tc->startY = y;
                tc->oldX   = x;
                tc->oldY   = y;
                tc->x      = x;
                tc->y      = y;
            }
            else
            {
                tc->oldX = tc->x;
                tc->oldY = tc->y;
                tc->x    = x;
                tc->y    = y;
            }

            //Update event with the previous state (old, start positions)
            ev->touchEvent.startX = tc->startX;
            ev->touchEvent.startY = tc->startY;
            ev->touchEvent.oldX   = tc->oldX;
            ev->touchEvent.oldY   = tc->oldY;
        }

        //If outside the number of fingers, just put every coords at (x, y)
        else
        {
            ev->touchEvent.oldX   = x;
            ev->touchEvent.oldY   = y;
            ev->touchEvent.startX = x;
            ev->touchEvent.startY = y;
        }

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

    TouchCoord* GLSurfaceViewData::getTouchCoord(int id)
    {
        if(id >= 0 && id < MAX_TOUCH_FINGER)
            return m_touchCoords+id;
        return NULL;
    }

    void GLSurfaceViewData::addEvent(Event* ev)
    {
        pthread_mutex_lock(&m_mutex);
            m_events.push_back(ev);
        pthread_mutex_unlock(&m_mutex);
    }
}
