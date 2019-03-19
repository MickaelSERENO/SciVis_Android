package com.sereno.vfv;

import android.support.v4.app.Fragment;

import java.util.ArrayList;

public class VFVFragment extends Fragment
{
    public interface IFragmentListener
    {
        void onEnableSwipping(Fragment fragment);
        void onDisableSwipping(Fragment fragment);
    }

    /** The list of registered listeners*/
    protected ArrayList<IFragmentListener> m_listeners = new ArrayList<>();

    /** @brief Add a new listener
     * @param l the new listener*/
    public void addListener(IFragmentListener l)
    {
        m_listeners.add(l);
    }

    /** @brief Remove an old listener
     * @param l the listener to remove*/
    public void removeListener(IFragmentListener l)
    {
        m_listeners.remove(l);
    }
}
