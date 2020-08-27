package com.sereno.vfv.Data.TF;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Objects;

import com.sereno.vfv.Data.CPCPTexture;
import com.sereno.vfv.Data.Dataset;
import com.sereno.vfv.Data.PointFieldDesc;
import com.sereno.vfv.Data.SubDataset;

/** Model used for the GTF*/
public class GTFData extends TransferFunction implements Dataset.IDatasetListener
{
    /** The Listener interface*/
    public interface IGTFDataListener
    {
        /** Function called when the SubDataset bound to this Model has changed.
         * @param model the model calling this method
         * @param dataset the new dataset*/
        void onSetDataset(GTFData model, SubDataset dataset);

        /** Function called when the GTF ranges has changed
         * @param model the model calling this method
         * @param ranges the HashMap containing the new ranges. Key == ptFieldID, Values = ranges (PointF.x == min, PointF.y == max). Do not modify the HashMap.*/
        void onSetGTFRanges(GTFData model, HashMap<Integer, GTFPoint> ranges);

        /** Function called when the GTF CPCP order has changed
         * @param model the model calling this method
         * @param order the new order to apply. See getCPCPOrder() for more details.*/
        void onSetCPCPOrder(GTFData model, int[] order);

        /** Function called when the SubDataset internal data has been loaded
         * @param model the model calling this method
         * @param dataset the dataset bound to this model*/
        void onLoadDataset(GTFData model, SubDataset dataset);
    }

    /** Class containing data per point field*/
    public static class GTFPoint
    {
        /** The center to apply*/
        public float center;

        /** The scale to apply*/
        public float scale;

        /** Is this point active?*/
        public boolean active;

        /** Default constructor, set center == scale == 0.5f*/
        public GTFPoint()
        {
            this(0.5f, 0.5f);
        }

        /** Constructor
         * @param c the center to apply
         * @param s the scale to apply*/
        public GTFPoint(float c, float s)
        {
            center = c;
            scale  = s;
            active = true;
        }

        @Override
        public boolean equals(Object o)
        {
            if(o == null)
                return false;

            try
            {
                GTFPoint p = (GTFPoint)o;
                return (center == p.center && scale == p.scale && active == p.active);
            }

            catch(ClassCastException e)
            {
                return false;
            }
        }

        @Override
        public Object clone()
        {
            GTFPoint p = new GTFPoint(center, scale);
            p.active = active;
            return p;
        }

        @Override
        public int hashCode()
        {
            return Objects.hash(center, scale, active);
        }
    }

    /** The Dataset from which we are manipulating the GTF ranges*/
    private SubDataset m_sd = null;

    /** The "parent" Dataset in use*/
    private Dataset m_dataset = null;

    /** The array of point field desc being used*/
    private PointFieldDesc[] m_ptFieldDescs = null;

    /** The HashMap bounding the ptFieldID to its ranges*/
    private HashMap<Integer, GTFPoint> m_ranges = new HashMap<>();

    /** The listeners to call when the current state of the GTF model changed*/
    private ArrayList<IGTFDataListener> m_listeners = new ArrayList<>();

    /** The C++ native pointer of the GTF / Triangular GTF objects*/
    private long m_ptr = 0;

    /** The current order of cpcp to display*/
    protected int[] m_cpcpOrder = new int[0];

    /** Is the gradient enabled?*/
    protected boolean m_gradientEnabled = false;

    public GTFData(SubDataset sd)
    {
        setDataset(sd);
    }

    /** Add a new listener to call if not already registered
     * @param l the new listener to add*/
    public void addListener(IGTFDataListener l)
    {
        if(!m_listeners.contains(l))
            m_listeners.add(l);
    }

    /** remove an already registered listener
     * @param l the listener to remove*/
    public void removeListener(IGTFDataListener l)
    {
        m_listeners.remove(l);
    }

    /** Set the Dataset bound to this GTF Data model
     * This will reinit the ranges and the cpcp order to their default values
     * @param sd the new SubDataset to consider.*/
    public void setDataset(SubDataset sd)
    {
        if(m_dataset != null)
            m_dataset.removeListener(this);

        m_dataset = null;
        m_sd = sd;
        m_ranges.clear();
        m_cpcpOrder = new int[0];

        //Load the ranges and update the new point field descs being used
        //Update also the CPCP order
        if(m_sd != null)
        {
            m_dataset = sd.getParent();

            m_ptFieldDescs = m_dataset.getPointFieldDescs();
            m_cpcpOrder = new int[m_ptFieldDescs.length];

            for(int i = 0; i < m_ptFieldDescs.length; i++)
            {
                PointFieldDesc desc = m_ptFieldDescs[i];
                m_ranges.put(desc.getID(), new GTFPoint());
                m_cpcpOrder[i] = desc.getID();
            }

            m_dataset.addListener(this);
            createNativeTransferFunction();
        }

        for(int i = 0; i < m_listeners.size(); i++)
            m_listeners.get(i).onSetDataset(this, m_sd);
    }

    /** Get the Dataset bound to this model
     * @return the SubDataset in use, can be null (i.e., no subdataset in use)*/
    public SubDataset getDataset()
    {
        return m_sd;
    }

