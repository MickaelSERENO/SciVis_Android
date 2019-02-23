package com.sereno;

import java.util.ArrayList;
import java.util.List;

/** @brief Class representing a tree
 * A tree is composed of multiple leaves, themselves composed of more leaves*/
public class Tree<T>
{
    /** @brief Tree Listener*/
    public interface TreeListener<T>
    {
        /** @brief Called when a child has been added
         * @param parent the parent after the addition
         * @param child the child added*/
        void onAddChild(Tree<T> parent, Tree<T> child);

        /** @brief Called when a child has been removed
         * @param parent the parent before removal
         * @param child the child removed*/
        void onRemoveChild(Tree<T> parent, Tree<T> child);
    }

    /** @brief The leaves*/
    private List<Tree> m_leaves = new ArrayList<>();

    /** @brief The stored value*/
    public T value = null;

    /** @brief The listeners to call when the Tree state changes*/
    public List<TreeListener<T>> m_listeners = new ArrayList<>();

    /** @brief The parent Tree*/
    private Tree m_parent = null;

    /** @brief Constructor
     * @param v the value bound to this Tree*/
    public Tree(T v)
    {
        value = v;
    }

    /** @brief Add a new listener
     * @param l the new listener*/
    public void addListener(TreeListener<T> l)
    {
        m_listeners.add(l);
    }

    /** @brief Remove an old listener
     * @param l the listener to remove*/
    public void removeListener(TreeListener<T> l)
    {
        m_listeners.remove(l);
    }

    /** @brief Get the parent of this Tree
     * @return The parent of the Tree*/
    public Tree getParent()
    {
        return m_parent;
    }

    /** @brief Get the list of leaves containing the children data
     * @return the children list*/
    public List<Tree> getChildren()
    {
        return m_leaves;
    }

    /** @brief Add a new child to this Tree
     * @param child the child to add. If it has already a parent, the parent changes
     * @param index the index to put this Tree. Value < 0 signifies that the child will be put at the end of the list*/
    public void addChild(Tree child, int index)
    {
        if(child.m_parent != null)
        {
            child.setParent(this, index);
            //Return because setParent will also call this method
            return;
        }

        if(index < 0)
            m_leaves.add(child);
        else
            m_leaves.add(index, child);

        for(TreeListener l : m_listeners)
            l.onAddChild(this, child);
    }

    /** @brief Remove a child from this Tree
     * @param child the child to remove. Done only if child.parent == this*/
    public void removeChild(Tree child)
    {
        if(child.m_parent == this)
        {
            for(TreeListener l : m_listeners)
                l.onRemoveChild(this, child);
            child.m_parent = null;
            m_leaves.remove(child);
        }
    }

    /** @brief Set the parent of this Tree
     * If m_parent != null, we first call remove child
     * @param parent the new parent
     * @param index the index in the parent children list. Value < 0 signifies that this object will be put at the end of the parent children list*/
    public void setParent(Tree parent, int index)
    {
        if(m_parent != null)
            m_parent.removeChild(this);
        if(parent != null)
            parent.addChild(this, index);
    }

    @Override
    public boolean equals(Object obj)
    {
        try
        {
            Tree l = (Tree)obj;
            return l.value.equals(this.value) && m_parent == l.m_parent; //Test the equality of the object (equals) and the reference of the parent (same parent)
        }
        catch(Exception e)
        {
            return false;
        }
    }
}