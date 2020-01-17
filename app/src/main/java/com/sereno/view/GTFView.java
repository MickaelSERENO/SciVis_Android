package com.sereno.view;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Canvas;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.Path;
import android.graphics.Point;
import android.graphics.PointF;
import android.icu.util.Measure;
import android.os.Handler;
import android.os.Looper;
import android.support.annotation.Nullable;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;

import com.sereno.color.ColorMode;
import com.sereno.vfv.Data.CPCPTexture;
import com.sereno.vfv.Data.PointFieldDesc;
import com.sereno.vfv.Data.SubDataset;
import com.sereno.vfv.R;

import java.util.HashMap;

/** Class proposing a way to manipulate Gaussian Transfer Function*/
public class GTFView extends View implements GTFData.IGTFDataListener
{
    public static final int HANDLE_HEIGHT      = 30;  /*!< The height of handles*/
    public static final int TEXT_SIZE          = 24;  /*!< The default text height*/
    public static final int TEXT_OFFSET_HANDLE = 5;   /*!< The offset applied to the handle's text in CPCP mode*/
    public static final int COLLISION_WIDTH    = 30;  /*!< The number of pixels allowed for collisions with CPCP*/
    public static final int MAX_1D_HEIGHT      = 150; /*!< The maximum 1D height in pixel*/
    public static final int MAX_2D_HEIGHT      = 920; /*!< The maximum 1D height in pixel*/

    public static final int MANIPULATING_NO_VALUE  = 0; /*!< Manipulating nothing (no touch)*/
    public static final int MANIPULATING_CENTER    = 1; /*!< Manipulating the center*/
    public static final int MANIPULATING_SCALE     = 2; /*!< Manipulating the scale factor*/
    public static final int MANIPULATING_PC_HEADER = 3; /*!< Manipulating the PC header (the PC order)*/

    private Paint    m_paint         = new Paint();           /*!< The general paint used of the canvas*/
    private Paint    m_pcStrokePaint = new Paint();           /*!< The paint used to draw the lines for the Parallel Coordinates*/
    private Paint    m_textPaint     = new Paint();           /*!< The paint applied for text */
    private Paint    m_handlesPaint  = new Paint();           /*!< The paint object permitting to draw the handlers*/
    private GTFData  m_model         = new GTFData(null); /*!< The model of this View*/
    private int      m_sliderHeight  = HANDLE_HEIGHT;         /*!< The handle Height*/

    private int m_valueInManipulation = MANIPULATING_NO_VALUE; /*!< What is the current handle being manipulated (i.e moved) ?*/
    private int m_pcInManipulation    = -1;                    /*!< The parallel coordinate ID in manipulation*/

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
        setMinimumHeight(0);
        setMinimumWidth(0);

        setFocusable(true);
        setFocusableInTouchMode(true);
        setClickable(true);

        m_model.addListener(this);

        TypedArray taGTF  = getContext().obtainStyledAttributes(attrs, R.styleable.GTFView);
        TypedArray taText = getContext().obtainStyledAttributes(attrs, R.styleable.TextView);

