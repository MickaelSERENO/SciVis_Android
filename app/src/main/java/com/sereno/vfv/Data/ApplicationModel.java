package com.sereno.vfv.Data;

import android.content.Context;
import android.graphics.Bitmap;
import android.util.Range;
import android.widget.ImageView;

import com.sereno.view.AnnotationData;
import com.sereno.view.RangeColorData;

import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;

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

        /** @brief Function called when a new Annotation has been added
         * @param model the app data
         * @param annot the annotation true value
         * @param metaData the annotation meta data value*/
        void onAddAnnotation(ApplicationModel model, AnnotationData annot, AnnotationMetaData metaData);

        /** @brief Method called when the current device action changed
         * @param model the app data
         * @param action the new current action*/
        void onChangeCurrentAction(ApplicationModel model, int action);

        /** @brief Method called when the current SubDataset changed
         * @param model the app data
         * @param sd the new current sub dataset*/
        void onChangeCurrentSubDataset(ApplicationModel model, SubDataset sd);
    }

    /** Annotation meta data*/
    public static class AnnotationMetaData
    {
        /** The subdataset this annotation is bound to*/
        public SubDataset m_subDataset;

        /** The annotation ID defined by the server.*/
        public int m_annotationID = 0;

        /** Constructor
         * @param subDataset the subdataset bound to this annotation
         * @param annotationID the subdataset ID*/
        public AnnotationMetaData(SubDataset subDataset, int annotationID)
        {
            m_subDataset   = subDataset;
            m_annotationID = annotationID;
        }

        /** Get the annotation ID
         * @return the annotation Server ID*/
        public int getAnnotationID()
        {
            return m_annotationID;
        }

        /** Get the SubDataset possessing this annotation
         * @return the SubDataset possessing this annotation*/
        public SubDataset getSubDataset()
        {
            return m_subDataset;
        }
    }

    /* All the available current action*/
    public final int CURRENT_ACTION_NOTHING   = 0;
    public final int CURRENT_ACTION_MOVING    = 1;
    public final int CURRENT_ACTION_SCALING   = 2;
    public final int CURRENT_ACTION_ROTATING  = 3;
    public final int CURRENT_ACTION_SKETCHING = 4;

    private ArrayList<VTKDataset>    m_vtkDatasets;     /**!< The vtk dataset */
    private ArrayList<BinaryDataset> m_binaryDatasets;  /**!< The open datasets */
    private ArrayList<IDataCallback> m_listeners;       /**!< The known listeners to call when the model changed*/
    private Configuration            m_config;          /**!< The configuration object*/
    private RangeColorData           m_rangeColorModel = null; /**!< The range color data model*/
    /** The bitmap showing the content of the annotations*/
    private HashMap<AnnotationData, AnnotationMetaData> m_annotations = new HashMap<>();

    /** The current action*/
    private int m_currentAction = CURRENT_ACTION_NOTHING;

    /** The current subdataset*/
    private SubDataset m_currentSubDataset = null;


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

    /** Add a new annotation
     * @param annotation the annotation to add
     * @param metaData the annotation meta data*/
    public void addAnnotation(AnnotationData annotation, AnnotationMetaData metaData)
    {
        m_annotations.put(annotation, metaData);
        metaData.getSubDataset().addAnnotation(annotation);

        for(IDataCallback clbk : m_listeners)
            clbk.onAddAnnotation(this, annotation, metaData);
    }

    /** Get the annations registered
     * @return a map containing the annotations and annotation metadata*/
    public HashMap<AnnotationData, AnnotationMetaData> getAnnotations()
    {
        return m_annotations;
    }

    public void setCurrentAction(int action)
    {
        m_currentAction = action;
        for(IDataCallback clbk : m_listeners)
            clbk.onChangeCurrentAction(this, action);
    }

    /** Get the current device action
     * @return The current device action*/
    public int getCurrentAction()
    {
        return m_currentAction;
    }

    /** Set the current SubDataset
     * @param sd The new current SubDataset*/
    public void setCurrentSubDataset(SubDataset sd)
    {
        m_currentSubDataset = sd;
        for(IDataCallback clbk : m_listeners)
            clbk.onChangeCurrentSubDataset(this, sd);
    }

    /** Get the current SubDataset
     * @return The current SubDataset*/
    public SubDataset getCurrentSubDataset()
    {
        return m_currentSubDataset;
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
