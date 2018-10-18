#ifndef  FLUIDDATASET_INC
#define  FLUIDDATASET_INC

#include <string>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "utils.h"
#include "Quaternion.h"

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
            Quaternionf getRotationQuaternion(uint32_t x, uint32_t y, uint32_t z);

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
    };
}

#endif
