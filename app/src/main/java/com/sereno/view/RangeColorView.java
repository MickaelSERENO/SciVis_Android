package com.sereno.view;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Path;
import android.util.AttributeSet;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;

import com.sereno.color.Color;
import com.sereno.color.ColorMode;
import com.sereno.color.HSVColor;
import com.sereno.color.LABColor;
import com.sereno.color.LUVColor;
import com.sereno.color.MSHColor;

import java.util.ArrayList;

public class RangeColorView extends View implements RangeColorData.IOnRangeChangeListener
{

    public static final int MAX_PIXELS    = 150; /*!< Maximum height*/
    public static final int TRIANGLE_SIZE = 30;  /*!< The triangles size*/

    public static final int MANIPULATING_NO_VALUE  = 0; /*!< Manipulating nothing (no touch)*/
    public static final int MANIPULATING_MIN_VALUE = 1; /*!< Manipulating the minimum value*/
    public static final int MANIPULATING_MAX_VALUE = 2; /*!< Manipulating the maximum value*/

    private Paint m_paint        = new Paint(); /*!< The object configuring the paint of the canvas (color)*/
    private Paint m_handlesPaint = new Paint(); /*!< The paint object permitting to draw the handlers*/

    private int   m_valueInManipulation = MANIPULATING_NO_VALUE; /*!< What is the current handle being manipulated (i.e moved) ?*/

    private RangeColorData m_model = new RangeColorData(); /*!< The internal data model*/

    /** @brief Constructor with the view's context as @parameter
     *
     * @param c the Context associated with the view
     */
    public RangeColorView(Context c)
    {
        super(c);
        init();
    }

    /** @brief Constructor with the view's context as @parameter and the XML data
     *
     * @param c the Context associated with the view
     * @param a the XML attributes of the view
     * @param style the style ID of the View (see View.Style)
     */
    public RangeColorView(Context c, AttributeSet a, int style)
    {
        super(c, a, style);
        init();
    }

    /** @brief Constructor with the view's context as @parameter and the XML data
     *
     * @param c the Context associated with the view
     * @param a the XML attributes of the view*/
    public RangeColorView(Context c, AttributeSet a)
    {
        super(c, a);
        init();
    }

    public void onDraw(Canvas canvas)
    {
        super.onDraw(canvas);

        //Draw the colors
        int width  = getWidth()  - TRIANGLE_SIZE;
        int height = (int)(getHeight() - 1 - TRIANGLE_SIZE*Math.sqrt(3.0f)/2.0f);

        for(int i = 0; i < width; i+=3)
        {
            float t = (float)(i) / (float)(width);
            Color c = ColorMode.computeRGBColor(t, m_model.getColorMode());
            c.a = 1.0f;
            int intColor = c.toARGB8888();

            m_paint.setColor(intColor);
            m_paint.setStyle(Paint.Style.STROKE);
            for(int j = 0; j < 3; j++)
                canvas.drawLine(i+j+TRIANGLE_SIZE/2.0f, 0, i+j+TRIANGLE_SIZE/2.0f, height, m_paint);
        }

        //Draw the handles
        int[] v = new int[]{(int)(width*m_model.getMinRange()), (int)(width*m_model.getMaxRange())};
        for(int j = 0; j < 2; j++)
        {
            Path path = new Path();
            path.moveTo(v[j] + TRIANGLE_SIZE / 2.0f, height);
            path.lineTo(v[j], getHeight());
            path.lineTo(v[j] + TRIANGLE_SIZE, getHeight());
            path.close();
            canvas.drawPath(path, m_handlesPaint);
        }
    }

    @Override
    public boolean onTouchEvent(MotionEvent e)
    {
        super.onTouchEvent(e);
        int      width       = getWidth() - TRIANGLE_SIZE;
        int      x           = (int)e.getX();
        boolean valueChanged = false;
        float   indice       = Math.min(Math.max((x - TRIANGLE_SIZE/2.0f)/width, 0.0f), 1.0f);

        float minValue = m_model.getMinRange();
        float maxValue = m_model.getMaxRange();

        //Set the cursor and store which value we are manipulating
        if(e.getAction() == MotionEvent.ACTION_DOWN)
        {
            if(Math.abs(indice - m_model.getMinRange()) < Math.abs(indice - m_model.getMaxRange()))
            {
                m_valueInManipulation = MANIPULATING_MIN_VALUE;
                minValue = indice;
            }
            else
            {
                m_valueInManipulation = MANIPULATING_MAX_VALUE;
                maxValue = indice;
            }
            valueChanged = true;
        }
        else if(e.getAction() == MotionEvent.ACTION_UP)
            m_valueInManipulation = MANIPULATING_NO_VALUE;

        //Move the cursor
        else if(e.getAction() == MotionEvent.ACTION_MOVE)
        {
            if(m_valueInManipulation == MANIPULATING_MIN_VALUE)
            {
                //Switch manipulating indice if we went hover
                if(indice > maxValue)
                {
                    minValue = maxValue;
                    maxValue = indice;
                    m_valueInManipulation = MANIPULATING_MAX_VALUE;
                }
                else
                    minValue = indice;
            }

            else if(m_valueInManipulation == MANIPULATING_MAX_VALUE)
            {
                //Switch manipulating indice if we went hover
                if(indice < minValue)
                {
                    maxValue = minValue;
                    minValue = indice;
                    m_valueInManipulation = MANIPULATING_MIN_VALUE;
                }
                else
                    maxValue = indice;
            }
            valueChanged = true;
        }
        if(valueChanged)
            m_model.setRange(minValue, maxValue);
        return true;
    }

    /** Get the model of this RangeColorView
     * @return the RangeColorData model*/
    public RangeColorData getModel()
    {
        return m_model;
    }

    @Override
    public void onRangeChange(RangeColorData view, float minVal, float maxVal, int mode)
    {
        invalidate();
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec)
    {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);

        int widthSize  = MeasureSpec.getSize(widthMeasureSpec);
        int heightMode = MeasureSpec.getMode(heightMeasureSpec);
        int heightSize = MeasureSpec.getSize(heightMeasureSpec);

        int width  = widthSize;
        int height = heightSize;

        /* Try to apply MAX_PIXELS in y axis*/
        switch(heightMode)
        {
            case MeasureSpec.AT_MOST:
                height = Math.min(height, MAX_PIXELS);
                break;
            case MeasureSpec.UNSPECIFIED:
                height = MAX_PIXELS;
        }

        setMeasuredDimension(width, height);
    }

    @Override
    public boolean performClick()
    {
        return super.performClick();
    }

    /** \brief Initialize the RangeColor view*/
    private void init()
    {
        setMinimumHeight(0);
        setMinimumWidth(0);

        setFocusable(true);
        setFocusableInTouchMode(true);
        setClickable(true);

        m_paint.setColor(android.graphics.Color.BLACK);
        m_paint.setStyle(Paint.Style.FILL);

        m_model.addOnRangeChangeListener(this);
    }
}