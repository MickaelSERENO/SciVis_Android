package com.sereno.view;

import android.content.Context;
import android.support.annotation.Nullable;
import android.util.AttributeSet;
import android.view.View;

public class GTFTransferFunctionView extends View
{
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
}
