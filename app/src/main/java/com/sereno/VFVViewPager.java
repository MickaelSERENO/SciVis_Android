package com.sereno;

import android.support.v4.view.ViewPager;

import android.content.Context;
import android.util.AttributeSet;
import android.view.MotionEvent;

public class VFVViewPager extends ViewPager
{
    private boolean m_enabled=true;

    public VFVViewPager(Context context, AttributeSet attrs)
    {
        super(context, attrs);
    }

    public VFVViewPager(Context context)
    {
        super(context);
    }

    @Override
    public boolean onTouchEvent(MotionEvent event)
    {
        return m_enabled && super.onTouchEvent(event);
    }

    @Override
    public boolean onInterceptTouchEvent(MotionEvent event)
    {
        return m_enabled && super.onInterceptTouchEvent(event);
    }

    public void setPagingEnabled(boolean enabled)
    {
        m_enabled = enabled;
    }

    public boolean isPagingEnabled()
    {
        return m_enabled;
    }
}