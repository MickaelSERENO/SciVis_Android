package com.sereno.vfv.Data;

/** @brief VTKDataset class, represent a full VTK Dataset to take account of. Represent a pair of VTKFieldValue[] and VTKParser*/
public class VTKDataset
{
    /** @brief The point field values to take account of*/
    private VTKFieldValue[] m_ptValues;

    /** @brief The cell field values to take account of*/
    private VTKFieldValue[] m_cellValues;

    /** @brief The VTKParser associated*/
    private VTKParser       m_parser;

    /** @brief Constructor
     * @param parser The VTKParser containing VTK information
     * @param ptValues the point VTKFieldValue to take account here
     * @param cellValues the cell VTKFieldValue to take account here*/
    public VTKDataset(VTKParser parser, VTKFieldValue[] ptValues, VTKFieldValue[] cellValues)
    {
        m_parser     = parser;
        m_ptValues   = ptValues;
        m_cellValues = cellValues;
    }

    /** @brief Get the VTKParser associated with this dataset
     * @return the VTKParser*/
    public VTKParser getParser() {return m_parser;}

    /** @brief Get the point VTKFieldValue array to use for this dataset
     * @return a VTKFieldValue array*/
    public VTKFieldValue[] getSelectedPtFieldValues() {return m_ptValues;}

    /** @brief Get the cell VTKFieldValue array to use for this dataset
     * @return a VTKFieldValue array*/
    public VTKFieldValue[] getSelectedCellFieldValues() {return m_cellValues;}
}
