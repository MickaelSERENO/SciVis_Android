package com.sereno.vfs.Data;

import java.util.ArrayList;

/* \brief The Model component on the MVC architecture */
public class ApplicationModel
{
    /* \brief Interface possessing functions called when deleting or adding new datasets */
    public interface IDataCallback
    {
        /* \brief Function called when a dataset has been added (the call if after the addition)*/
        void OnAddDataset(ApplicationModel model, FluidDataset d);

        /* \brief Function called when a dataset is about to be deleted. The call is done before the true deletion */
        void OnDeleteDataset(ApplicationModel model, FluidDataset d);
    }

    private ArrayList<FluidDataset> m_datasets; /*!< The open datasets */

    /* \brief Basic constructor, initialize the data at its default state */
    public ApplicationModel()
    {
        m_datasets = new ArrayList<>();
    }

    /* \brief Get the fluid datasets available
    * \return a List of the datasets*/
    public ArrayList<FluidDataset> getFluidDatasets() {return m_datasets;}
}
