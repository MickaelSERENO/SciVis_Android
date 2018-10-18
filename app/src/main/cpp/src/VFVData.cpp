#include "VFVData.h"

namespace sereno
{
    VFVData::VFVData()
    {}

    void VFVData::setCallback(IVFVCallback* clbk)
    {
        m_clbk = clbk;
    }
}
