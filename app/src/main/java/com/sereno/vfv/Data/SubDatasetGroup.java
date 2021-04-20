package com.sereno.vfv.Data;

import android.graphics.Bitmap;

import com.sereno.vfv.Data.Annotation.DrawableAnnotationPosition;
import com.sereno.view.AnnotationCanvasData;

import java.util.ArrayList;

/** Base class for subdataset groups*/
public abstract class SubDatasetGroup implements SubDataset.ISubDatasetListener
{
    /** Interface to handle events from SubDatasetGroup objects*/
    public interface ISubDatasetGroupListener
    {
        /** Event called when a subdataset has been added to this group
         * @param sdg the group calling this method
         * @param sd the subdataset being added*/
        void onAddSubDataset(SubDatasetGroup sdg, SubDataset sd);

        /** Event called when a subdataset has been removed to this group
         * @param sdg the group calling this method
         * @param sd the subdataset being removed*/
        void onRemoveSubDataset(SubDatasetGroup sdg, SubDataset sd);

        /** Event called when subdatasets belonging to a group have been updated
         * @param sdg the group calling this method*/
        void onUpdateSubDatasets(SubDatasetGroup sdg);
    };

    /** Is the subdataset group "group" a subjective view group? (SubDatasetSubjectiveStackedGroup)
     * @param group the group to test
     * @return true if group is a group of subjective views, false otherwise*/
    public static boolean isSubjective(SubDatasetGroup group)
    {
        return group.m_type == SD_GROUP_SV_LINKED  ||
               group.m_type == SD_GROUP_SV_STACKED ||
               group.m_type == SD_GROUP_SV_STACKED_LINKED;
    }

    public static final int SD_GROUP_SV_STACKED        = 0;
    public static final int SD_GROUP_SV_LINKED         = 1;
    public static final int SD_GROUP_SV_STACKED_LINKED = 2;
    public static final int SD_GROUP_NONE              = 3;

    /** The subdatasets belonging to this group*/
    protected ArrayList<SubDataset>               m_subdatasets = new ArrayList<>();

    /** All the registered listeners*/
    private ArrayList<ISubDatasetGroupListener> m_listeners     = new ArrayList<>();

    /** The native C++ pointer pointing to a SubDatasetGroup (or inherited class)*/
    protected long m_ptr;

    /** The type of the subdataset group. See SD_GROUP_* */
    protected int  m_type;

    protected int m_id;

    /** Constructor
     * @param ptr the native C++ pointer
     * @param type the type of the subdataset group; see SD_GROUP_*
     * @param id the ID of this group as defined by the server*/
    protected SubDatasetGroup(long ptr, int type, int id)
    {
        m_ptr  = ptr;
        m_type = type;
        m_id   = id;
    }

    /** Register a new listener to call on events
     * @param l the listener to consider from now*/
    public void addListener(ISubDatasetGroupListener l)
    {
        if(m_listeners.contains(l))
            return;
        m_listeners.add(l);
    }

    /** Unregister a new listener to call on events
     * @param l the listener to not consider anymore*/
    public void removeListener(ISubDatasetGroupListener l)
    {
        if(!m_listeners.contains(l))
            return;
        m_listeners.remove(l);
    }

    /** Remove a subdataset from this group. Multiple subdatasets can be removed if they are connected with each other (e.g., linked and stacked subjective views)
     * @param sd the subdataset to not consider anymore
     * @return true on success, false otherwise.
     * This function returns false if the native code fails to remove the subdataset.*/
    public boolean removeSubDataset(SubDataset sd)
    {
        if(!m_subdatasets.contains(sd))
            return false;

        if(!nativeRemoveSubDatasets(m_ptr, sd.getNativePtr()))
            return false;

        //The purpose here is to resynch m_subdatasets array by removing what should not be there anymore
        //Indeed, removing ONE subdataset can lead to removing MULTIPLE subdatasets (e.g., with subjective views, we have to remove the stacked and linked subdataset)
        long[] nativeSDPtrs = nativeGetSubDatasets(m_ptr);
        for(int i = 0; i < m_subdatasets.size();)
        {
            boolean found = false;
            for(int j = 0; j < nativeSDPtrs.length; j++)
            {
                if(m_subdatasets.get(i).getNativePtr() == nativeSDPtrs[j])
                {
                    found = true;
                    break;
                }
            }

            if(!found)
            {
                SubDataset sdToRemove = m_subdatasets.get(i);
                m_subdatasets.remove(i);
                sdToRemove.setSubDatasetGroup(null);

                for(ISubDatasetGroupListener l : m_listeners)
                    l.onRemoveSubDataset(this, sd);
            }
            else
                i++;
        }
        return true;
    }

