#include "Graphics/GameObject.h"
#include "Graphics/Render.h"
#include "Graphics/GLRenderer.h"

namespace sereno
{
    GameObject::GameObject(GameObject* parent, GLRenderer* renderer, Material* mtl) : Drawable(renderer, mtl), Transformable(), m_parent(parent)
    {
        if(parent)
            parent->addChild(this);
        setScale(glm::vec3(1, 1, 1));
        setPosition(glm::vec3(0, 0, 0));
        setRotate(Quaternionf(0, 0, 0, 1));

        setApplyTransformation(parent);
    }

    GameObject::~GameObject()
    {
        setParent(NULL);
        clearChild();
    }

    void GameObject::update(Render* render)
    {
        onUpdate(render);
        if(m_isVisible && render && m_mtl)
            render->addToDraw(this);

        for(GameObject* child : m_children)
            child->update(render);
    }

    void GameObject::onUpdate(Render* render)
    {}

    void GameObject::setParent(GameObject* parent, int pos)
    {
        if(m_parent)
            m_parent->removeChild(this);
        
        m_parent = parent;	

        if(parent)
            m_parent->addChild(this, pos);
    }

    void GameObject::addChild(GameObject* child, int pos)
    {
        if(child->m_parent != this)
            child->setParent(this, pos);

        if(child != NULL && !isChild(child))
        {
            if(pos < 0 || pos >= (int)(m_children.size()))
                m_children.push_back(child);

            else
            {
                std::list<GameObject*>::iterator it = m_children.begin();
                std::advance(it, pos);
                m_children.insert(it, child);
            }
        }
    }

    bool GameObject::removeChild(GameObject *child)
    {
        if(child->getParent() == this)
        {
            for(GameObject* c : m_children)
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

    void GameObject::clearChild()
    {
        for(GameObject* child : m_children)
            child->m_parent = NULL;
        m_children.clear();
    }

    bool GameObject::isChild(GameObject* child)
    {
        bool isChild = false;

        for(GameObject* c : m_children)
            if(c == child)
            {
                isChild = true;
                break;
            }

        return isChild;
    }
}
