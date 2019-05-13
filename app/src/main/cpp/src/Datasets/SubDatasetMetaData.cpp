#include "Datasets/SubDatasetMetaData.h"

namespace sereno
{
    SubDatasetMetaData::SubDatasetMetaData(SubDataset* publicSD, SubDataset* privateSD, int visibility) : m_public(publicSD), m_private(privateSD), m_visibility(visibility) {}
}
