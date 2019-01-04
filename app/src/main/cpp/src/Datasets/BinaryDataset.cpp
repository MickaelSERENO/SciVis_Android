#include "Datasets/BinaryDataset.h"

namespace sereno
{
    BinaryDataset::BinaryDataset(FILE* file) : Dataset()
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

        //Only one sub dataset available
        SubDataset* subData = new SubDataset(this);
        m_subDatasets.push_back(subData);

        //read data
        //We do not precompute magnitude or so because of memory issue. We prefer using CPU time instead of RAM
        //However we store the ampltitude range
        uint32_t velID = 0;
        float amplitude[2] = {std::numeric_limits<float>::max(), std::numeric_limits<float>::min()};
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
                if(amp < amplitude[0])
                    amplitude[0] = amp;
                else if(amp > amplitude[1])
                    amplitude[1] = amp;
            }
        }while(readSize != 0);

        amplitude[0] = sqrt(amplitude[0]);
        amplitude[1] = sqrt(amplitude[1]);
        setSubDatasetAmplitude(subData, amplitude);
        setSubDatasetValidity(subData, true);
#undef BUFFER_SIZE
    }

    BinaryDataset::BinaryDataset(const BinaryDataset& copy) : Dataset(copy)
    {
        *this = copy;
    }

    BinaryDataset::BinaryDataset(BinaryDataset&& mvt) : Dataset(mvt)
    {
        for(uint8_t i = 0; i < 3; i++)
            m_size[i] = mvt.m_size[i];
        m_velocity = mvt.m_velocity;
        mvt.m_velocity = NULL;
    }

    BinaryDataset& BinaryDataset::operator=(const BinaryDataset& copy)
    {
        if(this == &copy)
            return *this;

        for(uint8_t i = 0; i < 3; i++)
            m_size[i] = copy.m_size[i];
        uint32_t s = sizeof(float*)*m_size[0]*m_size[1]*m_size[2];
        m_velocity = (float*)malloc(s);
        memcpy(m_velocity, copy.m_velocity,s);
        return *this;
    }

    BinaryDataset::~BinaryDataset()
    {
        if(m_velocity)
            free(m_velocity);
        for(auto d : m_subDatasets)
            delete d;
    }

    BinaryDataset* BinaryDataset::readFromFilePath(const std::string& path)
    {
        //Open and check the file
        FILE* file = fopen(path.c_str(), "r");
        if(file == NULL)
            return NULL;

        BinaryDataset* data = new BinaryDataset(file);

        //Check if the data is valid or not
        if(!data->getSubDataset(0)->isValid())
        {
            delete data;
            data = NULL;
        }

        fclose(file);
        return data;
    }

    Quaternionf BinaryDataset::getRotationQuaternion(uint32_t x, uint32_t y, uint32_t z) const
    {
        uint32_t ind = x + m_size[0]*y + m_size[0]*m_size[1]*z;
        float vel[3] = {m_velocity[3*ind], m_velocity[3*ind+1], m_velocity[3*ind+2]};
        float pitch  = atan2(vel[1], vel[0]);
        float roll   = asin(vel[2]);

        return Quaternionf(pitch, roll, 0);
    }

}
