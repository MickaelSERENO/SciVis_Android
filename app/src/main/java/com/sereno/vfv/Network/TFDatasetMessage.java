package com.sereno.vfv.Network;

import android.util.Log;

import com.sereno.vfv.Data.SubDataset;
import com.sereno.vfv.MainActivity;

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

    /** Class permitting to parse successfully merge transfer function*/
    public static class MergeTFData
    {
        /** The t parameter to pass to the merge tf data object*/
        public float t = 0;

        /** The first transfer function message being parsed*/
        public TFDatasetMessage tf1Msg = new TFDatasetMessage();

        /** The second transfer function message being parsed*/
        public TFDatasetMessage tf2Msg = new TFDatasetMessage();
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

    /** The current transfer function data*/
    private Object m_tfData = null;

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
                    GTFData data = (GTFData)m_tfData;

                    if(cursor == 5)
                    {
                        data.propData = new GTFData.PropData[val];
                        for(int i = 0; i < val; i++)
                            data.propData[i] = new GTFData.PropData();
                    }
                    else if(cursor >= 6)
                    {
                        int propID = (cursor - 6)/3;
                        int offset = (cursor - 6)%3;
                        if(propID < data.propData.length && offset == 0)
                            data.propData[propID].propID = val;
                    }
                    break;
                }
                case SubDataset.TRANSFER_FUNCTION_MERGE:
                {
                    MergeTFData data = (MergeTFData)m_tfData;

                    if(data.tf1Msg.cursor <= data.tf1Msg.getMaxCursor())
                        data.tf1Msg.pushValue(val);
                    else if(data.tf2Msg.cursor <= data.tf2Msg.getMaxCursor())
                        data.tf2Msg.pushValue(val);
                    break;
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
                    m_tfData = new GTFData();
                    break;
                }
                case SubDataset.TRANSFER_FUNCTION_MERGE:
                {
                    m_tfData = new MergeTFData();
                    ((MergeTFData) m_tfData).tf1Msg.cursor = 3; //Ignore datasetID, subdatasetID, and headsetID
                    ((MergeTFData) m_tfData).tf2Msg.cursor = 3;
                    break;
                }
                default:
                    break;
            }
        }

        else if(cursor == 4)
            m_colorMode = val;
        else
        {
            switch(m_tfID)
            {
                case SubDataset.TRANSFER_FUNCTION_MERGE:
                {
                    MergeTFData data = (MergeTFData) m_tfData;

                    if(data.tf1Msg.cursor <= data.tf1Msg.getMaxCursor())
                        data.tf1Msg.pushValue(val);
                    else if(data.tf2Msg.cursor <= data.tf2Msg.getMaxCursor())
                        data.tf2Msg.pushValue(val);
                    break;
                }
                default:
                    break;
            }
        }
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
                GTFData data = (GTFData)m_tfData;

                if(cursor >= 6)
                {
                    int propID = (cursor - 6)/3;
                    int offset = (cursor - 6)%3;
                    if(propID < data.propData.length)
                    {
                        if(offset == 1)
                            data.propData[propID].center = val;
                        else if(offset == 2)
                            data.propData[propID].scale  = val;
                    }
                }
                break;
            }
            case SubDataset.TRANSFER_FUNCTION_MERGE:
            {
                MergeTFData data = (MergeTFData) m_tfData;

                if(cursor == 5)
                    data.t = val;
                else
                {
                    if(data.tf1Msg.cursor <= data.tf1Msg.getMaxCursor())
                        data.tf1Msg.pushValue(val);
                    else if(data.tf2Msg.cursor <= data.tf2Msg.getMaxCursor())
                        data.tf2Msg.pushValue(val);
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
    public GTFData getGTFData()
    {
        if(m_tfID == SubDataset.TRANSFER_FUNCTION_GTF || m_tfID == SubDataset.TRANSFER_FUNCTION_TGTF)
            return (GTFData)m_tfData;
        return null;
    }

    public MergeTFData getMergeTFData()
    {
        if(m_tfID == SubDataset.TRANSFER_FUNCTION_MERGE)
        {
            MergeTFData data = (MergeTFData)m_tfData;
            return data;
        }
        return null;
    }

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
            {
                GTFData data = (GTFData)m_tfData;

                if(cursor == 5)
                    return 'I'; //nbProps
                else
                {
                    if((cursor-6)/3 < data.propData.length)
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
            case SubDataset.TRANSFER_FUNCTION_MERGE:
            {
                if(cursor == 5)
                    return 'f'; //t

                MergeTFData data = (MergeTFData)m_tfData;
                if(data.tf1Msg.cursor <= data.tf1Msg.getMaxCursor())
                    return data.tf1Msg.getCurrentType();
                else if(data.tf2Msg.cursor <= data.tf2Msg.getMaxCursor())
                    return data.tf2Msg.getCurrentType();
                break;
            }
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
            {
                GTFData data = (GTFData)m_tfData;

                maxCursor += (1 + 3 * data.propData.length);
                break;
            }
            case SubDataset.TRANSFER_FUNCTION_MERGE:
            {
                MergeTFData data = (MergeTFData)m_tfData;
                maxCursor += 1 + data.tf1Msg.getMaxCursor() + data.tf2Msg.getMaxCursor() - 4; //-4 == datasetID + subDatasetID + headsetID for BOTH transfer functions (we ignore them).
                                                                                              // We remind that the current cursor is included (hence -4 and not -6)
                Log.e(MainActivity.TAG, "MergeTF maxCursor : " + Integer.toString(maxCursor));
                break;
            }
            default:
                break;
        }

        return maxCursor;
    }
}
