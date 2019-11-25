package com.sereno.view;

import android.graphics.PointF;

import java.util.ArrayList;
import java.util.HashMap;

import com.sereno.color.ColorMode;
import com.sereno.vfv.Data.CPCPTexture;
import com.sereno.vfv.Data.Dataset;
import com.sereno.vfv.Data.PointFieldDesc;
import com.sereno.vfv.Data.SubDataset;

/** Model used for the GTF*/
public class GTFData implements Dataset.IDatasetListener
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
        void onSetGTFRange(GTFData model, HashMap<Integer, PointF> ranges);

        /** Function called when the GTF CPCP order has changed
         * @param model the model calling this method
         * @param order the new order to apply. See getCPCPOrder() for more details.*/
        void onSetCPCPOrder(GTFData model, int[] order);

        /** Function called when a new color mode is to be applied to the visualization widget
         * @param model the model colling this method
         * @param colorMode the new color mode to apply (see ColorMode static fields)*/
        void onSetColorMode(GTFData model, int colorMode);
    }

    /** The Dataset from which we are manipulating the GTF ranges*/
    private SubDataset m_sd = null;

    /** The "parent" Dataset in use*/
    private Dataset m_dataset = null;

    /** The array of point field desc being used*/
    private PointFieldDesc[] m_ptFieldDescs = null;

    /** The HashMap bounding the ptFieldID to its ranges*/
    private HashMap<Integer, PointF> m_ranges = new HashMap<>();

    /** The listeners to call when the current state of the GTF model changed*/
    private ArrayList<IGTFDataListener> m_listeners = new ArrayList<>();

    /** The color mode to apply in the GTF visualization widget*/
    private int m_colorMode = ColorMode.GRAYSCALE;

    /** The current order of cpcp to display*/
    protected int[] m_cpcpOrder = new int[0];

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

            m_ranges.clear();
            m_ptFieldDescs = m_dataset.getPointFieldDescs();
            m_cpcpOrder = new int[m_ptFieldDescs.length];

            for(int i = 0; i < m_ptFieldDescs.length; i++)
            {
                PointFieldDesc desc = m_ptFieldDescs[i];
                m_ranges.put(desc.getID(), new PointF(0.0f, 1.0f));
                m_cpcpOrder[i] = desc.getID();
            }

            m_dataset.addListener(this);
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
    public HashMap<Integer, PointF> getRanges()
    {
        return m_ranges;
    }

    /** Set the ranges of the GTF parameters for a given ptFieldID
     * @param ptFieldID the ptFieldID to change the ranges to.
     * @param range the new ranges (Point.getX()== min, Point.getY() == max)
     * @return true if ptFieldID is valid, false otherwise. If false, the status of this method does not change and no callback method are fired*/
    public boolean setRange(int ptFieldID, PointF range)
    {
        if(m_ranges.containsKey(ptFieldID))
        {
            m_ranges.put(ptFieldID, range);
            for(int i = 0; i < m_listeners.size(); i++)
                m_listeners.get(i).onSetGTFRange(this, m_ranges);
            return true;
        }
        return false;
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

    /** Get the color mode to apply to the visualization widget
     * @return the color mode to apply (see ColorMode static fields)*/
    public int getColorMode()
    {
        return m_colorMode;
    }

    /** Set the color mode to apply to the visualization widget
     * @param mode the color mode to apply (see ColorMode static fields)*/
    public void setColorMode(int mode)
    {
        m_colorMode = mode;
        for(int i = 0; i < m_listeners.size(); i++)
            m_listeners.get(i).onSetColorMode(this, mode);
    }

    @Override
    public void onRemoveSubDataset(Dataset dataset, SubDataset sd) {}

    @Override
    public void onAddSubDataset(Dataset dataset, SubDataset sd) {}

    @Override
    public void onLoadDataset(Dataset dataset, boolean success)
    {
        //Reinit the ranges and order upon loading
        if(dataset == m_dataset && success)
            setDataset(m_sd);
    }

    @Override
    public void onLoadCPCPTexture(Dataset dataset, CPCPTexture texture)
    {}

    @Override
    public void onLoad1DHistogram(Dataset dataset, float[] values, int pID)
    {}
}