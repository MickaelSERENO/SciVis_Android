package com.sereno.vfv.Data;

import java.util.ArrayList;

/** @brief The Model component on the MVC architecture */
public class ApplicationModel
{
    /** @brief Interface possessing functions called when deleting or adding new datasets */
    public interface IDataCallback
    {
        /** @brief Function called when a dataset has been added (the call if after the addition)
         * @param model the app data
         * @param d the dataset to add*/
        void onAddBinaryDataset(ApplicationModel model, BinaryDataset d);

        /** @brief Function called when a VTK dataset has been added (the call if after the addition)
         * @param model the app data
         * @param d the dataset to add*/
        void onAddVTKDataset(ApplicationModel model, VTKDataset d);
    }

    private ArrayList<VTKDataset>    m_vtkDatasets;    /**!< The vtk dataset */
    private ArrayList<BinaryDataset> m_binaryDatasets; /**!< The open datasets */
    private ArrayList<IDataCallback> m_listeners;      /**!< The known listeners to call when the model changed*/

    /** @brief Basic constructor, initialize the data at its default state */
    public ApplicationModel()
    {
        m_vtkDatasets    = new ArrayList<>();
        m_binaryDatasets = new ArrayList<>();
        m_listeners      = new ArrayList<>();
    }

    /** @brief Add a callback object to call when the model changed
     * @param clbk the new callback to take account of*/
    public void addCallback(IDataCallback clbk)
    {
        m_listeners.add(clbk);
    }

    /** @brief Add a BinaryDataset to our model
     *  @param dataset the dataset to add*/
    public void addBinaryDataset(BinaryDataset dataset)
    {
        m_binaryDatasets.add(dataset);
        for(IDataCallback clbk : m_listeners)
            clbk.onAddBinaryDataset(this, dataset);
    }

    /** @brief Add a VTKParser object into the known object loaded
     * @param dataset the VTKDataset object*/
    public void addVTKDataset(VTKDataset dataset)
    {
        m_vtkDatasets.add(dataset);
        for(IDataCallback clbk : m_listeners)
            clbk.onAddVTKDataset(this, dataset);
    }
}
