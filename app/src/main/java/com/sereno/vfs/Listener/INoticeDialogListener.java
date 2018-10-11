package com.sereno.vfs.Listener;

import android.app.DialogFragment;

/* \brief Listener for Alert dialog fragment */
public interface INoticeDialogListener
{
    /* \brief Function called when the position click button is pressed
    * \param dialog the dialog calling this function*/
    void onDialogPositiveClick(DialogFragment dialog);

    /* \brief Function called when the negative click button is pressed
     * \param dialog the dialog calling this function*/
    void onDialogNegativeClick(DialogFragment dialog);
}