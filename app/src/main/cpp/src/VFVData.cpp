#include "VFVData.h"

namespace sereno
{
    VFVData::VFVData()
    {
        m_mutex = PTHREAD_MUTEX_INITIALIZER;
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

    void VFVData::addEvent(VFVEvent* ev)
    {
        pthread_mutex_lock(&m_mutex);
            m_events.push_back(ev);
        pthread_mutex_unlock(&m_mutex);
    }
}
