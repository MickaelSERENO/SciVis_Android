package com.sereno.vfv.Data;

import android.content.Context;
import android.graphics.Bitmap;

import com.sereno.math.Quaternion;
import com.sereno.vfv.Network.HeadsetBindingInfoMessage;
import com.sereno.vfv.Network.HeadsetsStatusMessage;
import com.sereno.view.AnnotationData;

import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;

/** @brief The Model component on the MVC architecture */
public class ApplicationModel implements Dataset.IDatasetListener
{
    /** @brief Interface possessing functions called when deleting or adding new datasets */
    public interface IDataCallback
    {
        /** @brief Function called when a dataset has been added (the call is after the addition)
         * @param model the app data
         * @param d the dataset to add*/
        void onAddVectorFieldDataset(ApplicationModel model, VectorFieldDataset d);

        /** @brief Function called when a dataset has been added (the call is after the addition)
         * @param model the app data
         * @param d the dataset to add*/
        void onAddCloudPointDataset(ApplicationModel model, CloudPointDataset d);

        /** @brief Function called when a VTK dataset has been added (the call is after the addition)
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

        /** Method called when the tablet's location is being updated
         * @param model the app data
         * @param pos the new position
         * @param rot the new rotation*/
        void onSetLocation(ApplicationModel model, float[] pos, float[] rot);

        /** called when setting the virtual tablet scale
         * @param model the app data
         * @param scale the tablet's scale
         * @param width the tablet view's width
         * @param height the tablet view's height
         * @param posx the tablet view's horizontal position
         * @param posy the tablet view's vertical position*/
        void onSetTabletScale(ApplicationModel model, float scale, float width, float height, float posx, float posy);

        /** called when the lasso is traced
         * @param model the app data
         * @param data the lasso data*/
        void onSetLasso(ApplicationModel model, float[] data);

        /** called when confirming the selection
         * @param model the app data*/
        void onConfirmSelection(ApplicationModel model);

        /** Called when the current boolean operation has changed
         * @param model the app data model
         * @param op the new operation in action*/
        void onSetCurrentBooleanOperation(ApplicationModel model, int op);

        /** Called when the tangible mode of the application has changed
         * @param model the app data model
         * @param inTangibleMode the device current the tangible mode?*/
        void onSetTangibleMode(ApplicationModel model, int inTangibleMode);

        /** Called when the selection mode of the application has changed
         * @param model the app data model
         * @param selectMode the new selection mode to use. See SELECTION_MODE_* */
        void onSetSelectionMode(ApplicationModel model, int selectMode);
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
    public static final int CURRENT_ACTION_NOTHING             = 0;
    public static final int CURRENT_ACTION_MOVING              = 1;
    public static final int CURRENT_ACTION_SCALING             = 2;
    public static final int CURRENT_ACTION_ROTATING            = 3;
    public static final int CURRENT_ACTION_SKETCHING           = 4;
    public static final int CURRENT_ACTION_LASSO               = 6;
    public static final int CURRENT_ACTION_SELECTING           = 7;
    public static final int CURRENT_ACTION_REVIEWING_SELECTION = 8;

    /** The pointing technique IDs*/
    public static final int POINTING_GOGO        = 0;
    public static final int POINTING_WIM         = 1;
    public static final int POINTING_WIM_POINTER = 2;
    public static final int POINTING_MANUAL      = 3;

    /** The type of tangible selection mode*/
    /** Absolute position*/
    public static final int SELECTION_MODE_ABSOLUTE = 0;

    /** Relative position but absolute rotation*/
    public static final int SELECTION_MODE_RELATIVE_ALIGNED = 1;

    /** Relative position and rotation*/
    public static final int SELECTION_MODE_RELATIVE_FULL    = 2;

    /** Boolean operation IDs*/
    public static final int BOOLEAN_NONE         = -1;
    public static final int BOOLEAN_UNION        = 0;
    public static final int BOOLEAN_MINUS        = 1;
    public static final int BOOLEAN_INTERSECTION = 2;

    /** The different tangible mode*/
    public static final int TANGIBLE_MODE_NONE   = 0;
    public static final int TANGIBLE_MODE_MOVE   = 1;
    public static final int TANGIBLE_MODE_ORIGIN = 2;

    /** The handedness values*/
    public static final int HANDEDNESS_LEFT = 0;
    public static final int HANDEDNESS_RIGHT  = 1;

