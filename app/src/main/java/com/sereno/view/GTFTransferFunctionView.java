package com.sereno.view;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.support.annotation.Nullable;
import android.util.AttributeSet;
import android.view.View;

public class GTFTransferFunctionView extends View
{
    private Paint m_paint = new Paint(); /*!< The object configuring the paint of the canvas*/

    public GTFTransferFunctionView(Context context)
    {
        super(context);
        init(null);
    }

    public GTFTransferFunctionView(Context context, @Nullable AttributeSet attrs)
    {
        super(context, attrs);
        init(attrs);
    }

    public GTFTransferFunctionView(Context context, @Nullable AttributeSet attrs, int defStyleAttr)
    {
        super(context, attrs, defStyleAttr);
        init(attrs);
    }

    public GTFTransferFunctionView(Context context, @Nullable AttributeSet attrs, int defStyleAttr, int defStyleRes)
    {
        super(context, attrs, defStyleAttr, defStyleRes);
        init(attrs);
    }

    private void init(@Nullable AttributeSet attrs)
    {

    }

    public void onDraw(Canvas canvas)
    {
        super.onDraw(canvas);

        //TODO
    }
}