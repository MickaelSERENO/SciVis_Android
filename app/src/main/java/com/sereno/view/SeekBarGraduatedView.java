package com.sereno.view;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Rect;
import android.util.AttributeSet;
import android.widget.SeekBar;

import com.sereno.vfv.R;

public class SeekBarGraduatedView extends SeekBarHintView implements SeekBarGraduatedData.ISeekBarGraduatedListener
{
    public static final int GRADUATION_HEIGHT = 24;
    public static final int GRADUATION_WIDTH  = 3;
    private int m_graduationHeight = GRADUATION_HEIGHT;
    private SeekBarGraduatedData m_model = new SeekBarGraduatedData();
    private Paint m_paint = new Paint();

    public SeekBarGraduatedView(Context context)
    {
        super(context);
    }

    public SeekBarGraduatedView(Context context, AttributeSet attrs)
    {
        super(context, attrs);
        init(attrs);
    }

    public SeekBarGraduatedView(Context context, AttributeSet attrs, int defStyleAttr)
    {
        super(context, attrs, defStyleAttr);
        init(attrs);
    }

    public SeekBarGraduatedView(Context context, AttributeSet attrs, int defStyleAttr, int defStyleRes)
    {
        super(context, attrs, defStyleAttr, defStyleRes);
        init(attrs);
    }

    public SeekBarGraduatedData getModel()
    {
        return m_model;
    }

    private void init(AttributeSet attrs)
    {
        m_paint.setColor(0xff000000);
        m_model.addListener(this);

        TypedArray ta     = getContext().obtainStyledAttributes(attrs, R.styleable.SeekBarGraduatedView);

        m_graduationHeight = ta.getDimensionPixelSize(R.styleable.SeekBarGraduatedView_stepHeight, GRADUATION_HEIGHT);
        m_paint.setStrokeWidth(ta.getDimensionPixelSize(R.styleable.SeekBarGraduatedView_stepWidth, GRADUATION_WIDTH));
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec)
    {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec);

        int heightMode = MeasureSpec.getMode(heightMeasureSpec);
        int heightSize = MeasureSpec.getSize(heightMeasureSpec);

        float width  = getMeasuredWidth();
        float height = getMeasuredHeight();

        float textHeight = Math.max(computeTextHeight(), m_graduationHeight);

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
        int height = getHeight();
        int paddingLeft = getPaddingLeft();
        int width  = getWidth() - paddingLeft - getPaddingRight();
        float textHeight = computeTextHeight();

        for(int i = 0; i < m_model.getNbSteps(); i++)
        {
            int x = (int)(((float)i) * width/(m_model.getNbSteps()-1)) + paddingLeft;
            c.drawLine(x, (height-m_graduationHeight)/2, x, (height+m_graduationHeight)/2, m_paint);
            if(m_model.getLabels().length > i)
                c.drawText(m_model.getLabels()[i].toString(), x, (height+m_graduationHeight)/2+textHeight, m_textPaint);
        }

        super.onDraw(c);
    }

    @Override
    public void onSetNbSteps(SeekBarGraduatedData model, int nbSteps)
    {
        invalidate();
    }

    @Override
    public void onSetLabels(SeekBarGraduatedData model, Object[] labels)
    {
        invalidate();
    }
}
