package com.sereno.view;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Point;
import android.util.AttributeSet;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.sereno.Tree;
import com.sereno.vfv.R;

public class TreeView extends ViewGroup implements Tree.TreeListener<View>
{
    /** @brief Class representing the measure of the tree view*/
    private class TreeViewMeasureState
    {
        int width;  /*!< The current width*/
        int height; /*!< The current height*/
        int measureState; /*!< The state of the measure*/
        int widthMode;    /*!< What is the width mode of this layout?*/
        int heightMode;   /*!< What is the height mode of this layout?*/
        int maximumWidth; /*!< The width associated with the width mode*/
        int maximumHeight; /*!< The height associated with the height mode*/

        public Object clone()
        {
            TreeViewMeasureState s = new TreeViewMeasureState();
            s.width             = width;
            s.height            = height;
            s.measureState      = measureState;
            s.widthMode         = widthMode;
            s.heightMode        = heightMode;
            s.maximumWidth      = maximumWidth;
            s.maximumHeight     = maximumHeight;
            return s;
        }
    }

    /** @brief The left offset to apply per level in the tree view*/
    private int m_leftOffsetPerLevel = 0;

    /** @brief The top offset to apply per level in the tree view*/
    private int m_topOffsetPerChild  = 0;

    /** @brief The internal tree data*/
    private Tree<View> m_tree;

    private Paint m_paint;

    /** @brief Constructor with the view's context as @parameter
     * @param c the Context associated with the view*/
    public TreeView(Context c)
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
    public TreeView(Context c, AttributeSet a, int style)
    {
        super(c, a, style);
        init(a);
    }

    /** @brief Constructor with the view's context as @parameter and the XML data
     *
     * @param c the Context associated with the view
     * @param a the XML attributes of the view*/
    public TreeView(Context c, AttributeSet a)
    {
        super(c, a);
        init(a);
    }

    /** @brief Initialize the TreeView object*/
    public void init(AttributeSet a)
    {
        //Read the AttributeSet
        TypedArray ta = getContext().obtainStyledAttributes(a, R.styleable.TreeView);
        m_leftOffsetPerLevel = ta.getInt(R.styleable.TreeView_leftOffsetPerLevel, 10);
        m_topOffsetPerChild  = ta.getInt(R.styleable.TreeView_topOffsetPerChild,  15);
        ta.recycle();

        m_tree = new Tree<>(null);
        m_tree.addListener(this);
        setWillNotDraw(false);
        m_paint = new Paint();
    }

    @Override
    public MarginLayoutParams generateLayoutParams(AttributeSet attrs) {
        return new MarginLayoutParams(getContext(), attrs);
    }

