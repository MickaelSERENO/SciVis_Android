package com.sereno.view;

import android.graphics.Point;
import android.view.KeyEvent;

import java.util.ArrayList;

/** Class representing a text annotation*/
public class AnnotationText
{
    /** Listener containing methods to call when the internal state of the AnnotationText object changes*/
    public interface IAnnotationTextListener
    {
        /** Method called when the text of this annotation changes
         * @param text the Annotation being modified
         * @param str the new annotation string*/
        void onSetText(AnnotationText text, String str);

        /** Method called when the position of this annotation changes
         * @param text the Annotation being modified
         * @param pos the new position to apply*/
        void onSetPosition(AnnotationText text, Point pos);
    }

    /** The annotation string*/
    private String m_text = "";

    /**The annotation listeners*/
    private Point  m_pos  = new Point(0, 0);

    /**The cursor position*/
    private int m_cursorPos = 0;

    /** The listeners to call when the current state of the annotations changed*/
    private ArrayList<IAnnotationTextListener> m_listeners = new ArrayList<>();

    /** Default constructor*/
    public AnnotationText()
    {}

    /** Add a new listener to call if not already registered
     * @param l the new listener to add*/
    public void addListener(IAnnotationTextListener l)
    {
        if(!m_listeners.contains(l))
            m_listeners.add(l);
    }

    /** remove an already registered listener
     * @param l the listener to remove*/
    public void removeListener(IAnnotationTextListener l)
    {
        m_listeners.remove(l);
    }

    /** Set the current string of this annotation
     * @param txt the new string to apply*/
    public void setText(String txt)
    {
        m_text = txt;
        for(IAnnotationTextListener l : m_listeners)
            l.onSetText(this, txt);
    }

    /** Get the current text of this annotation
     * @return the current annotation text*/
    public String getText()
    {
        return m_text;
    }

    /** Set the current position of this annotation
     * @param pos the new position to apply*/
    public void setPosition(Point pos)
    {
        m_pos = pos;
        for(IAnnotationTextListener l : m_listeners)
            l.onSetPosition(this, pos);
    }

    /** Get the current position of this annotation
     * @return the current annotation position*/
    public Point getPosition()
    {
        return m_pos;
    }

    public void addKey(int keycode, int unicode)
    {
        if(unicode == 0)
        {
            if(keycode == KeyEvent.KEYCODE_DEL && m_cursorPos > 0 && m_text.length() > m_cursorPos)
            {
                m_text = m_text.substring(0, m_cursorPos) + m_text.substring(m_cursorPos);
                m_cursorPos--;
            }
        }
        else
        {
            m_text = m_text.substring(0, m_cursorPos) + Character.toChars(unicode) + (m_cursorPos == 0 ? m_text.substring(m_cursorPos) : m_text.substring(m_cursorPos-1));
            m_cursorPos++;
        }

        for(IAnnotationTextListener l : m_listeners)
            l.onSetText(this, m_text);
    }

    public void setCursorPosition(int pos)
    {
        m_cursorPos = pos;
    }

    public int getCursorPosition()
    {
        return m_cursorPos;
    }
}
