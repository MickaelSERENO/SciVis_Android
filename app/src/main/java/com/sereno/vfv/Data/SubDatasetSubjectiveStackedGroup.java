package com.sereno.vfv.Data;

import android.util.Pair;

import java.util.ArrayList;

/** Class representing subjective group where subdatasets can both be stacked and linked */
public class SubDatasetSubjectiveStackedGroup extends SubDatasetGroup
{
    public static final int STACK_VERTICAL   = 0;
    public static final int STACK_HORIZONTAL = 1;
    public static final int STACK_END        = 2;

    private boolean m_focusOnBase = false;

    /** Constructor
     * @param base the original subdataset acting as a base
     * @param type the type of the group. See SubDatasetGroup.SD_GROUP_*
     * @param id the ID of this group as defined by the server*/
    public SubDatasetSubjectiveStackedGroup(SubDataset base, int type, int id)
    {
        super(nativeCreatePtr(base.getNativePtr()), type, id);
        addSubDataset(base);
    }

    /** Set the gap distance separating stacked subdatasets
     * @param gap the new gap distance (world-space distance)*/
    public void setGap(float gap)
    {
        nativeSetGap(m_ptr, gap);
    }

    /** Get the gap distance separating stacked subdatasets
     * @return the gap distance (world-space distance)*/
    public float getGap()
    {
        return nativeGetGap(m_ptr);
    }

    /** Should stacked subdataset be merged?
     * @param merge true if yes, false otherwise*/
    public void setMerge(boolean merge)
    {
        nativeSetMerge(m_ptr, merge);
    }

    /** Should stacked subdataset be merged?
     * @return true if yes, false otherwise*/
    public boolean getMerge()
    {
        return nativeGetMerge(m_ptr);
    }

    /** Set the stacking method of stacked subdatasets
     * @param stack the stack method; see STACK_* */
    public void setStackingMethod(int stack)
    {
        nativeSetStackingMethod(m_ptr, stack);
    }

    /** Get the stacking method of stacked subdatasets
     * @return the stack method; see STACK_* */
    public int getStackingMethod()
    {
        return nativeGetStackingMethod(m_ptr);
    }

    /** Add a new subjective view in this group
     * @param sdStacked the subdataset to stack above the original. null to not stack anything
     * @param sdLinked the subdataset to link with the stacked view. null to not link anything*/
    public boolean addSubjectiveSubDataset(SubDataset sdStacked, SubDataset sdLinked)
    {
        long stackedPtr = 0;
        long linkedPtr  = 0;

        if(sdStacked != null)
            stackedPtr = sdStacked.getNativePtr();
        if(sdLinked != null)
            linkedPtr  = sdLinked.getNativePtr();

        if(nativeAddSubjectiveSubDataset(m_ptr, stackedPtr, linkedPtr))
        {
            if(sdStacked != null)
                addSubDataset(sdStacked);
            if(sdLinked != null)
                addSubDataset(sdLinked);
            return true;
        }
        return false;
    }

    public ArrayList<Pair<SubDataset, SubDataset>> getSubjectiveSubDatasets()
    {
        long[] sdIDs = nativeGetSubjectiveSubDatasets(m_ptr);
        ArrayList<Pair<SubDataset, SubDataset>> ret = new ArrayList<>();

        for(int i = 0; i < sdIDs.length; i += 2)
        {
            SubDataset sdStacked = (sdIDs[i]   == 0 ? null : getSubDatasetFromNativePointer(sdIDs[i]));
            SubDataset sdLinked  = (sdIDs[i+1] == 0 ? null : getSubDatasetFromNativePointer(sdIDs[i+1]));

            ret.add(new Pair<SubDataset, SubDataset>(sdStacked, sdLinked));
        }

        return ret;
    }

    public Pair<SubDataset, SubDataset> getSubjectiveSubDataset(int ownerID)
    {
        long[] sdIDs = nativeGetSubjectiveSubDatasets(m_ptr);

        for(int i = 0; i < sdIDs.length; i += 2)
        {
            SubDataset sdStacked = (sdIDs[i]   == 0 ? null : getSubDatasetFromNativePointer(sdIDs[i]));
            SubDataset sdLinked  = (sdIDs[i+1] == 0 ? null : getSubDatasetFromNativePointer(sdIDs[i+1]));

            if(sdStacked != null && sdStacked.getOwnerID() == ownerID ||
               sdLinked  != null && sdLinked.getOwnerID()  == ownerID)
                return new Pair<SubDataset, SubDataset>(sdStacked, sdLinked);
        }
        return null;
    }

    private static native long    nativeCreatePtr(long basePtr);
    private static native void    nativeSetGap(long ptr, float gap);
    private static native long    nativeGetGap(long ptr);
    private static native void    nativeSetMerge(long ptr, boolean merge);
    private static native boolean nativeGetMerge(long ptr);
    private static native void    nativeSetStackingMethod(long ptr, int stack);
    private static native int     nativeGetStackingMethod(long ptr);
    private static native boolean nativeAddSubjectiveSubDataset(long sdgPtr, long sdStackedPtr, long sdLinkedPtr);
    private static native long[]  nativeGetSubjectiveSubDatasets(long sdgPtr);
}