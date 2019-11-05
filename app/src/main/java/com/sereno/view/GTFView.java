package com.sereno.view;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.support.annotation.Nullable;
import android.util.AttributeSet;
import android.view.View;

/** Class proposing a way to manipulate Gaussian Transfer Function*/
public class GTFView extends View
{
    private Paint    m_paint = new Paint(); /*!< The object configuring the paint of the canvas*/
    private GTFData m_model = new GTFData(null); /*!< The model of this View*/

    public GTFView(Context context)
    {
        super(context);
        init(null);
    }

    public GTFView(Context context, @Nullable AttributeSet attrs)
    {
        super(context, attrs);
        init(attrs);
    }

    public GTFView(Context context, @Nullable AttributeSet attrs, int defStyleAttr)
    {
        super(context, attrs, defStyleAttr);
        init(attrs);
    }

    public GTFView(Context context, @Nullable AttributeSet attrs, int defStyleAttr, int defStyleRes)
    {
        super(context, attrs, defStyleAttr, defStyleRes);
        init(attrs);
    }

    /** Initialize the view
     * @param attrs the attribute pass by XML parsing*/
    private void init(@Nullable AttributeSet attrs)
    {

    }

    @Override
    public void onDraw(Canvas canvas)
    {
        super.onDraw(canvas);

        //TODO
    }
}