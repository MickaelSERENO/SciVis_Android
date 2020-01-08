package com.sereno.vfv.Dialog.Listener;

import android.app.DialogFragment;
import android.view.View;

/** \brief Listener for Alert dialog fragment */
public interface INoticeDialogListener
{
    /** \brief Function called when the position click button is pressed
    * @param dialogFragment the dialog that created this dialog
    * @param dialog the dialog calling this method*/
    void onDialogPositiveClick(DialogFragment dialogFrag, View view);

    /** \brief Function called when the negative click button is pressed
     * @param dialogFragment the dialog that created this dialog
     * @param view the dialog view*/
    void onDialogNegativeClick(DialogFragment dialogFrag, View view);
}