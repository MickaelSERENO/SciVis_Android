package com.sereno.vfv.Data;

/** @brief VTKDataset class, represent a full VTK Dataset to take account of. Represent a pair of VTKFieldValue[] and VTKParser*/
public class VTKDataset extends Dataset
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
     * @param cellValues the cell VTKFieldValue to take account here
     * @param name  the java name of this Dataset*/
    public VTKDataset(VTKParser parser, VTKFieldValue[] ptValues, VTKFieldValue[] cellValues, String name)
    {
        super(nativeInitPtr(parser.getPtr(),
                            getFieldValueNativePointers(ptValues),
                            getFieldValueNativePointers(cellValues)), name);
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

    /** @brief get the field value pointers in a long array
     * @param values the cell values to convert
     * @return the long array containing native pointers*/
    private static long[] getFieldValueNativePointers(VTKFieldValue[] values)
    {
        //Constructed native pointer arrays
        long[] fieldValues = new long[values.length];
        for(int i = 0; i < values.length; i++)
            fieldValues[i] = values[i].getPtr();
        return fieldValues;
    }

    /** @brief init the VTKDataset pointer
     * @param parserPtr the VTKParser pointer
     * @param ptFieldValuesPtr the point VTKFieldValues to take account of
     * @param cellFieldValuesPtr the cell VTKFieldValues to take account of
     * @return a native pointer*/
    private static native long nativeInitPtr(long parserPtr, long[] ptFieldValuesPtr, long[] cellFieldValuesPtr);
}
