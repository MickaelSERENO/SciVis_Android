#include "Graphics/Drawable.h"
#include "Graphics/Render.h"

namespace sereno
{
    Drawable::Drawable(Drawable* parent, Material* mtl) : m_mtl(mtl), m_parent(parent)
    {
        if(parent)
            parent->addChild(this);
    }

    Drawable::~Drawable()
    {
        setParent(NULL);
        clearChild();
    }

    void Drawable::update(Render* render)
    {
        onUpdate(render);
        if(m_isVisible && render)
            render->addToDraw(this);

        for(Drawable* child : m_children)
            child->update(render);
    }

    void Drawable::onUpdate(Render* render)
    {}

    void Drawable::draw(const glm::mat4& cameraMat)
    {}

    void Drawable::postDraw(const glm::mat4& cameraMat)
    {}

    void Drawable::setParent(Drawable* parent, int pos)
    {
        if(m_parent)
            m_parent->removeChild(this);
        
        m_parent = parent;	

        if(parent)
            m_parent->addChild(this, pos);
    }

    void Drawable::addChild(Drawable* child, int pos)
    {
        if(child->m_parent != this)
            child->setParent(this, pos);

        if(child != NULL && !isChild(child))
        {
            if(pos < 0 || pos >= (int)(m_children.size()))
                m_children.push_back(child);

            else
            {
                std::list<Drawable*>::iterator it = m_children.begin();
                std::advance(it, pos);
                m_children.insert(it, child);
            }
        }
    }

    bool Drawable::removeChild(Drawable *child)
    {
        if(child->getParent() == this)
        {
            for(Drawable* c : m_children)
            {
                if(c == child)
                {
                    child->m_parent = NULL;
                    return true;
                }
            }
        }
        return false;
    }

    void Drawable::clearChild()
    {
        for(Drawable* child : m_children)
            child->m_parent = NULL;
        m_children.clear();
    }

    bool Drawable::isChild(Drawable* child)
    {
        bool isChild = false;

        for(Drawable* c : m_children)
            if(c == child)
            {
                isChild = true;
                break;
            }

        return isChild;
    }
}
