package com.sereno.vfv.Data;

import java.util.ArrayList;
import java.util.List;

/** SubDataset meta data encapsulating meta data around SubDataset (e.g., visibility)*/
public class SubDatasetMetaData
{
    public interface ISubDatasetMetaDataListener
    {
        /** Function called when the visibility of the SubDataset changed
         * @param metaData the metaData changing
         * @param v the new visibility*/
        void onSetVisibility(SubDatasetMetaData metaData, int v);
    }

    /** List of listeners bound to this SubDatasetMetaData*/
    private List<ISubDatasetMetaDataListener> m_listeners = new ArrayList<>();

    /** The SubDataset public state*/
    private SubDataset m_public;

    /** The cloned SubDataset private state*/
    private SubDataset m_private;

    /** The visibility of this current SubDataset*/
    private int        m_visibility = SubDataset.VISIBILITY_PUBLIC;

    /** Constructor
     * @param publicSD the public state data. A private cloned state data will be created from this. However the private state "lose" his true type*/
    public SubDatasetMetaData(SubDataset publicSD)
    {
        m_public  = publicSD;
        m_private = (SubDataset)publicSD.clone();
    }

    /** Add a new callback Listener
     * @param listener the listener to call for new events*/
    public void addListener(ISubDatasetMetaDataListener listener)
    {
        m_listeners.add(listener);
    }

    /** Remove an old callback Listener
     * @param listener the listener to remove from the list*/
    public void removeListener(ISubDatasetMetaDataListener listener)
    {
        m_listeners.remove(listener);
    }

    /** Get the public state
     * @return the public subdataset state*/
    public SubDataset getPublicState()
    {
        return m_public;
    }

    /** Get the private state
     * @return the private subdataset state*/
    public SubDataset getPrivateState()
    {
        return m_private;
    }

    /** Get the current visibility of this SubDataset
     * @return the current visibility of this SubDataset*/
    public int getVisibility()
    {
        return m_visibility;
    }

    /** Set the current visibility of this SubDataset
     * @param v the current visibility of this SubDataset*/
    public void setVisibility(int v)
    {
        m_visibility = v;
        for(int i = 0; i < m_listeners.size(); i++)
            m_listeners.get(i).onSetVisibility(this, v);
    }

    @Override
    public void finalize() throws Throwable
    {
        m_private.free();
        super.finalize();
    }
}
