#ifndef  SUBDATASETMETADATA_INC
#define  SUBDATASETMETADATA_INC

#include "visibility.h"
#include "Datasets/SubDataset.h"

namespace sereno
{
    /** \brief  SubDataset information per headset user */
    class SubDatasetMetaData
    {
        public:
            /** \brief  Default constructor.
             * \param publicSD the public subdataset states
             * \param privateSD the private subdataset states
             * \param visibility the current visibility (see visibility.h)*/
            SubDatasetMetaData(SubDataset* publicSD, SubDataset* privateSD, int visibility = VISIBILITY_PUBLIC);

            /* \brief  Set the visibility of this SubDataset
             * \param v the new visibility */
            void setVisibility(int v) {m_visibility = v;}

            /* \brief  Get the visibility of this SubDataset
             * \return  The SuBDataset visibility */
            int getVisibility() const {return m_visibility;}

            /* \brief  Get the public subdataset states
             * \return  The public subdataset states */
            SubDataset* getPublicSubDataset() {return m_public;}

            /* \brief Get the private subdataset states 
             * \return   The private subdataset states */
            SubDataset* getPrivateSubDataset() {return m_private;}

            /* \brief  Get the current subdataset state
             * \return   The current subdataset state*/
            SubDataset* getCurrentState() {return m_visibility == VISIBILITY_PUBLIC ? m_public : m_private;}

            /* \brief  Get the public subdataset states
             * \return  The public subdataset states */
            const SubDataset* getPublicSubDataset() const {return m_public;}

            /* \brief Get the private subdataset states 
             * \return   The private subdataset states */
            const SubDataset* getPrivateSubDataset() const {return m_private;}

            /* \brief  Get the current subdataset state
             * \return   The current subdataset state*/
            const SubDataset* getCurrentState() const {return m_visibility == VISIBILITY_PUBLIC ? m_public : m_private;}
        private:
            SubDataset* m_public     = NULL;              /*!< The bound SubDataset public state*/
            SubDataset* m_private;                        /*!< The bound SubDataset private state*/
            int         m_visibility = VISIBILITY_PUBLIC; /*!< The visibility regarding the subdataset*/
    };
}

#endif