    @Override
    protected MarginLayoutParams generateDefaultLayoutParams() {
        return new MarginLayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT);
    }

    @Override
    protected ViewGroup.LayoutParams generateLayoutParams(ViewGroup.LayoutParams p) {
        return new LayoutParams(p);
    }

    @Override
    protected boolean checkLayoutParams(ViewGroup.LayoutParams p) {
        return p instanceof LayoutParams;
    }

    /** @brief Measure each leaf size and aggregate the different leaf sizes
     * @param leftOffset the left offset for the current leaf
     * @param topOffset the top offset for the current leaf
     * @param state the layout state (current width, height and measure specifications)
     * @param t the current leaf to look at*/
    protected TreeViewMeasureState onMeasureLeaf(int leftOffset, int topOffset, TreeViewMeasureState state, Tree<View> t)
    {
        if(t.value != null)
        {
            int maxWidth  = Math.max(0, state.maximumWidth-leftOffset);
            int maxHeight = Math.max(0, state.maximumHeight-topOffset);

            measureChild(t.value, MeasureSpec.makeMeasureSpec(maxWidth,  state.widthMode), MeasureSpec.makeMeasureSpec(maxHeight, state.heightMode));
            state.height = Math.max(topOffset+t.value.getMeasuredHeight(), state.height);
            state.width  = Math.max(leftOffset+t.value.getMeasuredWidth(), state.width);

            topOffset  += t.value.getMeasuredHeight() + m_topOffsetPerChild;
            leftOffset += m_leftOffsetPerLevel;
            state.measureState = combineMeasuredStates(state.measureState, t.value.getMeasuredState());
        }

        for(Tree<View> l : t.getChildren())
            state = onMeasureLeaf(topOffset, leftOffset, state, l);

        return state;
    }

    @Override
    protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec)
    {
        //Define the state used for the leaves and retrieve the width/height of this Layout
        TreeViewMeasureState state = new TreeViewMeasureState();
        state.width  = getSuggestedMinimumWidth();
        state.height = getSuggestedMinimumHeight();
        state.measureState   = 0;
        state.widthMode      = MeasureSpec.getMode(widthMeasureSpec);
        state.maximumWidth   = Math.max(0, MeasureSpec.getSize(widthMeasureSpec) - getPaddingRight() - getPaddingLeft());
        state.heightMode     = MeasureSpec.getMode(heightMeasureSpec);
        state.maximumHeight  = Math.max(0, MeasureSpec.getSize(heightMeasureSpec) - getPaddingTop() - getPaddingBottom());

        if(state.widthMode == MeasureSpec.EXACTLY)
            state.widthMode = MeasureSpec.AT_MOST;
        if(state.heightMode == MeasureSpec.EXACTLY)
            state.heightMode = MeasureSpec.AT_MOST;
        onMeasureLeaf(0, 0, state, m_tree);
        setMeasuredDimension(resolveSizeAndState(state.width, widthMeasureSpec, state.measureState),
                             resolveSizeAndState(state.height, heightMeasureSpec, state.measureState << MEASURED_HEIGHT_STATE_SHIFT));
    }

    private int onLayoutLeaf(boolean b, int leftMargin, int topMargin,
                              int left, int top, int right, int bottom, Tree<View> leaf)
    {
        final View child = leaf.value;
        int height=0;
        if(child != null)
        {
            final int width = child.getMeasuredWidth();
            height = child.getMeasuredHeight();

            // These are the far left and right edges in which we are performing layout.
            int leftPos  = getPaddingLeft()+leftMargin;
            int rightPos = right - left - getPaddingRight();

            // These are the top and bottom edges in which we are performing layout.
            final int parentTop = getPaddingTop()+topMargin;
            final int parentBottom = bottom - top - getPaddingBottom();

            // Place the child.
            child.layout(leftPos, parentTop, Math.min(leftPos + width, rightPos), Math.min(parentTop + height, parentBottom));
        }


        for(Tree<View> l : leaf.getChildren())
            height += onLayoutLeaf(b, leftMargin + (child != null ? m_leftOffsetPerLevel : 0),
                                   topMargin + height + (child != null ? m_topOffsetPerChild : 0),
                                   left, top, right, bottom, l);
        return height;
    }

    @Override
    protected void onLayout(boolean b, int left, int top, int right, int bottom)
    {
        onLayoutLeaf(b, 0, 0, left,
                     top, right, bottom, m_tree);
    }

    /** @brief Dispatch the draw calls to the leaves. Do that only if the leaf is visible (or is null)
     * @param canvas the canvas
     * @param tree  the current leaf to look at*/
    public void dispatchDrawLeaf(Canvas canvas, Tree<View> tree)
    {
        if(tree.value == null || tree.value.getVisibility() == View.VISIBLE || tree.value.getVisibility() == View.INVISIBLE)
        {
            if (tree.value != null)
                tree.value.draw(canvas);
            for (Tree<View> t : tree.getChildren())
                dispatchDrawLeaf(canvas, t);
        }
    }

    @Override
    public void dispatchDraw(Canvas canvas)
    {
        dispatchDrawLeaf(canvas, m_tree);
    }

    @Override
    public void onDraw(Canvas canvas)
    {
        super.onDraw(canvas);
        m_paint.setColor(Color.GRAY);
        drawLeaf(canvas, m_tree);
    }

    public void drawLeaf(Canvas canvas, Tree<View> tree)
    {
        for(Tree<View> t : tree.getChildren())
        {
            if(tree.value != null && t.value != null)
            {
                canvas.drawLine(tree.value.getX() + 5, tree.value.getY() + tree.value.getHeight(),
                        tree.value.getX() + 5, t.value.getY() + tree.value.getHeight() / 2, m_paint);
                canvas.drawLine(tree.value.getX() + 5, t.value.getY() + tree.value.getHeight() / 2,
                        t.value.getX(), t.value.getY() + tree.value.getHeight() / 2, m_paint);
            }
            drawLeaf(canvas, t);
        }
    }

    @Override
    public void onAddChild(Tree<View> parent, Tree<View> child)
    {
        if(child.value != null)
            addView(child.value);
        child.addListener(this);
        for(Tree<View> v : child.getChildren())
        {
            if (v.value != null)
                addView(v.value);
            v.addListener(this);
        }
        invalidate();
    }

    @Override
    public void onRemoveChild(Tree<View> parent, Tree<View> child)
    {
        if(child.value != null)
            removeView(child.value);
        child.removeListener(this);
        for(Tree<View> v : child.getChildren())
        {
            if(v.value != null)
                removeView(v.value);
            v.removeListener(this);
        }
        invalidate();
    }

    public Tree<View> getData()
    {
        return m_tree;
    }
}