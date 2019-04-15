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

    private float m_minValue     = 0.0f; /*!< The current minimum value (between 0 and 1)*/
    private float m_maxValue     = 1.0f; /*!< The current maximum value (between 0 and 1)*/

    private ArrayList<IOnRangeChangeListener> m_onRangeChangeListeners = new ArrayList<>(); /*!< Listeners to call when the range has changed*/

    /** Set the color mode of this View
     * @param mode the color mode (See ColorMode class)*/
    public void setColorMode(int mode)
    {
        m_colorMode = mode;
        launchEvent();
    }

    /** Set the range color. If min > max, we invert the value
     * @param min the minimum range. Will be clamped between 0.0f and 1.0f.
     * @param max the maximum range. Will be clamped between 0.0f and 1.0f.
     */
    public void setRange(float min, float max)
    {
        m_minValue = Math.min(min, max);
        m_maxValue = Math.max(min, max);
        m_minValue = Math.min(Math.max(0.0f, m_minValue), 1.0f);
        m_maxValue = Math.min(Math.max(0.0f, m_maxValue), 1.0f);

        launchEvent();
    }

    /** Launch the events of the range color changement*/
    private void launchEvent()
    {
        float min       = m_minValue;
        float max       = m_maxValue;
        int   colorMode = m_colorMode;
        for(int i = 0; i < m_onRangeChangeListeners.size(); i++)
            m_onRangeChangeListeners.get(i).onRangeChange(this, min, max, colorMode);
    }

    /** Get the minimum range
     * return the minimum range (between 0.0f and 1.0f)*/
    public float getMinRange()
    {
        return m_minValue;
    }

    public float getMaxRange()
    {
        return m_maxValue;
    }

    public int getColorMode()
    {
        return m_colorMode;
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