    private ArrayList<IDataCallback> m_listeners = new ArrayList<>(); /**!< The known listeners to call when the model changed*/
    private Configuration            m_config;                        /**!< The configuration object*/

    /*********************************************************************/
    /************************ DATASETS ATTRIBUTES ************************/
    /*********************************************************************/

    private ArrayList<VTKDataset>         m_vtkDatasets         = new ArrayList<>(); /**!< The vtk dataset */
    private ArrayList<VectorFieldDataset> m_vectorFieldDatasets = new ArrayList<>(); /**!< The open vectorField Datasets */
    private ArrayList<CloudPointDataset>  m_cloudPointDatasets  = new ArrayList<>(); /**!< The open cloud point Datasets*/
    private ArrayList<Dataset>            m_datasets            = new ArrayList<>(); /**!< The open Dataset (vtk + vectorField)*/

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

    /** The current pointing technique to use*/
    private int m_curPointingTechnique = POINTING_MANUAL;

    /*********************************************************************/
    /***************** TANGIBLE INTERACTION ATTRIBUTES *******************/
    /*********************************************************************/

    /** Current tablet's virtual position*/
    private float[] m_position = new float[]{0.0f, 0.0f, 0.0f};

    /** Current tablet's virtual rotation*/
    private float[] m_rotation = new float[]{1.0f, 0.0f, 0.0f, 0.0f};

    /** Current lasso*/
    private float[] m_lasso;

    /** The physical physical origin for relative movements*/
    private float[] m_originPosition = new float[3];

    /** The physical orientation origin for relative movements*/
    private Quaternion m_originRotation = new Quaternion();

    /** The start position of the tablet during its movements*/
    private float[] m_startPosition = new float[3];

    /** The start orientation of the tablet during its movements*/
    private Quaternion m_startRotation = new Quaternion();

    /** Was the tangible movement restarted?*/
    private boolean m_reinitTangible = false;

    /** Is the volumetric selection constrained toward the tablet's normal axis?*/
    private boolean m_volumeSelectionConstrained = false;

    /** THe current boolean operation in use*/
    private int m_currentBooleanOperation = BOOLEAN_UNION;

    /** The current selection mode technique in use*/
    private int m_curSelectionMode = SELECTION_MODE_ABSOLUTE;

    /** The application current tangible mode?*/
    private int m_tangibleMode = TANGIBLE_MODE_NONE;

    /** @brief Basic constructor, initialize the data at its default state */
    public ApplicationModel(Context ctx)
    {
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

    /** @brief Function that tells whether we can modify or not a given SubDataset
     * @param sd the subdataset to check the modify property*/
    public boolean canModifySubDataset(SubDataset sd)
    {
        if(sd == null || getBindingInfo() == null) //no subdataset or no connection...
            return false;

        if(getBindingInfo().getHeadsetID() == -1) //Not bound to a headset: modify nothing
            return false;

        if(sd.getOwnerID() != -1 && sd.getOwnerID() == getBindingInfo().getHeadsetID()) //Our private subdataset
            return true;

        if(sd.getOwnerID() == -1) //Public -> check the lock status
        {
            if(sd.getCurrentHeadset() == -1 || sd.getCurrentHeadset() == getBindingInfo().getHeadsetID())
                return true;
        }

        return false;
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
            public void onUpdateTF(SubDataset dataset)
            {
                //TODO update associated GTFData. But normally MainActivity is already handling it...
            }

            @Override
            public void onSetCurrentHeadset(SubDataset dataset, int headsetID)
            {
                dataset.setCanBeModified(canModifySubDataset(dataset));
            }

            @Override
            public void onSetOwner(SubDataset dataset, int headsetID)
            {
                dataset.setCanBeModified(canModifySubDataset(dataset));
            }

            @Override
            public void onSetCanBeModified(SubDataset dataset, boolean status){}

            @Override
            public void onSetMapVisibility(SubDataset dataset, boolean visibility)
            {}

            @Override
            public void onSetVolumetricMask(SubDataset dataset)
            {}
        });

        sd.setCanBeModified(canModifySubDataset(sd));
    }

    /** Perform common actions when adding datasets
     * @param d the dataset in adding state.*/
    private void onAddDataset(Dataset d)
    {
        m_datasets.add(d);
        d.addListener(this);
    }

