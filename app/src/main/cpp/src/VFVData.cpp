#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <jniData.h>
#include "VFVData.h"
#include "utils.h"
#include "Datasets/SubDataset.h"
#include "Datasets/nativeSubDataset.h"

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

        for(auto& it : m_sdMetaDatas)
            delete it.second;
    }

    void VFVData::setCallback(IVFVCallback* clbk)
    {
        m_clbk = clbk;
    }

    void VFVData::updateHeadsetsStatus(std::shared_ptr<std::vector<HeadsetStatus>> status)
    {
        lock();
            m_headsetsStatus = status;
        unlock();
    }

    void VFVData::addBinaryData(std::shared_ptr<BinaryDataset> dataset)
    {
        VFVEvent* ev = new VFVEvent(VFV_ADD_BINARY_DATA);
        ev->binaryData.dataset = dataset;

        lock();
        {
            m_datas.push_back(dataset);
        }
        unlock();
        addEvent(ev);
    }

    void VFVData::addVTKData(std::shared_ptr<VTKDataset> dataset)
    {
        VFVEvent* ev = new VFVEvent(VFV_ADD_VTK_DATA);
        ev->vtkData.dataset = dataset;

        lock();
        {
            m_datas.push_back(dataset);
        }
        unlock();
        addEvent(ev);
    }

    void VFVData::removeData(int dataID)
    {
        //TODO
    }

    void VFVData::setCurrentSubDataset(SubDataset* sd)
    {
        VFVEvent* ev = NULL;
        ev = new VFVEvent(VFV_SET_CURRENT_DATA);
        ev->sdEvent.sd = sd;

        addEvent(ev);
    }

    void VFVData::setCurrentAction(VFVCurrentAction a)
    {
        jniMainThread->CallVoidMethod(m_javaObj, jVFVSurfaceView_setCurrentAction, (int)a);
    }

    void VFVData::onClampingChange(float min, float max, SubDataset* sd)
    {
        addSubDatasetEvent(sd, VFV_COLOR_RANGE_CHANGED);
    }

    void VFVData::onRotationChange(SubDataset* data)
    {
        addSubDatasetEvent(data, VFV_SET_ROTATION_DATA);
    }

    void VFVData::onPositionChange(SubDataset* data)
    {
        addSubDatasetEvent(data, VFV_SET_POSITION_DATA);
    }

    void VFVData::onScaleChange(SubDataset* data)
    {
        addSubDatasetEvent(data, VFV_SET_SCALE_DATA);
    }

    void VFVData::addSubDatasetEvent(SubDataset* sd, VFVEventType type)
    {
        VFVEvent* ev = NULL;
        ev = new VFVEvent(type);
        ev->sdEvent.sd = sd;

        addEvent(ev);
    }

    VFVEvent* VFVData::pollEvent()
    {
        VFVEvent* ev = NULL;
        lock();
        {
            if(m_events.size() > 0)
            {
                ev = m_events.front();
                m_events.pop_front();
            }
        }
        unlock();

        return ev;
    }

    void VFVData::addEvent(VFVEvent* ev)
    {
        lock();
            m_events.push_back(ev);
        unlock();
    }

    void VFVData::addSubDatasetMetaData(const SubDatasetMetaData& metaData, jobject publicJObjectSD, jobject privateJObjectSD)
    {
        lock();
            SubDatasetMetaData* m = new SubDatasetMetaData(metaData);
            m_sdMetaDatas.insert(std::pair<const SubDataset*, SubDatasetMetaData*>(m->getPublicSubDataset(), m));
            m_sdMetaDatas.insert(std::pair<const SubDataset*, SubDatasetMetaData*>(m->getPrivateSubDataset(), m));
            m_jSubDatasetMap.insert(std::pair<SubDataset*, jobject>(m->getPublicSubDataset(), publicJObjectSD));
            m_jSubDatasetMap.insert(std::pair<SubDataset*, jobject>(m->getPrivateSubDataset(), privateJObjectSD));

        unlock();
    }

    const SubDatasetMetaData* VFVData::getSubDatasetMetaData(const SubDataset* sd) const
    {
        auto it = m_sdMetaDatas.find(sd);
        if(it != m_sdMetaDatas.end())
            return it->second;
        return NULL;
    }

    SubDatasetMetaData* VFVData::getSubDatasetMetaData(const SubDataset* sd)
    {
        auto it = m_sdMetaDatas.find(sd);
        if(it != m_sdMetaDatas.end())
            return it->second;
        return NULL;
    }

    bool VFVData::setSubDatasetVisibility(const SubDataset* sd, int visibility)
    {
        SubDatasetMetaData* metaData = getSubDatasetMetaData(sd);
        if(metaData)
        {
            metaData->setVisibility(visibility);
            VFVEvent* ev = new VFVEvent(VFV_SET_VISIBILITY_DATA);
            ev->sdEvent.sd = metaData->getCurrentState();
            addEvent(ev);
        }

        return metaData != NULL;
    }

    /*----------------------------------------------------------------------------*/
    /*----------------------------Send events to Java-----------------------------*/
    /*----------------------------------------------------------------------------*/

    void VFVData::sendRotationEvent(SubDataset* sd)
    {
        //Create the quaternion array
        jfloatArray arr = jniMainThread->NewFloatArray(4);
        Quaternionf q   = sd->getGlobalRotate();
        float qArr[4] = {q.w, q.x, q.y, q.z};
        jniMainThread->SetFloatArrayRegion(arr, 0, 4, qArr);

        jniMainThread->CallVoidMethod(m_jSubDatasetMap[sd], jSubDataset_setRotation, arr);
        jniMainThread->DeleteLocalRef(arr);
    }

    void VFVData::sendPositionEvent(SubDataset* sd)
    {
        //Create the quaternion array
        jfloatArray arr = jniMainThread->NewFloatArray(3);
        jniMainThread->SetFloatArrayRegion(arr, 0, 3, glm::value_ptr(sd->getPosition()));

        jniMainThread->CallVoidMethod(m_jSubDatasetMap[sd], jSubDataset_setPosition, arr);
        jniMainThread->DeleteLocalRef(arr);
    }

    void VFVData::sendScaleEvent(SubDataset* sd)
    {
        //Create the quaternion array
        jfloatArray arr = jniMainThread->NewFloatArray(3);
        jniMainThread->SetFloatArrayRegion(arr, 0, 3, glm::value_ptr(sd->getScale()));

        jniMainThread->CallVoidMethod(m_jSubDatasetMap[sd], jSubDataset_setScale, arr);
        jniMainThread->DeleteLocalRef(arr);
    }

    void VFVData::sendSnapshotEvent(SubDataset* sd)
    {
        jobject bitmap = Java_com_sereno_vfv_Data_SubDataset_nativeGetSnapshot(jniMainThread, NULL, (jlong)sd);
        jniMainThread->CallVoidMethod(m_jSubDatasetMap[sd], jSubDataset_onSnapshotEvent, bitmap);
        jniMainThread->DeleteLocalRef(bitmap);
    }

}
