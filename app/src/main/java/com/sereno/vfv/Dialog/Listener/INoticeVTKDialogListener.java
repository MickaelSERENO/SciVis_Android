package com.sereno.vfv.Dialog.Listener;

import com.sereno.vfv.Dialog.OpenVTKDatasetDialog;

public interface INoticeVTKDialogListener
{
    /** \brief Function called when the position click button is pressed
     * @param dialogFragment the VTK Dialog*/
    void onDialogPositiveClick(OpenVTKDatasetDialog dialog);

    /** \brief Function called when the negative click button is pressed
     * @param dialogFragment the VTK dialog*/
    void onDialogNegativeClick(OpenVTKDatasetDialog dialog);
}
