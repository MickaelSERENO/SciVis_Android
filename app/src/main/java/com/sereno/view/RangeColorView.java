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

public class RangeColorView extends View
{
    public static final int MAX_PIXELS    = 150; /*!< Maximum height*/
    public static final int TRIANGLE_SIZE = 30;  /*!< The triangles size*/

    public static final int MANIPULATING_NO_VALUE  = 0;
    public static final int MANIPULATING_MIN_VALUE = 1;
    public static final int MANIPULATING_MAX_VALUE = 2;

    private Paint m_paint        = new Paint(); /*!< The object configuring the paint of the canvas (color)*/
    private Paint m_handlesPaint = new Paint();
    private int   m_colorMode    = ColorMode.RAINBOW; /*!< The colormode to display*/

    private float m_minValue     = 0.0f;
    private float m_maxValue     = 1.0f;

    private int   m_valueInManipulation = MANIPULATING_NO_VALUE;

    /** @brief Constructor with the view's context as parameter
     *
     * @param c the Context associated with the view
     */
    public RangeColorView(Context c)
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
    public RangeColorView(Context c, AttributeSet a, int style)
    {
        super(c, a, style);
        init();
    }

    /** @brief Constructor with the view's context as parameter and the XML data
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
        int height = (int)(getHeight() - TRIANGLE_SIZE*Math.sqrt(3.0f)/2.0f);

        for(int i = 0; i < width; i+=3)
        {
            float t = (float)(i) / (float)(width);
            Color c = null;

            if(t > 0.5)
                Log.i("Main", "arg");

            switch(m_colorMode)
            {
                case ColorMode.RAINBOW:
                {
                    c = new HSVColor(260.0f * (1.0f-t), 1.0f, 1.0f, 1.0f).toRGB();
                    break;
                }
                case ColorMode.GRAYSCALE:
                {
                    c = new Color(t, t, t, 1.0f);
                    break;
                }
                case ColorMode.WARM_COLD_CIELAB:
                {
                    if(t < 0.5)
                        c = LABColor.lerp(ColorMode.coldLAB, ColorMode.whiteLAB, 2.0f*t).toRGB();
                    else
                        c = LABColor.lerp(ColorMode.whiteLAB, ColorMode.warmLAB, 2.0f*t-1.0f).toRGB();
                    break;
                }
                case ColorMode.WARM_COLD_CIELUV:
                {
                    if(t < 0.5)
                        c = LUVColor.lerp(ColorMode.coldLUV, ColorMode.whiteLUV, 2.0f*t).toRGB();
                    else
                        c = LUVColor.lerp(ColorMode.whiteLUV, ColorMode.warmLUV, 2.0f*t-1.0f).toRGB();
                    break;
                }
                case ColorMode.WARM_COLD_MSH:
                {
                    c = MSHColor.fromColorInterpolation(ColorMode.coldRGB, ColorMode.warmRGB, t).toRGB();
                    break;
                }
                default:
                    return;
            }
            int intColor = (255 << 24) + ((int)(c.r*255) << 16) +
                           ((int)(c.g*255) << 8) + (int)(c.b*255);
            m_paint.setColor(intColor);
            m_paint.setStyle(Paint.Style.STROKE);
            for(int j = 0; j < 3; j++)
                canvas.drawLine(i+j+TRIANGLE_SIZE/2.0f, 0, i+j+TRIANGLE_SIZE/2.0f, height, m_paint);
        }
        //Draw the handles
        int[] v = new int[]{(int)(width*m_minValue), (int)(width*m_maxValue)};
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

    public void setColorMode(int mode)
    {
        m_colorMode = mode;
        invalidate();
    }

    @Override
    public boolean onTouchEvent(MotionEvent e)
    {
        super.onTouchEvent(e);
        int width  = getWidth() - TRIANGLE_SIZE;
        int x = (int)e.getX();
        float indice = Math.min(Math.max((x - TRIANGLE_SIZE/2.0f)/width, 0.0f), 1.0f);

        //Set the cursor and store which value we are manipulating
        if(e.getAction() == MotionEvent.ACTION_DOWN)
        {
            if(Math.abs(indice - m_minValue) < Math.abs(indice - m_maxValue))
            {
                m_valueInManipulation = MANIPULATING_MIN_VALUE;
                m_minValue = indice;
            }
            else
            {
                m_valueInManipulation = MANIPULATING_MAX_VALUE;
                m_maxValue = indice;
            }

        }
        else if(e.getAction() == MotionEvent.ACTION_UP)
            m_valueInManipulation = MANIPULATING_NO_VALUE;

        //Move the cursor
        else if(e.getAction() == MotionEvent.ACTION_MOVE)
        {
            if(m_valueInManipulation == MANIPULATING_MIN_VALUE)
            {
                //Switch manipulating indice if we went hover
                if(indice > m_maxValue)
                {
                    m_minValue = m_maxValue;
                    m_maxValue = indice;
                    m_valueInManipulation = MANIPULATING_MAX_VALUE;
                }
                else
                    m_minValue = indice;
            }

            else if(m_valueInManipulation == MANIPULATING_MAX_VALUE)
            {
                //Switch manipulating indice if we went hover
                if(indice < m_minValue)
                {
                    m_maxValue = m_minValue;
                    m_minValue = indice;
                    m_valueInManipulation = MANIPULATING_MIN_VALUE;
                }
                else
                    m_maxValue = indice;
            }
        }
        invalidate();
        return true;
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
    }
}