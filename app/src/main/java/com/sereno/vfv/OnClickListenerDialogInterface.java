package com.sereno.vfv;

import android.view.View;

public abstract class OnClickListenerDialogInterface implements android.content.DialogInterface.OnClickListener
{
    private View m_view = null; /**!< The view associated with this dialog interface*/

    /** \brief Constructor. Associated a view with a dialog interface
     * @param v the view to associate*/
    public OnClickListenerDialogInterface(View v)
    {
        m_view = v;
    }

    /** \brief Get the view associated with this interface
     * \return the View associated with this interface*/
    public View getView()
    {
        return m_view;
    }
}
