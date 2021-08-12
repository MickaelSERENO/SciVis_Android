package com.sereno.view;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Path;
import android.support.annotation.Nullable;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;

import com.sereno.color.Color;
import com.sereno.color.ColorMode;
import com.sereno.vfv.R;

public class TwoHandlesSeekBarView extends View implements TwoHandlesSeekBarData.IOnRangeChangeListener
{
    public static final int TRIANGLE_SIZE = 30;  /*!< The triangles size*/
    public static final int TEXT_SIZE     = 24;  /*!< The default text height*/
    public static final int LINE_HEIGHT   = 8;

    public static final int MANIPULATING_NO_VALUE  = 0; /*!< Manipulating nothing (no touch)*/
    public static final int MANIPULATING_MIN_VALUE = 1; /*!< Manipulating the minimum value*/
    public static final int MANIPULATING_MAX_VALUE = 2; /*!< Manipulating the maximum value*/

    private Paint   m_paint        = new Paint(); /*!< The object configuring the paint of the canvas (color)*/
    private Paint   m_handlesPaint = new Paint(); /*!< The paint object permitting to draw the handlers*/
    private Paint   m_textPaint    = new Paint(); /*!< The paint applied for text */

    private int   m_valueInManipulation = MANIPULATING_NO_VALUE; /*!< What is the current handle being manipulated (i.e moved) ?*/
    private int m_lineHeight  = LINE_HEIGHT;
    private int m_handleWidth = TRIANGLE_SIZE;

    private TwoHandlesSeekBarData m_model = new TwoHandlesSeekBarData(); /*!< The internal data model*/

    /** @brief Constructor with the view's context as @parameter
     *
     * @param c the Context associated with the view
     */
    public TwoHandlesSeekBarView(Context c)
    {
        super(c);
        init(null);
    }

    /** @brief Constructor with the view's context as @parameter and the XML data
     *
     * @param c the Context associated with the view
     * @param a the XML attributes of the view
     * @param style the style ID of the View (see View.Style)
     */
    public TwoHandlesSeekBarView(Context c, AttributeSet a, int style)
    {
        super(c, a, style);
        init(a);
    }

    /** @brief Constructor with the view's context as @parameter and the XML data
     *
     * @param c the Context associated with the view
     * @param a the XML attributes of the view*/
    public TwoHandlesSeekBarView(Context c, AttributeSet a)
    {
        super(c, a);
        init(a);
    }

    /** Compute the displayed text height (using descent and ascent)
     * @return the floating point text height based on m_textPaint*/
    private float computeTextHeight()
    {
        return m_textPaint.getFontMetrics().descent - m_textPaint.getFontMetrics().ascent;
    }

    public void onDraw(Canvas canvas)
    {
        super.onDraw(canvas);

        String minText = Float.toString(m_model.getMinRawRange());
        String maxText = Float.toString(m_model.getMaxRawRange());

        //Do measurements
        float leftTextSize  = m_textPaint.measureText(minText);
        float rightTextSize = m_textPaint.measureText(maxText);

        //To center things
        leftTextSize  = Math.max(leftTextSize, rightTextSize);
        rightTextSize = leftTextSize;
        leftTextSize  = Math.max(leftTextSize, m_handleWidth);
        rightTextSize = Math.max(rightTextSize, m_handleWidth);

        float triangleHeight = (float)(m_handleWidth*Math.sqrt(3.0f)/2.0f);

        float width    = getWidth() - (leftTextSize+rightTextSize);

        //Draw the color bands
        m_paint.setStyle(Paint.Style.FILL_AND_STROKE);
        m_paint.setColor(0xff555555);
        canvas.drawRect(leftTextSize, 0, width+leftTextSize, m_lineHeight, m_paint);
        m_paint.setColor(0xff11aabb);
        canvas.drawRect(leftTextSize+width*m_model.getMinClampingRange(), 0, leftTextSize+width*m_model.getMaxClampingRange(), m_lineHeight, m_paint);

        //Draw the handles
        int[] v = new int[]{(int)(leftTextSize+width*m_model.getMinClampingRange()), (int)(leftTextSize+width*m_model.getMaxClampingRange())};
        for(int j = 0; j < 2; j++)
        {
            Path path = new Path();
            path.moveTo(v[j], m_lineHeight);
            path.lineTo(v[j]-TRIANGLE_SIZE/2.0f, m_lineHeight+triangleHeight);
            path.lineTo(v[j]+TRIANGLE_SIZE/2.0f, m_lineHeight+triangleHeight);
            path.close();
            canvas.drawPath(path, m_handlesPaint);
        }

        //Draw the min and max ranges texts
        canvas.drawText(minText, leftTextSize/2.0f,             m_lineHeight+triangleHeight, m_textPaint);
        canvas.drawText(maxText, getWidth()-rightTextSize/2.0f, m_lineHeight+triangleHeight, m_textPaint);

        //Draw the min and max current range texts
        String minCurValue = String.format("%.3f", m_model.getMinClampingRange()*(m_model.getMaxRawRange()-m_model.getMinRawRange()) + m_model.getMinRawRange());
        String maxCurValue = String.format("%.3f", m_model.getMaxClampingRange()*(m_model.getMaxRawRange()-m_model.getMinRawRange()) + m_model.getMinRawRange());
        canvas.drawText(minCurValue, leftTextSize+width*m_model.getMinClampingRange(), m_lineHeight+2*triangleHeight, m_textPaint);
        canvas.drawText(maxCurValue, leftTextSize+width*m_model.getMaxClampingRange(), m_lineHeight+2*triangleHeight, m_textPaint);
    }