    /** Get the ranges of the GTF parameters for each ptFieldID
     * @return the HashMap containing the new ranges. Key == ptFieldID, Values = ranges (PointF.x == min, PointF.y == max). Values are normalized. Do not modify the HashMap*/
    public HashMap<Integer, GTFPoint> getRanges()
    {
        return m_ranges;
    }

    /** Set the ranges of the GTF parameters for a given ptFieldID
     * @param ptFieldID the ptFieldID to change the ranges to.
     * @param range the new range
     * @return true if ptFieldID is valid, false otherwise. If false, the status of this method does not change and no callback method are fired*/
    public boolean setRange(int ptFieldID, GTFPoint range)
    {
        if(m_ranges.containsKey(ptFieldID))
        {
            boolean callListener = !(m_ranges.get(ptFieldID).equals(range));
            m_ranges.put(ptFieldID, range);
            updatePtrRanges();

            if(callListener)
                for(int i = 0; i < m_listeners.size(); i++)
                    m_listeners.get(i).onSetGTFRanges(this, m_ranges);
            return true;
        }
        return false;
    }

    /** Update whole ranges
     * @param ranges the new ranges to use*/
    public void updateRanges(HashMap<Integer, GTFPoint> ranges)
    {
        boolean callListener = !m_ranges.equals(ranges);
        m_ranges = ranges;
        if(callListener)
        {
            updatePtrRanges();
            for(int i = 0; i < m_listeners.size(); i++)
                m_listeners.get(i).onSetGTFRanges(this, m_ranges);
        }
    }

    /** Get the CPCP order.
     * 0 element == default, 1 element == 1D histogram should be used, 2 elements or more: stitch the CPCPTexture together
     * @return an array of cpcp ID to follow.*/
    public int[] getCPCPOrder() {return m_cpcpOrder;}

    /** Set the cpcp order to use.
     * 0 element == default, 1 element == 1D histogram should be used, 2 elements or more: stitch the CPCPTexture together
     * @param order the new cpcp order to use*/
    public void setCPCPOrder(int[] order)
    {
        m_cpcpOrder = order;
        for(int i = 0; i < m_listeners.size(); i++)
            m_listeners.get(i).onSetCPCPOrder(this, order);
    }

    /** Set the gradient enability of this GTF
     * @param enabled the new value to apply*/
    public void setGradient(boolean enabled)
    {
        boolean oldGradient = m_gradientEnabled;
        m_gradientEnabled = enabled;

        if(oldGradient != enabled)
             createNativeTransferFunction(); //Change the type of the transfer function
    }


    /** Is the gradient enabled for this GTF
     * @return true if yes, false otherwise*/
    public boolean isGradientEnabled()
    {
        return m_gradientEnabled;
    }

    @Override
    public void onRemoveSubDataset(Dataset dataset, SubDataset sd) {}

    @Override
    public void onAddSubDataset(Dataset dataset, SubDataset sd) {}

    @Override
    public void onLoadDataset(Dataset dataset, boolean success)
    {
        for(int i = 0; i < m_listeners.size(); i++)
            m_listeners.get(i).onLoadDataset(this, m_sd);
    }

    @Override
    public void onLoadCPCPTexture(Dataset dataset, CPCPTexture texture)
    {}

    @Override
    public void onLoad1DHistogram(Dataset dataset, float[] values, int pID)
    {}

    @Override
    public long getNativeTransferFunction()
    {
        if(m_ptr == 0)
            createNativeTransferFunction();
        return m_ptr;
    }

    @Override
    public int getType()
    {
        return (m_gradientEnabled ? SubDataset.TRANSFER_FUNCTION_TGTF : SubDataset.TRANSFER_FUNCTION_GTF);
    }

    private void createNativeTransferFunction()
    {
        if(m_sd == null)
            return;

        if(m_ptr != 0)
            nativeDeleteTF(m_ptr);
        m_ptr = nativeCreatePtr(m_dataset.getPtr(), m_gradientEnabled, getColorMode());
        updatePtrRanges();
    }

    private void updatePtrRanges()
    {
        if(m_sd == null)
            return;

        HashMap<Integer, GTFPoint> ranges = getRanges();

        //Parse the HashMap in a easy-to-send data to C++
        int[]   pIDs    = new int[ranges.size()];
        float[] centers = new float[ranges.size()];
        float[] scales  = new float[ranges.size()];
        int i = 0;

        for (Integer pID : getRanges().keySet())
        {
            pIDs[i] = pID;
            centers[i] = ranges.get(pID).center;
            scales[i]  = ranges.get(pID).scale;
            i++;
        }

        nativeUpdateRanges(m_ptr, m_dataset.getPtr(), m_gradientEnabled, getColorMode(), pIDs, centers, scales);
        callOnUpdateListeners();
    }

    native private long nativeCreatePtr(long datasetPtr, boolean enableGradient, int colorMode);
    native private void nativeUpdateRanges(long ptr, long datasetPtr, boolean enabledGradient, int colorMode, int[] pIDS, float[] centers, float[] scales);
}