#include "Datasets/DatasetMetaData.h"

namespace sereno
{
    DatasetMetaData::DatasetMetaData(std::shared_ptr<Dataset> ptr) : m_dataset(ptr)
    {}

    DatasetMetaData::~DatasetMetaData()
    {}
}
