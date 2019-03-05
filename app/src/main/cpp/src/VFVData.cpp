#include <jniData.h>
#include "VFVData.h"
#include "utils.h"
#include "../../../../../../../../../../../home/mickael/.local/android/x86/include/Datasets/SubDataset.h"

namespace sereno
{
    VFVData::VFVData(jobject javaObj)
    {

        m_mutex = PTHREAD_MUTEX_INITIALIZER;
        m_javaObj = javaObj;
    }

    VFVData::~VFVData()
    {
        pthread_mutex_destroy(&m_mutex);
        for(VFVEvent* ev : m_events)
            delete ev;
        for(auto& it : m_jSubDatasetMap)
        {
            JNIEnv* env;
            if (javaVM->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK)
                return;
            env->DeleteGlobalRef(it.second);
        }
    }

    void VFVData::setCallback(IVFVCallback* clbk)
    {
        m_clbk = clbk;
    }

    void VFVData::addBinaryData(std::shared_ptr<BinaryDataset> dataset, const std::vector<jobject>& jSubDatasets)
    {
        VFVEvent* ev = new VFVEvent(VFV_ADD_BINARY_DATA);
        ev->binaryData.dataset = dataset;

        pthread_mutex_lock(&m_mutex);
        {
            m_datas.push_back(dataset);
            for(int i = 0; i < jSubDatasets.size(); i++)
                m_jSubDatasetMap.insert(std::pair<SubDataset*, jobject>(dataset->getSubDataset(i), jSubDatasets[i]));
        }
        pthread_mutex_unlock(&m_mutex);
        addEvent(ev);
    }

    void VFVData::addVTKData(std::shared_ptr<VTKDataset> dataset, const std::vector<jobject>& jSubDatasets)
    {
        VFVEvent* ev = new VFVEvent(VFV_ADD_VTK_DATA);
        ev->vtkData.dataset = dataset;

        pthread_mutex_lock(&m_mutex);
        {
            m_datas.push_back(dataset);
            for(int i = 0; i < jSubDatasets.size(); i++)
                m_jSubDatasetMap.insert(std::pair<SubDataset*, jobject>(dataset->getSubDataset(i), jSubDatasets[i]));
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

    void VFVData::onRangeColorChange(float min, float max, ColorMode mode, SubDataset* sd)
    {
        VFVEvent* ev = NULL;
        ev = new VFVEvent(VFV_COLOR_RANGE_CHANGED);
        ev->sdEvent.sd = sd;

        addEvent(ev);
    }

    void VFVData::onRotationChange(SubDataset* data)
    {
        VFVEvent* ev = NULL;
        ev = new VFVEvent(VFV_SET_ROTATION_DATA);
        ev->sdEvent.sd = data;

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


    /*----------------------------------------------------------------------------*/
    /*----------------------------Send events to Java-----------------------------*/
    /*----------------------------------------------------------------------------*/
    void VFVData::sendRotationEvent(JNIEnv* env, SubDataset* sd, float roll, float pitch, float yaw)
    {
        jvalue val[] = {{.f = roll},
                        {.f = pitch},
                        {.f = yaw}};

        env->CallVoidMethodA(m_jSubDatasetMap[sd], jSubDataset_onRotationEvent, val);
    }
}
