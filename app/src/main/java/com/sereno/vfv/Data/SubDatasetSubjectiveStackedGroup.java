package com.sereno.vfv.Data;

import android.util.Pair;

import com.sereno.vfv.Data.TF.TransferFunction;

import java.util.ArrayList;

/** Class representing subjective group where subdatasets can both be stacked and linked */
public class SubDatasetSubjectiveStackedGroup extends SubDatasetGroup
{
    public interface ISubDatasetSubjectiveStackedGroup
    {
        void onSetGap(SubDatasetSubjectiveStackedGroup group, float gap);
        void onSetMerge(SubDatasetSubjectiveStackedGroup group, boolean merge);
        void onSetStackingMethod(SubDatasetSubjectiveStackedGroup group, int method);
        void onAddSubjectiveViews(SubDatasetSubjectiveStackedGroup group, Pair<SubDataset, SubDataset> subjViews);
        void onSetFocus(SubDatasetSubjectiveStackedGroup group, boolean onBase);
    }

    public static final int STACK_VERTICAL   = 0;
    public static final int STACK_HORIZONTAL = 1;
    public static final int STACK_END        = 2;

    /** Is the current application focus on the base or on the subjective view?*/
    private boolean m_focusOnBase = true;

    private ArrayList<ISubDatasetSubjectiveStackedGroup> m_listeners     = new ArrayList<>();

    /** Constructor
     * @param base the original subdataset acting as a base
     * @param type the type of the group. See SubDatasetGroup.SD_GROUP_*
     * @param id the ID of this group as defined by the server*/
    public SubDatasetSubjectiveStackedGroup(SubDataset base, int type, int id)
    {
        super(nativeCreatePtr(base.getNativePtr()), type, id);
        addSubDataset(base);
    }

    /** Register a new listener to call on events
     * @param l the listener to consider from now*/
    public void addListener(ISubDatasetSubjectiveStackedGroup l)
    {
        if(m_listeners.contains(l))
            return;
        m_listeners.add(l);
    }

    /** Unregister a new listener to call on events
     * @param l the listener to not consider anymore*/
    public void removeListener(ISubDatasetSubjectiveStackedGroup l)
    {
        if(!m_listeners.contains(l))
            return;
        m_listeners.remove(l);
    }

    /** Set the gap distance separating stacked subdatasets
     * @param gap the new gap distance (world-space distance)*/
    public void setGap(float gap)
    {
        if(gap != getGap())
        {
            nativeSetGap(m_ptr, gap);
            for(ISubDatasetSubjectiveStackedGroup clbk : m_listeners)
                clbk.onSetGap(this, gap);
        }
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
        if(merge != getMerge())
        {
            nativeSetMerge(m_ptr, merge);
            for(ISubDatasetSubjectiveStackedGroup clbk : m_listeners)
                clbk.onSetMerge(this, merge);
        }
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
        if(stack != getStackingMethod())
        {
            nativeSetStackingMethod(m_ptr, stack);
            for(ISubDatasetSubjectiveStackedGroup clbk : m_listeners)
                clbk.onSetStackingMethod(this, stack);
        }
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

            for(ISubDatasetSubjectiveStackedGroup clbk : m_listeners)
                clbk.onAddSubjectiveViews(this, new Pair<SubDataset, SubDataset>(sdStacked, sdLinked));
            return true;
        }
        return false;
    }

    public void updateSubDatasets()
    {
        ArrayList<Pair<SubDataset, SubDataset>> subjViews = getSubjectiveSubDatasets();

        for(Pair<SubDataset, SubDataset> subj : subjViews)
        {
            if(subj.first != null && subj.second != null)
            {
                subj.first.setTransferFunction((TransferFunction)subj.second.getTransferFunction().clone());
            }
        }
        super.updateSubDatasets();
    }

    public SubDataset getBase()
    {
        return getSubDatasetFromNativePointer(nativeGetBase(m_ptr));
    }

    /** Get pairs of subjective views already registered
     * @return pairs of subjective views. First == Stacked, Second == Linked. If a component is equal to null, it means that there is no view to fill this role*/
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


    /** Get the pair of subjective views already registered that is owned by ownerID
     * @param ownerID  the owner to look for.
     * @return The pair of subjective views owned by ownerID. First == Stacked, Second == Linked. If a component is equal to null, it means that there is no view to fill this role*/
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

    /** Set the current application focus.
     * @param onBase set this at true to focus on the base subjective view, or false to focus on the client's subjective view*/
    public void setFocus(boolean onBase)
    {
        if(m_focusOnBase != onBase)
        {
            m_focusOnBase = onBase;
            for(ISubDatasetSubjectiveStackedGroup clbk : m_listeners)
                clbk.onSetFocus(this, onBase);
        }
    }

    public boolean focusOnBase()
    {
        return m_focusOnBase;
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
    private static native long    nativeGetBase(long sdgPtr);
}