    /** Add a new subdataset on this group. protected by default because there might be more constraints from inherited classes.
     * @param sd  the subdataset to add
     * @return true on success, false otherwise.*/
    protected boolean addSubDataset(SubDataset sd)
    {
        if(m_subdatasets.contains(sd))
            return false;

        //Add into array ONLY if we are able to find the subdataset in the C++ source code
        long[] nativeSDPtrs = nativeGetSubDatasets(m_ptr);
        boolean found = false;
        for(int i = 0; i < nativeSDPtrs.length; i++)
            if(nativeSDPtrs[i] == sd.getNativePtr())
            {
                found = true;
                break;
            }
        if(!found)
            return false;

        m_subdatasets.add(sd);
        sd.setSubDatasetGroup(this);

        for(ISubDatasetGroupListener l : m_listeners)
            l.onAddSubDataset(this, sd);
        return true;
    }

    public ArrayList<SubDataset> getSubDatasets() {return m_subdatasets;}

    public void updateSubDatasets()
    {
        nativeUpdateSubDatasets(m_ptr);
        for(ISubDatasetGroupListener l : m_listeners)
            l.onUpdateSubDatasets(this);
    }

    public int getID()
    {
        return m_id;
    }

    protected SubDataset getSubDatasetFromNativePointer(long sdID)
    {
        for(SubDataset sd : m_subdatasets)
            if(sdID == sd.getNativePtr())
                return sd;
        return null;
    }

    @Override
    protected void finalize()
    {
        nativeDeletePtr(m_ptr);
    }

    @Override
    public void onRotationEvent(SubDataset dataset, float[] quaternion)
    {
        updateSubDatasets();
    }

    @Override
    public void onPositionEvent(SubDataset dataset, float[] position)
    {
        updateSubDatasets();
    }

    @Override
    public void onScaleEvent(SubDataset dataset, float[] scale)
    {
        updateSubDatasets();
    }

    @Override
    public void onSnapshotEvent(SubDataset dataset, Bitmap snapshot) {}

    @Override
    public void onAddCanvasAnnotation(SubDataset dataset, AnnotationCanvasData annotation) {}

    @Override
    public void onRemove(SubDataset dataset){}

    @Override
    public void onRemoveCanvasAnnotation(SubDataset dataset, AnnotationCanvasData annotation) {}

    @Override
    public void onUpdateTF(SubDataset dataset)
    {
        updateSubDatasets();
    }

    @Override
    public void onSetCurrentHeadset(SubDataset dataset, int headsetID)
    {}

    @Override
    public void onSetOwner(SubDataset dataset, int headsetID)
    {}

    @Override
    public void onSetCanBeModified(SubDataset dataset, boolean status)
    {}

    @Override
    public void onSetMapVisibility(SubDataset dataset, boolean visibility)
    {
        updateSubDatasets();
    }

    @Override
    public void onSetVolumetricMask(SubDataset dataset)
    {
        updateSubDatasets();
    }

    @Override
    public void onAddDrawableAnnotationPosition(SubDataset dataset, DrawableAnnotationPosition pos)
    {}

    @Override
    public void onSetDepthClipping(SubDataset dataset, float depthClipping)
    {
        updateSubDatasets();
    }

    @Override
    public void onSetSubDatasetGroup(SubDataset sd, SubDatasetGroup group) {}

    private static native void    nativeDeletePtr(long ptr);
    private static native long[]  nativeGetSubDatasets(long sdgPtr);
    private static native boolean nativeRemoveSubDatasets(long sdgPtr, long sdPtr);
    private static native void    nativeUpdateSubDatasets(long sdgPtr);
}