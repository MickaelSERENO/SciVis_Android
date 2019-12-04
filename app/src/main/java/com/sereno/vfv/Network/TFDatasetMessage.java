package com.sereno.vfv.Network;

import com.sereno.vfv.Data.SubDataset;

public class TFDatasetMessage extends ServerMessage
{
    /** Class representing GTF (and TriangularGTF) data*/
    public static class GTFData
    {
        /** Class representing properties data*/
        public static class PropData
        {
            /** The property ID to look at*/
            public int   propID;

            /** The center value*/
            public float center;

            /** The scale value*/
            public float scale;
        }

        /** The allocated property data*/
        public PropData[] propData = new PropData[0];
    }

    /** The datasetID of the message*/
    private int m_datasetID;

    /** The subdataset ID of the message*/
    private int m_subDatasetID;

    /** The headset ID performing this movement. -1 == server call*/
    private int m_headsetID;

    /** The Type of the Transfer Function*/
    private int m_tfID = SubDataset.TRANSFER_FUNCTION_NONE;

    /** The color mode to apply*/
    private int m_colorMode;

    /** The GTF data for GTF and TriangularGTF*/
    private GTFData m_gtfData = null;

    @Override
    public void pushValue(int val)
    {
        if(cursor == 0)
            m_datasetID = val;
        else if(cursor == 1)
            m_subDatasetID = val;
        else if(cursor == 2)
            m_headsetID = val;
        else
        {
            switch(m_tfID)
            {
                case SubDataset.TRANSFER_FUNCTION_GTF:
                case SubDataset.TRANSFER_FUNCTION_TGTF:
                {
                    if(cursor == 5)
                    {
                        m_gtfData.propData = new GTFData.PropData[val];
                        for(int i = 0; i < val; i++)
                            m_gtfData.propData[i] = new GTFData.PropData();
                    }
                    else if(cursor >= 6)
                    {
                        int propID = (cursor - 6)/3;
                        int offset = (cursor - 6)%3;
                        if(propID < m_gtfData.propData.length && offset == 0)
                            m_gtfData.propData[propID].propID = val;
                    }
                }
            }
        }

        super.pushValue(val);
    }

    @Override
    public void pushValue(byte val)
    {
        if(cursor == 3)
        {
            m_tfID = val;
            switch(m_tfID) //Allocate the correct data depending on the type of the transfer function
            {
                case SubDataset.TRANSFER_FUNCTION_GTF:
                case SubDataset.TRANSFER_FUNCTION_TGTF:
                {
                    m_gtfData = new GTFData();
                    break;
                }
                default:
                    break;
            }
        }

        else if(cursor == 4)
            m_colorMode = val;
        super.pushValue(val);
    }

    @Override
    public void pushValue(float val)
    {
        switch(m_tfID)
        {
            case SubDataset.TRANSFER_FUNCTION_GTF:
            case SubDataset.TRANSFER_FUNCTION_TGTF:
            {
                if(cursor >= 6)
                {
                    int propID = (cursor - 6)/3;
                    int offset = (cursor - 6)%3;
                    if(propID < m_gtfData.propData.length)
                    {
                        if(offset == 1)
                            m_gtfData.propData[propID].center = val;
                        else if(offset == 2)
                            m_gtfData.propData[propID].scale  = val;
                    }
                }
                break;
            }
        }

        super.pushValue(val);
    }

    /** Get the dataset ID parsed
     * @return the dataset ID to update. Need to be used with getSubDatasetID()*/
    public int getDatasetID()
    {
        return m_datasetID;
    }

    /** Get the subdataset ID parsed
     * @return the subdataset ID to update. Update dataset ID = getDatasetID() .getSubDataset[getSubDatasetID()].*/
    public int getSubDatasetID()
    {
        return m_subDatasetID;
    }

    /** Get the headset ID parsed.
     * @return the headset ID moving this dataset. -1 == server call*/
    public int getHeadsetID() {return m_headsetID;}

    /** Get the transfer function type.
     * @return the transfer function type. See SubDataset.TRANSFER_FUNCTION_* for more details*/
    public int getTFType() {return m_tfID;}

    /** Get the GTF Data, used for TRANSFER_FUNCTION_GTF and TRANSFER_FUNCTION_TGTF transfer functions
     * @return the gtf data. If getTFType is not equal to TRANSFER_FUNCTION_GTF or TRANSFER_FUNCTION_TGTF, this function returns null. */
    public GTFData getGTFData() {return m_gtfData;}

    /** Get the color mode to apply
     * @return the color mode. See ColorMode class for more details.*/
    public int getColorMode() {return m_colorMode;}

    @Override
    byte getCurrentType()
    {
        if(cursor <= 2)
            return 'I'; //General info (datasetID, subdatasetID, headsetID)
        else if(cursor == 3)
            return 'b'; //tfID
        else if(cursor == 4)
            return 'b'; //colorMode

        switch(m_tfID)
        {
            case SubDataset.TRANSFER_FUNCTION_GTF:
            case SubDataset.TRANSFER_FUNCTION_TGTF:
                if(cursor == 5)
                    return 'I'; //nbProps
                else
                {
                    if((cursor-6)/3 < m_gtfData.propData.length)
                    {
                        int offset = (cursor-6)%3;
                        switch(offset)
                        {
                            case 0:
                                return 'I'; //propID
                            case 1:
                            case 2:
                                return 'f'; //center/scale
                        }
                    }
                }
                break;
        }
        return 0;
    }

    @Override
    public int getMaxCursor()
    {
        //Default values
        int maxCursor = 4;

        //Then an offset depending on the type of the transfer function
        switch(m_tfID)
        {
            case SubDataset.TRANSFER_FUNCTION_GTF:
            case SubDataset.TRANSFER_FUNCTION_TGTF:
                maxCursor += (1 + 3*m_gtfData.propData.length);
                break;
            default:
                break;
        }

        return maxCursor;
    }
}
