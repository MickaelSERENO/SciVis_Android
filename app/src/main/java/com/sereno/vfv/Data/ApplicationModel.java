package com.sereno.vfv.Data;

import android.content.Context;
import android.graphics.Bitmap;
import android.util.Range;
import android.widget.ImageView;

import com.sereno.vfv.Network.HeadsetBindingInfoMessage;
import com.sereno.vfv.Network.HeadsetsStatusMessage;
import com.sereno.view.AnnotationData;
import com.sereno.view.RangeColorData;

import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;

/** @brief The Model component on the MVC architecture */
public class ApplicationModel implements RangeColorData.IOnRangeChangeListener, SubDataset.ISubDatasetListener
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

        /** @brief Function called when an annotation is waiting to be added
         * @param model the app data
         * @param sd the targeted subdataset */
        void onPendingAnnotation(ApplicationModel model, SubDataset sd);

        /** @brief Function called when a pending annotation ended
         * @param model the app data
         * @param sd the targeted subdataset
         * @param cancel true if the annotation has been canceled, false otherwise*/
        void onEndPendingAnnotation(ApplicationModel model, SubDataset sd, boolean cancel);

        /** @brief Method called when the current device action changed
         * @param model the app data
         * @param action the new current action*/
        void onChangeCurrentAction(ApplicationModel model, int action);

        /** @brief Method called when the current SubDataset changed
         * @param model the app data
         * @param sd the new current sub dataset*/
        void onChangeCurrentSubDataset(ApplicationModel model, SubDataset sd);

        /** Method called when the headsets status changed
         * @param model the app data
         * @param headsetsStatus the new headsets status. If a headset ID disappeares, it means it has been disconnected*/
        void onUpdateHeadsetsStatus(ApplicationModel model, HeadsetsStatusMessage.HeadsetStatus[] headsetsStatus);

        /** Method called when the binding information changed
         * @param model the app data
         * @param info the new binding information*/
        void onUpdateBindingInformation(ApplicationModel model, HeadsetBindingInfoMessage info);

        /** Remove a Dataset
         * @param model the application model
         * @param dataset the Dataset to remove*/
        void onRemoveDataset(ApplicationModel model, Dataset dataset);
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

    /** The HashMap linking subdatasets to their meta data*/
    private HashMap<SubDataset, SubDatasetMetaData> m_metaDatas = new HashMap<>();

    /** The bitmap showing the content of the annotations*/
    private HashMap<AnnotationData, AnnotationMetaData> m_annotations = new HashMap<>();

    /** The current action*/
    private int m_currentAction = CURRENT_ACTION_NOTHING;

    /** The current subdataset*/
    private SubDataset m_currentSubDataset = null;

    /** The headsets status*/
    private HeadsetsStatusMessage.HeadsetStatus[] m_headsetsStatus = null;

    /** The headset binding information*/
    private HeadsetBindingInfoMessage m_bindingInfo = null;

    private SubDataset m_pendingSubDataset = null;

    /** @brief Basic constructor, initialize the data at its default state */
    public ApplicationModel(Context ctx)
    {
        m_vtkDatasets    = new ArrayList<>();
        m_binaryDatasets = new ArrayList<>();
        m_listeners      = new ArrayList<>();

        readConfig(ctx);
    }

    /** Remove an already registered listener
     * @param clbk the listener to not call anymore*/
    public void removeListener(IDataCallback clbk)
    {
        m_listeners.remove(clbk);
    }

    /** @brief Add a callback object to call when the model changed
     * @param clbk the new callback to take account of*/
    public void addListener(IDataCallback clbk)
    {
        if(!m_listeners.contains(clbk))
            m_listeners.add(clbk);
    }

    /** Perform common actions when adding datasets
     * @param d the dataset in adding state.*/
    private void onAddDataset(Dataset d)
    {
        //Create the meta data associated to each subdatasets
        for(SubDataset sd : d.getSubDatasets())
        {
            SubDatasetMetaData metaData = new SubDatasetMetaData(sd);
            m_metaDatas.put(sd, metaData);
            m_metaDatas.put(metaData.getPrivateState(), metaData);
        }
    }

    /** @brief Add a BinaryDataset to our model
     *  @param dataset the dataset to add*/
    public void addBinaryDataset(BinaryDataset dataset)
    {
        m_binaryDatasets.add(dataset);
        onAddDataset(dataset);
        for(IDataCallback clbk : m_listeners)
            clbk.onAddBinaryDataset(this, dataset);
    }

    /** @brief Add a VTKParser object into the known object loaded
     * @param dataset the VTKDataset object*/
    public void addVTKDataset(VTKDataset dataset)
    {
        m_vtkDatasets.add(dataset);
        onAddDataset(dataset);
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

    public void removeSubDataset(SubDataset sd)
    {
        if(sd == m_currentSubDataset)
            setCurrentSubDataset(null);
        else if(sd == m_pendingSubDataset)
            pendingAnnotation(null);

        m_metaDatas.remove(m_metaDatas.get(sd).getPrivateState());
        m_metaDatas.remove(sd);
        sd.getParent().removeSubDataset(sd);
    }

    /**Remove a given dataset from the model
     * @param dataset the dataset to remove*/
    public void removeDataset(Dataset dataset)
    {
        //First remove the subdatasets
        for(SubDataset sd : dataset.getSubDatasets())
        {
            removeSubDataset(sd);
        }

        //Then the dataset in itself
        if(m_vtkDatasets.contains(dataset))
        {
            for(IDataCallback clbk : m_listeners)
                clbk.onRemoveDataset(this, dataset);
            m_vtkDatasets.remove(dataset);
        }

        else if(m_binaryDatasets.contains(dataset))
        {
            for(IDataCallback clbk : m_listeners)
                clbk.onRemoveDataset(this, dataset);
            m_binaryDatasets.remove(dataset);
        }
    }

    /**@brief Set the RangeColorData model being used to clamp subdatasets color displayed
     * @param model the RangeColorData model being used to clamp subdatasets color displayed*/
    public void setRangeColorModel(RangeColorData model)
    {
        if(m_rangeColorModel != null)
            m_rangeColorModel.removeOnRangeChangeListener(this);
        m_rangeColorModel = model;
        m_rangeColorModel.addOnRangeChangeListener(this);
    }

    @Override
    public void onRangeChange(RangeColorData data, float minVal, float maxVal, int mode)
    {
        if(m_currentSubDataset != null)
            m_currentSubDataset.setClamping(minVal, maxVal);
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
        if(m_currentSubDataset != null)
            m_currentSubDataset.removeListener(this);

        m_currentSubDataset = sd;

        if(sd != null)
        {
            sd.addListener(this);
            onClampingChange(sd, sd.getMinClampingColor(), sd.getMaxClampingColor());
        }

        for(IDataCallback clbk : m_listeners)
            clbk.onChangeCurrentSubDataset(this, sd);
    }

    /** Get the current SubDataset
     * @return The current SubDataset*/
    public SubDataset getCurrentSubDataset()
    {
        return m_currentSubDataset;
    }

    public void setHeadsetsStatus(HeadsetsStatusMessage.HeadsetStatus[] status)
    {
        m_headsetsStatus = status;
        for(IDataCallback clbk : m_listeners)
            clbk.onUpdateHeadsetsStatus(this, status);
    }

    /** Get the current available headsets status
     * @return array of headsets status or null if no data have been received yet*/
    public HeadsetsStatusMessage.HeadsetStatus[] getHeadsetsStatus()
    {
        return m_headsetsStatus;
    }

    /** Set the binding information
     * @param info the new binding information regarding this device and the headset*/
    public void setBindingInfo(HeadsetBindingInfoMessage info)
    {
        m_bindingInfo = info;
        for(IDataCallback clbk : m_listeners)
            clbk.onUpdateBindingInformation(this, info);
    }

    /** Get the binding information
     * @return the binding information regarding this device and the headset*/
    public HeadsetBindingInfoMessage getBindingInfo() {return m_bindingInfo;}

    public SubDatasetMetaData getSubDatasetMetaData(SubDataset d) {return m_metaDatas.get(d);}

    public void pendingAnnotation(SubDataset sd)
    {
        if(m_pendingSubDataset != null)
            endPendingAnnotation(true);

        m_pendingSubDataset = sd;
        for(IDataCallback clbk : m_listeners)
            clbk.onPendingAnnotation(this, sd);
    }

    public void endPendingAnnotation(boolean cancel)
    {
        for(IDataCallback clbk : m_listeners)
            clbk.onEndPendingAnnotation(this, m_pendingSubDataset, cancel);
        m_pendingSubDataset = null;
    }

    @Override
    public void onClampingChange(SubDataset sd, float min, float max)
    {
        if(m_rangeColorModel == null)
            return;

        if(m_currentSubDataset != null)
            m_currentSubDataset.removeListener(this);
        m_rangeColorModel.removeOnRangeChangeListener(this);

        if(min != m_rangeColorModel.getMinRange() || max != m_rangeColorModel.getMaxRange())
            m_rangeColorModel.setRange(min, max);

        if(m_currentSubDataset != null)
            m_currentSubDataset.addListener(this);
        m_rangeColorModel.addOnRangeChangeListener(this);
    }

    @Override
    public void onRotationEvent(SubDataset dataset, float[] quaternion) {}

    @Override
    public void onPositionEvent(SubDataset dataset, float[] position) {}

    @Override
    public void onScaleEvent(SubDataset dataset, float[] scale) {}

    @Override
    public void onSnapshotEvent(SubDataset dataset, Bitmap snapshot) {}

    @Override
    public void onAddAnnotation(SubDataset dataset, AnnotationData annotation) {}

    @Override
    public void onRemove(SubDataset dataset) {}


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
