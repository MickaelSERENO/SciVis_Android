package com.sereno.vfv.Data.TF;

import com.sereno.vfv.Data.Dataset;
import com.sereno.vfv.Data.SubDataset;

import java.util.ArrayList;

public class MergeTFData extends TransferFunction
{
    /** Listener used in junction with MergeTFData*/
    public interface IMergeTFDataListener
    {
        /** Function called when the linear parameter t has changed
         * @param model the model calling this method
         * @param t the linear t parameter. SHould be between 0.0f and 1.0f*/
        void OnChangeLinearInterpolation(MergeTFData model, float t);

        /** Function called when the SubDataset bound to this Model has changed.
         * @param model the model calling this method
         * @param dataset the new dataset*/
        void onSetDataset(MergeTFData model, SubDataset dataset);
    }

    /** The Dataset from which we are manipulating the GTF ranges*/
    private SubDataset m_sd = null;

    /** The "parent" Dataset in use*/
    private Dataset m_dataset = null;

    /** The linear interpolation parameter. Must be between 0.0f and 1.0f*/
    private float  m_t = 0.5f;

    /** The type of the first transfer function.*/
    private int    m_tf1Type = SubDataset.TRANSFER_FUNCTION_NONE;

    /** The first transfer function data. Must be casted according to m_tf1Type*/
    private TransferFunction m_tf1 = null;

    /** The type of the second transfer function.*/
    private int    m_tf2Type = SubDataset.TRANSFER_FUNCTION_NONE;

    /** The second transfer function data. Must be casted according to m_tf2Type*/
    private TransferFunction m_tf2 = null;

    /** The native C++ std::shared_ptr\<TF\> pointer representing tf1*/
    private long m_tf1Ptr = 0;

    /** The native C++ std::shared_ptr\<TF\> pointer representing tf2*/
    private long m_tf2Ptr = 0;

    /** The listeners to call when the state of this model has changed*/
    private ArrayList<IMergeTFDataListener> m_listeners = new ArrayList<>();

    /** Constructor
     * @param sd the SubDataset bound to this model
     * @param tf1Type the type of the first transfer function to merge. See SubDataset.TRANSFER_FUNCTION_*
     * @param tf1 the data of the first transfer function.
     * @param tf2Type the type of the first transfer function to merge. See SubDataset.TRANSFER_FUNCTION_*
     * @param tf2 the data of the second transfer function*/
    public MergeTFData(SubDataset sd, int tf1Type, TransferFunction tf1, int tf2Type, TransferFunction tf2)
    {
        m_tf1Type = tf1Type;
        m_tf1     = tf1;
        m_tf2Type = tf2Type;
        m_tf2     = tf2;

        m_tf1Ptr = tf1.getNativeTransferFunction();
        m_tf2Ptr = tf2.getNativeTransferFunction();

        setDataset(sd);
    }

    /** Set the Dataset bound to this GTF Data model
     * This will reinit the ranges and the cpcp order to their default values
     * @param sd the new SubDataset to consider.*/
    public void setDataset(SubDataset sd)
    {
        m_dataset = null;
        m_sd = sd;

        //Load the ranges and update the new point field descs being used
        //Update also the CPCP order
        if(m_sd != null)
        {
            m_dataset = sd.getParent();
        }

        for(int i = 0; i < m_listeners.size(); i++)
            m_listeners.get(i).onSetDataset(this, m_sd);
    }

    /** Get the linear t parameter. Value ranges between 0.0f and 1.0f
     * @return the linear t parameter. 0.0f == getTF1() being fully rendered. 1.0f == getTF2() being fully rendered.*/
    public float getLinearParameter() {return m_t;}

    /** Set the linear t parameter. Value ranges between 0.0f and 1.0f
     * @param t the linear t parameter. 0.0f == getTF1() being fully rendered. 1.0f == getTF2() being fully rendered.*/
    public void setLinearParameter(float t)
    {
        t = Math.min(Math.max(t, 0.0f), 1.0f);

        if(t != m_t)
        {
            m_t = t;
            for(int i = 0; i < m_listeners.size(); i++)
                m_listeners.get(i).OnChangeLinearInterpolation(this, t);
        }
    }

    public int getTF1Type()
    {
        return m_tf1Type;
    }

    public int getTF2Type()
    {
        return m_tf2Type;
    }

    public TransferFunction getTF1Data()
    {
        return m_tf1;
    }

    public TransferFunction getTF2Data()
    {
        return m_tf2;
    }

    @Override
    public long getNativeTransferFunction()
    {
        return nativeCreatePtr(m_t, m_tf1Ptr, m_tf2Ptr);
    }

    @Override
    public int getType()
    {
        return SubDataset.TRANSFER_FUNCTION_MERGE;
    }

    public void finalize()
    {
        nativeDeletePtr(m_tf1Ptr);
        nativeDeletePtr(m_tf2Ptr);
    }

    private native long nativeCreatePtr(float t, long tf1Ptr, long tf2Ptr);

    private native void nativeDeletePtr(long ptr);
}