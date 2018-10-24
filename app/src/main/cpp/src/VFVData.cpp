#include "VFVData.h"

namespace sereno
{
    VFVData::VFVData()
    {
        m_mutex = PTHREAD_MUTEX_INITIALIZER;
    }

    VFVData::~VFVData()
    {
        pthread_mutex_destroy(&m_mutex);
        for(VFVEvent* ev : m_events)
            delete ev;
    }

    void VFVData::setCallback(IVFVCallback* clbk)
    {
        m_clbk = clbk;
    }

    void VFVData::addData(FluidDataset* dataset)
    {
        VFVEvent* ev = new VFVEvent;
        ev->type = VFV_ADD_DATA;
        ev->fluidData.fluidID = m_datas.size();
        ev->fluidData.dataset = dataset;

        m_datas.push_back(dataset);
        addEvent(ev);
    }

    void VFVData::removeData(int dataID)
    {
        //TODO
    }

    void VFVData::setCurrentData(int dataID)
    {
    }

    VFVEvent* VFVData::pollEvent()
    {
        VFVEvent* ev = NULL;
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

    void VFVData::addEvent(VFVEvent* ev)
    {
        pthread_mutex_lock(&m_mutex);
            m_events.push_back(ev);
        pthread_mutex_unlock(&m_mutex);
    }
}
