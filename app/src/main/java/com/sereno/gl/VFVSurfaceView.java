package com.sereno.gl;

import android.content.Context;
import android.graphics.Bitmap;
import android.util.AttributeSet;

import com.sereno.vfv.Data.ApplicationModel;
import com.sereno.vfv.Data.BinaryDataset;
import com.sereno.vfv.Data.Dataset;
import com.sereno.vfv.Data.SubDataset;
import com.sereno.vfv.Data.SubDatasetMetaData;
import com.sereno.vfv.Data.VTKDataset;
import com.sereno.vfv.Network.HeadsetBindingInfoMessage;
import com.sereno.vfv.Network.HeadsetsStatusMessage;
import com.sereno.view.AnnotationData;

import java.util.ArrayList;
import java.util.List;

public class VFVSurfaceView extends GLSurfaceView implements ApplicationModel.IDataCallback, SubDataset.ISubDatasetListener, SubDatasetMetaData.ISubDatasetMetaDataListener
{
    /** VFVSurfaceView Listener interface.*/
    public interface IVFVSurfaceViewListener
    {
        /** Method called when the VFVSurfaceView wants to change the current action
         * This method can be called asynchronously
         * @param action the current action asked*/
        void onChangeCurrentAction(int action);
    }

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
    public void onAddAnnotation(ApplicationModel model, AnnotationData annot, ApplicationModel.AnnotationMetaData metaData) {}

    @Override
    public void onPendingAnnotation(ApplicationModel model, SubDataset sd) {

    }

    @Override
    public void onEndPendingAnnotation(ApplicationModel model, SubDataset sd, boolean cancel) {

    }

    @Override
    public void onChangeCurrentAction(ApplicationModel model, int action) {

    }

    @Override
    public void onChangeCurrentSubDataset(ApplicationModel model, SubDataset sd)
    {
        nativeChangeCurrentSubDataset(m_ptr, sd.getNativePtr());
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

    private void onAddDataset(ApplicationModel model, Dataset d)
    {
        for(SubDataset sd : d.getSubDatasets())
        {
            SubDatasetMetaData metaData = model.getSubDatasetMetaData(sd);
            metaData.addListener(this);
            metaData.getPublicState().addListener(this);
            metaData.getPrivateState().addListener(this);
            nativeInitSubDatasetMetaData(m_ptr,
                                         metaData.getPublicState().getNativePtr(), metaData.getPublicState(),
                                         metaData.getPrivateState().getNativePtr(), metaData.getPrivateState(),
                                         metaData.getVisibility());
        }
    }

    @Override
    public void onClampingChange(SubDataset sd, float min, float max)
    {
        nativeOnClampingChange(m_ptr, min, max, sd.getNativePtr());
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
    public void onSetVisibility(SubDatasetMetaData metaData, int v)
    {
        nativeSetSubDatasetVisibility(m_ptr, metaData.getPublicState().getNativePtr(), v);
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

    /** Remove the dataset index i into the cpp application
     * @param ptr the ptr associated with the main argument
     * @param idx the BinaryDataset index*/
    private native void nativeRemoveData(long ptr, int idx);

    /** Set the clamping range of the current dataset
     * @param ptr the ptr associated with the main Argument
     * @param min the minimum range (0.0, 1.0)
     * @param max the maximum range (0.0, 1.0)
     * @param sdPtr the SubDataset native pointer*/
    private native void nativeOnClampingChange(long ptr, float min, float max, long sdPtr);

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

    /** Init the subdataset meta data information
     * @param ptr the ptr associated with the main Argument
     * @param sdPublicPtr the SubDataset public states native pointer
     * @param sdPublic the Java SubDataset public states object
     * @param sdPrivatePtr the SubDataset private states native pointer
     * @param sdPrivate the Java SubDataset private states object
     * @param visibility the current visibility*/
    private native void nativeInitSubDatasetMetaData(long ptr, long sdPublicPtr, SubDataset sdPublic, long sdPrivatePtr, SubDataset sdPrivate, int visibility);

    /** Set the subdataset visibility.
     * @param ptr the ptr associated with the main Argument
     * @param sdPtr the SubDataset public or private states native pointer
     * @param visibility the current visibility*/
    private native void nativeSetSubDatasetVisibility(long ptr, long sdPtr, int visibility);
}
