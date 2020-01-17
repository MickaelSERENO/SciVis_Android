package com.sereno.view;

import com.sereno.color.ColorMode;

import java.util.ArrayList;

/** The RangeColor Data model*/
public class RangeColorData
{
    /* \brief Interface permitting to send a message when the range color has changed*/
    public interface IOnRangeChangeListener
    {
        /** Function called when the range has changed
         * @param data the data calling this method
         * @param minVal the current minimum value (between 0.0 and 1.0)
         * @param maxVal the current maximum value (between 0.0 and 1.0)
         * @param mode the color mode applied*/
        void onRangeChange(RangeColorData data, float minVal, float maxVal, int mode);
    }


    private int   m_colorMode    = ColorMode.RAINBOW; /*!< The colormode to display*/

    private float m_minRangeValue = 0.0f; /*!< The current minimum value (between 0 and 1)*/
    private float m_maxRangeValue = 1.0f; /*!< The current maximum value (between 0 and 1)*/
    private float m_minRawValue   = 0.0f; /*!< The raw values minimum range*/
    private float m_maxRawValue   = 0.0f; /*!< The raw values maximum range*/

    private ArrayList<IOnRangeChangeListener> m_onRangeChangeListeners = new ArrayList<>(); /*!< Listeners to call when the range has changed*/

    /** Set the color mode of this View
     * @param mode the color mode (See ColorMode class)*/
    public void setColorMode(int mode)
    {
        if(mode != m_colorMode)
        {
            m_colorMode = mode;
            launchEvent();
        }
    }

    /** Set the range clamping values in percentage. If min > max, we invert the value
     * @param min the minimum range. Will be clamped between 0.0f and 1.0f.
     * @param max the maximum range. Will be clamped between 0.0f and 1.0f.
     */
    public void setClampingRange(float min, float max)
    {

        float tempMin = min;
        min = Math.min(min, max);
        max = Math.max(tempMin, max);

        boolean changed = false;
        if(min != m_minRawValue || max != m_maxRawValue)
            changed=true;

        m_minRangeValue = Math.min(Math.max(0.0f, min), 1.0f);
        m_maxRangeValue = Math.min(Math.max(0.0f, max), 1.0f);

        if(changed)
            launchEvent();
    }

    /** Launch the events of the range color changement*/
    private void launchEvent()
    {
        float min       = m_minRangeValue;
        float max       = m_maxRangeValue;
        int   colorMode = m_colorMode;
        for(int i = 0; i < m_onRangeChangeListeners.size(); i++)
            m_onRangeChangeListeners.get(i).onRangeChange(this, min, max, colorMode);
    }

    /** Get the minimum clamping range
     * @return the minimum clamping range (between 0.0f and 1.0f)*/
    public float getMinClampingRange()
    {
        return m_minRangeValue;
    }

    /** Get the maximum clamping range
     * @return the maximum clamping range (between 0.0f and 1.0f)*/
    public float getMaxClampingRange()
    {
        return m_maxRangeValue;
    }

    /** Get the color mode to apply
     * @return the color mode to apply. See ColorMode enum*/
    public int getColorMode()
    {
        return m_colorMode;
    }

    /** Set the range raw values of the data (i.e., the ranges that users can understand, such as "min == 0.0kg, max == 100.0kg"
     * @param min the minimum range.
     * @param max the maximum range.
     * @param updateRange Should the function recompute the percentage selective ranges entered by "setRange"?*/
    public void setRawRange(float min, float max, boolean updateRange)
    {
        float tempMin = min;
        min = Math.min(min, max);
        max = Math.max(tempMin, max);

        boolean changed = false;
        if(min != m_minRawValue || max != m_maxRawValue)
            changed=true;

        if(updateRange)
        {
            //Get raw clamping values
            float minClamping = m_minRangeValue * min;
            float maxClamping = m_maxRangeValue * max;

            //Update our values
            m_minRawValue = min;
            m_maxRawValue = max;

            //Recompute the range
            m_maxRangeValue = (maxClamping-m_minRangeValue)/(m_maxRawValue - m_minRangeValue);
            m_minRangeValue = (minClamping-m_minRangeValue)/(m_maxRawValue - m_minRangeValue);

            //Clamp again
            m_minRangeValue = Math.min(Math.max(0.0f, m_minRangeValue), 1.0f);
            m_maxRangeValue = Math.min(Math.max(0.0f, m_maxRangeValue), 1.0f);
        }

        if(!updateRange)
        {
            m_minRawValue = Math.min(min, max);
            m_maxRawValue = Math.max(min, max);
        }

        if(changed)
            launchEvent();
    }

    /** Get the range raw values of the data (i.e., the ranges that users can understand, such as "min == 0.0kg"
     * @return the minimum range value*/
    public float getMinRawRange()
    {
        return m_minRawValue;
    }

    /** Get the range raw values of the data (i.e., the ranges that users can understand, such as "min == 0.0kg"
     * @return the maximum range value*/
    public float getMaxRawRange()
    {
        return m_maxRawValue;
    }

    /** Add an object to the list of listeners to call when the range color has changed
     * @param l the new listener to add*/
    public void addOnRangeChangeListener(IOnRangeChangeListener l)
    {
        m_onRangeChangeListeners.add(l);
    }

    /** \brief Remove an existing object to the list of listeners to call when the range color has changed
     * @param l the old listener to remove*/
    public void removeOnRangeChangeListener(IOnRangeChangeListener l)
    {
        m_onRangeChangeListeners.remove(l);
    }
}
