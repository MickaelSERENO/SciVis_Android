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

        /** Method called when the color of this annotation changes
         * @param text the Annotation being modified
         * @param color the new color to apply*/
        void onSetColor(AnnotationText text, int color);
    }

    /** The annotation string*/
    private String m_text = "";

    /**The annotation listeners*/
    private Point  m_pos  = new Point(0, 0);

    /**The cursor position*/
    private int m_cursorPos = -1;

    /**The text color*/
    private int m_color = 0xff000000;

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

    /** Add a new key entry in the text
     * @param keycode the key code
     * @param unicode the UTF8 unichar code*/
    public void addKey(int keycode, int unicode)
    {
        String strAdd = null;
        if(unicode == 0)
        {

            if(keycode == KeyEvent.KEYCODE_DEL && m_cursorPos >= 0 && m_text.length() > m_cursorPos)
            {
                m_text = m_text.substring(0, m_cursorPos) + m_text.substring(m_cursorPos, m_text.length()-1);
                m_cursorPos--;
            }
            else if(keycode == KeyEvent.KEYCODE_ENTER)
                strAdd = "\n";
        }

        else
            strAdd = new String(Character.toChars(unicode));

        if(strAdd != null)
        {
            if(m_cursorPos == -1)
                m_text = strAdd;
            else if(m_text.length()-1 >= 0)
                m_text = m_text.substring(0, m_cursorPos+1) + strAdd + m_text.substring(m_cursorPos, m_text.length()-1);
            else
                m_text = m_text.substring(0, m_cursorPos+1) + strAdd;
            m_cursorPos+=strAdd.length();
        }

        for(IAnnotationTextListener l : m_listeners)
            l.onSetText(this, m_text);
    }

    /** get the annotation text color
     * @return the text color*/
    public int getColor()
    {
        return m_color;
    }

    /** Set the annotation text color
     * @param color the new text color*/
    public void setColor(int color)
    {
        m_color = color;
        for(IAnnotationTextListener l : m_listeners)
            l.onSetColor(this, color);
    }

    /** Set the current cursor position
     * @param pos the new cursor position*/
    public void setCursorPosition(int pos)
    {
        m_cursorPos = pos;
    }

    /** Get the current cursor position
     * @return the current cursor position*/
    public int getCursorPosition()
    {
        return m_cursorPos;
    }
}
