#include "Datasets/DatasetMetaData.h"

namespace sereno
{
    DatasetMetaData::DatasetMetaData(std::shared_ptr<Dataset> ptr, jobject jVTK) : m_dataset(ptr), m_jDataset(0)
    {
        //Create a new reference for the Java object
        bool shouldDetach; 
        JNIEnv* env = getJNIEnv(&shouldDetach);
        if(env)
        {
            m_jDataset = env->NewGlobalRef(jVTK);
            if(shouldDetach)
                javaVM->DetachCurrentThread();
        }
    }

    DatasetMetaData::DatasetMetaData(const DatasetMetaData& copy)
    {
        *this = copy;
    }

    DatasetMetaData::DatasetMetaData(DatasetMetaData&& mvt)
    {
        m_dataset = std::move(mvt.m_dataset);
        m_1dHistograms = std::move(mvt.m_1dHistograms);
        m_2dHistograms = std::move(mvt.m_2dHistograms);

        if(mvt.m_jDataset)
        {
            m_jDataset = mvt.m_jDataset;
            mvt.m_jDataset = 0;
        }
    }

    DatasetMetaData& DatasetMetaData::operator=(const DatasetMetaData& copy)
    {
        if(this != &copy)
        {
            m_dataset = copy.m_dataset;
            m_1dHistograms = copy.m_1dHistograms;
            m_2dHistograms = copy.m_2dHistograms;
            m_jDataset = 0;
            if(copy.m_jDataset != 0)
            {
                //Create a new reference for the Java object
                bool shouldDetach; 
                JNIEnv* env = getJNIEnv(&shouldDetach);
                if(env)
                {
                    m_jDataset = env->NewGlobalRef(copy.m_jDataset);
                    if(shouldDetach)
                        javaVM->DetachCurrentThread();
                }
            }
        }
        return *this;
    }

    DatasetMetaData::~DatasetMetaData()
    {
        //Delete the reference of the Java object
        if(m_jDataset != 0)
        {
            bool shouldDetach; 
            JNIEnv* env = getJNIEnv(&shouldDetach);
            if(env)
            {
                env->DeleteGlobalRef(m_jDataset);
                if(shouldDetach)
                    javaVM->DetachCurrentThread();
            }
        }
    }
}
