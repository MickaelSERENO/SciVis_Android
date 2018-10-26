package com.sereno.view;

import android.content.Context;
import android.graphics.Canvas;
import android.util.AttributeSet;
import android.view.View;

public class RangeColor extends View
{
    /** @brief Constructor with the view's context as parameter
     *
     * @param c the Context associated with the view
     */
    public RangeColor(Context c)
    {
        super(c);
        init();
    }

    /** @brief Constructor with the view's context as parameter and the XML data
     *
     * @param c the Context associated with the view
     * @param a the XML attributes of the view
     * @param style the style ID of the View (see View.Style)
     */
    public RangeColor(Context c, AttributeSet a, int style)
    {
        super(c, a, style);
        init();
    }

    /** @brief Constructor with the view's context as parameter and the XML data
     *
     * @param c the Context associated with the view
     * @param a the XML attributes of the view*/
    public RangeColor(Context c, AttributeSet a)
    {
        super(c, a);
        init();
    }

    public void onDraw(Canvas c)
    {
        super.onDraw(c);

        //Draw the colors
    }

    /** \brief Initialize the RangeColor view*/
    private void init()
    {

    }
}