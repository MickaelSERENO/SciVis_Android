package com.sereno.vfv;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.GridLayout;
import android.widget.TextView;

import com.sereno.vfv.Data.VTKFieldValue;
import com.sereno.vfv.Data.VTKParser;
import com.sereno.vfv.Listener.INotiveVTKDialogListener;

import java.util.ArrayList;

/** Class handling the dialogs for VTK dataset*/
public class OpenVTKDatasetDialog
{
    /** Structure permitting to know if a field value is selected or not*/
    public class VTKFieldValueSelection
    {
        /** Is the vtk field selected ?*/
        private boolean       m_isSelected = false;

        /** What is the bounded VTKFieldValue ?*/
        public  VTKFieldValue fieldValue   = null;

        /** Is the vtk field value selected ?
         * @return true if selected, false otherwise*/
        public boolean isSelected(){return m_isSelected;}

        /** Set the selection status of this vtk field value
         * @param s the new selection status*/
        private void setSelected(boolean s) {m_isSelected = s;}
    }

    /** The OnCheckChangeListener applied for VTKFieldValueSelection*/
    public class VTKFieldValueCheckChange implements CompoundButton.OnCheckedChangeListener
    {
        /** @brief The VTKFieldValueSelection bound to this listener*/
        private VTKFieldValueSelection m_value;

        /** @brief constructor
         * @param f the VTKFieldValueSelection to bind with this Listener*/
        public VTKFieldValueCheckChange(VTKFieldValueSelection f)
        {
            m_value = f;
        }


        @Override
        public void onCheckedChanged(CompoundButton compoundButton, boolean b)
        {
            m_value.setSelected(b);
        }
    }

    /** The VTKParser to use*/
    private VTKParser m_parser;

    /** The context associated with the dialog*/
    private Context   m_ctx;

    /** The view generated based on the VTKParser information*/
    private View      m_view;

    /** The point field value selection binding*/
    private VTKFieldValueSelection[] m_ptFieldValue;

    /** The cell field value selection binding*/
    private VTKFieldValueSelection[] m_cellFieldValue;


    /** The constructor
     * @param ctx the Context needed to create the AlertDialog
     * @param parser  the VTKParser containing the VTK file object information*/
    public OpenVTKDatasetDialog(Context ctx, VTKParser parser)
    {
        m_parser  = parser;
        m_ctx     = ctx;
        m_view    = ((LayoutInflater)m_ctx.getSystemService(Context.LAYOUT_INFLATER_SERVICE)).inflate(R.layout.open_vtk_dataset_dialog, null);

        //Get the field values
        VTKFieldValue[] ptFieldValue   = parser.getPointFieldValues();
        VTKFieldValue[] cellFieldValue = parser.getCellFieldValues();

        m_ptFieldValue   = new VTKFieldValueSelection[ptFieldValue.length];
        m_cellFieldValue = new VTKFieldValueSelection[cellFieldValue.length];

        for(int i = 0; i < ptFieldValue.length; i++)
        {
            m_ptFieldValue[i] = new VTKFieldValueSelection();
            m_ptFieldValue[i].fieldValue = ptFieldValue[i];
        }

        for(int i = 0; i < cellFieldValue.length; i++)
        {
            m_cellFieldValue[i] = new VTKFieldValueSelection();
            m_cellFieldValue[i].fieldValue = cellFieldValue[i];
        }
        //Generate the view
        for(int i = 0; i < m_ptFieldValue.length; i++)
            pushFieldValue(m_ptFieldValue[i], i);
        for(int i = 0; i < m_cellFieldValue.length; i++)
            pushFieldValue(m_cellFieldValue[i], i+m_ptFieldValue.length);
    }

    /** Open the Dialog
     * @param listener the callback object*/
    public void open(final INotiveVTKDialogListener listener)
    {
        AlertDialog.Builder builder = new AlertDialog.Builder(m_ctx);
        builder.setTitle(R.string.openVTKTitle);
        builder.setNegativeButton(R.string.cancel, new DialogInterface.OnClickListener()
        {
            @Override
            public void onClick(DialogInterface dialogInterface, int i)
            {
                listener.onDialogNegativeClick(OpenVTKDatasetDialog.this);
            }
        });
        builder.setPositiveButton(R.string.open, new DialogInterface.OnClickListener()
        {
            @Override
            public void onClick(DialogInterface dialogInterface, int i)
            {
                listener.onDialogPositiveClick(OpenVTKDatasetDialog.this);
            }
        });
        builder.setView(m_view);

        builder.show();
    }

    /** @brief Get the selected point field values
     * @return Array of selected point field values*/
    public ArrayList<VTKFieldValue> getSelectedPtFieldValues()
    {
        return getSelectedValues(m_ptFieldValue);
    }

    /** @brief Get the selected cell field values
     * @return Array of selected cell field values*/
    public ArrayList<VTKFieldValue> getSelectedCellFieldValues()
    {
        return getSelectedValues(m_cellFieldValue);
    }

    /** @brief Get the VTKParser associated with this Dialog
     * @return the VTKParser object*/
    public VTKParser getVTKParser()
    {
        return m_parser;
    }

    /** @brief Get the selected field values
     * @param values array of VTKFieldValueSelection to look at
     * @return Array of selected field values*/
    private ArrayList<VTKFieldValue> getSelectedValues(VTKFieldValueSelection[] values)
    {
        ArrayList<VTKFieldValue> arr = new ArrayList<>();
        for(VTKFieldValueSelection v : values)
            if(v.isSelected())
                arr.add(v.fieldValue);
        return arr;
    }

    /** Push a field value in the GridLayout
     * @param f the field value to push
     * @param r the row where it belongs to*/
    private void pushFieldValue(VTKFieldValueSelection f, int r)
    {
        GridLayout grid = (GridLayout)m_view;

        //The name
        TextView   name = new TextView(m_ctx);
        name.setText(f.fieldValue.getName());

        //The checkbox
        CheckBox   sele = new CheckBox(m_ctx);
        sele.setChecked(f.isSelected());
        sele.setOnCheckedChangeListener(new VTKFieldValueCheckChange(f));

        grid.addView(sele, new GridLayout.LayoutParams(GridLayout.spec(r, GridLayout.CENTER),
                                                       GridLayout.spec(0, GridLayout.CENTER, 1.0f)));
        grid.addView(name, new GridLayout.LayoutParams(GridLayout.spec(r, GridLayout.CENTER),
                                                       GridLayout.spec(1, GridLayout.CENTER, 3.0f)));
    }
}