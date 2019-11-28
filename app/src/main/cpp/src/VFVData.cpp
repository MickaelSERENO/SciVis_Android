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

        m_mutex      = PTHREAD_MUTEX_INITIALIZER;
        m_eventMutex = PTHREAD_MUTEX_INITIALIZER;
        m_javaObj = javaObj;
    }

    VFVData::~VFVData()
    {
        pthread_mutex_destroy(&m_mutex);
        for(VFVEvent* ev : m_events)
            delete ev;

        bool shouldDetach;
        JNIEnv* env = getJNIEnv(&shouldDetach);
        if(env)
        {
            for(auto& it : m_jSubDatasetMap)
                env->DeleteGlobalRef(it.second);
            if(shouldDetach)
                javaVM->DetachCurrentThread();
        }
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

    void VFVData::addVTKData(std::shared_ptr<VTKDataset> dataset, jobject jVTK)
    {
        VFVEvent* ev = new VFVEvent(VFV_ADD_VTK_DATA);
        ev->vtkData.dataset = dataset;

        lock();
        {
            m_datas.push_back(dataset);
            m_datasetMetaDatas.insert(std::pair<std::shared_ptr<Dataset>, std::shared_ptr<DatasetMetaData>>(dataset, std::shared_ptr<DatasetMetaData>(new DatasetMetaData(dataset, jVTK))));
        }
        unlock();
        addEvent(ev);
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

    void VFVData::onTFChange(SubDataset* data)
    {
        addSubDatasetEvent(data, VFV_SET_TF_DATA);
    }


    void VFVData::addSubDatasetEvent(SubDataset* sd, VFVEventType type)
    {
        VFVEvent* ev = NULL;
        ev = new VFVEvent(type);
        ev->sdEvent.sd = sd;

        addEvent(ev);
    }

    void VFVData::addDatasetEvent(std::shared_ptr<Dataset> dataset, VFVEventType type)
    {
        VFVEvent* ev = NULL;
        ev = new VFVEvent(type);
        ev->dataset.dataset = dataset;

        addEvent(ev);
    }

    void VFVData::onRemoveSubDataset(SubDataset* sd)
    {
        addSubDatasetEvent(sd, VFV_REMOVE_SUBDATASET);
        lock();
            removeSubDataset(sd);
        unlock();
    }

    void VFVData::removeSubDataset(SubDataset* sd)
    {
        auto it = m_jSubDatasetMap.find(sd);
        if(it != m_jSubDatasetMap.end())
            m_jSubDatasetMap.erase(it);
    }


    void VFVData::onRemoveDataset(std::shared_ptr<Dataset> dataset)
    {
        addDatasetEvent(dataset, VFV_REMOVE_DATASET);
        lock();
            for(uint32_t i = 0; i < dataset->getNbSubDatasets(); i++)
            {
                removeSubDataset(dataset->getSubDataset(i));
            }

            if(m_datas.size() > 0)
            {
                std::vector<std::shared_ptr<Dataset>>::iterator it = m_datas.begin();
                while(it != m_datas.end())
                {
                    if((*it) == dataset)
                        it = m_datas.erase(it);
                    else
                        ++it;
                }
            }
        unlock();
    }

    VFVEvent* VFVData::pollEvent()
    {
        VFVEvent* ev = NULL;
        {
            pthread_mutex_lock(&m_eventMutex);
                if(m_events.size() > 0)
                {
                    ev = m_events.front();
                    m_events.pop_front();
                }
            pthread_mutex_unlock(&m_eventMutex);
        }

        return ev;
    }

    void VFVData::addEvent(VFVEvent* ev)
    {
        pthread_mutex_lock(&m_eventMutex);
            m_events.push_back(ev);
        pthread_mutex_unlock(&m_eventMutex);
    }

    void VFVData::bindSubDatasetJava(SubDataset* sd, jobject publicJObjectSD)
    {
        bool shouldDetach;
        JNIEnv* env = getJNIEnv(&shouldDetach);
        if(env != NULL)
        {
            lock();
                m_jSubDatasetMap.insert(std::pair<SubDataset*, jobject>(sd, env->NewGlobalRef(publicJObjectSD)));
            unlock();
            if(shouldDetach)
                javaVM->DetachCurrentThread();
        }
    }

    void VFVData::addSubDatasetFromJava(SubDataset* sd)
    {
        addSubDatasetEvent(sd, VFV_ADD_SUBDATASET);
    }

    /*----------------------------------------------------------------------------*/
    /*----------------------------Send events to Java-----------------------------*/
    /*----------------------------------------------------------------------------*/

    void VFVData::sendRotationEvent(SubDataset* sd)
    {
        auto it = m_jSubDatasetMap.find(sd);
        if(it != m_jSubDatasetMap.end())
        {
            //Create the quaternion array
            jfloatArray arr = jniMainThread->NewFloatArray(4);
            Quaternionf q   = sd->getGlobalRotate();
            float qArr[4] = {q.w, q.x, q.y, q.z};
            jniMainThread->SetFloatArrayRegion(arr, 0, 4, qArr);

            jniMainThread->CallVoidMethod(it->second, jSubDataset_setRotation, arr);
            jniMainThread->DeleteLocalRef(arr);
        }
    }

    void VFVData::sendPositionEvent(SubDataset* sd)
    {
        auto it = m_jSubDatasetMap.find(sd);
        if(it != m_jSubDatasetMap.end())
        {
            //Create the quaternion array
            jfloatArray arr = jniMainThread->NewFloatArray(3);
            jniMainThread->SetFloatArrayRegion(arr, 0, 3, glm::value_ptr(sd->getPosition()));

            jniMainThread->CallVoidMethod(it->second, jSubDataset_setPosition, arr);
            jniMainThread->DeleteLocalRef(arr);
        }
    }

    void VFVData::sendScaleEvent(SubDataset* sd)
    {
        auto it = m_jSubDatasetMap.find(sd);
        if(it != m_jSubDatasetMap.end())
        {
            //Create the quaternion array
            jfloatArray arr = jniMainThread->NewFloatArray(3);
            jniMainThread->SetFloatArrayRegion(arr, 0, 3, glm::value_ptr(sd->getScale()));

            jniMainThread->CallVoidMethod(it->second, jSubDataset_setScale, arr);
            jniMainThread->DeleteLocalRef(arr);
        }
    }

    void VFVData::sendSnapshotEvent(SubDataset* sd)
    {

        auto it = m_jSubDatasetMap.find(sd);
        if(it != m_jSubDatasetMap.end())
        {
            jobject bitmap = Java_com_sereno_vfv_Data_SubDataset_nativeGetSnapshot(jniMainThread, NULL, (jlong)sd);
            if(bitmap == NULL || m_jSubDatasetMap[sd] == NULL)
                return;
            jniMainThread->CallVoidMethod(it->second, jSubDataset_onSnapshotEvent, bitmap);
            jniMainThread->DeleteLocalRef(bitmap);
        }
    }

    void VFVData::sendOnDatasetLoaded(std::shared_ptr<Dataset> pDataset, bool success)
    {
        std::shared_ptr<DatasetMetaData> metaData = getDatasetMetaData(pDataset);
        if(metaData)
        {
            jniMainThread->CallVoidMethod(metaData->getJavaDatasetObj(), jDataset_onLoadDataset, success);
        }
    }

    void VFVData::sendCPCPTexture(std::shared_ptr<Dataset> pDataset, uint32_t* pixels, uint32_t width, uint32_t height,
                                  uint32_t pIDLeft, uint32_t pIDRight)
    {
        std::shared_ptr<DatasetMetaData> metaData = getDatasetMetaData(pDataset);
        if(metaData)
        {
            jobject bitmap = createjARGBBitmap(pixels, width, height, jniMainThread);
            jniMainThread->CallVoidMethod(metaData->getJavaDatasetObj(), jDataset_onLoadCPCPTexture, bitmap, pIDLeft, pIDRight);
            jniMainThread->DeleteLocalRef(bitmap);
        }
    }

    void VFVData::send1DHistogram(std::shared_ptr<Dataset> pDataset, float* values, uint32_t width, uint32_t pID)
    {
        std::shared_ptr<DatasetMetaData> metaData = getDatasetMetaData(pDataset);
        if(metaData)
        {
            jfloatArray jarr = createjFloatArray(values, width, jniMainThread);
            jniMainThread->CallVoidMethod(metaData->getJavaDatasetObj(), jDataset_onLoad1DHistogram, jarr, pID);
            jniMainThread->DeleteLocalRef(jarr);
        }
    }

}
