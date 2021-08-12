package com.sereno.view;

import java.util.ArrayList;

/** Data model of SeekBarGraduateView*/
public class SeekBarGraduatedData
{
    /** The Listener interface of SeekBarGraduated data*/
    public interface ISeekBarGraduatedListener
    {
        /** Event called when the number of steps (graduations) have changed
         * @param model the data model calling this method
         * @param nbSteps the new number of steps*/
        void onSetNbSteps(SeekBarGraduatedData model, int nbSteps);
    }

    /** The number of graduations*/
    private int m_steps = 2;

    /** Listeners to call on events*/
    private ArrayList<ISeekBarGraduatedListener> m_listeners = new ArrayList<>();

    public SeekBarGraduatedData(){}

    /** Add a new listener to call if not already registered
     * @param l the new listener to add*/
    public void addListener(ISeekBarGraduatedListener l)
    {
        if(!m_listeners.contains(l))
            m_listeners.add(l);
    }

    /** remove an already registered listener
     * @param l the listener to remove*/
    public void removeListener(ISeekBarGraduatedListener l)
    {
        m_listeners.remove(l);
    }

    /** Get the number of steps (graduations) to render and use
     * @return the number of steps/graduations*/
    public int getNbSteps()
    {
        return m_steps;
    }

    /** Set the number of steps (graduations) to render and use. Minimum: 2 (beginning and end)
     * @param nbSteps The numbef of steps to render*/
    public void setNbSteps(int nbSteps)
    {
        if(nbSteps != m_steps)
        {
            m_steps = nbSteps;
            for(ISeekBarGraduatedListener l : m_listeners)
                l.onSetNbSteps(this, nbSteps);
        }
    }
}
