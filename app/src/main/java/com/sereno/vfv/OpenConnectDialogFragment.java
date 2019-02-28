package com.sereno.vfv;

import android.app.AlertDialog;
import android.app.Dialog;
import android.app.DialogFragment;
import android.content.DialogInterface;
import android.os.Bundle;
import android.view.View;

import com.sereno.vfv.Listener.INoticeDialogListener;

/* \brief Dialog fragment for setting the network connection */
public class OpenConnectDialogFragment extends DialogFragment
{
    private INoticeDialogListener m_listener       = null; /**!< The Notice Dialog Listener object to call when the user has finished with the dialog*/

    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState)
    {
        // Use the Builder class for convenient dialog construction
        AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());

        //Create the AlertDialog object and return it
        View view = getActivity().getLayoutInflater().inflate(R.layout.open_connection_dialog, null);
        builder.setView(view);

        //Set the message, button and layout
        builder.setNegativeButton(R.string.cancel, new OnClickListenerDialogInterface(view)
        {
            public void onClick(DialogInterface dialog, int id)
            {
                if(m_listener != null)
                    m_listener.onDialogNegativeClick(OpenConnectDialogFragment.this, getView());
            }
        });

        builder.setPositiveButton(R.string.connect, new OnClickListenerDialogInterface(view)
        {
            public void onClick(DialogInterface dialog, int id) {
                if (m_listener != null)
                    m_listener.onDialogPositiveClick(OpenConnectDialogFragment.this, getView());
            }
        });
        builder.setMessage(R.string.connect);

        AlertDialog dialog = builder.create();
        return dialog;
    }

    /** \brief set the INoticeDialogListener associated with this object
     * @param listener the new INoticeDialogListener*/
    public void setNoticeDialogListener(INoticeDialogListener listener)
    {
        m_listener = listener;
    }
}
