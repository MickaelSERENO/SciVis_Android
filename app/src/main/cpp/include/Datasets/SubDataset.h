#ifndef  SUBDATASET_INC
#define  SUBDATASET_INC

#include <limits>
#include <stdint.h>
#include "Quaternion.h"
#include "ColorMode.h"

namespace sereno
{
    class Dataset;

    /** \brief  Represent a dataset. Aims to be derived */
    class SubDataset
    {
        public:
            /** \brief  Constructor */
            SubDataset(Dataset* parent);

            virtual ~SubDataset(){}

            /** \brief Set the color of this dataset at rendering time
             * \param mode the color mode to apply
             * \param min the minimum clamping
             * \param max the maximum clamping*/
            void setColor(float min, float max, ColorMode mode);

            /* \brief Set the global rotation of this fluid dataset
             * \param quat the global rotation quaternion to apply */
            void setGlobalRotate(const Quaternionf& quat) {m_rotation = quat;}

            /* \brief Get the global rotation quaternion of this dataset
             * \return a reference to the global rotation quaternion of this dataset */
            const Quaternionf& getGlobalRotate() const {return m_rotation;}

            /* \brief Get the minimum clamping value in ratio (0.0, 1.0)
             * \return the minimum clamping value */
            float     getMinClamping() const {return m_minClamp;}

            /* \brief Get the maximum clamping value in ratio (0.0, 1.0)
             * \return the maximum clamping value */
            float     getMaxClamping() const {return m_maxClamp;}

            /* \brief Get the color mode currently in application
             * \return the color mode */
            ColorMode getColorMode() const {return m_colorMode;}

            /* \brief Get the minimum amplitude of this dataset
             * \return the minimum amplitude */
            float getMinAmplitude() const {return m_amplitude[0];}

            /* \brief Get the maximum amplitude of this dataset
             * \return the maximum amplitude */
            float getMaxAmplitude() const {return m_amplitude[1];}

            /* \brief Is this dataset valid ? */
            bool isValid() const {return m_isValid;}

            /** \brief  Get the parent dataset
             * \return  the parent dataset */
            Dataset* getParent() {return m_parent;}

            /** \brief  Get the parent dataset
             * \return  the parent dataset */
            const Dataset* getParent() const {return m_parent;}
        protected:
            bool        m_isValid  = false;    /*!< Is this dataset in a valid state ?*/
            ColorMode   m_colorMode = RAINBOW; /*!< The color mode of this dataset*/
            float       m_minClamp  = 0.0f;    /*!< The minimum color clamping*/
            float       m_maxClamp  = 1.0f;    /*!< The maximum color clamping (ratio : 0.0f 1.0)*/
            float       m_amplitude[2];        /*!< The dataset amplitude*/
            Quaternionf m_rotation;            /*!< The quaternion rotation*/
            Dataset*    m_parent = NULL;       /*!< The parent dataset*/

        friend class Dataset;
    };
}

#endif
