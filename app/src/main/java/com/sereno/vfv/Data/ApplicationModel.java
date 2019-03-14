package com.sereno.vfv.Data;

import android.content.Context;
import android.util.Range;

import com.sereno.view.RangeColorData;

import java.io.File;
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

    private ArrayList<VTKDataset>    m_vtkDatasets;     /**!< The vtk dataset */
    private ArrayList<BinaryDataset> m_binaryDatasets;  /**!< The open datasets */
    private ArrayList<IDataCallback> m_listeners;       /**!< The known listeners to call when the model changed*/
    private Configuration            m_config;          /**!< The configuration object*/
    private RangeColorData           m_rangeColorModel = null; /**!< The range color data model*/

    /** @brief Basic constructor, initialize the data at its default state */
    public ApplicationModel(Context ctx)
    {
        m_vtkDatasets    = new ArrayList<>();
        m_binaryDatasets = new ArrayList<>();
        m_listeners      = new ArrayList<>();

        readConfig(ctx);
    }

    /** @brief Add a callback object to call when the model changed
     * @param clbk the new callback to take account of*/
    public void addListener(IDataCallback clbk)
    {
        if(!m_listeners.contains(clbk))
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

    /** @brief Get the Configuration object
     * @return the Configuration object*/
    public Configuration getConfiguration()
    {
        return m_config;
    }

    /** @brief Get a list of VTK Datasets
     * @return the list of VTK Datasets opened*/
    public ArrayList<VTKDataset> getVTKDatasets() {return m_vtkDatasets;}

    /** @brief Get a list of Binary Datasets
     * @return the list of Binary Datasets opened*/
    public ArrayList<BinaryDataset> getBinaryDatasets() {return m_binaryDatasets;}

    /**@brief Get the RangeColorData model being used to clamp subdatasets color displayed
     * @return the RangeColorData model being used to clamp subdatasets color displayed*/
    public RangeColorData getRangeColorModel()
    {
        return m_rangeColorModel;
    }

    /**@brief Set the RangeColorData model being used to clamp subdatasets color displayed
     * @param model the RangeColorData model being used to clamp subdatasets color displayed*/
    public void setRangeColorModel(RangeColorData model)
    {
        m_rangeColorModel = model;
    }

    /** @brief Read the configuration file
     * @param ctx The Context object*/
    private void readConfig(Context ctx)
    {
        File configFile = new File(ctx.getExternalFilesDir(null), "config.json");
        if(configFile == null)
            m_config = new Configuration();
        else
            m_config = new Configuration(configFile);
    }
}
