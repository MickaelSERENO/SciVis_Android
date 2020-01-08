package com.sereno.vfv.Dialog.Listener;

import com.sereno.vfv.Dialog.OpenCreateSDDialog;

public interface INoticeCreateSDDialogListener
{
    /** \brief Function called when the position click button is pressed
     * @param dialogFragment the Create SubDataset Dialog*/
    void onDialogPositiveClick(OpenCreateSDDialog dialog);

    /** \brief Function called when the negative click button is pressed
     * @param dialogFragment the Create SubDataset  dialog*/
    void onDialogNegativeClick(OpenCreateSDDialog dialog);
}
