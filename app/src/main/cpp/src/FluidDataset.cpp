#include "FluidDataset.h"

namespace sereno
{
    FluidDataset::FluidDataset(FILE* file)
    {
#define BUFFER_SIZE 3*12*270
        uint8_t buffer[BUFFER_SIZE];

        //Read widthxheightxdepth
        uint32_t readSize = fread(buffer, sizeof(uint8_t), 3*sizeof(uint32_t), file);
        if(readSize < 3*sizeof(uint32_t))
            return;
        for(uint8_t i = 0; i < 3; i++)
            m_size[i] = uint8ToUint32(buffer+sizeof(uint32_t)*i);

        //Check and get the data size
        uint32_t pos = ftell(file);
        fseek(file, 0, SEEK_END);
        uint32_t fileSize = ftell(file);
        if(fileSize != m_size[0]*m_size[1]*m_size[2]*3*sizeof(float))
            return;
        fseek(file, pos, SEEK_SET);
        m_velocity = (float*)malloc(sizeof(float*)*m_size[0]*m_size[1]*m_size[2]);

        //read data
        //We do not precompute magnitude or so because of memory issue. We prefer using CPU time instead of RAM
        uint32_t i     = 0;
        uint32_t velID = 0;
        while(i < fileSize)
        {
            readSize = fread(buffer, sizeof(uint8_t), BUFFER_SIZE, file);
            for(uint32_t j = 0; j < readSize; j++, velID++)
                m_velocity[velID] = uint8ToFloat(buffer+sizeof(float)*j);
            i+=readSize;
        }

        m_isValid = true;
#undef BUFFER_SIZE
    }

    FluidDataset::FluidDataset(const FluidDataset& copy)
    {
        *this = copy;
    }

    FluidDataset::FluidDataset(FluidDataset&& mvt)
    {
        for(uint8_t i = 0; i < 3; i++)
            m_size[i] = mvt.m_size[i];
        m_isValid = mvt.m_isValid;

        m_velocity = mvt.m_velocity;
        mvt.m_velocity = NULL;
    }

    FluidDataset& FluidDataset::operator=(const FluidDataset& copy)
    {
        if(this == &copy)
            return *this;

        for(uint8_t i = 0; i < 3; i++)
            m_size[i] = copy.m_size[i];
        m_isValid = copy.m_isValid;
        uint32_t s = sizeof(float*)*m_size[0]*m_size[1]*m_size[2];
        m_velocity = (float*)malloc(s);
        memcpy(m_velocity, copy.m_velocity,s);
        return *this;
    }

    FluidDataset::~FluidDataset()
    {
        if(m_velocity)
            free(m_velocity);
    }

    FluidDataset* FluidDataset::readFromFilePath(const std::string& path)
    {
        //Open and check the file
        FILE* file = fopen(path.c_str(), "r");
        if(file == NULL)
            return NULL;

        FluidDataset* data = new FluidDataset(file);

        //Check if the data is valid or not
        if(!data->isValid())
        {
            delete data;
            data = NULL;
        }

        fclose(file);
        return data;
    }

    Quaternionf FluidDataset::getRotationQuaternion(uint32_t x, uint32_t y, uint32_t z)
    {
        uint32_t ind = x + m_size[0]*y + m_size[0]*m_size[1]*z;
        float vel[3] = {m_velocity[3*ind], m_velocity[3*ind+1], m_velocity[3*ind+2]};
        float pitch  = atan2(vel[1], vel[0]);
        float roll   = asin(vel[2]);

        return Quaternionf(pitch, roll, 0);
    }
}
