#ifndef  VTKDATASET_INC
#define  VTKDATASET_INC

#include <string>
#include <memory>
#include <vector>
#include "VTKParser.h"
#include "Dataset.h"

namespace sereno
{
    /** \brief  Represent a VTK dataset information to take account of */
    class VTKDataset : public Dataset
    {
        public:
            /**
             * \brief  Constructor. Initialize this VTKDataset with a set of VTKFieldValue to take account of
             * \param parser the parser
             * \param ptFieldValues   the point field values to take account of. Must be part of the parser parameter.
             * \param cellFieldValues the cell  field values to take account of. Must be part of the parser parameter.
             */
            VTKDataset(std::shared_ptr<VTKParser>& parser, const std::vector<VTKFieldValue*>& ptFieldValues, 
                                     const std::vector<VTKFieldValue*>& cellFieldValues);

            /**
             * \brief  Get the VTKParser
             * \return  the VTKParser object 
             */
            const std::shared_ptr<VTKParser> getParser() const {return m_parser;}

            /**
             * \brief  Get the point field values
             * \return   the VTKFieldValue* objects
             */
            const std::vector<VTKFieldValue*>& getPtFieldValues() const {return m_ptFieldValues;}
        private:
            std::vector<VTKFieldValue*> m_ptFieldValues;   /*!< The point field values*/
            std::vector<VTKFieldValue*> m_cellFieldValues; /*!< The cell  field values*/
            std::shared_ptr<VTKParser>  m_parser;          /*!< The VTK parser*/
    };
}

#endif
