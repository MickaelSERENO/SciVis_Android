#include "Datasets/Dataset.h"
#include <utility>

namespace sereno
{
    Dataset::Dataset(const Dataset& copy)
    {
        *this = copy;
    }

    Dataset::Dataset(Dataset&& mvt)
    {
        m_subDatasets = std::move(mvt.m_subDatasets);
    }

    Dataset& Dataset::operator=(const Dataset& copy)
    {
        if(this == &copy)
            return *this;
        m_subDatasets = std::move(copy.m_subDatasets);
        return *this;
    }

    Dataset::~Dataset(){}
}
