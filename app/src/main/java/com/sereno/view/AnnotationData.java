package com.sereno.view;

import android.graphics.Color;
import android.graphics.Point;

import java.lang.annotation.Annotation;
import java.util.ArrayList;

public class AnnotationData
{
    /** Listener of the class AnnotationData*/
    public interface IAnnotationDataListener
    {
        /** Method called when a new stroke has been added
         * @param data the AnnotationData firing this call
         * @param stroke  the stroke added*/
        void onAddStroke(AnnotationData data, AnnotationStroke stroke);

        /** Method called when a new text has been added
         * @param data the AnnotationData firing this call
         * @param stroke  the stroke added*/
        void onAddText(AnnotationData data, AnnotationText text);

        void onAddImage(AnnotationData data);

        /** Methode called when the annotation mode changed
         * @param data the AnnotationData firing this call
         * @param mode the new mode*/
        void onSetMode(AnnotationData data, AnnotationMode mode);
    }

    /** Enum listing the different annotation mode available*/
    public enum AnnotationMode
    {
        /** Strokes*/
        STROKE,
        /** Text*/
        TEXT,
        /** Image*/
        IMAGE
    }

    private int m_width;
    private int m_height;

    /** The current annotation mode*/
    private AnnotationMode m_mode = AnnotationMode.STROKE;

    /** List of strokes*/
    private ArrayList<AnnotationStroke> m_strokes = new ArrayList<>();

    /** List of texts*/
    private ArrayList<AnnotationText> m_texts = new ArrayList<>();

    /** The listeners to call when the current state of the annotations changed*/
    private ArrayList<IAnnotationDataListener> m_listeners = new ArrayList<>();

    /** Constructor
     * @param width  the texture width where this annotation belongs to
     * @param height the texture height where this annotation belongs to*/
    public AnnotationData(int width, int height)
    {
        m_width  = width;
        m_height = height;
    }

    /** Add a new listener to call if not already registered
     * @param l the new listener to add*/
    public void addListener(IAnnotationDataListener l)
    {
        if(!m_listeners.contains(l))
            m_listeners.add(l);
    }

    /** remove an already registered listener
     * @param l the listener to remove*/
    public void removeListener(IAnnotationDataListener l)
    {
        m_listeners.remove(l);
    }

    /** Get the texture width where this annotation belongs to
     * @return the texture width*/
    public int getWidth()
    {
        return m_width;
    }

    /** Get the texture height where this annotation belongs to
     * @return the texture height*/
    public int getHeight()
    {
        return m_height;
    }

    /** Get the current annotation mode
     * @return the current annotation mode*/
    public AnnotationMode getMode()
    {
        return m_mode;
    }

    /** Set the current annotation mode
     * @param mode the current annotation mode*/
    public void setMode(AnnotationMode mode)
    {
        m_mode = mode;
        for(int i = 0; i < m_listeners.size(); i++)
            m_listeners.get(i).onSetMode(this, mode);
    }

    /** Add a new stroke to the annotation stroke list
     * @param s the new stroke to add*/
    public void addStroke(AnnotationStroke s)
    {
        m_strokes.add(s);
        for(int i = 0; i < m_listeners.size(); i++)
            m_listeners.get(i).onAddStroke(this, s);
    }

    public void addText(AnnotationText t)
    {
        m_texts.add(t);
        for(int i = 0; i < m_listeners.size(); i++)
            m_listeners.get(i).onAddText(this, t);
    }

    /** Get the list of strokes. Do not modify the list!
     * @return the list of strokes. Do not modify the list! (but each AnnotationStroke can be)*/
    public ArrayList<AnnotationStroke> getStrokes()
    {
        return m_strokes;
    }

    /** Get the list of texts. Do not modify the list!
     * @return the list of texts. Do not modify the list! (but each AnnotationText can be)*/
    public ArrayList<AnnotationText> getTexts() {return m_texts;}
}
