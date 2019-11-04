package com.sereno.view;


import android.graphics.Point;
import android.graphics.PointF;

import java.util.ArrayList;
import java.util.HashMap;

import com.sereno.vfv.Data.Dataset;
import com.sereno.vfv.Data.PointFieldDesc;

/** Model used for the GTF*/
public class GTFData
{
    /** The Listener interface*/
    public interface IGTFDataListener
    {
        /** Function called when the Dataset bound to this Model has changed.
         * @param model the model calling this method
         * @param dataset the new dataset*/
        void onSetDataset(GTFData model, Dataset dataset);

        /** Function called when the GTF ranges has changed
         * @param model the model calling this method
         * @param ranges the HashMap containing the new ranges. Key == ptFieldID, Values = ranges (PointF.x == min, PointF.y == max). Do not modify the HashMap.*/
        void onModifyGTFRange(GTFData model, HashMap<Integer, PointF> ranges);
    }

    /** The Dataset from which we are manipulating the GTF ranges*/
    private Dataset m_dataset = null;

    /** The array of point field desc being used*/
    private PointFieldDesc[] m_ptFieldDescs = null;

    /** The HashMap bounding the ptFieldID to its ranges*/
    private HashMap<Integer, PointF> m_ranges = new HashMap<>();

    /** The listeners to call when the current state of the GTF model changed*/
    private ArrayList<IGTFDataListener> m_listeners = new ArrayList<>();

    public GTFData(Dataset dataset)
    {
        m_dataset = dataset;
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
     * @param dataset the newly dataset created. Can be null*/
    public void setDataset(Dataset dataset)
    {
        m_dataset = dataset;

        //Load the ranges and update the new point field descs being used
        if(m_dataset != null)
        {
            m_ranges.clear();
            m_ptFieldDescs = dataset.getPointFieldDescs();
            for(PointFieldDesc desc : m_ptFieldDescs)
                m_ranges.put(desc.getID(), new PointF(desc.getMin(), desc.getMax()));
        }

        for(int i = 0; i < m_listeners.size(); i++)
            m_listeners.get(i).onSetDataset(this, dataset);
    }

    /** Get the Dataset bound to this model
     * @return the Dataset in use, can be null (i.e., no dataset in use)*/
    public Dataset getDataset()
    {
        return m_dataset;
    }

    /** Get the ranges of the GTF parameters for each ptFieldID
     * @return the HashMap containing the new ranges. Key == ptFieldID, Values = ranges (PointF.x == min, PointF.y == max). Do not modify the HashMap*/
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
                m_listeners.get(i).onModifyGTFRange(this, m_ranges);
            return true;
        }
        return false;
    }
}