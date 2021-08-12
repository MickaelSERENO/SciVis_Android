#ifndef  DATASETHISTOGRAMS_INC
#define  DATASETHISTOGRAMS_INC

#include <memory>
#include <cstdint>
#include <vector>
#include "jniData.h"
#include "Graphics/Texture.h"
#include "Datasets/Dataset.h"

namespace sereno
{
    /** \brief 2D Histogram data of a dataset*/
    struct Dataset2DHistogram
    {
        std::shared_ptr<Texture2D> texture;    /*!< The generated 2D Texture*/
        std::shared_ptr<Texture2D> pcpTexture; /*!< The continuous parallel coordinate plot Texture*/
        uint32_t ptFieldID1;                   /*!< The point field ID of X axis*/
        uint32_t ptFieldID2;                   /*!< The point field ID of Y axis*/
    };

    /** \brief  1D Histogram data of a dataset */
    struct Dataset1DHistogram
    {
        std::shared_ptr<Texture2D> texture; /*!< The generated 2D (in fact 1D: height==1) texture*/
        uint32_t ptFieldID;                 /*!< The associated point field*/
    };

    /** \brief  Class containing histograms related to a Dataset */
    class DatasetHistograms
    {
        public:
            /* \brief  Constructor
             * \param dataset The related Dataset
             * \param jDataset the jni Dataset object */
            DatasetHistograms(std::shared_ptr<Dataset> dataset, jobject jDataset);

            /* \brief  Movement constructor
             * \param mvt the object to move */
            DatasetHistograms(DatasetHistograms&& mvt);

            /* \brief  Copy constructor
             * \param copy the object to copy */
            DatasetHistograms(const DatasetHistograms& copy);

            /* \brief  Copy assignment
             * \param copy the object to copy
             * \return   reference to this */
            DatasetHistograms& operator=(const DatasetHistograms& copy);

            /** \brief  Destructor */
            virtual ~DatasetHistograms();

            /** \brief  Get the bound Dataset
             * \return  the bound Dataset smart pointer*/
            std::shared_ptr<Dataset> getDataset()              {return m_dataset;}

            /** \brief  Get the bound Dataset
             * \return  the bound Dataset smart pointer*/
            const std::shared_ptr<Dataset>& getDataset() const {return m_dataset;}

            /** \brief  Add a 1D Histogram for this Dataset
             * \param hist the histogram to add */
            void add1DHistogram(Dataset1DHistogram& hist)
            {
                m_1dHistograms.push_back(hist);
            }

            /** \brief  Add a 2D Histogram to this Dataset
             * \param hist the histogram to add */
            void add2DHistogram(Dataset2DHistogram& hist)
            {
                m_2dHistograms.push_back(hist);
            }

            /** \brief  Get the list of 1D Histograms registered
             * \return  The 1D Histograms registered */
            const std::vector<Dataset1DHistogram>& get1DHistograms() const
            {
                return m_1dHistograms;
            }

            /** \brief  Get the list of 2D Histograms registered
             * \return  The 2D Histograms registered */
            const std::vector<Dataset2DHistogram>& get2DHistograms() const
            {
                return m_2dHistograms;
            }

            /** \brief  Get the Java object bound to this Dataset
             * \return   the Java Obj */
            jobject getJavaDatasetObj() const
            {
                return m_jDataset;
            }
        private:
            std::shared_ptr<Dataset> m_dataset;                   /*!< The Dataset possessing this metadata*/
            std::vector<Dataset1DHistogram> m_1dHistograms; /*!< List of generated 1D histograms*/
            std::vector<Dataset2DHistogram> m_2dHistograms; /*!< List of generated 2D histograms*/

            jobject m_jDataset;                                   /*!< The Java object corresponding to this Dataset. A global reference is made*/
    };
}

#endif
