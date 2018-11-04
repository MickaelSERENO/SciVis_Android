#include "FluidDataset.h"

namespace sereno
{
    FluidDataset::FluidDataset(FILE* file) : m_amplitude{std::numeric_limits<float>::max(), 0.0}
    {
#define BUFFER_SIZE 3*sizeof(float)*270
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
        if(fileSize-pos != m_size[0]*m_size[1]*m_size[2]*3*sizeof(float))
        {
            LOG_ERROR("Error : the current file may be broken");
            return;
        }
        fseek(file, pos, SEEK_SET);
        m_velocity = (float*)malloc(3*sizeof(float*)*m_size[0]*m_size[1]*m_size[2]);

        //read data
        //We do not precompute magnitude or so because of memory issue. We prefer using CPU time instead of RAM
        //However we store the ampltitude range
        uint32_t velID = 0;
        do
        {
            readSize = fread(buffer, sizeof(uint8_t), BUFFER_SIZE, file);
            for(uint32_t j = 0; j < readSize; j+=sizeof(float)*3, velID+=3)
            {
                float amp = 0.0;
                for(uint32_t k = 0; k < 3; k++)
                {
                    m_velocity[velID+k] = uint8ToFloat(buffer+j+k*sizeof(float));
                    amp += m_velocity[velID+k]*m_velocity[velID+k];
                }

                //Update the amplitude. We store the square of the amplitude for better performances (the square root is done only at the end)
                if(amp < m_amplitude[0])
                    m_amplitude[0] = amp;
                else if(amp > m_amplitude[1])
                    m_amplitude[1] = amp;
            }
        }while(readSize != 0);

        m_amplitude[0] = sqrt(m_amplitude[0]);
        m_amplitude[1] = sqrt(m_amplitude[1]);

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

    Quaternionf FluidDataset::getRotationQuaternion(uint32_t x, uint32_t y, uint32_t z) const
    {
        uint32_t ind = x + m_size[0]*y + m_size[0]*m_size[1]*z;
        float vel[3] = {m_velocity[3*ind], m_velocity[3*ind+1], m_velocity[3*ind+2]};
        float pitch  = atan2(vel[1], vel[0]);
        float roll   = asin(vel[2]);

        return Quaternionf(pitch, roll, 0);
    }

    void FluidDataset::setColor(float min, float max, ColorMode mode)
    {
        m_colorMode = mode;
        m_minClamp  = min;
        m_maxClamp  = max;
    }
}
