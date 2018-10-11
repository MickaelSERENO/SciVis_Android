#ifndef  FLUIDDATASET_INC
#define  FLUIDDATASET_INC

#include <string>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "utils.h"

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

        /* \brief Is this dataset valid ? */
        bool isValid() {return m_isValid;}

        /* \brief Get the grid size of this dataset
         * \return the grid size as a pointer with length == 3 */
        const uint32_t* getGridSize() {return m_size;}
    private:
        bool     m_isValid  = false;  /*!< Is this dataset in a valid state ?*/
        uint32_t m_size[3];           /*!< The 3D size of the grid*/
        float*   m_velocity = NULL;   /*!< The velocity array of all the grid cell. Access via m_velocity[i + j*width + k*width*height] */ 
};

#endif