    @Override
    public boolean onTouchEvent(MotionEvent e)
    {
        super.onTouchEvent(e);

        String minText = Float.toString(m_model.getMinRawRange());
        String maxText = Float.toString(m_model.getMaxRawRange());

        //Do measurements
        float leftTextSize  = m_textPaint.measureText(minText);
        float rightTextSize = m_textPaint.measureText(maxText);

        //To center things
        leftTextSize  = Math.max(leftTextSize, rightTextSize);
        rightTextSize = leftTextSize;
        leftTextSize  = Math.max(leftTextSize, m_handleWidth);
        rightTextSize = Math.max(rightTextSize, m_handleWidth);

        int     width       = getWidth() - (int)(leftTextSize+rightTextSize);
        int     x           = (int)e.getX();
        boolean valueChanged = false;
        float   indice       = Math.min(Math.max((x - leftTextSize - m_handleWidth/2.0f)/width, 0.0f), 1.0f);

        float minValue = m_model.getMinClampingRange();
        float maxValue = m_model.getMaxClampingRange();

        //Set the cursor and store which value we are manipulating
        if(e.getAction() == MotionEvent.ACTION_DOWN)
        {
            if(Math.abs(indice - m_model.getMinClampingRange()) < Math.abs(indice - m_model.getMaxClampingRange()))
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
            m_model.setClampingRange(minValue, maxValue);
        return true;
    }

    /** Get the model of this TwoHandleSeekBarView
     * @return the TwoHandleSeekBarData model*/
    public TwoHandlesSeekBarData getModel()
    {
        return m_model;
    }

    @Override
    public void onRawRangeChange(TwoHandlesSeekBarData view, float minVal, float maxVal)
    {
        invalidate();
    }

    @Override
    public void onClippingChange(TwoHandlesSeekBarData view, float min, float max)
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

        int triangleHeight = (int)(m_handleWidth*Math.sqrt(3.0f)/2.0f);


        /* Try to apply MAX_PIXELS in y axis*/
        switch(heightMode)
        {
            case MeasureSpec.AT_MOST:
                height = Math.min(height, m_lineHeight+triangleHeight+(int)m_textPaint.getTextSize());
                break;

            case MeasureSpec.UNSPECIFIED:
                height = m_lineHeight+triangleHeight+(int)m_textPaint.getTextSize();
                break;
        }

        setMeasuredDimension(width, height);
    }

    @Override
    public boolean performClick()
    {
        return super.performClick();
    }

    /** \brief Initialize the RangeColor view*/
    private void init(@Nullable AttributeSet attrs)
    {
        setMinimumHeight(0);
        setMinimumWidth(0);

        setFocusable(true);
        setFocusableInTouchMode(true);
        setClickable(true);

        m_paint.setColor(android.graphics.Color.BLACK);
        m_paint.setStyle(Paint.Style.FILL);

        TypedArray ta     = getContext().obtainStyledAttributes(attrs, R.styleable.TwoHandleSeekBarView);
        TypedArray taText = getContext().obtainStyledAttributes(attrs, R.styleable.TextView);

        m_handleWidth = ta.getDimensionPixelSize(R.styleable.TwoHandleSeekBarView_handleWidth, TRIANGLE_SIZE);
        m_lineHeight  = ta.getDimensionPixelSize(R.styleable.TwoHandleSeekBarView_lineHeight, LINE_HEIGHT);

        m_textPaint.setTextSize(taText.getDimensionPixelSize(R.styleable.TextView_textSize, TEXT_SIZE));
        m_textPaint.setTextAlign(Paint.Align.CENTER);

        m_model.addOnRangeChangeListener(this);
    }
}