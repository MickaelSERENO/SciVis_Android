package com.sereno.view;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.util.AttributeSet;
import android.widget.SeekBar;

import com.sereno.vfv.R;

public class SeekBarHintView extends SeekBar
{
    public static final int TEXT_SIZE = 24;  /*!< The default text height*/

    protected Paint m_textPaint = new Paint(); /*!< The paint applied for text */

    public SeekBarHintView(Context context)
    {
        super(context);
    }

    public SeekBarHintView(Context context, AttributeSet attrs)
    {
        super(context, attrs);
        init(attrs);
    }

    public SeekBarHintView(Context context, AttributeSet attrs, int defStyleAttr)
    {
        super(context, attrs, defStyleAttr);
        init(attrs);
    }

    public SeekBarHintView(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes)
    {
        super(context, attrs, defStyleAttr, defStyleRes);
        init(attrs);
    }

    private void init(AttributeSet attrs)
    {
        TypedArray ta = getContext().obtainStyledAttributes(attrs, R.styleable.TextView);

        m_textPaint.setTextSize(ta.getDimensionPixelSize(R.styleable.TextView_textSize, TEXT_SIZE));
        m_textPaint.setTextAlign(Paint.Align.CENTER);
    }

    /** Compute the displayed text height (using descent and ascent)
     * @return the floating point text height based on m_textPaint*/
    protected float computeTextHeight()
    {
        return m_textPaint.getFontMetrics().descent - m_textPaint.getFontMetrics().ascent;
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec)
    {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);

        int heightMode = MeasureSpec.getMode(heightMeasureSpec);
        int heightSize = MeasureSpec.getSize(heightMeasureSpec);

        float width  = getMeasuredWidth();
        float height = getMeasuredHeight();

        float textHeight = computeTextHeight();

        //Try to apply MAX_PIXELS in y axis if relevant data should be shown
        switch(heightMode)
        {
            case MeasureSpec.AT_MOST:
                height = Math.min(heightSize, height + 2.0f*textHeight);
                break;
            case MeasureSpec.UNSPECIFIED:
                height += 2.0f*textHeight;
                break;
        }

        setMeasuredDimension((int)width, (int)height);
    }

    @Override
    protected void onDraw(Canvas c)
    {
        super.onDraw(c);

        int thumbWidth = this.getThumb().getIntrinsicWidth();

        String text = Float.toString((float)getProgress()/getMax());
        double x = ((double)getProgress())/getMax() * (getWidth()-2.0*thumbWidth) + thumbWidth;
        double y = computeTextHeight();

        c.drawText(text, (float)x, (float)y, m_textPaint);
    }
}
