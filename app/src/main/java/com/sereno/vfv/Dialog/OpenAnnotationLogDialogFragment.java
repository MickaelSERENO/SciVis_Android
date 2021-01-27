package com.sereno.vfv.Dialog;

import android.app.AlertDialog;
import android.app.Dialog;
import android.support.v4.app.DialogFragment;
import android.content.DialogInterface;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.RadioGroup;
import android.widget.Spinner;

import com.sereno.vfv.Data.Annotation.AnnotationLogContainer;
import com.sereno.vfv.Data.DataFile;
import com.sereno.vfv.Dialog.Listener.INoticeDialogListener;
import com.sereno.vfv.R;

/**Dialog fragment for the opening of a new annotation data */
public class OpenAnnotationLogDialogFragment extends DialogFragment
{
    private INoticeDialogListener m_listener = null; /**!< The Notice Dialog Listener object to call when the user has finished with the dialog*/
    private Spinner  m_datasetSpinner = null;
    private CheckBox m_hasHeader        = null;
    private Spinner  m_headerStrSpinner = null;
    private Spinner  m_headerIntSpinner = null;


    @Override
    public Dialog onCreateDialog(Bundle savedInstanceState)
    {
        // Use the Builder class for convenient dialog construction
        AlertDialog.Builder builder = new AlertDialog.Builder(getActivity());

        //Create the AlertDialog object and return it
        View view = getActivity().getLayoutInflater().inflate(R.layout.open_annotation_log_dialog, null);
        builder.setView(view);
        boolean viewInitialized = setupLayout(view);


        //Set the message, button and layout
        builder.setNegativeButton(R.string.cancel, new OnClickListenerDialogInterface(view)
        {
            public void onClick(DialogInterface dialog, int id)
            {
                if(m_listener != null)
                    m_listener.onDialogNegativeClick(OpenAnnotationLogDialogFragment.this, getView());
            }
        });

        if(viewInitialized)
        {
            builder.setPositiveButton(R.string.open, new OnClickListenerDialogInterface(view)
            {
                public void onClick(DialogInterface dialog, int id) {
                    if (m_listener != null)
                        m_listener.onDialogPositiveClick(OpenAnnotationLogDialogFragment.this, getView());
                }
            });
            builder.setMessage(R.string.openAnnotationLogs);
        }
        else
            builder.setMessage(R.string.errorAnnotationLogs);

        AlertDialog dialog = builder.create();
        return dialog;
    }

    /** set the INoticeDialogListener associated with this object
     * @param listener the new INoticeDialogListener*/
    public void setNoticeDialogListener(INoticeDialogListener listener)
    {
        m_listener = listener;
    }

    /** setup the layout of the dialog
     * @return true if the layout can be set up correctly. If no datasets, this function returns false*/
    private boolean setupLayout(View view)
    {
        m_datasetSpinner = (Spinner)view.findViewById(R.id.openAnnotationLogsSpinner);
        m_hasHeader = (CheckBox)view.findViewById(R.id.annotLogHasHeader);
        m_headerStrSpinner = (Spinner)view.findViewById(R.id.annotLogTimeHeaderStr);
        m_headerIntSpinner = (Spinner)view.findViewById(R.id.annotLogTimeHeaderInt);

        final Runnable updateHeader = new Runnable()
        {
            @Override
            public void run()
            {
                if(m_datasetSpinner.getSelectedItem() == null)
                    return;

                DataFile df = (DataFile)m_datasetSpinner.getSelectedItem();
                AnnotationLogContainer log   = new AnnotationLogContainer(-1, df.getFile().getAbsolutePath(), m_hasHeader.isChecked());

                if(log.hasHeaders())
                {
                    //Update the header:
                    ArrayAdapter<String> adapter = new ArrayAdapter<>(getActivity(), android.R.layout.simple_spinner_dropdown_item);
                    adapter.add("-1"); //No time header
                    adapter.addAll(log.getHeaders());
                    m_headerStrSpinner.setAdapter(adapter);

                    //Update visibility
                    m_headerIntSpinner.setVisibility(View.GONE);
                    m_headerStrSpinner.setVisibility(View.VISIBLE);
                }
                else
                {
                    Integer[] headers = new Integer[log.getNbColumns()+1];
                    for(int i = 0; i < headers.length; i++)
                        headers[i] = i-1; //-1 == no time header
                    ArrayAdapter<Integer> adapter = new ArrayAdapter<>(getActivity(), android.R.layout.simple_spinner_dropdown_item, headers);
                    m_headerIntSpinner.setAdapter(adapter);

                    //Update visibility
                    m_headerStrSpinner.setVisibility(View.GONE);
                    m_headerIntSpinner.setVisibility(View.VISIBLE);
                }
            }
        };

        //Files openable
        DataFile[] dataFiles   = DataFile.getAvailableAnnotationLogs(getActivity());
        if(dataFiles == null)
        {
            m_datasetSpinner.setVisibility(View.GONE);
            return false;
        }
        ArrayAdapter<DataFile> adapter = new ArrayAdapter<>(getActivity(), android.R.layout.simple_spinner_dropdown_item, dataFiles);
        m_datasetSpinner.setAdapter(adapter);
        m_datasetSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener()
        {
            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l)
            {
                updateHeader.run();
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView)
            {}
        });

        //Handle the checkbox
        m_hasHeader.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener()
        {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b)
            {
                updateHeader.run();
            }
        });

        updateHeader.run();

        return true;
    }

    /** Get the file that has been selected
     * @return the selected file*/
    public DataFile getSelectedFile()
    {
        return m_datasetSpinner != null ? (DataFile)m_datasetSpinner.getSelectedItem() : null;
    }

    /** Get the selected header as being the time header
     * @return the indice of the header (column ID) corresponding to time. -1 == no time*/
    public int getTimeHeaderID()
    {
        if(m_hasHeader != null)
        {
            if(m_hasHeader.isChecked())
            {
                if(m_headerStrSpinner != null)
                    return (int)m_headerStrSpinner.getSelectedItemId()-1;
                return -1;
            }

            if(m_headerIntSpinner != null)
                return (int)m_headerIntSpinner.getSelectedItemId()-1;
            return -1;
        }
        return -1;
    }

    /** Has this log annotation a header?
     * @return true if yes, false otherwise*/
    public boolean hasHeader()
    {
        if(m_hasHeader != null)
            return m_hasHeader.isChecked();
        return false;
    }
}