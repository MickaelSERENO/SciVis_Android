#include "Datasets/VTKDataset.h"

namespace sereno
{
    VTKDataset::VTKDataset(std::shared_ptr<VTKParser>& parser, const std::vector<VTKFieldValue*>& ptFieldValues, 
                           const std::vector<VTKFieldValue*>& cellFieldValues) : m_ptFieldValues(ptFieldValues), m_cellFieldValues(cellFieldValues), m_parser(parser)
    {
    }
}
