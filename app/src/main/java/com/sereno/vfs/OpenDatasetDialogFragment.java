package com.sereno.vfs;

import android.app.AlertDialog;
import android.app.Dialog;
import android.app.DialogFragment;
import android.content.DialogInterface;
import android.os.Bundle;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Spinner;

import com.sereno.vfs.Data.DataFile;
import com.sereno.vfs.Listener.INoticeDialogListener;


/* \brief Dialog fragment for the opening of a new dataset */
public class OpenDatasetDialogFragment extends DialogFragment
{
    private INoticeDialogListener m_listener       = null; /*!< The Notice Dialog Listener object to call when the user has finished with the dialog*/
    private Spinner               m_datasetSpinner = null; /*!< The spinner associated with this dialog*/

    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState)
    {
        // Use the Builder class for convenient dialog construction
        AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());

        //Create the AlertDialog object and return it
        View view = getActivity().getLayoutInflater().inflate(R.layout.open_dataset_dialog, null);
        builder.setView(view);
        boolean viewInitialized = setupLayout(view);


        //Set the message, button and layout
        builder.setNegativeButton(R.string.cancel, new DialogInterface.OnClickListener()
        {
            public void onClick(DialogInterface dialog, int id)
            {
                if(m_listener != null)
                    m_listener.onDialogNegativeClick(OpenDatasetDialogFragment.this);
            }
        });

        if(viewInitialized)
        {
            builder.setPositiveButton(R.string.open, new DialogInterface.OnClickListener() {
                public void onClick(DialogInterface dialog, int id) {
                    if (m_listener != null)
                        m_listener.onDialogPositiveClick(OpenDatasetDialogFragment.this);
                }
            });
            builder.setMessage(R.string.open_dataset);
        }
        else
            builder.setMessage(R.string.errorDatasets);

        AlertDialog dialog = builder.create();
        return dialog;
    }

    /* \brief set the INoticeDialogListener associated with this object
     * \param listener the new INoticeDialogListener*/
    public void setNoticeDialogListener(INoticeDialogListener listener)
    {
        m_listener = listener;
    }

    /* \brief Get the selected data at the end of this dialog
     * \return the selected data*/
    public DataFile getSelectedDataset()
    {
        return (DataFile)m_datasetSpinner.getSelectedItem();
    }

    /* \brief setup the layout of the dialog
     * \return true if the layout can be set up correctly. If no datasets, this function returns false*/
    private boolean setupLayout(View view)
    {
        m_datasetSpinner               = view.findViewById(R.id.openDatasetSpinner);
        DataFile[] dataFiles           = DataFile.getAvailableDatasets(getActivity());
        if(dataFiles == null)
        {
            m_datasetSpinner.setVisibility(View.GONE);
            return false;
        }

        ArrayAdapter<DataFile> adapter = new ArrayAdapter<>(getActivity(), android.R.layout.simple_spinner_dropdown_item, dataFiles);
        m_datasetSpinner.setAdapter(adapter);

        return true;
    }
}
