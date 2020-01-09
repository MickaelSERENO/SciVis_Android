package com.sereno.gl;

import android.content.Context;
import android.graphics.Bitmap;
import android.util.AttributeSet;

import com.sereno.vfv.Data.ApplicationModel;
import com.sereno.vfv.Data.BinaryDataset;
import com.sereno.vfv.Data.CPCPTexture;
import com.sereno.vfv.Data.Dataset;
import com.sereno.vfv.Data.SubDataset;
import com.sereno.vfv.Data.VTKDataset;
import com.sereno.vfv.Network.HeadsetBindingInfoMessage;
import com.sereno.vfv.Network.HeadsetsStatusMessage;
import com.sereno.view.AnnotationData;

import java.util.ArrayList;
import java.util.List;

/** This class represents the C++ SurfaceView in use.
 * This object launches also a C++ thread which computes what ever is needed for this application (e.g., loading Datasets)*/
public class VFVSurfaceView extends GLSurfaceView implements ApplicationModel.IDataCallback, SubDataset.ISubDatasetListener, Dataset.IDatasetListener
{
    /** VFVSurfaceView Listener interface.*/
    public interface IVFVSurfaceViewListener
    {
        /** Method called when the VFVSurfaceView wants to change the current action
         * This method can be called asynchronously
         * @param action the current action asked*/
        void onChangeCurrentAction(int action);
    }

    public static final int DATASET_TYPE_VTK    = 0;
    public static final int DATASET_TYPE_BINARY = 1;

    /** The native C++ pointer*/
    private long m_ptr;

    /** List of listeners bound to this VFVSurfaceView*/
    private List<IVFVSurfaceViewListener> m_listeners = new ArrayList<>();

    public VFVSurfaceView(Context context)
    {
        super(context);
        m_ptr = nativeCreateMainArgs();
    }

    public VFVSurfaceView(Context context, AttributeSet attrs)
    {
        super(context, attrs);
        m_ptr = nativeCreateMainArgs();
    }

    public VFVSurfaceView(Context context, AttributeSet attrs, int defStyle)
    {
        super(context, attrs, defStyle);
        m_ptr = nativeCreateMainArgs();
    }

    @Override
    protected void finalize() throws Throwable
    {
        super.finalize();
        nativeDeleteMainArgs(m_ptr);
    }

    /** \brief Function which has for aim to be overrided.
     * \return the cpp argument to send to the main function*/
    @Override
    protected long getMainArgument()
    {
        return m_ptr;
    }

    /** @brief Add a new callback Listener
     * @param listener the listener to call for new events*/
    public void addListener(IVFVSurfaceViewListener listener)
    {
        m_listeners.add(listener);
    }

    /** @brief Remove an old callback Listener
     * @param listener the listener to remove from the list*/
    public void removeListener(IVFVSurfaceViewListener listener)
    {
        m_listeners.remove(listener);
    }

    @Override
    public void onAddBinaryDataset(ApplicationModel model, BinaryDataset fd)
    {
        onAddDataset(model, fd);
        nativeAddBinaryDataset(fd, m_ptr, fd.getPtr());
    }

    @Override
    public void onAddVTKDataset(ApplicationModel model, VTKDataset d)
    {
        //C++ callback
        onAddDataset(model, d);
        nativeAddVTKDataset(d, m_ptr, d.getPtr());
    }

    @Override
    public void onRemoveSubDataset(Dataset dataset, SubDataset sd)
    {}

    @Override
    public void onAddSubDataset(Dataset dataset, SubDataset sd)
    {
        sd.addListener(this);
        nativeBindSubDataset(m_ptr, sd.getNativePtr(), sd); //Bind a not-yet known subdataset
        nativeOnAddSubDataset(m_ptr, sd.getNativePtr());
    }

    @Override
    public void onLoadDataset(Dataset dataset, boolean success) {}

    @Override
    public void onLoadCPCPTexture(Dataset dataset, CPCPTexture texture) {}

    @Override
    public void onLoad1DHistogram(Dataset dataset, float[] values, int pID)
    {

    }

    @Override
    public void onAddAnnotation(ApplicationModel model, AnnotationData annot, ApplicationModel.AnnotationMetaData metaData) {}

    @Override
    public void onPendingAnnotation(ApplicationModel model, SubDataset sd) {

    }

    @Override
    public void onEndPendingAnnotation(ApplicationModel model, SubDataset sd, boolean cancel) {

    }

    @Override
    public void onChangeCurrentAction(ApplicationModel model, int action)
    {

    }

    @Override
    public void onChangeCurrentSubDataset(ApplicationModel model, SubDataset sd)
    {
        if(sd != null)
            nativeChangeCurrentSubDataset(m_ptr, sd.getNativePtr());
        else
            nativeChangeCurrentSubDataset(m_ptr, 0);
    }

    @Override
    public void onUpdateHeadsetsStatus(ApplicationModel model, HeadsetsStatusMessage.HeadsetStatus[] headsetsStatus)
    {
        nativeUpdateHeadsetsStatus(m_ptr, headsetsStatus);
    }

    @Override
    public void onUpdateBindingInformation(ApplicationModel model, HeadsetBindingInfoMessage info)
    {
        nativeUpdateBindingInformation(m_ptr, info);
    }

    @Override
    public void onRemoveDataset(ApplicationModel model, Dataset dataset)
    {
        if(model.getBinaryDatasets().contains(dataset))
            nativeRemoveDataset(m_ptr, dataset.getPtr(), DATASET_TYPE_BINARY);
        else if(model.getVTKDatasets().contains(dataset))
            nativeRemoveDataset(m_ptr, dataset.getPtr(), DATASET_TYPE_VTK);
    }