    /** @brief Add a VectorFieldDataset to our model
     *  @param dataset the dataset to add*/
    public void addVectorFieldDataset(VectorFieldDataset dataset)
    {
        m_vectorFieldDatasets.add(dataset);
        onAddDataset(dataset);
        for(IDataCallback clbk : m_listeners)
            clbk.onAddVectorFieldDataset(this, dataset);

        for(SubDataset sd : dataset.getSubDatasets())
            onAddSubDataset(sd);
    }

    public void addCloudPointDataset(CloudPointDataset dataset)
    {
        m_cloudPointDatasets.add(dataset);
        onAddDataset(dataset);

        for(IDataCallback clbk : m_listeners)
            clbk.onAddCloudPointDataset(this, dataset);

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

    /** @brief Get a list of VectorField Datasets
     * @return the list of VectorField Datasets opened*/
    public ArrayList<VectorFieldDataset> getVectorFieldDatasets() {return m_vectorFieldDatasets;}

    /** @brief Get a list of CloudPoint Datasets opened
     * @return the list of CloudPoints Datasets opened*/
    public ArrayList<CloudPointDataset> getCloudPointDataset() {return m_cloudPointDatasets;}

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
            for(SubDataset newSD : d.getSubDatasets())
            {
                if(newSD != sd)
                {
                    setCurrentSubDataset(newSD);
                    break;
                }
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

        else if(m_vectorFieldDatasets.contains(dataset))
            m_vectorFieldDatasets.remove(dataset);

        else if(m_cloudPointDatasets.contains(dataset))
            m_cloudPointDatasets.remove(dataset);
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
        if(action != m_currentAction)
        {
            m_currentAction = action;
            for(IDataCallback clbk : m_listeners)
                clbk.onChangeCurrentAction(this, action);
        }
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

        //Update the status of subdatasets
        for(Dataset d : m_datasets)
            for(SubDataset sd : d.getSubDatasets())
                sd.setCanBeModified(canModifySubDataset(sd));

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

    /** Change whether or not should the volumetric brushing should be constrained with respect to the device's normal axis
     * @param b true if constrained mode should be activated, false otherwise*/
    public void changeConstrainVolumeSelectionMode(boolean b)
    {
        m_volumeSelectionConstrained = b;
    }

    /** Set the current selection mode to use
     * @param selectMode the new selection mode to use (see SELECTION_MODE* )*/
    public void setCurrentSelectionMode(int selectMode)
    {
        for(IDataCallback clbk : m_listeners)
            clbk.onSetSelectionMode(this, selectMode);
        m_curSelectionMode = selectMode;
    }

    /** Get the current selection mode in use
     * @return the current selection mode ID. See SELECTION_MODE_* */
    public int getCurrentSelectionMode()
    {
        return m_curSelectionMode;
    }

    /** @brief update the tablet's location if the location is significant for the current mode of the tablet
     * @param pos the tablet's position
     * @param rot the tablet's rotation*/
    public void setLocation(float[] pos, float[] rot)
    {
        if(m_currentAction == CURRENT_ACTION_SELECTING || m_currentAction == CURRENT_ACTION_LASSO)
        {
            boolean isReinited = false;
            if(m_reinitTangible)
            {
                isReinited = true;
                m_reinitTangible = false;
                m_startPosition  = pos.clone();
                m_startRotation  = new Quaternion(rot[1], rot[2], rot[3], rot[0]);

                m_originPosition = m_position.clone();
                m_originRotation = new Quaternion(m_rotation[1], m_rotation[2], m_rotation[3], m_rotation[0]);
            }

            float[]    p = pos.clone();
            Quaternion r = new Quaternion(rot[1], rot[2], rot[3], rot[0]);

            //Set the origin
            if(m_tangibleMode == TANGIBLE_MODE_ORIGIN)
            {
                m_originPosition = p.clone();
                m_originRotation = (Quaternion)r.clone();

                m_position = p;
                m_rotation = r.toFloatArray();
            }

            else if(m_tangibleMode == TANGIBLE_MODE_MOVE)
            {
                //Apply our choice in the selection mode
                if (m_curSelectionMode == SELECTION_MODE_ABSOLUTE) {} //Nothing to do here

                else if (m_curSelectionMode == SELECTION_MODE_RELATIVE_ALIGNED)
                    for (int i = 0; i < 3; i++)
                        p[i] = p[i] - m_startPosition[i] + m_originPosition[i];

                else if (m_curSelectionMode == SELECTION_MODE_RELATIVE_FULL)
                {
                    //Find the suitable rotation

                    //First position
                    if (isReinited)
                    {
                        for (int i = 0; i < 3; i++)
                            p[i] = m_originPosition[i];
                    }

                    else
                    {
                        //Rotate the displacement
                        float[] movementRotate = new float[3];
                        for (int i = 0; i < 3; i++)
                            movementRotate[i] = (p[i] - m_startPosition[i]);

                        p = m_originRotation.multiplyBy(m_startRotation.getInverse()).rotateVector(movementRotate);
                        for (int i = 0; i < 3; i++)
                            p[i] = p[i] + m_originPosition[i];
                    }

                    r = m_originRotation.multiplyBy(r).multiplyBy(m_startRotation.getInverse());
                }

                //Handle the constraint mode
                if(m_volumeSelectionConstrained && m_tangibleMode != TANGIBLE_MODE_ORIGIN)
                {
                    float[] t = new float[]{0.0f, -1.0f, 0.0f};
                    t = m_originRotation.rotateVector(t);
                    float mag = 0.0f;
                    for (int i = 0; i < 3; i++) //Dot product
                        mag += t[i] * (p[i] - m_position[i]);

                    for (int i = 0; i < 3; i++)
                        t[i] = t[i] * mag + m_position [i];

                    m_rotation = m_originRotation.toFloatArray();
                    m_position = t;
                }

                else
                {
                    m_position = p;
                    m_rotation = r.toFloatArray();
                }
            }

            for(IDataCallback clbk : m_listeners)
                clbk.onSetLocation(this, m_position, m_rotation);

        }
    }

    /** @brief set the tablet's scale
     * @param scale the tablet's scale
     * @param width the tablet view's width
     * @param height the tablet view's height
     * @param posx the tablet view's horizontal position
     * @param posy the tablet view's vertical position*/
    public void setTabletScale(float scale, float width, float height, float posx, float posy)
    {
        for(IDataCallback clbk : m_listeners)
            clbk.onSetTabletScale(this, scale, width, height, posx, posy);
    }

    /** @brief called when the lasso is traced
     * @param data the lasso data*/
    public void setLasso(final float[] data)
    {
        for(IDataCallback clbk : m_listeners)
            clbk.onSetLasso(this, data);
        m_lasso = data;
    }

    /** @brief confirm the current selection*/
    public void confirmSelection()
    {
        for(IDataCallback clbk : m_listeners)
            clbk.onConfirmSelection(this);
    }

    /** Set whether or not the tablet is currently in a tangible mode
     * @param mode true if true, false otherwise*/
    public void setTangibleMode(int mode)
    {
        if(m_tangibleMode == TANGIBLE_MODE_NONE &&
           mode != TANGIBLE_MODE_NONE)
        {
            m_reinitTangible = true;
        }

        for(IDataCallback clbk : m_listeners)
            clbk.onSetTangibleMode(this, mode);
        m_tangibleMode = mode;
    }

    /** Get the tablet current tangible mode
     * @return the tablet current tangible mode*/
    public int getCurrentTangibleMode()
    {
        return m_tangibleMode;
    }

    /** Set the current boolean operation the tablet is performing in selection mode
     * @param op the current boolean operation in use. See BOOLEAN_UNION, BOOLEAN_MINUS, and BOOLEAN_INTERSECTION*/
    public void setCurrentBooleanOperation(int op)
    {
        for(IDataCallback clbk : m_listeners)
            clbk.onSetCurrentBooleanOperation(this, op);
        m_currentBooleanOperation = op;
    }

    /** Get the current boolean operation the tablet is performing in selection mode
     * @return the current boolean operation. See BOOLEAN_UNION, BOOLEAN_MINUS, and BOOLEAN_INTERSECTION*/
     public int getCurrentBooleanOperation()
    {
        return m_currentBooleanOperation;
    }

    @Override
    public void onRemoveSubDataset(Dataset dataset, SubDataset sd){}

    @Override
    public void onAddSubDataset(Dataset dataset, SubDataset sd)
    {
        onAddSubDataset(sd);
    }

    @Override
    public void onLoadDataset(Dataset dataset, boolean success){}

    @Override
    public void onLoadCPCPTexture(Dataset dataset, CPCPTexture texture) {}

    @Override
    public void onLoad1DHistogram(Dataset dataset, float[] values, int pID) {}

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
