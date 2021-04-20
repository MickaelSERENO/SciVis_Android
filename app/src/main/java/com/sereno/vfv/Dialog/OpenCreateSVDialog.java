package com.sereno.vfv.Dialog;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.Spinner;

import com.sereno.vfv.Dialog.Listener.INoticeCreateSVDialogListener;
import com.sereno.vfv.R;

/** Initialize the view than can later be opened to gather information about an "in creation" subdataset*/
public class OpenCreateSVDialog
{
    /** The Android Context needed to create the AlertDialog*/
    private Context m_ctx;

    /** The displayed view created*/
    private View m_view;

    private Spinner m_svTypeSpinner;

    /** Constructor, initialize the view to later open
      * @param ctx the Context needed to create the AlertDialog*/
    public OpenCreateSVDialog(Context ctx)
    {
        m_ctx     = ctx;

        //Initialize the view to display
        m_view          = ((LayoutInflater)m_ctx.getSystemService(Context.LAYOUT_INFLATER_SERVICE)).inflate(R.layout.open_create_sv_dialog, null);
        m_svTypeSpinner = (Spinner)m_view.findViewById(R.id.svType);
    }

    /** Open the Dialog. Do not call this function again while the dialog is still opened
     * @param listener the callback object*/
    public void open(final INoticeCreateSVDialogListener listener)
    {
        AlertDialog.Builder builder = new AlertDialog.Builder(m_ctx);
        builder.setTitle(R.string.createSVTitle);
        builder.setNegativeButton(R.string.cancel, new DialogInterface.OnClickListener()
        {
            @Override
            public void onClick(DialogInterface dialogInterface, int i)
            {
                listener.onDialogNegativeClick(OpenCreateSVDialog.this);
            }
        });
        builder.setPositiveButton(R.string.open, new DialogInterface.OnClickListener()
        {
            @Override
            public void onClick(DialogInterface dialogInterface, int i)
            {
                listener.onDialogPositiveClick(OpenCreateSVDialog.this);
            }
        });
        builder.setView(m_view);

        builder.show();
    }

    public int getSelectedSVType()
    {
        return (int)m_svTypeSpinner.getSelectedItemId();
    }
}