        m_textPaint.setTextSize(taText.getDimensionPixelSize(R.styleable.TextView_textSize, TEXT_SIZE));
        m_textPaint.setTextAlign(Paint.Align.CENTER);
        m_sliderHeight = taGTF.getDimensionPixelSize(R.styleable.GTFView_sliderDim, HANDLE_HEIGHT);
        m_pcStrokePaint.setStrokeWidth(taGTF.getDimensionPixelSize(R.styleable.GTFView_pcStrokeWidth, 0)); //Default: hairline
    }

    /** Is the attached model valid?
     * @return true if yes, false otherwise*/
    private boolean isModelInvalid()
    {
        return m_model == null || m_model.getDataset() == null || !m_model.getDataset().getParent().isLoaded() || m_model.getCPCPOrder().length == 0;
    }

    /** Compute the displayed text height (using descent and ascent)
     * @return the floating point text height based on m_textPaint*/
    private float computeTextHeight()
    {
        return m_textPaint.getFontMetrics().descent - m_textPaint.getFontMetrics().ascent;
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

        /* Try to apply MAX_PIXELS in y axis if relevant data should be shown*/
        switch(heightMode)
        {
            case MeasureSpec.AT_MOST:
                if(isModelInvalid() || m_model.getCPCPOrder().length==0)
                    height = 0;
                else if(m_model.getCPCPOrder().length == 1)
                    height = Math.min(heightSize, MAX_1D_HEIGHT);
                else
                    height = Math.min(heightSize, MAX_2D_HEIGHT);
                break;

            case MeasureSpec.UNSPECIFIED:
                if(isModelInvalid() || m_model.getCPCPOrder().length==0)
                    height = 0;
                else if(m_model.getCPCPOrder().length > 1)
                    height = MAX_2D_HEIGHT;
                else
                    height = MAX_1D_HEIGHT;
                break;
        }

        setMeasuredDimension(width, height);
    }

    /** Get the bitmap height in the cpcp mode
     * @return the bitmap (i.e., the texture of the CPCP) height in pixels*/
    private float getBitmapHeightCPCPMode()
    {
        float textHeight = computeTextHeight();
        return getHeight()-3*textHeight - m_sliderHeight;
    }

    /** Get the starting Y position of the CPCP bitmaps drawing
     * @return the y offset in pixels*/
    private float getBitmapStartY()
    {
        return 2*computeTextHeight() + m_sliderHeight/2.0f;
    }

    /** Get the X position of the CPCP bitmaps drawing
     * @return the x offset in pixels*/
    private float getBitmapStartX()
    {
        int[] order = m_model.getCPCPOrder();
        PointFieldDesc[] descs = m_model.getDataset().getParent().getPointFieldDescs();

        PointFieldDesc firstDesc = descs[order[0]];
        float firstDescWidth = Math.max(Math.max(m_textPaint.measureText(Float.toString(firstDesc.getMax())),
                                                 m_textPaint.measureText(Float.toString(firstDesc.getMin()))),
                                        m_textPaint.measureText(firstDesc.getName()))/2.0f;

        return firstDescWidth;
    }

    /** Get the "bitmap" width window
     * @return the total width allowed for CPCP texture only (stitched together) in pixels*/
    private float getBitmapWidth()
    {
        int[] order = m_model.getCPCPOrder();
        PointFieldDesc[] descs = m_model.getDataset().getParent().getPointFieldDescs();

        //Measure the text to have space to display it
        float firstDescWidth = getBitmapStartX();
        PointFieldDesc lastDesc = descs[order[order.length-1]];
        float lastDescWidth = Math.max(Math.max(m_textPaint.measureText(Float.toString(lastDesc.getMax())),
                                                m_textPaint.measureText(Float.toString(lastDesc.getMin()))),
                                       m_textPaint.measureText(lastDesc.getName()))/2.0f;

        return getWidth() - firstDescWidth - lastDescWidth;
    }

    /** Get the stroke width used to draw the PC lines
     * @return the stroke width in pixels*/
    private float getStrokeWidth()
    {
        float strokeWidth = m_paint.getStrokeWidth(); //Used to know the gap between CPCP texture
        if(strokeWidth == 0) //TODO hairline mode. In fact we should look at the canvas matrix, but... maybe too much
            strokeWidth = 1;
        return strokeWidth;
    }

    /** Get the texture width used for EACH CPCP
     * @return the texture width in pixels*/
    private float getOneTextureWidth()
    {
        return (getBitmapWidth() - getStrokeWidth() * m_model.getCPCPOrder().length) / (m_model.getCPCPOrder().length-1);
    }

    /** Draw the Continuous Parallel Coordinate Plot
     * @param canvas the canvas where to draw*/
    private void drawCPCP(Canvas canvas)
    {
        int[] order = m_model.getCPCPOrder();

        //Draw the CPCP in the correct order.
        float width  = getBitmapWidth();
        float height = getHeight();

        //Drawing related width and height
        float strokeWidth  = getStrokeWidth();
        float textHeight   = computeTextHeight();
        float bitmapHeight = getBitmapHeightCPCPMode();
        float startBitmapY = getBitmapStartY(); //Where does start the bitmap rendering
        float bitmapWidth  = getOneTextureWidth();

        float bitmapStartX = getBitmapStartX();

        //Draw all the images
        for(int i = 0; i < order.length-1; i++)
        {
            float x           = (i+1)*strokeWidth + bitmapWidth*i + strokeWidth + bitmapStartX;

            CPCPTexture tex = m_model.getDataset().getParent().findCPCPTexture(order[i], order[i+1]);
            if(tex != null)
            {
                //Determine the transformation matrix to apply
                Matrix mat = new Matrix();
                float scaleX = bitmapWidth / tex.getBitmap().getWidth();
                float scaleY = bitmapHeight / tex.getBitmap().getHeight();
                if(tex.getPIDLeft() != order[i]) //Reversed texture
                    scaleX*=-1;
                mat.setScale(scaleX, scaleY);
                mat.postTranslate(x, startBitmapY);

                canvas.drawBitmap(tex.getBitmap(), mat, m_paint);
            }
        }

        //Draw the sliders
        for(int i = 0; i < order.length; i++)
        {
            float x = i*strokeWidth + bitmapWidth*i + bitmapStartX;

            //Draw the line
            canvas.drawLine(x, startBitmapY, x, startBitmapY+bitmapHeight, m_pcStrokePaint);
            for(PointFieldDesc desc : m_model.getDataset().getParent().getPointFieldDescs())
                if(desc.getID() == order[i])
                {
                    //prop name
                    String text = desc.getName();
                    canvas.drawText(text, x, textHeight-m_textPaint.getFontMetrics().descent, m_textPaint);

                    //Max
                    text = Float.toString(desc.getMax());
                    canvas.drawText(text, x, 2*textHeight-m_textPaint.getFontMetrics().descent, m_textPaint);

                    //Min
                    text = Float.toString(desc.getMin());
                    canvas.drawText(text, x, height-m_textPaint.getFontMetrics().descent, m_textPaint);

                    //The handles
                    GTFData.GTFPoint pointData = m_model.getRanges().get(order[i]);

                    float handleY = bitmapHeight - pointData.center*bitmapHeight + startBitmapY;

                    //Graphical handle
                    Path path = new Path();

                    float xMultiplier = (i == 0 ? -1 : 1);
                    {
                        path.moveTo(x-m_sliderHeight*xMultiplier, handleY - m_sliderHeight/2.0f);
                        path.lineTo(x, handleY);
                        path.lineTo(x-m_sliderHeight*xMultiplier, handleY + m_sliderHeight/2.0f);
                        path.close();
                    }
                    canvas.drawPath(path, m_handlesPaint);

                    float textOffset = TEXT_OFFSET_HANDLE;

                    //The associated text

                    //Update Paint Alignment for the text
                    if(i == order.length-1)
                    {
                        m_textPaint.setTextAlign(Paint.Align.RIGHT);
                        textOffset = -textOffset - m_sliderHeight;
                    }
                    else
                        m_textPaint.setTextAlign(Paint.Align.LEFT);

                    if(i == 0)
                        textOffset += m_sliderHeight;

                    canvas.drawText(Float.toString(pointData.center * (desc.getMax()-desc.getMin()) + desc.getMin()), x+textOffset, handleY - m_textPaint.getFontMetrics().ascent/2.0f, m_textPaint);

                    //Restore alignment alignment
                    m_textPaint.setTextAlign(Paint.Align.CENTER);

                    break;
                }
        }
    }

    /** Draw the 1D Histogram on screen
     * @param canvas the canvas where to draw*/
    private void draw1DHistogram(Canvas canvas)
    {
        int width  = getWidth();
        int height = getHeight();

        int[] order = m_model.getCPCPOrder();

        float textHeight = computeTextHeight();

        m_paint.setStyle(Paint.Style.FILL_AND_STROKE);

        height = (int)(height - 1 - HANDLE_HEIGHT*Math.sqrt(3.0f)/2.0f - textHeight);
        width = width - HANDLE_HEIGHT;

        float[] histo = m_model.getDataset().getParent().get1DHistogram(order[0]);
        if(histo == null)
            return;

        for (int i = 0; i < width/2; i++)
        {
            int histoID = (int)Math.floor(histo.length * (double)2*i/width);
            if(histoID > histo.length - 1)
                histoID = histo.length - 1;

            m_paint.setColor(ColorMode.computeRGBColor(histo[histoID], ColorMode.GRAYSCALE).toARGB8888());
            canvas.drawRect(2*i+m_sliderHeight/2, 0, 2*i+m_sliderHeight/2+1, height - 1, m_paint);
        }

        GTFData.GTFPoint pointData = m_model.getRanges().get(order[0]);

        //Draw the handles
        int   v = (int)(width*pointData.center);

        Path path = new Path();
        path.moveTo(v+ m_sliderHeight / 2.0f, height);
        path.lineTo(v, getHeight()-textHeight-1);
        path.lineTo(v + m_sliderHeight, getHeight()-textHeight-1);
        path.close();
        canvas.drawPath(path, m_handlesPaint);

        //Draw the text below the handles
        PointFieldDesc ptDesc = m_model.getDataset().getParent().getPointFieldDescs()[order[0]];
        for(int j = 0; j < 2; j++)
            canvas.drawText(Float.toString(pointData.center * (ptDesc.getMax()-ptDesc.getMin()) + ptDesc.getMin()), v+m_sliderHeight/2.0f, getHeight()-m_textPaint.getFontMetrics().descent, m_textPaint);
    }

    @Override
    public void onDraw(Canvas canvas)
    {
        super.onDraw(canvas);

        //Nothing to display in such cases
        if(isModelInvalid())
            return;

        int[] order = m_model.getCPCPOrder();

        //If order == 1 --> display 1D histogram
        if(order.length == 1)
            draw1DHistogram(canvas);
        else
            drawCPCP(canvas);
    }

    /** Function handling touch even when in histogram mode. This will mostly move the displayed handles
     * @param e the MotionEvent to handle.*/
    private boolean onTouchHistogram(MotionEvent e)
    {
        //Security check
        if(m_model.getCPCPOrder().length == 0)
            return false;

        int      width       = getWidth() - m_sliderHeight;
        int      x           = (int)e.getX();
        boolean  valueChanged = false;
        float    indice       = Math.min(Math.max((x - m_sliderHeight/2.0f)/width, 0.0f), 1.0f);

        float center = m_model.getRanges().get(m_model.getCPCPOrder()[0]).center;
        float scale  = m_model.getRanges().get(m_model.getCPCPOrder()[0]).scale;

        //Set the cursor and store which value we are manipulating
        if(e.getAction() == MotionEvent.ACTION_DOWN)
        {
            center = indice;
            m_valueInManipulation = MANIPULATING_CENTER;
            valueChanged = true;
        }
        else if(e.getAction() == MotionEvent.ACTION_UP)
            m_valueInManipulation = MANIPULATING_NO_VALUE;

        //Move the cursor
        else if(e.getAction() == MotionEvent.ACTION_MOVE)
        {
            if(m_valueInManipulation == MANIPULATING_CENTER)
                center = indice;
            valueChanged = true;
        }

        //TODO, handles scale

        if(valueChanged && m_model.getDataset().getCanBeModified())
            m_model.setRange(m_model.getCPCPOrder()[0], new GTFData.GTFPoint(center, scale));

        return true;
    }

    /** Function handling touch event with in CPCP use-case
     * @param e the MotionEvent received*/
    private boolean onTouchCPCP(MotionEvent e)
    {
        int[] order = m_model.getCPCPOrder();
        boolean valueChanged = false;

        float center = 0.0f;
        float scale = 0.0f;

        //Determining what is being manipulated
        if(e.getAction() == MotionEvent.ACTION_DOWN)
        {
            m_pcInManipulation = -1;

            //First, which PC is being manipulated?
            float minCollisionDistance = Float.MAX_VALUE;
            for(int i = 0; i < order.length; i++)
            {
                float x = i*(getStrokeWidth() + getOneTextureWidth()) + getBitmapStartX(); //The line X position
                if(e.getX() < x + COLLISION_WIDTH  && e.getX() > x - COLLISION_WIDTH &&    //Check bounding box collision
                   Math.abs(e.getX() - x) < minCollisionDistance)                          //Check distance
                {
                    m_pcInManipulation   = i;
                    minCollisionDistance = Math.abs(e.getX() - x);
                }
            }

            //Nothing to do if no PC has been selected
            if(m_pcInManipulation == -1)
                return true;

            //Second Y axis: Header or range?
            if (e.getY() < getBitmapStartY()) //Header
                m_valueInManipulation = MANIPULATING_PC_HEADER;

            else //Ranges
            {
                float indice   = 1.0f-Math.min(Math.max((e.getY() - getBitmapStartY())/getBitmapHeightCPCPMode(), 0.0f), 1.0f);
                center = m_model.getRanges().get(m_model.getCPCPOrder()[m_pcInManipulation]).center;
                scale  = m_model.getRanges().get(m_model.getCPCPOrder()[m_pcInManipulation]).scale;

                //Search whose value has been manipulated and update it accordingly
                m_valueInManipulation = MANIPULATING_CENTER;
                center = indice;

                //TODO, handle scale
                valueChanged = true;
            }
        }

        //Update PC header and ranges while moving
        else if(e.getAction() == MotionEvent.ACTION_MOVE && m_pcInManipulation != -1)
        {
            if(m_valueInManipulation == MANIPULATING_PC_HEADER)
            {
                //TODO, move the header
            }

            else if(m_valueInManipulation == MANIPULATING_CENTER)
            {
                float indice   = 1.0f-Math.min(Math.max((e.getY() - getBitmapStartY())/getBitmapHeightCPCPMode(), 0.0f), 1.0f);
                center = m_model.getRanges().get(m_model.getCPCPOrder()[m_pcInManipulation]).center;
                scale  = m_model.getRanges().get(m_model.getCPCPOrder()[m_pcInManipulation]).scale;

                if(m_valueInManipulation == MANIPULATING_CENTER)
                    center = indice;

                valueChanged = true;
            }
        }

        //Reinitialize things when in action down
        else if(e.getAction() == MotionEvent.ACTION_DOWN)
        {
            m_pcInManipulation    = -1;
            m_valueInManipulation = MANIPULATING_NO_VALUE;
        }

        if(valueChanged && m_model.getDataset().getCanBeModified())
            m_model.setRange(m_model.getCPCPOrder()[m_pcInManipulation], new GTFData.GTFPoint(center, scale));

        return true;
    }

    @Override
    public boolean onTouchEvent(MotionEvent e)
    {
        super.onTouchEvent(e);

        //Do nothing in such cases (nothing displayed)
        if(isModelInvalid())
            return false;

        if(m_model.getCPCPOrder().length == 1)
            return onTouchHistogram(e);

        return onTouchCPCP(e);
    }

    /** Set the new Model to use for this Widget. The view will be automatically updated
     * @param model the new GTF Model to use*/
    public void setModel(GTFData model)
    {
        if(m_model != null)
            m_model.removeListener(this);
        m_model = model;
        if(m_model != null)
            m_model.addListener(this);

        safeRequestLayout();
    }

    /** Get the current model used by this Widget
     * @return the current GTFData in use. The default GTFData displays nothing and needs to be updated as soon as possible*/
    public GTFData getModel()
    {
        return m_model;
    }

    @Override
    public void onSetDataset(GTFData model, SubDataset dataset)
    {
        safeRequestLayout();
    }

    @Override
    public void onSetGTFRanges(GTFData model, HashMap<Integer, GTFData.GTFPoint> ranges)
    {
        safeInvalidate();
    }

    @Override
    public void onSetCPCPOrder(GTFData model, int[] order)
    {
        safeRequestLayout();
    }

    @Override
    public void onSetColorMode(GTFData model, int colorMode)
    {
        safeInvalidate();
    }

    @Override
    public void onLoadDataset(GTFData model, SubDataset dataset)
    {
        safeRequestLayout();
    }

    /** Invalidate the View in a safe manner thread-wise. The function will check if the UI thread is the current thread or not and will call invalidate() or postInvalidate() accordingly*/
    private void safeInvalidate()
    {
        if(Looper.getMainLooper().getThread() == Thread.currentThread())
            invalidate();
        else
            postInvalidate();
    }

    /** Request a new layout in a safe manner thread-wise. The function will check if the UI thread is the current thread or not and will call requestLayout() or run a new Thread calling it in the UI thread*/
    private void safeRequestLayout()
    {
        if(Looper.getMainLooper().getThread() == Thread.currentThread())
        {
            requestLayout();
            invalidate();
        }
        else
            new Handler(Looper.getMainLooper()).post(new Runnable() {
                @Override
                public void run() {
                    // do UI work
                    requestLayout();
                    invalidate();
                }
            });
    }
}