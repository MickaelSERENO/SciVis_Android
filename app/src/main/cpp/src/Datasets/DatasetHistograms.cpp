#include "Datasets/DatasetHistograms.h"

namespace sereno
{
    DatasetHistograms::DatasetHistograms(std::shared_ptr<Dataset> ptr, jobject jVTK) : m_dataset(ptr), m_jDataset(0)
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

    DatasetHistograms::DatasetHistograms(const DatasetHistograms& copy)
    {
        *this = copy;
    }

    DatasetHistograms::DatasetHistograms(DatasetHistograms&& mvt)
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

    DatasetHistograms& DatasetHistograms::operator=(const DatasetHistograms& copy)
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

    DatasetHistograms::~DatasetHistograms()
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
