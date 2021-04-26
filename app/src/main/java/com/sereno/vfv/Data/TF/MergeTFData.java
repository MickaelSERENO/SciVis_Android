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

    /** The first transfer function data. Must be casted according to m_tf1Type*/
    private TransferFunction m_tf1 = null;

    /** The second transfer function data. Must be casted according to m_tf2Type*/
    private TransferFunction m_tf2 = null;

    /** The transfer function native C++ std::share_ptr\<TF\> pointer*/
    private long m_ptr = 0;

    /** The listeners to call when the state of this model has changed*/
    private ArrayList<IMergeTFDataListener> m_listeners = new ArrayList<>();

    /** Constructor
     * @param sd the SubDataset bound to this model
     * @param tf1 the data of the first transfer function.
     * @param tf2 the data of the second transfer function*/
    public MergeTFData(SubDataset sd, TransferFunction tf1, TransferFunction tf2)
    {
        m_tf1     = tf1;
        m_tf2     = tf2;
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

    /** Get the interpolation t parameter. Value ranges between 0.0f and 1.0f
     * @return the interpolation t parameter. 0.0f == getTF1() being fully rendered. 1.0f == getTF2() being fully rendered.*/
    public float getInterpolationParameter() {return nativeGetInterpolationParameter(getNativeTransferFunction());}

    /** Set the linear t parameter. Value ranges between 0.0f and 1.0f
     * @param t the linear t parameter. 0.0f == getTF1() being fully rendered. 1.0f == getTF2() being fully rendered.*/
    public void setInterpolationParameter(float t)
    {
        t = Math.min(Math.max(t, 0.0f), 1.0f);

        if(t != getInterpolationParameter())
        {
            nativeSetInterpolationParameter(getNativeTransferFunction(), t);
            for(int i = 0; i < m_listeners.size(); i++)
                m_listeners.get(i).OnChangeLinearInterpolation(this, t);
            callOnUpdateListeners();
        }
    }

    /** Get the first transfer function being merged (at t==0.0)
     * @return the first transfer function data*/
    public TransferFunction getTF1Data()
    {
        return m_tf1;
    }

    /** Get the second transfer function being merged (at t==1.0)
     * @return the second transfer function data*/
    public TransferFunction getTF2Data()
    {
        return m_tf2;
    }

    @Override
    public long getNativeTransferFunction()
    {
        if(m_ptr == 0)
            m_ptr = nativeCreatePtr(0.5f,  m_tf1.getNativeTransferFunction(), m_tf2.getNativeTransferFunction());
        return m_ptr;
    }

    @Override
    public int getType()
    {
        return SubDataset.TRANSFER_FUNCTION_MERGE;
    }

    public void finalize()
    {
        nativeDeleteTF(m_ptr);
    }

    public Object clone()
    {
        MergeTFData mergeTF = new MergeTFData(m_sd, (TransferFunction)m_tf1.clone(), (TransferFunction)m_tf2.clone());

        long oldTF = mergeTF.m_ptr;
        long newTF = nativeCloneTF(oldTF);
        mergeTF.m_ptr = newTF;
        nativeDeleteTF(oldTF);

        return mergeTF;
    }

    private native long  nativeCreatePtr(float t, long tf1Ptr, long tf2Ptr);
    private native long  nativeSetInterpolationParameter(long ptr, float t);
    private native float nativeGetInterpolationParameter(long ptr);
}