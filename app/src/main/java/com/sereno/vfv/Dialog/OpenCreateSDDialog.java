package com.sereno.vfv.Dialog;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.CompoundButton;

import com.sereno.vfv.Data.Dataset;
import com.sereno.vfv.Dialog.Listener.INoticeCreateSDDialogListener;
import com.sereno.vfv.R;

/** Initialize the view than can later be opened to gather information about an "in creation" subdataset*/
public class OpenCreateSDDialog
{
    /** The dataset of interest (the parent of the possible created subdataset)*/
    private Dataset m_dataset;

    /** The Android Context needed to create the AlertDialog*/
    private Context m_ctx;

    /** The displayed view created*/
    private View m_view;

    private CompoundButton m_privacyState;

    /** Constructor, initialize the view to later open
      * @param ctx the Context needed to create the AlertDialog
      * @param dataset the Dataset of interest.*/
    public OpenCreateSDDialog(Context ctx, Dataset dataset)
    {
        m_dataset = dataset;
        m_ctx     = ctx;

        //Initialize the view to display
        m_view         = ((LayoutInflater)m_ctx.getSystemService(Context.LAYOUT_INFLATER_SERVICE)).inflate(R.layout.open_create_sd_dialog, null);
        m_privacyState = (CompoundButton)m_view.findViewById(R.id.sdPrivacy);
    }

    /** Open the Dialog. Do not call this function again while the dialog is still opened
     * @param listener the callback object*/
    public void open(final INoticeCreateSDDialogListener listener)
    {
        AlertDialog.Builder builder = new AlertDialog.Builder(m_ctx);
        builder.setTitle(R.string.createSDTitle);
        builder.setNegativeButton(R.string.cancel, new DialogInterface.OnClickListener()
        {
            @Override
            public void onClick(DialogInterface dialogInterface, int i)
            {
                listener.onDialogNegativeClick(OpenCreateSDDialog.this);
            }
        });
        builder.setPositiveButton(R.string.open, new DialogInterface.OnClickListener()
        {
            @Override
            public void onClick(DialogInterface dialogInterface, int i)
            {
                listener.onDialogPositiveClick(OpenCreateSDDialog.this);
            }
        });
        builder.setView(m_view);

        builder.show();
    }

    /** Get wheter the created SD should be public or not
     * @return true if public, false otherwise*/
    public boolean isSDPublic()
    {
        return m_privacyState.isChecked();
    }
}
