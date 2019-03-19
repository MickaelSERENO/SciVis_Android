package com.sereno.view;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.Point;
import android.os.Handler;
import android.support.annotation.Nullable;
import android.text.InputType;
import android.util.AttributeSet;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.view.inputmethod.BaseInputConnection;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputConnection;
import android.view.inputmethod.InputMethodManager;

import com.sereno.vfv.MainActivity;
import com.sereno.vfv.R;

import java.util.ArrayList;
import java.util.Timer;
import java.util.TimerTask;

public class AnnotationView extends View implements AnnotationData.IAnnotationDataListener, AnnotationStroke.IAnnotationStrokeListener, AnnotationText.IAnnotationTextListener
{
    private static final int TEXT_TIMER=1000;

    /** The internal data of the annotation view*/
    private AnnotationData m_model = new AnnotationData();

    /** The paint object used to draw strokes on screen*/
    private Paint m_strokePaint = new Paint();

    /** The paint object used to draw texts on screen*/
    private Paint m_textPaint = new Paint();

    /** The timer used to draw the cursor on screen*/
    private Timer m_textTimer = null;

    /** The text handler message to draw text on UI thread*/
    private Handler m_textHandler = new Handler();

    /** Should we draw the text cursor? Works only on Text mode*/
    private boolean m_drawTextCursor = false;

    public AnnotationView(Context context)
    {
        super(context);
        init(null);
    }

    public AnnotationView(Context context, @Nullable AttributeSet attrs)
    {
        super(context, attrs);
        init(attrs);
    }

    public AnnotationView(Context context, @Nullable AttributeSet attrs, int defStyleAttr)
    {
        super(context, attrs, defStyleAttr);
        init(attrs);
    }

    public AnnotationView(Context context, @Nullable AttributeSet attrs, int defStyleAttr, int defStyleRes)
    {
        super(context, attrs, defStyleAttr, defStyleRes);
        init(attrs);
    }

    /** Initialize the annotation view internal state*/
    private void init(AttributeSet attrs)
    {
        setFocusable(true);
        setFocusableInTouchMode(true);
        m_model.addListener(this);

        TypedArray ta = getContext().obtainStyledAttributes(attrs, R.styleable.AnnotationView);
        int fontSize = ta.getInt(R.styleable.AnnotationView_fontSize, -1);
        if(fontSize != -1)
            m_textPaint.setTextSize(fontSize);
        ta.recycle();

        m_strokePaint.setStyle(Paint.Style.STROKE);
    }

    @Override
    public void onDraw(Canvas canvas)
    {
        if(m_model == null)
            return;

        //Draw the strokes
        for(AnnotationStroke s : m_model.getStrokes())
        {
            //Parameterize the paint
            m_strokePaint.setColor(s.getColor());
            m_strokePaint.setStrokeWidth(s.getWidth());

            //Draw the path
            Path path = new Path();
            ArrayList<Point> points = s.getPoints();

            if(points.size() > 0)
                path.moveTo(points.get(0).x, points.get(0).y);
            for(int i = 1; i < points.size(); i++)
                path.lineTo(points.get(i).x, points.get(i).y);

            canvas.drawPath(path, m_strokePaint);
        }

        //Draw the texts
        for(AnnotationText t : m_model.getTexts())
        {
            int y = t.getPosition().y;
            for (String line: t.getText().split("\n"))
            {
                canvas.drawText(line, t.getPosition().x, y, m_textPaint);
                y += m_textPaint.descent() - m_textPaint.ascent();
            }
        }
    }

