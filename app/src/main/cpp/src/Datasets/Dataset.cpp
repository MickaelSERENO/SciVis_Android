#include "Datasets/Dataset.h"

namespace sereno
{
    Dataset::Dataset() :  m_amplitude{std::numeric_limits<float>::max(), std::numeric_limits<float>::min()}
    {}

    void Dataset::setColor(float min, float max, ColorMode mode)
    {
        m_colorMode = mode;
        m_minClamp  = min;
        m_maxClamp  = max;
    }
}
