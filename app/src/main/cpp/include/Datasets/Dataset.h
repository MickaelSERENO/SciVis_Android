#ifndef  DATASET_INC
#define  DATASET_INC

#include "Datasets/SubDataset.h"
#include <vector>
#include <cstdint>

namespace sereno
{
    /** \brief  Dataset class. */
    class Dataset
    {
        public:
            /** \brief  Default constructor */
            Dataset(){}

            /* \brief  Copy constructor
             * \param copy argument to copy*/
            Dataset(const Dataset& copy);

            /* \brief  Movement constructor
             * \param mvt the object to move */
            Dataset(Dataset&& mvt);

            /* \brief  Copy operator
             * \param copy the argument to copy
             * \return   *this */
            Dataset& operator=(const Dataset& copy);

            /** \brief  Destructor */
            virtual ~Dataset();

            /**
             * \brief  Get the subdataset indice i
             * \param i the indice of the subdataset required
             * \return  subDatasets[i] or NULL if i is out of range
             */
            SubDataset* getSubDataset(uint32_t i) {return (i < m_subDatasets.size()) ? m_subDatasets[i] : NULL;}

            /**
             * \brief  Get the number of subdatasets this dataset possesses
             * \return  the number of subdataset this dataset possesses 
             */
            uint32_t getNbSubDatasets() const {return m_subDatasets.size();}
        protected:
            /**
             * \brief  Set the subdataset amplitude using friendship
             * \param dataset the subdataset to modify
             * \param amplitude the new amplitude array
             */
            void setSubDatasetAmplitude(SubDataset* dataset, float* amplitude)
            {
                dataset->m_amplitude[0] = amplitude[0];
                dataset->m_amplitude[1] = amplitude[1];
            }

            /** \brief  Set the subdataset validity using friendship
             * \param dataset the subdataset to modify
             * \param isValid the new validity*/
            void setSubDatasetValidity(SubDataset* dataset, bool isValid)
            {
                dataset->m_isValid = isValid;
            }
            std::vector<SubDataset*> m_subDatasets; /*!< Array of sub datasets*/
    };
}

#endif