    @Override
    public boolean onTouchEvent(MotionEvent e)
    {
        super.onTouchEvent(e);

        if(m_model == null)
            return false;

        if(m_model.getMode() == AnnotationData.AnnotationMode.STROKE)
        {
            boolean addStrokePoint = false;
            if(e.getAction() == MotionEvent.ACTION_DOWN)
            {
                addStrokePoint = true;
                m_model.addStroke(new AnnotationStroke());
            }

            //Just to tell that we can modify the stroke
            else if(e.getAction() == MotionEvent.ACTION_MOVE)
                addStrokePoint = true;

            //Add the event point. The callback listeners will invalidate this view
            if(addStrokePoint && m_model.getStrokes().size() > 0)
            {
                AnnotationStroke s = m_model.getStrokes().get(m_model.getStrokes().size()-1);
                s.addPoint(new Point((int)e.getX(), (int)e.getY()));

                return true;
            }
        }

        else if(m_model.getMode() == AnnotationData.AnnotationMode.TEXT)
        {
            if(e.getAction() == MotionEvent.ACTION_DOWN)
            {
                AnnotationText t = new AnnotationText();
                t.setPosition(new Point((int)e.getX(), (int)e.getY()));
                t.addListener(this);
                m_model.addText(t);
                InputMethodManager imm = (InputMethodManager)getContext().getSystemService(Context.INPUT_METHOD_SERVICE);
                requestFocus();
                requestFocusFromTouch();
                if(imm != null)
                    imm.showSoftInput(this, 0);
                return true;
            }
        }

        return false;
    }

    /** Set the AnnotationData model
     * @param model the new AnnotationData model*/
    public void setModel(AnnotationData model)
    {
        if(m_model != null)
            m_model.removeListener(this);
        m_model = model;
        if(m_model != null)
        {
            m_model.addListener(this);
            checkMode(m_model.getMode());
        }
        else
        {
            if(m_textTimer != null)
                m_textTimer.cancel();
            m_drawTextCursor = false;
        }
        invalidate();
    }

    @Override
    public InputConnection onCreateInputConnection(EditorInfo outAttrs)
    {
        BaseInputConnection fic = new BaseInputConnection(this, false);
        outAttrs.inputType = InputType.TYPE_NULL;
        return fic;
    }

    @Override
    public boolean onKeyUp(int code, KeyEvent event)
    {
        if(m_model.getMode() == AnnotationData.AnnotationMode.TEXT)
        {
            ArrayList<AnnotationText> texts = m_model.getTexts();
            texts.get(texts.size()-1).addKey(code, event.getUnicodeChar());
            return true;
        }
        return false;
    }

    /** Get the AnnotationData model
     * @return the AnnotationData model*/
    public AnnotationData getModel()
    {
        return m_model;
    }

    /** Check the current mode to handle specific annimations
     * @param mode the mode to apply*/
    private void checkMode(AnnotationData.AnnotationMode mode)
    {
        if(mode == AnnotationData.AnnotationMode.TEXT)
        {
            m_textTimer = new Timer();
            m_textTimer.schedule(new TimerTask() {
                @Override
                public void run() {
                    m_textHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            m_drawTextCursor = !m_drawTextCursor;
                            invalidate();
                        }
                    });
                }
            }, TEXT_TIMER);
        }
        else if(m_textTimer != null)
            m_textTimer.cancel();
    }

    @Override
    public void onAddStroke(AnnotationData data, AnnotationStroke stroke)
    {
        stroke.addListener(this);
        invalidate();
    }

    @Override
    public void onAddText(AnnotationData data, AnnotationText text) {
        invalidate();
    }

    @Override
    public void onAddImage(AnnotationData data)
    {
        invalidate();
    }

    @Override
    public void onSetMode(AnnotationData data, AnnotationData.AnnotationMode mode)
    {
        checkMode(mode);
    }

    @Override
    public void onAddPoint(AnnotationStroke stroke, Point p)
    {
        invalidate();
    }

    @Override
    public void onSetColor(AnnotationStroke stroke, int c)
    {
        invalidate();
    }

    @Override
    public void onSetWidth(AnnotationStroke stroke, float w)
    {
        invalidate();
    }

    @Override
    public void onSetText(AnnotationText text, String str) {
        invalidate();
    }

    @Override
    public void onSetPosition(AnnotationText text, Point pos) {
        invalidate();
    }
}
