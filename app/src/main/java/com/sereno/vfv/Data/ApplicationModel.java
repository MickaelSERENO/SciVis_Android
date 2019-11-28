package com.sereno.vfv.Data;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.PointF;

import com.sereno.vfv.Network.HeadsetBindingInfoMessage;
import com.sereno.vfv.Network.HeadsetsStatusMessage;
import com.sereno.view.AnnotationData;
import com.sereno.view.GTFData;

import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;

/** @brief The Model component on the MVC architecture */
public class ApplicationModel implements SubDataset.ISubDatasetListener, Dataset.IDatasetListener, GTFData.IGTFDataListener
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

        /** Method called when the pointing technique is being updated
         * @param model the app data
         * @param pt the new pointing technique in use*/
        void onUpdatePointingTechnique(ApplicationModel model, int pt);
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

    /** All the available current action*/
    public final int CURRENT_ACTION_NOTHING   = 0;
    public final int CURRENT_ACTION_MOVING    = 1;
    public final int CURRENT_ACTION_SCALING   = 2;
    public final int CURRENT_ACTION_ROTATING  = 3;
    public final int CURRENT_ACTION_SKETCHING = 4;

    /** The pointing technique IDs*/
    public static final int POINTING_GOGO        = 0;
    public static final int POINTING_WIM         = 1;
    public static final int POINTING_WIM_POINTER = 2;
    public static final int POINTING_MANUAL      = 3;

    /** The handedness values*/
    public static final int HANDEDNESS_LEFT = 0;
    public static final int HANDEDNESS_RIGHT  = 1;

    private ArrayList<VTKDataset>    m_vtkDatasets;     /**!< The vtk dataset */
    private ArrayList<BinaryDataset> m_binaryDatasets;  /**!< The open binary Datasets */
    private ArrayList<Dataset>       m_datasets;        /**!< The open Dataset (vtk + binary)*/
    private ArrayList<IDataCallback> m_listeners;       /**!< The known listeners to call when the model changed*/
    private Configuration            m_config;          /**!< The configuration object*/

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

    /** The subdataset waiting to be added*/
    private SubDataset m_pendingSubDataset = null;

    /** Array containing the GTF Data of all the SubDataset*/
    private ArrayList<GTFData> m_gtfData = new ArrayList<>();

    private int m_curPointingTechnique = POINTING_MANUAL;

    /** @brief Basic constructor, initialize the data at its default state */
    public ApplicationModel(Context ctx)
    {
        m_vtkDatasets    = new ArrayList<>();
        m_binaryDatasets = new ArrayList<>();
        m_datasets       = new ArrayList<>();
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

    /** Add a new SubDataset to the known list
     * @param sd the SubDataset to add*/
    private void onAddSubDataset(SubDataset sd)
    {
        //Add the listener to the subdatasets
        sd.addListener(new SubDataset.ISubDatasetListener() {
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
            public void onRemove(SubDataset dataset) {removeSubDataset(dataset);}

            @Override
            public void onRemoveAnnotation(SubDataset dataset, AnnotationData annotation)
            {
                m_annotations.remove(annotation);
            }

            @Override
            public void onUpdateTF(SubDataset dataset) {}
        });

        GTFData newGTF = new GTFData(sd);
        m_gtfData.add(newGTF);

        newGTF.addListener(this);
    }

    /** Perform common actions when adding datasets
     * @param d the dataset in adding state.*/
    private void onAddDataset(Dataset d)
    {
        m_datasets.add(d);
        d.addListener(this);
    }

    /** @brief Add a BinaryDataset to our model
     *  @param dataset the dataset to add*/
    public void addBinaryDataset(BinaryDataset dataset)
    {
        m_binaryDatasets.add(dataset);
        onAddDataset(dataset);
        for(IDataCallback clbk : m_listeners)
            clbk.onAddBinaryDataset(this, dataset);

        for(SubDataset sd : dataset.getSubDatasets())
            onAddSubDataset(sd);
    }

    /** @brief Add a VTKParser object into the known object loaded
     * @param dataset the VTKDataset object*/
    public void addVTKDataset(VTKDataset dataset)
    {
        m_vtkDatasets.add(dataset);
        onAddDataset(dataset);
        for(IDataCallback clbk : m_listeners)
            clbk.onAddVTKDataset(this, dataset);

        for(SubDataset sd : dataset.getSubDatasets())
            onAddSubDataset(sd);
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

    public ArrayList<Dataset> getDatasets() {return m_datasets;}

    /** Remove a SubDataset from the model. Callback what is needed and reinitialize the status of the model
     * @param sd the SubDataset to remove*/
    private void removeSubDataset(SubDataset sd)
    {
        if(sd == m_currentSubDataset)
            setCurrentSubDataset(null);
        else if(sd == m_pendingSubDataset)
            pendingAnnotation(null);

        /////////////////////////////////////
        //Look for a new current subdataset//
        /////////////////////////////////////

        for(Dataset d : getDatasets())
        {
            if (d.getSubDatasets().size() > 0)
            {
                setCurrentSubDataset(d.getSubDatasets().get(0));
                break;
            }
        }
    }

    /**Remove a given dataset from the model
     * @param dataset the dataset to remove*/
    public void removeDataset(Dataset dataset)
    {
        //First remove the subdatasets
        for(int i = 0; i < dataset.getNbSubDataset(); i++)
        {
            dataset.removeSubDataset(dataset.getSubDataset(i));
        }

        //Then the dataset in itself
        if(m_datasets.contains(dataset))
        {
            for(IDataCallback clbk : m_listeners)
                clbk.onRemoveDataset(this, dataset);
            m_datasets.remove(dataset);
        }

        if(m_vtkDatasets.contains(dataset))
            m_vtkDatasets.remove(dataset);

        else if(m_binaryDatasets.contains(dataset))
            m_binaryDatasets.remove(dataset);
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

    /** Set the current action of the tablet bound to the tablet
     * @param action the type of action (see CURRENT_ACTION_*)*/
    public void setCurrentAction(int action)
    {
        m_currentAction = action;
        for(IDataCallback clbk : m_listeners)
            clbk.onChangeCurrentAction(this, action);
    }

    /** Get the current device action
     * @return The current device action. See CURRENT_ACTION_* values */
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

    /** Set the headsets' status connected to the server
     * @param status array of headsets' status*/
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

    /** Put a new SubDataset in wait for an annotation
     * @param sd the SubDataset waiting the annotation to be anchored*/
    public void pendingAnnotation(SubDataset sd)
    {
        if(m_pendingSubDataset != null)
            endPendingAnnotation(true);

        m_pendingSubDataset = sd;
        for(IDataCallback clbk : m_listeners)
            clbk.onPendingAnnotation(this, sd);
    }

    /** Get the SubDataset waiting to create an annotation
     * @return the subdataset waiting for the annotation. Can be null (no subdataset waiting)*/
    public SubDataset getPendingSubDatasetForAnnotation()
    {
        return m_pendingSubDataset;
    }

    /** End the waiting for an annotation to be anchored
     * @param cancel true if the pending is to be canceled, false otherwise*/
    public void endPendingAnnotation(boolean cancel)
    {
        for(IDataCallback clbk : m_listeners)
            clbk.onEndPendingAnnotation(this, m_pendingSubDataset, cancel);
        m_pendingSubDataset = null;
    }

    /** Set the current pointing technique to use
     * @param pt the new pointing technique to use (see POINTING_* )*/
    public void setCurrentPointingTechnique(int pt)
    {
        for(IDataCallback clbk : m_listeners)
            clbk.onUpdatePointingTechnique(this, pt);
        m_curPointingTechnique = pt;
    }

    /** Get the current pointing technique in use
     * @return the current pointing technique ID. See POINTING_* */
    public int getCurrentPointingTechnique()
    {
        return m_curPointingTechnique;
    }

    /** Return the GTF bound to a given SubDataset
     * @param sd the key dataset
     * @return the corresponding GTFData, null if not found*/
    public GTFData getGTFData(SubDataset sd)
    {
        for(GTFData gtf : m_gtfData)
            if(gtf.getDataset() == sd)
                return gtf;
        return null;
    }

    /*@Override
    public void onClampingChange(SubDataset sd, float min, float max)
    {
        if(m_rangeColorModel == null)
            return;

        //Avoid any "loop" between the range color being changed and the subdataset being changed
        if(m_currentSubDataset != null)
            m_currentSubDataset.removeListener(this);
        m_rangeColorModel.removeOnRangeChangeListener(this);

        if(min != m_rangeColorModel.getMinRange() || max != m_rangeColorModel.getMaxRange())
            m_rangeColorModel.setRange(min, max);

        if(m_currentSubDataset != null)
            m_currentSubDataset.addListener(this);
        m_rangeColorModel.addOnRangeChangeListener(this);
    }*/

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
    public void onRemove(SubDataset dataset)
    {
        dataset.removeListener(this);
    }

    @Override
    public void onRemoveAnnotation(SubDataset dataset, AnnotationData annotation) {}

    @Override
    public void onUpdateTF(SubDataset dataset) {}

    @Override
    public void onRemoveSubDataset(Dataset dataset, SubDataset sd) {}

    @Override
    public void onAddSubDataset(Dataset dataset, SubDataset sd)
    {
        onAddSubDataset(sd);
    }

    @Override
    public void onLoadDataset(Dataset dataset, boolean success)
    {
        //Reinit every gtf data
        for (GTFData gtf : m_gtfData)
            if (gtf.getDataset().getParent() == dataset)
            {
                SubDataset sd = gtf.getDataset();
                gtf.setDataset(sd);

                //Update ranges as well
                if(sd.getTransferFunctionType() == SubDataset.TRANSFER_FUNCTION_TGTF ||
                   sd.getTransferFunctionType() == SubDataset.TRANSFER_FUNCTION_GTF)
                {
                    sd.setGTFRanges(gtf.getRanges());
                    sd.setColorMode(gtf.getColorMode());
                }
            }
    }

    @Override
    public void onLoadCPCPTexture(Dataset dataset, CPCPTexture texture) {}

    @Override
    public void onLoad1DHistogram(Dataset dataset, float[] values, int pID) {}

    @Override
    public void onSetDataset(GTFData model, SubDataset dataset)
    {

    }

    @Override
    public void onSetGTFRanges(GTFData model, HashMap<Integer, PointF> ranges)
    {
        //Update transfer function
        SubDataset sd = model.getDataset();

        if(sd.getTransferFunctionType() == SubDataset.TRANSFER_FUNCTION_GTF ||
           sd.getTransferFunctionType() == SubDataset.TRANSFER_FUNCTION_TGTF)
            sd.setGTFRanges(ranges);
    }

    @Override
    public void onSetCPCPOrder(GTFData model, int[] order)
    {

    }

    @Override
    public void onSetColorMode(GTFData model, int colorMode)
    {
        SubDataset sd = model.getDataset();
        sd.setColorMode(colorMode);
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
