#ifndef  BINARYDATASET_INC
#define  BINARYDATASET_INC

#include <string>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <limits>
#include "utils.h"
#include "Quaternion.h"
#include "ColorMode.h"
#include "Dataset.h"

namespace sereno
{
    /*  \brief Class representing the fluid datasets */
    class BinaryDataset : public Dataset
    {
        public:
            /* \brief Constructor. Read the file defined by dataPath 
             * \param file the file to read*/
            BinaryDataset(FILE* file);

            /* \brief Copy constructor
             * \param copy the BinaryDataset to copy */
            BinaryDataset(const BinaryDataset& copy);

            /* \brief movement constructor
             * \param mvt the variable to move */
            BinaryDataset(BinaryDataset&& mvt);

            /* \brief operator=
             * \param copy the BinaryDataset to copy */
            BinaryDataset& operator=(const BinaryDataset& copy);

            /* \brief Destructor. */
            ~BinaryDataset();

            /* \brief Create a BinaryDataset from a filepath.
             * \param path the file path to read at
             * \param the created BinaryDataset, null if the file is not defined 
             * \return the BinaryDataset. Destroy it using delete operator*/
            static BinaryDataset* readFromFilePath(const std::string& path);

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

            /* \brief Get the grid size of this dataset
             * \return the grid size as a pointer with length == 3 */
            const uint32_t* getGridSize() const {return m_size;}

            /* \brief Compute how many cells exist in this grid (X*Y*Z)
             * \return the number of cells */
            const uint32_t  nbCells() const {return m_size[0]*m_size[1]*m_size[2];}

            /* \brief Get the rotation quaternion of this cell (orientation of the vector)
             * \param x the x coordinate
             * \param y the y coordinate
             * \param z the z coordinate
             * \return the direction encoded in a Quaternion */
            Quaternionf getRotationQuaternion(uint32_t x, uint32_t y, uint32_t z) const;
        private:
            uint32_t m_size[3];           /*!< The 3D size of the grid*/
            float*   m_velocity = NULL;   /*!< The velocity array of all the grid cell. Access via m_velocity[i + j*width + k*width*height] */ 
    };
}

#endif
