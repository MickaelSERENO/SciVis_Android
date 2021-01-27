package com.sereno.view;

import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Matrix;
import android.graphics.Point;
import android.graphics.drawable.ColorDrawable;
import android.graphics.drawable.Drawable;
import android.view.View;

public class CustomShadowBuilder extends View.DragShadowBuilder
{
    private Drawable m_shadow; //Background shadow
    private float m_scaleFactor = 0.8f;

    public CustomShadowBuilder(View v)
    {
        super(v);
        m_shadow = new ColorDrawable(Color.LTGRAY);
    }

    @Override
    public void onProvideShadowMetrics (Point size, Point touch) {
        int width, height;

        width = (int)(getView().getWidth() * m_scaleFactor);
        height = (int)(getView().getHeight() * m_scaleFactor);
        m_shadow.setBounds(0, 0, width, height);
        size.set(width, height);
        touch.set(width, height / 2); //middle right
    }

    // Defines a callback that draws the drag shadow in a Canvas that the system constructs
    // from the dimensions passed in onProvideShadowMetrics().
    @Override
    public void onDrawShadow(Canvas canvas)
    {
        m_shadow.draw(canvas);
        canvas.scale(m_scaleFactor, m_scaleFactor);
        getView().draw(canvas);
    }
}