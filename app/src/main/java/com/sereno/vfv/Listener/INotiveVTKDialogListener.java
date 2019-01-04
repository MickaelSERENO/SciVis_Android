package com.sereno.vfv.Listener;

import com.sereno.vfv.OpenVTKDatasetDialog;

public interface INotiveVTKDialogListener
{
    /** \brief Function called when the position click button is pressed
     * @param dialogFragment the VTK Dialog*/
    void onDialogPositiveClick(OpenVTKDatasetDialog dialog);

    /** \brief Function called when the negative click button is pressed
     * @param dialogFragment the VTK dialog*/
    void onDialogNegativeClick(OpenVTKDatasetDialog dialog);
}
