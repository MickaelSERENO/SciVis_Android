package com.sereno.vfv.Dialog.Listener;

import com.sereno.vfv.Dialog.OpenCreateSVDialog;

public interface INoticeCreateSVDialogListener
{
    /** \brief Function called when the position click button is pressed
     * @param dialog the Create subjective view  dialog*/
    void onDialogPositiveClick(OpenCreateSVDialog dialog);

    /** \brief Function called when the negative click button is pressed
     * @param dialog the Create subjective view  dialog*/
    void onDialogNegativeClick(OpenCreateSVDialog dialog);
}
