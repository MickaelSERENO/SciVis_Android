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

    void VFVData::addBinaryData(std::shared_ptr<BinaryDataset> dataset)
    {
        VFVEvent* ev = new VFVEvent(VFV_ADD_BINARY_DATA);
        ev->binaryData.dataset = dataset;

        pthread_mutex_lock(&m_mutex);
        {
            if(m_currentData == NULL)
            {
                m_currentData       = dataset;
                m_currentSubDataIdx = 0;
            }

            m_datas.push_back(dataset);
        }
        pthread_mutex_unlock(&m_mutex);
        addEvent(ev);
    }

    void VFVData::addVTKData(std::shared_ptr<VTKDataset> dataset)
    {
        VFVEvent* ev = new VFVEvent(VFV_ADD_VTK_DATA);
        ev->vtkData.dataset = dataset;

        pthread_mutex_lock(&m_mutex);
        {
            if(m_currentData == NULL)
            {
                m_currentData       = dataset;
                m_currentSubDataIdx = 0;
            }

            m_datas.push_back(dataset);
        }

        pthread_mutex_unlock(&m_mutex);
        addEvent(ev);
    }

    void VFVData::removeData(int dataID)
    {
        //TODO
    }

    void VFVData::setCurrentData(int dataID)
    {
        //TODO
    }

    void VFVData::onRangeColorChange(float min, float max, ColorMode mode)
    {
        VFVEvent* ev = NULL;
        pthread_mutex_lock(&m_mutex);
        {
            if(m_currentData)
            {
                ev = new VFVEvent(VFV_COLOR_RANGE_CHANGED);
                ev->colorRange.min  = min;
                ev->colorRange.max  = max;
                ev->colorRange.mode = mode;
                ev->colorRange.currentData = m_currentData;
                ev->colorRange.subDataID   = m_currentSubDataIdx;
                m_currentData->getSubDataset(m_currentSubDataIdx)->setColor(min, max, mode);
            }
        }
        pthread_mutex_unlock(&m_mutex);

        addEvent(ev);
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
        LOG_INFO("New event added\n");
        pthread_mutex_lock(&m_mutex);
            m_events.push_back(ev);
        pthread_mutex_unlock(&m_mutex);
    }
}
