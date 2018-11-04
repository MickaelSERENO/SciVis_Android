#ifndef  FLUIDDATASET_INC
#define  FLUIDDATASET_INC

#include <string>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <limits>
#include "utils.h"
#include "Quaternion.h"
#include "ColorMode.h"

namespace sereno
{
    /*  \brief Class representing the fluid datasets */
    class FluidDataset
    {
        public:
            /* \brief Constructor. Read the file defined by dataPath 
             * \param file the file to read*/
            FluidDataset(FILE* file);

            /* \brief Copy constructor
             * \param copy the FluidDataset to copy */
            FluidDataset(const FluidDataset& copy);

            /* \brief movement constructor
             * \param mvt the variable to move */
            FluidDataset(FluidDataset&& mvt);

            /* \brief operator=
             * \param copy the FluidDataset to copy */
            FluidDataset& operator=(const FluidDataset& copy);

            /* \brief Destructor. */
            ~FluidDataset();

            /* \brief Create a FluidDataset from a filepath.
             * \param path the file path to read at
             * \param the created FluidDataset, null if the file is not defined 
             * \return the FluidDataset. Destroy it using delete operator*/
            static FluidDataset* readFromFilePath(const std::string& path);

            /* \brief Get the rotation quaternion of this cell (orientation of the vector)
             * \param x the x coordinate
             * \param y the y coordinate
             * \param z the z coordinate
             * \return the direction encoded in a Quaternion */
            Quaternionf getRotationQuaternion(uint32_t x, uint32_t y, uint32_t z) const;

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

            /* \brief Get the velocity array. Size : getGridSize
             * \return the velocity array packed in (x, y, z) like : 
             * for(int k = 0; k < getGridSize[2]; k++)
             *     for(int j = 0; j < getGridSize[1]; j++)
             *         for(int i = 0; i < getGridSize[0]; i++)
             *         {
             *             float x = m_velocity[3*(i+j*getGriSize[0]+k*getGridSize[1]*getGridSize[2])+0];
             *             float y = m_velocity[3*(i+j*getGriSize[0]+k*getGridSize[1]*getGridSize[2])+1];
             *             float z = m_velocity[3*(i+j*getGriSize[0]+k*getGridSize[1]*getGridSize[2])+2];
             *         } */
            const float* getVelocity() const {return m_velocity;}

            /* \brief Get the minimum amplitude of this dataset
             * \return the minimum amplitude */
            float getMinAmplitude() const {return m_amplitude[0];}

            /* \brief Get the maximum amplitude of this dataset
             * \return the maximum amplitude */
            float getMaxAmplitude() const {return m_amplitude[1];}

            /* \brief Is this dataset valid ? */
            bool isValid() const {return m_isValid;}

            /* \brief Get the grid size of this dataset
             * \return the grid size as a pointer with length == 3 */
            const uint32_t* getGridSize() const {return m_size;}

            /* \brief Compute how many cells exist in this grid (X*Y*Z)
             * \return the number of cells */
            const uint32_t  nbCells() const {return m_size[0]*m_size[1]*m_size[2];}
        private:
            bool     m_isValid  = false;  /*!< Is this dataset in a valid state ?*/
            uint32_t m_size[3];           /*!< The 3D size of the grid*/
            float*   m_velocity = NULL;   /*!< The velocity array of all the grid cell. Access via m_velocity[i + j*width + k*width*height] */ 
            float    m_amplitude[2];      /*!< The dataset amplitude*/

            ColorMode   m_colorMode = RAINBOW; /*!< The color mode of this dataset*/
            float       m_minClamp  = 0.0f;    /*!< The minimum color clamping*/
            float       m_maxClamp  = 1.0f;    /*!< The maximum color clamping (ratio : 0.0f 1.0)*/
            Quaternionf m_rotation;            /*!< The quaternion rotation*/
    };
}

#endif
