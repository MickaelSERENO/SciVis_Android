package com.sereno.vfs.Data;

import java.util.ArrayList;

/** \brief The Model component on the MVC architecture */
public class ApplicationModel
{
    /** \brief Interface possessing functions called when deleting or adding new datasets */
    public interface IDataCallback
    {
        /** \brief Function called when a dataset has been added (the call if after the addition)
         * \param d the dataset to add*/
        void onAddDataset(ApplicationModel model, FluidDataset d);

        /** \brief Function called when a dataset is about to be deleted. The call is done before the true deletion
         * \param idx the dataset index to remove*/
        void onDeleteDataset(ApplicationModel model, int idx);
    }

    private ArrayList<FluidDataset>  m_datasets;  /**!< The open datasets */
    private ArrayList<IDataCallback> m_listeners; /**!< The known listeners to call when the model changed*/

    /** \brief Basic constructor, initialize the data at its default state */
    public ApplicationModel()
    {
        m_datasets  = new ArrayList<>();
        m_listeners = new ArrayList<>();
    }

    /** Add a callback object to call when the model changed
     * @param clbk the new callback to take account of*/
    public void addCallback(IDataCallback clbk)
    {
        m_listeners.add(clbk);
    }

    /** \brief Add a FluidDataset to our model
     *  \param dataset the dataset to add*/
    public void addFluidDataset(FluidDataset dataset)
    {
        m_datasets.add(dataset);
        for(IDataCallback clbk : m_listeners)
            clbk.onAddDataset(this, dataset);
    }

    public void deleteFluidDataset(int idx)
    {
        for(IDataCallback clbk : m_listeners)
            clbk.onDeleteDataset(this, idx);
        //TODO
    }

    /** \brief Get the fluid datasets available
     * \return a List of the datasets*/
    public ArrayList<FluidDataset> getFluidDatasets() {return m_datasets;}
}
