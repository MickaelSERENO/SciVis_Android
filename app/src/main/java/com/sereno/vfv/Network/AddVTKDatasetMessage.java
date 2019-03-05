package com.sereno.vfv.Network;

public class AddVTKDatasetMessage extends ServerMessage
{
    private int  m_dataID;
    private String m_path;
    private int[] m_ptFieldValueIndices;
    private int[] m_cellFieldValueIndices;

    private int m_nbPtFieldValueIndices = 0;
    private int m_nbCellFieldValueIndices = 0;

    @Override
    public byte getCurrentType()
    {
        if(cursor == 1)
            return 's';
        return 'I';
    }

    @Override
    public void pushValue(String value)
    {
        m_path = value;
        super.pushValue(value);
    }

    @Override
    public void pushValue(int value)
    {
        if(cursor == 0)
            m_dataID = value;
        else if(cursor == 2)
        {
            m_nbPtFieldValueIndices = value;
            m_ptFieldValueIndices = new int[value];
        }
        else if(cursor - 3 < m_nbPtFieldValueIndices)
            m_ptFieldValueIndices[cursor-3] = value;
        else if(cursor == 3+m_nbPtFieldValueIndices)
        {
            m_nbCellFieldValueIndices = value;
            m_cellFieldValueIndices = new int[value];
        }
        else if(cursor - 4 - m_nbPtFieldValueIndices < m_nbCellFieldValueIndices)
        {
            m_cellFieldValueIndices[cursor-4-m_nbPtFieldValueIndices] = value;
        }
        else
            return;
        super.pushValue(value);
    }

    @Override
    public int getMaxCursor()
    {
        return 3+m_nbPtFieldValueIndices+m_nbCellFieldValueIndices;
    }

    /** Get the data ID of the VTK Dataset
     * @return the data ID saved by the server*/
    public int getDataID() {return m_dataID;}

    /** Get the path of the VTK Dataset
     * @return the VTKDataset path*/
    public String getPath() {return m_path;}

    /** Get the point field value indices to take account of
     * @return the VTK Point FieldValue indices to use in the VTK Dataset*/
    public int[]  getPtFieldValueIndices() {return m_ptFieldValueIndices;}

    /** Get the cell field value indices to take account of
     * @return the VTK Cell FieldValue indices to use in the VTK Dataset*/
    public int[]  getCellFieldValueIndices() {return m_cellFieldValueIndices;}
}