    @Override
    public void onUpdatePointingTechnique(ApplicationModel model, int pt) {}

    private void onAddDataset(ApplicationModel model, Dataset d)
    {
        d.addListener(this);
    }

    @Override
    public void onRotationEvent(SubDataset dataset, float[] quaternion)
    {
        nativeOnRotationChange(m_ptr, dataset.getNativePtr());
    }

    @Override
    public void onPositionEvent(SubDataset dataset, float[] position)
    {
        nativeOnPositionChange(m_ptr, dataset.getNativePtr());
    }

    @Override
    public void onScaleEvent(SubDataset dataset, float[] scale)
    {
        nativeOnScaleChange(m_ptr, dataset.getNativePtr());
    }

    @Override
    public void onSnapshotEvent(SubDataset dataset, Bitmap snapshot)
    {
        //We generated this event
    }

    @Override
    public void onAddAnnotation(SubDataset dataset, AnnotationData annotation) {}

    @Override
    public void onRemove(SubDataset dataset)
    {
        dataset.removeListener(this);
        nativeRemoveSubDataset(m_ptr, dataset.getNativePtr());
    }

    @Override
    public void onRemoveAnnotation(SubDataset dataset, AnnotationData annotation) {}

    @Override
    public void onUpdateTF(SubDataset dataset)
    {
        nativeOnTFUpdated(m_ptr, dataset.getNativePtr());
    }

    @Override
    public void onSetCurrentHeadset(SubDataset dataset, int headsetID)
    {

    }

    @Override
    public void onSetCanBeModified(SubDataset dataset, boolean status)
    {
        //TODO notify application about this status
    }

    /** Function called from the native code when the native code needs to change the current action
     * Pay attention that this is done asynchronously
     * @param a the new action to use*/
    private void setCurrentAction(int a)
    {
        for(IVFVSurfaceViewListener l : m_listeners)
            l.onChangeCurrentAction(a);
    }

    /** Create the argument to send to the main function
     * \return the main argument as a ptr (long value)*/
    private native long nativeCreateMainArgs();

    /** Delete the arguments sent to the main function
     * @param ptr the ptr to delete*/
    private native long nativeDeleteMainArgs(long ptr);

    /** Change the current sub dataset to display
     * @param ptr the ptr associated with the main Argument
     * @param sdPtr the SubDataset native ptr*/
    private native void nativeChangeCurrentSubDataset(long ptr, long sdPtr);

    /** Add the dataset into the cpp application
     * @param bd the binary dataset bound to the fd pointer
     * @param ptr the ptr associated with the main Argument
     * @param fd the BinaryDataset to add*/
    private native void nativeAddBinaryDataset(BinaryDataset bd, long ptr, long fd);

    /** Add a VTKParser into the cpp application
     * @param vtk the VTKDataset bound to the vtkDataPtr
     * @param ptr the ptr associated with the main Argument
     * @param vtkDataPtr the vtk dataset C++ pointer object*/
    private native void nativeAddVTKDataset(VTKDataset vtk, long ptr, long vtkDataPtr);

    /** Remove the dataset into the cpp application
     * @param ptr the ptr associated with the main argument
     * @param sdPtr the subdataset native pointer*/
    private native void nativeRemoveSubDataset(long ptr, long sdPtr);

    /** Remove the dataset into the cpp application
     * @param ptr the ptr associated with the main argument
     * @param datasetPtr the dataset native pointer
     * @param datasetType  the type of the dataset (VTK, binary, etc.)*/
    private native void nativeRemoveDataset(long ptr, long datasetPtr, int datasetType);

    /** Send an event regarding an update from a SubDataset rotation
     * @param ptr the ptr associated with the main Argument
     * @param sdPtr the SubDataset native pointer*/
    private native void nativeOnRotationChange(long ptr, long sdPtr);

    /** Send an event regarding an update from a SubDataset position
     * @param ptr the ptr associated with the main Argument
     * @param sdPtr the SubDataset native pointer*/
    private native void nativeOnPositionChange(long ptr, long sdPtr);

    /** Send an event regarding an update from a SubDataset scaling
     * @param ptr the ptr associated with the main Argument
     * @param sdPtr the SubDataset native pointer*/
    private native void nativeOnScaleChange(long ptr, long sdPtr);

    /** Update the headsets status in the native code to display correctly the status of each persons
     * @param ptr the ptr associated with the main Argument
     * @param status array of the current Headsets Status*/
    private native void nativeUpdateHeadsetsStatus(long ptr, HeadsetsStatusMessage.HeadsetStatus[] status);

    /** Update the binding information
     * @param ptr the ptr associated with the main Argument
     * @param info the new binding information*/
    private native void nativeUpdateBindingInformation(long ptr, HeadsetBindingInfoMessage info);

    /** Tell the C++ visualization that a new SubDataset was added
     * @param ptr the ptr associated with the main Argument
     * @param sdPtr the C++ SubDataset Ptr*/
    private native void nativeOnAddSubDataset(long ptr, long sdPtr);

    /** Bind a native C++ SubDataset to its Java counter part
     * @param ptr the ptr associated with the main Argument
     * @param sdPtr the C++ SubDataset Ptr
     * @param sdJava Its Java counter part*/
    private native void nativeBindSubDataset(long ptr, long sdPtr, SubDataset sdJava);

    /** Tells the native C++ code that the transfer function of a SubDataset has been updated in the UI
     * @param ptr the ptr associated with the main Argument
     * @param sdPtr the C++ SubDataset Ptr*/
    private native void nativeOnTFUpdated(long ptr, long sdPtr);
}
